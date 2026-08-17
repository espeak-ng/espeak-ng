# Why we implement `espeak_TextToWordPhonemePairsWithTerminator` internally instead of externally

This chapter explains why the word-level phoneme API lives inside
`src/libespeak-ng/speech.c` rather than being built on top of the already
exposed API from a separate repository (e.g. a C++ wrapper).

## The contract

`espeak_TextToWordPhonemePairsWithTerminator()` must return, for each clause:

- the flat clause phoneme string, byte-identical to what
  `espeak_TextToPhonemesWithTerminator()` returns;
- one pair per word of the original text: the word (span + copy) and its
  phonemes, with no leading/trailing space;
- the invariant that joining all pairs' phonemes with single spaces
  reproduces the flat clause string character-for-character.

That last invariant is the hard requirement: word phonemes are not a separate
pronunciation pass, they are a grouping of the exact same phoneme stream that
the flat renderer emits.

## What the internal implementation relies on

The implementation in `src/libespeak-ng/speech.c` (staged diff) reads the
following, none of which is part of the public `speak_lib.h` API:

| Symbol | Where it lives | What it is used for |
|---|---|---|
| `phoneme_list[]`, `n_phoneme_list` | `synthesize.h` (extern) | the per-clause phoneme stream; the walker iterates it and reads each entry's `newword` (`PHLIST_START_OF_WORD/SENTENCE/CLAUSE`), `sourceix`, `synthflags` (`SFLAG_SYLLABLE/LENGTHEN`), `stresslevel`, `type`, `tone_ph`, `ph->code` |
| `WritePhMnemonic`, `WritePhMnemonicWithStress` | `dictionary.h` | the exact per-entry renderer, including lengthen/syllabic/tone extras via `phoneme_tab[phonLENGTHEN]`, `phoneme_tab[phonSYLLABIC]`, `phoneme_tab[tone_ph]` and the `STRESS_IS_*` thresholds |
| `clause_start_char` | `translate.h` (extern) | message-relative span base for `word_position` |
| `phoneme_tab[]`, `phonLENGTHEN`, `phonSYLLABIC`, `phonSWITCH`, `PHLIST_*`, `SFLAG_*` | `synthesize.h` / `phoneme.h` | constants and tables used by the renderer replica |

## What the public API exposes instead

An external implementation has two sources of truth:

1. **The flat clause string** via `espeak_TextToPhonemesWithTerminator()`.
   This gives no word boundaries and no per-word phonemes.
2. **The synthesis event channel**: `espeak_Synth()` with
   `espeakINITIALIZE_PHONEME_EVENTS | espeakINITIALIZE_PHONEME_IPA`
   emits `espeakEVENT_WORD` events (positions/lengths) and
   `espeakEVENT_PHONEME` events whose names are rendered by
   `WritePhMnemonicWithStress` — the same renderer the flat path uses.

An experiment grouping phoneme events by word events and joining the words
with single spaces reproduced the flat output of
`espeak_TextToPhonemesWithTerminator()` byte-for-byte for the default IPA and
ascii modes (including the internal spaces of `2.5` and `.NET`).

## Where an external implementation cannot be identical

The event channel is not a full proxy for the flat renderer. The divergences:

| Case | Internal implementation | External event-based implementation |
|---|---|---|
| Tie/separator phoneme modes (bits 8-23 of `phonememode`) | replicates `GetTranslatedPhonemeString`'s `use_tie` insertion and `separate_phonemes` characters, with the `ix > 1` rule and the space/sentence-clause logic driven by `newword` | events carry only `WritePhMnemonicWithStress(use_ipa)` — no ties, no separators → different bytes |
| Extra phonemes after a `(lang)` switch | lengthen/syllabic/tone extras are appended only when `ph->code != phonSWITCH`; boundary phonemes attach to the preceding group exactly where the flat renderer places them | only an ordered name stream; the suppression decision must be inferred and the switch entry's extras would still appear as events → not provably identical |
| Long phoneme names | the flat buffer is unbounded | `espeak_EVENT.id.string` holds only 8 bytes; names at or beyond that length truncate |
| Command-queue pressure | a single pass, nothing is ever dropped | `DoPhonemeMarker` skips markers when the command queue is full (`WcmdqFree() > 5`) → silent loss |
| Span base | `word_position` starts from `clause_start_char` (phoneme path) and is refined against the original text | WORD events carry a position derived from the *synthesis* path (a different `+1` look-ahead convention) and the 31-char-capped packed length; the base value itself is not exposed |

## Conclusion

The internal implementation guarantees the byte-identical contract by
grouping the same `phoneme_list[]` stream the flat renderer uses and calling
the same renderer helpers. An external implementation can reproduce the
default IPA/ascii case through the event channel, but it cannot guarantee
identity for tie/separator modes, switch-entry edge cases, long names, or
under queue pressure, and it has no direct access to the span base. Keeping
the implementation in the library is the only way to make the invariant
exact rather than best-effort.
