# Mongolian (Khalkha)

Khalkha is the standard spoken variety of Mongolian and the basis of the
Cyrillic literary language used in Mongolia. This note describes how eSpeak NG
implements it, and — more usefully for anyone changing the files — *why* the
rules are shaped the way they are. Khalkha differs from the Slavic languages
that share its alphabet in four ways that between them account for almost every
rule in `dictsource/mn_rules`.

The phonological description follows Svantesson, Tsendina, Karlsson and
Franzén, *The Phonology of Mongolian* (Oxford University Press, 2005).

**Files**

| File | Purpose |
|---|---|
| `espeak-ng-data/lang/xgn/mn` | voice definition (male) |
| `espeak-ng-data/lang/xgn/mn-f` | voice definition (female, `mn` variant 4) |
| `phsource/ph_mongolian` | phoneme definitions |
| `phsource/mongolian/` | vowel formant data (a template, not recordings — see §6) |
| `dictsource/mn_rules` | spelling-to-phoneme rules |
| `dictsource/mn_list` | exception lexicon |
| `src/libespeak-ng/tr_languages.c` | `case L('m','n')` — stress, letter groups, number options |

---

> **The acoustic numbers here are historical.** They were produced by a
> measurement toolkit, `tools/mnlab`, which was untracked, never meant to ship,
> and has since been removed (§17). Passages describing it record how a value
> was arrived at; they are not a procedure you can run today, and no number
> below can currently be re-derived.

## 1. Aspiration, not voicing

This is the single largest difference from Russian, and getting it wrong makes
Mongolian sound like Cyrillic read by a Russian speaker.

Khalkha has no voiced obstruents. The stop and affricate series contrast
**aspirated** with **unaspirated**; the letters that look like voiced/voiceless
pairs in Cyrillic are in fact unaspirated/aspirated pairs.

| Cyrillic | eSpeak symbol | IPA | Cyrillic | eSpeak symbol | IPA |
|---|---|---|---|---|---|
| б | `p` | /p/ | п | `ph` | /pʰ/ |
| д | `t` | /t/ | т | `th` | /tʰ/ |
| г | `k` / `q` | /k/ ~ /ɢ/ | к | `kh` | /kʰ/ |
| з | `ts` | /ts/ | ц | `tsh` | /tsʰ/ |
| ж | `tS` | /tʃ/ | ч | `tSh` | /tʃʰ/ |

So **бар** is `p'ar` and **пар** is `ph'ar`; **дал** is `t'al#` and **тал** is
`th'al#`.

**Measured.** Word-initial voice-onset time in mbspeech confirms the contrast
empirically, not just from the literature:

| | б | д | г | т (aspirated) |
|---|---|---|---|---|
| VOT | 15–20 ms | 20 ms | 20–25 ms | **30–50 ms** |

Short-lag for the unaspirated series, long-lag for т. п and к had too few
word-initial tokens to report.

The voiced series `b d g dZ z v` is still defined — it is inherited from
`base1` — but nothing in `mn_rules` produces it. It exists for loanwords listed
explicitly in `mn_list` where a speaker really does voice the consonant.

The remaining consonants:

| Cyrillic | Symbol | IPA | Note |
|---|---|---|---|
| м н | `m` `n` `N` | /m n ŋ/ | н is [ŋ] before a velar/uvular and word-finally |
| л | `l#` / `l` | /ɬ/ ~ /ɮ/ | lateral **fricative**, see §4 |
| р | `r` | /r/ | apical trill |
| с ш щ | `s` `S` | /s ʃ/ | |
| х | `x` / `X` | /x/ ~ /χ/ | harmony-dependent, see §2 |
| в | `w` | /w/ | `v` only in listed loanwords |
| ф | `f` | /f/ | loanwords only |
| й | `j` | /j/ | |

---

## 2. Vowel harmony, and what it is actually needed for

Roots are **RTR** (а о у ы, and я ё ю) or **ATR** (э ө ү е); **и** is neutral
and harmonically transparent. Khalkha harmony is pharyngeal, so the classes are
not front/back and the acoustic contrast is carried by F1 — see §6. For the
letter-to-sound rules the distinction does not matter: the same letters group
the same way whichever name the classes carry.

Harmony is already spelled out in the vowel letters, so the rules do not need
to derive it for them — Mongolian orthography writes -аар on a back root and
-ээр on a front one, and the letter-to-sound rules simply read what is there.

What harmony *is* needed for is the consonants, because the spelling does not
show it:

- **г** is a velar stop `k` in an ATR word and a uvular stop `q` in an RTR word,
  in every position. Both are voiced. An earlier revision used a uvular
  fricative `Q"` /ʁ/ between vowels; a native listener rejected it (§24).
- **х** is velar `x` in an ATR word and uvular `X` /χ/ in an RTR word.

**Measured, and the two are not equally strong.** Taking F2 at the vowel onset
minus F2 at its midpoint — which controls for the vowel's own F2 — word-initial:

| | RTR | ATR | difference |
|---|---|---|---|
| **х** | −81 | +194 | **275 Hz** |
| **г** | −97 | −27 | **70 Hz** |

х genuinely backs hard in RTR words, so a uvular fricative is right. г retracts
only about 70 Hz more than in ATR words. An earlier revision built `q` from
`WAV(ustop/q)` — the Arabic **qaf** — which is far too strong for that, and made
**газар** come out as [qaʦar]. It is now the unaspirated velar burst with the
measured locus (~1300 Hz). That change measured better on MCD (+0.23 dB,
CI [+0.02, +0.47]).

| Word | Output | |
|---|---|---|
| гэр | `k'er` | front root, velar |
| газар | `q'atsa#r` | back root, uvular |
| бага | `p'aqa#` | back root, uvular |
| хэл | `x'el#` | front root, velar |
| хот | `X'Oth` | back root, uvular |

Because **и** is transparent, a suffix containing only и inherits the root's
class. The rules look leftward past the и to find the nearest harmonic vowel,
which is why **цагийн** comes out `tsh'aqi:n` (uvular, from back цаг) while
**гэрийн** comes out `k'eri:n` (velar, from front гэр) — two spellings that look
alike but are not.

### How the lookback is written

The obvious tool for "find the nearest vowel to my left" is eSpeak's `J` skip
(`L01J)`). It works, but **`J` scans past the start of the word into the
previous word**, which silently made `нуруу олон` reduce the initial о of олон
because it found the final у of нуруу.

So every lookback in `mn_rules` is instead written as an explicit ladder:

```
L01)            vowel immediately before
L01L05)         vowel, then one consonant
L01L05L05)      vowel, then two consonants
L01L05L05L05)   vowel, then three consonants
```

Three intervening consonants is the phonotactic maximum, so the ladder is
exhaustive, and it cannot leak across a word boundary. `tests/language-pronunciation.test`
pins this with a `нуруу олон` case.

---

## 3. Unstable vowels (гээгдэх эгшиг)

Short vowels outside the first syllable are not pronounced as written. They
delete, or reduce to a schwa, depending on what surrounds them. This is the
most audible single feature of spoken Khalkha, and the reason a naive
letter-by-letter reading sounds so wrong.

The rules implement it with five guards:

| | Guard | Effect |
|---|---|---|
| G1 | Non-initial only | The first syllable's vowel is never touched. |
| G2 | Short only | Long vowels and diphthongs are two-letter matches carrying `+++`, so they outscore every deletion rule. |
| G3 | Word-final vowels reduce, never vanish | **бага** → `p'aqa#` [paɢă], not [paɢ]. |
| G4 | The result must be sayable | The vowel REDUCES, it is never deleted: **багана** → `p'aqa#na#` [paɢana], **хотод** → `X'OthO#t`, **нутаг** → `n'Utha#q`. Before a two-consonant cluster nothing happens at all: **нэмэлт** stays `n'eme#l#th`, because [nmlt] is not sayable.<br><br>This rule used to *delete* before a sonorant, and багана was the single example it was written for. A native speaker rejected that output; the vowel is reduced, not lost. Only `а` ever carried a deletion rule, so reducing it makes the whole G4 family consistent. |
| G5 | A word-final -ан/-эн/-ал/-ам reduces, it does not delete | **болон** → `p'OlO#n` [pɔɬɔ̆n], **эзэн** → `'etse#n`, **оюутан** → `'OjU:tha#n`, **төрөл** → `th'8r8#l#`. There is no syllabic-sonorant rule; these go through the ordinary reduction path, the same one that already gave **боловсрол** `p'OlO#wsrO#l#`. |

**и and ы are exempt.** They are the vowels of the genitive and accusative
suffixes (-ын, -ийн, -ыг, -ийг); deleting them would destroy the case system.

Two consequences worth knowing before editing:

- The reduction ladder runs one rung further than the harmony ladder, to four
  consonants, because **ь** and **ъ** count as letters even though they are
  silent. **амьдрах** has three consonant *sounds* between its vowels but four
  consonant *letters*.
- Reduction is **harmony-preserving**: each vowel reduces to its own short
  counterpart (а → `a#`, ө → `8#`, у → `U#`), not to a single central schwa.
  Khalkha reduced vowels keep their harmony class, and collapsing them all to
  `@` measured worse (57.4% vs 58.5% CER, §12). The `@` phoneme stays defined
  for genuinely epenthetic vowels — ones with no letter in the spelling.
- This is the aggressive end of the range described in the literature. Khalkha
  in careful speech keeps more of these vowels as reduced [ə] than the rules
  produce. If output ever needs to be softened further, the deletion rules are
  one contiguous, commented block per vowel group.

### G5 used to emit syllabic sonorants, and that was wrong

An earlier version deleted the vowel in word-final -ан/-эн/-ал/-ам and left
the sonorant syllabic: **болон** → `p'OlN-`, **эзэн** → `'etsN-`,
**төрөл** → `th'8rl-`. A whole-vocabulary sweep put that at **462 word
types, 4.80% of corpus tokens** — far too much of the language to get wrong.

Both transcriptions are defensible on paper. [ɔjʊːtʰŋ̩] for оюутан is
attested, while эзэн is usually cited as [et͡səŋ]; the difference is phonetic
realisation, not phonological analysis, so the corpus was allowed to decide.

Measured over the 184 native words affected, reducing beat deleting:

| | Δ vs syllabic | 95% CI | |
|---|---|---|---|
| MCD | −0.535 | [−0.729, −0.351] | significant |
| F1 distance | −3.73 Hz | [−6.04, −1.56] | significant |
| F2 distance | +1.27 Hz | [−4.75, +7.03] | n.s. |

Nothing degraded. Segment durations were **identical** between the two arms
(total duration ratio 0.903 vs 0.905), which matters because MCD is computed
over the whole DTW path including silence and can otherwise be moved just by
changing how much silence there is — see §7. Here it could not be, so the MCD
gain is real.

The loanwords in that bucket were a separate bug with the same symptom:
**ресторан** → `r'esthO#rN-` was simply wrong, since a Russian loan keeps its
full [a]. Those are fixed lexically in `mn_list`; the rule change fixes the
native words.

One case is knowingly left unresolved: the -тан/-тэн collective suffix
(оюутан, ажилтан, араатан) is the class most often cited WITH a syllabic
nasal, and it now reduces like the rest. It is 11 tokens in the corpus — too
few to test separately, so it follows the majority pattern rather than a guess.

---

## 4. л is a lateral fricative

Khalkha **л** is /ɮ/, a lateral **fricative**, not the liquid [l] of English or
Russian. It surfaces voiceless [ɬ] word-initially, word-finally and next to a
voiceless consonant, and voiced [ɮ] only between vowels.

| Word | Output | |
|---|---|---|
| лам | `l#'am` | word-initial → [ɬ] |
| хэл | `x'el#` | word-final → [ɬ] |
| алт | `'al#th` | before voiceless т → [ɬ] |
| алаг | `'ala#q` | between vowels → [ɮ] |

**On the evidence for [ɬ].** A direct test — voicing over the first 40 ms of
word-initial sonorants — came out **inconclusive**: л 0.69 voiced against м 0.76,
н 0.67, р 0.53. That window is too crude to isolate the consonant and n was only
47, so it neither confirms nor refutes the voiceless analysis. The stronger
evidence is indirect: an ablation replacing `l#` with a plain voiced [l]
throughout measured *worse* against human speech (+1.0 Hz F2 distance), so the
lateral fricative is kept.

A note for anyone tempted to "fix the formants" of `l#`: [ɬ] is a fricative.
Its identity is in the turbulence spectrum, which eSpeak models with `WAV()`
noise — a `vls frc` phoneme has no formant track, so setting F1/F2/F3 on it does
nothing. What does shape the percept is the `Vowelin`/`Vowelout` locus, tuned in
`ph_mongolian` to F2 ≈ 1600 with a raised F3, which is what separates lateral
friction from a plain [s].

---

## 5. Palatalisation and the iotated vowels

**ь** palatalises the preceding consonant and is itself silent. The contrast is
phonemic: **мор** `m'Or` 'trace' vs **морь** `m'Orj` 'horse'.

Coronals take a dedicated palatalised phoneme (`t;` `th;` `ts;` `tsh;` `l;`,
plus base1's `n^` and `s;`). Labials and velars take a following [j] instead,
which is how the contrast is realised acoustically at those places of
articulation.

**я ё ю** always carry their glide, unlike Russian where after a consonant they
merely palatalise it — and writing them as j+V is itself what realises
palatalisation on the labials and velars. Their long forms are handled
explicitly, including **юу** → `jU:` and **юү** → `jy:`, which a reading of the
letters in isolation would miss: **оюутан** is `'OjU:tha#n`, not `*OjUUtan`.

**е** is the exception: /je/ word-initially, after a vowel, and after ь; plain
/e/ after a consonant (mostly Russian loanwords).

When ь is followed by an iotated vowel the consonant stays plain, because the
vowel already supplies the glide — **ярья** is `j'arja`, not `*jarjja`. Each
palatalisable consonant therefore carries two rules, `C (ьL08` (plain) scoring
above `C (ь` (palatalised).

---

## 6. Vowels

Seven short, seven long, six diphthongs, plus an epenthetic schwa.

| Cyrillic | Short | Long | IPA |
|---|---|---|---|
| а | `a` | `a:` | /a aː/ |
| э | `e` | `e:` | /e eː/ |
| и | `i` | `i:` | /i iː/ |
| о | `O` | `O:` | /ɔ ɔː/ |
| ө | `8` | `8:` | /ɵ ɵː/ |
| у | `U` | `U:` | /ʊ ʊː/ |
| ү | `y` | `y:` | /u uː/ |

Long vowels are ~2.1× the corresponding short vowel, measured on bare tokens
(2.00–2.18×, mean 2.08).

| Cyrillic | Symbol | IPA |
|---|---|---|
| ай | `aI` | /ai/ |
| эй | `eI` | /ei/ |
| ой | `OI` | /ɔi/ |
| уй | `UI` | /ʊi/ |
| үй | `yI` | /ui/ |
| өй | `8I` | /ɵi/ |

`@` is the epenthetic schwa of §3. It is never written and never stressed.

### The vowel data: measured, and a model I had to correct

`phsource/mongolian/` is **not** a set of independently extracted recordings,
despite what the original comments in `ph_mongolian` said. All 19 files share
the same three peak-amplitude signatures byte for byte — an onset ramp, a
steady state and an offset ramp — and differ only in peak *frequencies*. That
is a formant template with per-vowel values filled in; real per-vowel Praat
extractions cannot produce identical amplitude envelopes.

**F1 and F2 are now measured from real Mongolian speech.** Word-initial (hence
stressed and unreduced) vowels in the mbspeech corpus, 3,270 utterances,
thousands of tokens per vowel, Praat Burg tracking:

| | и | э | а | о | ө | у | ү |
|---|---|---|---|---|---|---|---|
| F1 | 280 | 299 | 548 | 378 | 311 | 342 | 291 |
| F2 | 2069 | 1949 | 1453 | 1025 | 1227 | 874 | 1060 |

These F2 values are measured with label-shifted tokens rejected (see "The
formant label shift" below). An earlier revision of this table gave F2 as
2017 / 1905 / 1379 / 1089 / 1082 / 990 / **800**; those were medians over the
uncleaned distribution, and the ү figure in particular is not a measurement of
anything — 800 Hz sits between the two clusters, in the valley.

#### The ATR/RTR F2 offset, and an artifact I had baked in

A native speaker reported that **ү and ю did not sound clear**. They were right,
and the cause was in data I had written.

`y` (ү) carried **F2 800**, sitting 190 Hz *below* `u` (у) at 990. That is
backwards: ү is the ATR member of the pair, and an advanced tongue root *raises*
F2, so ү belongs above у. With only 190 Hz between them in the wrong direction
the two vowels are confusable — and `ю` renders as `j,'U`, so it inherits у
directly, which is why both sounded wrong.

The 800 came from the mbspeech corpus measurement. ү at F1 283 / F2 800 sits
only 517 Hz apart, which is exactly the configuration where LPC formant tracking
merges F1 and F2 — the failure mode already flagged in this document for back
rounded vowels. It was a tracking artifact, not a measurement.

Corroboration that it was an artifact: before the fix, **үүл could not be
measured at all** — the analyzer returned NaN. Afterwards F1 and F2 are 967 Hz
apart instead of 517, out of the merge zone, and it tracks cleanly.

Current values, read from the shipped spect files, and the offsets they encode:

| pair | RTR (F1/F2) | ATR (F1/F2) | ΔF1 | ΔF2 |
|---|---|---|---|---|
| о / ө | 387 / 1046 | 286 / 1302 | −101 | +256 |
| у / ү | 299 / 981 | 253 / 1035 | −46 | +54 |

An earlier revision of this table gave о 424/1089, ө 327/1179, у 302/990,
ү 283/1120. Those numbers matched neither the shipped files nor the corpus, and
its ү entry (+130 above у) contradicted the harmony section immediately below,
which states that ү is *backer* than у. Both have been replaced with values read
from the files themselves. **Read this table from `spectio.frequencies()`, never
by hand** — the divergence went unnoticed across several revisions.

#### Khalkha harmony is pharyngeal, not front/back

This is worth stating plainly because an earlier revision of these files got it
wrong. The harmony classes are RTR {а о у} versus ATR {э ө ү}, and the ATR
member is marked by **lower F1** — not by a higher F2. Measured on two
independent corpora:

| pair | ΔF1 mbspeech / fleurs-mn | ΔF2 mbspeech / fleurs-mn |
|---|---|---|
| а → э | −171 / −258 | +526 / +433 |
| о → ө | −98 / −166 | **−7 / −75** |
| у → ү | −19 / −85 | **−190 / −73** |

F1 falls for the ATR member in all six comparisons. F2 rises only for the
*unrounded* pair; for both rounded pairs it is flat or falls.

Confirmed with a **cluster bootstrap resampling utterances, not frames** —
frames within one utterance are correlated, so a frame-level bootstrap gives
CIs that are far too narrow:

| pair | ΔF1 [95% CI] | ΔF2 [95% CI] |
|---|---|---|
| а → э | −175 [−213, −150] significant | +532 [+503, +555] significant |
| о → ө | **−96 [−155, −44] significant** | **+3 [−118, +103] not significant** |
| у → ү | **−29 [−56, −1] significant** | −175 [−222, −146] significant |

The F1 half of this holds: ATR lowers F1 in every pair, on both corpora, and
that is the load-bearing claim — Khalkha harmony is pharyngeal, not front/back.

**The F2 half does not hold, and the numbers in the two tables above are
artifacts.** Both were computed over token sets contaminated by the formant
label shift described in the next section: for о, ө, у and ү, a quarter to a
half of tokens have every formant misassigned one position up, so the "F2"
being averaged is a mixture of real F2 and real F3. Re-measured with
label-shifted tokens rejected on a criterion independent of F2, the rounded
pairs give **ө +203 above о** and **ү +186 above у** — not the "indistinguishable"
and "significantly backer" reported above. The cluster bootstrap was sound
methodology applied to bad input; resampling contaminated tokens reproduces the
contamination with a tight confidence interval around it.

What this does *not* license is a return to the front/back model. ө and ү sit
above their RTR partners by ~200 Hz, not by the ~600 Hz a front/back reading
implies, and the F1 evidence is untouched.

A previous revision treated harmony as front/back and raised ө and ү to F2 1350
and 1400. That overshot: measurement puts ө at 1227 and **ү at 1060**. ү shipped
at 1388 — 328 Hz too front at 6.1 SE, independently corroborated by the long
vowel ү: at +359 (4.2 SE) — until it was corrected to 1035. The overshoot this
section warned about was real; it had simply never been removed from the files.

#### Long vowels are NOT separately measured, and that is a finding

Long vowels currently take their short counterpart's F1/F2. An attempt to
measure them separately had to be discarded: word-initial long vowels are rare,
and in this corpus the samples are dominated by single word types.

| | utterances | distinct words | most common |
|---|---|---|---|
| ээ | 21 | 6 | **76% "гээд"** |
| өө | 8 | 6 | — |
| оо | **5** | 5 | — |
| аа | 37 | 10 | 38% **"аарон"** — a Bible proper name |

The measurement suggested long vowels are more peripheral than short, which is
a real cross-linguistic effect and may well be true here — but it cannot be
claimed on 5–37 utterances skewed by proper nouns, so nothing was changed.
Establishing it needs a corpus with broader vocabulary.

The short vowels are on much firmer ground by the same test: и/э/а have
768–1187 utterances across 84–158 distinct word types, у/ү have 219 each, and
the thinnest — о and ө — still have 87 and 75 utterances across 45–50 word
types with no word exceeding 13% of the sample.

#### The formant label shift, and why it invalidated three earlier findings

This is the most reusable result in this document. **The LPC tracker does not
fail loudly on Mongolian's rounded vowels — it fails by relabelling.**

For о, ө, у and ү, F1 and F2 sit close together (median separation 667–713 Hz,
against 1687–1800 Hz for э and и). When the tracker merges them into one peak,
it reports that merged peak as F1 and then shifts every remaining label up by
one: the reported "F2" is really F3, the reported "F3" is really F4. This
affects 22–49% of tokens depending on the vowel.

The signature is unambiguous, because both quantities move together:

| vowel | low-F2 group (F2 / F3) | high-F2 group (F2 / F3) |
|---|---|---|
| о | 1015 / 2644 | 2183 / 3114 |
| ө | 1103 / 2514 | 1996 / 3076 |
| у | 830 / 2645 | 2157 / 3112 |
| ү | 877 / 2456 | 2040 / 3018 |

A ~1200 Hz jump in F2 accompanied by a ~500 Hz jump in F3 is not a vowel
difference; it is the same signal read one formant out of register. э and и show
no such split.

The consequence is that **the median over the mixed distribution is meaningless**
— it lands in the empty valley between the two clusters and moves hundreds of Hz
with the subset. This is why three separate sources gave three different answers
for ү's F2 (+130 above у, −175 below у, +648 above у): all three were medians of
the same bimodal data over different samples.

**The method that defeats it:** reject label-shifted tokens using a criterion
*independent of the formant being measured* — filter F2 on F3 (reject F3 > 2900)
and filter F3 on F2 (reject F2 > 1500, rounded vowels only). Filtering a formant
on itself silently truncates its own tail: rejecting F3 > 2900 to clean an F3
estimate biased и's F3 downward by 70 Hz and flipped the sign of its error.

#### F3 is now measured

The earlier decision not to measure F3 was recorded here as: adopting measured
F3 scored worse, F2 distance 480.9 → 455.1 Hz. **That conclusion is withdrawn,
for two independent reasons.** It judged an F3 change by an *F2* metric, so it
measured how the tracker's F2 estimate shifts when F3 moves — a tracker artifact,
not a quality change. And it ran when F2 itself was 480 Hz from the corpus, eight
times today's error; nothing concluded against a vowel space that wrong transfers
to the current one.

Measured with the independent-criterion method, F3 was systematically low —
mean 134 Hz, worst э −248 (11.8 SE) and о −174 (8.3 SE) — and is now fitted
across the short, long, reduced and diphthong sets. и and ү were left alone,
their errors falling under 3 SE. Agreement after fitting: **F1 10 Hz, F2 64 Hz,
F3 25 Hz.**

Note that the earlier figure of "F3 is 238 Hz low" that motivated this work was
itself inflated by the label shift; the real figure was 134 Hz. The artifact
corrupted the diagnosis as well as the baseline.

`8I` (өй) has its own data, `mongolian/oei`, derived from the ой set by
shifting the onset to the ө target and tapering the shift to zero at the /i/
offglide the two diphthongs share. It is not an approximation of ой.

The measurement harness that found this is described in §12.

#### Fit CONTRASTS, not vowels — the failure that made the voice unintelligible

This is the most important lesson in this document, because the process that
caused it looked completely correct at every step.

Each vowel was fitted to its **absolute** measured value, and each change was
gated independently at 3×SE. ү's error cleared the gate (6.1 SE) and it moved
353 Hz down to its correct value. у's error did not clear it (2.0 SE) and у was
left alone, 107 Hz too front. Both decisions were individually right.

Together they destroyed a phonemic contrast. The у/ү F2 difference collapsed from
+407 Hz to **+54 Hz** against the speaker's +186 — 29% of the real distinction,
and below the threshold at which two vowels are separable at all. у/ү is the
RTR/ATR harmony pair, with a high functional load, and the collapse applied in
both stressed and reduced positions, so it affected a large share of the
vocabulary. A native speaker reported being unable to understand some words.

Every aggregate metric passed while this was true: F1 10 Hz, F2 64 Hz, F3 25 Hz
from the corpus, 150/150 language hashes, 51/51 symbolic cases. **A mean cannot
see two phonemes sitting on top of each other.** The vowel space matched the
speaker on average and was unusable.

The fix was to move у to its own measured value (F2 843), restoring the contrast
to +192 against the speaker's +186 and the pairwise separation to 79 against 80.

So: **a fit that moves one member of a contrast pair must re-check the other,
and a per-vowel significance gate is not sufficient authority to move it.** The
gates below encode that.

#### Acceptance gates

Aggregate error is necessary and not sufficient. These are blocking:

| gate | requirement |
|---|---|
| contrast | each phonemic pair's F2 difference within ±40% of the speaker's |
| separation | every vowel pair ≥ 60 in (F1, F2/3) space, or ≥ the speaker's own |
| `formant_factor` | 0.96–1.04, i.e. `pitch` base in 69..95 (see §7) |
| reduction | no `X#` phoneme may point at a full-vowel spect file |
| pause structure | internal pauses per utterance near the speaker's 4.0 (see §7) |

---

## 7. Intonation

Four Khalkha tunes are defined in `phsource/intonation` and bound by a `tunes`
line in both voice files:

```
tunes mn_s mn_c mn_q mn_e
```

Khalkha stresses the first syllable of every content word, so a clause presents
the intonation model with a long run of closely-spaced accents rather than the
sparse pattern the English tunes are shaped around. Two consequences shape the
definitions: the head declines gently rather than alternating (with an accent on
nearly every word, large per-accent excursions read as sing-song, not emphasis),
and `headextend` matters more than usual because the head routinely outruns the
step count — its values descend smoothly so a long clause drifts down instead of
resetting on every word.

Measured F0 over the final third of a clause:

| Tune | Clause | Final third |
|---|---|---|
| `mn_s` | statement | −28.5 Hz |
| `mn_c` | comma / non-final | +6.8 Hz (near level) |
| `mn_q` | question | +33.0 Hz |
| `mn_e` | exclamation | −22.6 Hz, at −2.59 Hz/frame (steepest) |

`mn_c` is also the right shape at the end of a verse line whose sense continues
into the next.

### Measured against real contours

F0 was extracted from 613 mbspeech statements, converted to semitones relative
to each utterance's own median (so speaker pitch drops out) and averaged on a
normalised time axis. The same was done to eSpeak reading the same sentences.

**The statement tune is already close: RMSE 0.81 st.** Four candidate parameter
sets — lower head, added late peak, flat-low — all scored 0.812–0.813 st, i.e.
indistinguishable. The tune was left unchanged rather than fitted to noise.

One trap worth recording: the raw RMSE looks like 1.92 st, and the human contour
appears to start 5.4 semitones low. That is a **single frame**. By 40 ms the
human is already at +0.94 st. It is an onset artifact, not intonation, and
chasing it would have made every Mongolian utterance begin with a dip.

**Question intonation could not be fitted — there is no data.** mbspeech
contains 34 question utterances; fleurs-mn's test split has **3** and its
validation split has **0**. `mn_q` therefore remains a reasoned shape, verified
only for direction (+33.0 Hz over the final third against a statement's
−28.5 Hz). Fitting it needs a Mongolian corpus with interrogatives in it.

### Absolute pitch, range, and the `pitch` parameter

The contour work above normalises each utterance to its own median, so it is
blind to absolute pitch and to overall range. Both were then measured directly
against the speaker (floor 50 Hz, ceiling 600 Hz — a 400 Hz ceiling silently
discards a quarter of eSpeak's frames at wide settings and corrupts the result):

| | speaker | mn (`pitch 80 140`) |
|---|---|---|
| median F0 | 131 Hz | **131 Hz** |
| F0 IQR | 8.85 st | 3.85 st |
| declination, first 80% | +2.27 st | −0.67 st |

An intermediate revision used `pitch 55 145` to raise the range to 5.59 st. It
was reverted: see the `formant_factor` paragraph below. `pitch 80 140` matches
the speaker's median exactly and has the flattest declination of every candidate
tested; its narrower range is accepted deliberately, because absolute pitch and
consonant clarity are far more audible than range.

**`pitch` takes a BASE and a RANGE, not a minimum and a maximum.**
[voices.c:596](../../../src/libespeak-ng/voices.c#L596) computes
`pitch_base = (p1−9)<<12` and `pitch_range = (p2−p1)*108`. Raising `p2` widens
the range but also lifts the median substantially, and the lift is asymmetric —
`pitch 60 340` measures a median of 362 Hz, and `pitch 60 200` measures 208 Hz,
neither of them a male voice. Reaching the speaker's 8.85 st range by this knob
alone would require a median near 166 Hz, so it cannot be done: absolute pitch
is far more audible than range, and it wins.

**`p1` also sets `formant_factor`, and that is a trap.**
[voices.c:598](../../../src/libespeak-ng/voices.c#L598) computes it as
`(1 + ((p1−82)/82)/4) × 256`, and `AdjustFormants` applies it to the
consonant→vowel **F2 transition locus** — the primary place-of-articulation cue.

An earlier revision read this as harmless because it does not touch steady-state
vowel targets, and used `pitch 55 145` to buy 0.8 st of range. At `pitch 55` the
factor is **0.914: every consonant's place cue shifted −8.6%**, the most extreme
value of any shipped voice (all others cluster at 80–82, i.e. ≈1.00). A native
speaker reported consonants as mushy. The trade was bad on its own terms too —
`pitch 80 140` gives a *better* median (131 vs 138) with a neutral 0.994.

**Keep the base in 69..95** (factor 0.96–1.04). This does not apply to `mn-f`,
where a high base is correct for a female voice; see §13.

Range and declination trade off directly, because eSpeak's range comes mostly
from the head's downward ramp — flattening `head 5 98 30` to `5 92 78` moves
declination to +0.34 st but collapses the range to 3.97 st. With the base pinned
for consonant integrity, that trade is no longer available to spend, and the
narrower range stands.

### Inter-word pauses: the mean was right and the distribution was wrong

`word_gap`'s low bits make `phonemelist.c` insert a pause phoneme between every
pair of words. Mongolian used `2` (`phonPAUSE_SHORT`), chosen by matching the
**mean** gap per word — 98.5 ms against the speaker's 95.0. That mean was right
and the voice was staccato, because the mean is not what matters:

| | eSpeak (`word_gap 2`) | speaker |
|---|---|---|
| internal pauses per utterance | 12.5 | **4.0** |
| median pause length | 50 ms | **125 ms** |
| silent fraction | 20.4% | 13.6% |

The speaker concentrates silence into a few syntactic pauses; eSpeak was
spreading the same total evenly between every word, which is the acoustic
definition of staccato. Mongolian was the only language setting the low bits at
all (others use `0x8` / `0x21`, which are flag bits).

Now `0x20` — the flag that makes a word-final vowel's length treat the boundary
as "a pause + the next word's first phoneme", which is the juncture marking the
original comment actually wanted, with **no inserted silence**. Silent fraction
falls to 16.0%.

Two side effects, both checked. `word_gap`'s low bits were *overwriting* the
`phonPAUSE_NOLINK` that `vowel_pause = 1` requests, so `_|` markers now appear
before vowel-initial words; they add no silence and stop words linking together.
And the 51 symbolic test expectations lost a `_` at every word boundary.

**Contour shape remains unmatched:** correlation between eSpeak's F0 track and
the speaker's is r = 0.17. Range and declination are now fitted; matching the
shape of the speaker's contour means modelling Mongolian accent placement, which
is a separate project.

> **Voice files are staged at CONFIGURE time.** `cmake/data.cmake` copies
> `espeak-ng-data/lang` with `file(COPY)`, so `cmake --build build --target data`
> does **not** propagate an edit to this file — the build keeps serving the
> previously staged copy, silently. A prior revision's `pitch 60 200` (mn) and
> `pitch 120 300` (mn-f) sat in the tree while every test and measurement ran
> against the old values; both would have shipped badly wrong voices (208 Hz and
> 287 Hz medians). After editing a voice file, re-run `cmake -Bbuild`.

### Speaking rate: the problem was juncture, not tempo

An earlier version of this section reported eSpeak as **38% longer** than the
human and treated `speed` as the lever. That framing was wrong twice over.

**The sign depends on the corpus.** Against fleurs-mn eSpeak is longer; against
mbspeech it is *shorter*, at a duration ratio of **0.829** (95% CI [0.815,
0.845], n=200). eSpeak's tempo sits between two corpora that differ from each
other more than either differs from eSpeak, so there is no single global rate
error to correct, and `speed` is the wrong knob.

**The deficit is concentrated in junctures, not segments.** Splitting each
recording into speech and non-speech by energy alone — no alignment, so no DTW
artefacts:

| | ratio (eSpeak / human) | 95% CI |
|---|---|---|
| speech | 0.902 | [0.888, 0.917] |
| quiet | 0.706 | [0.667, 0.747] |

The intervals do not overlap. eSpeak's segments are mildly short; its pauses are
badly short. Raising `speed` shortens the segments further, which is why it
looked like a trade-off before. `speed 85` stands, and the real fix was
`word_gap` — see below.

### word_gap: 1 → 2

`langopts.word_gap = 1` inserts `phonPAUSE_VSHORT`, whose declared `length` is
**1**. That is not a short gap, it is no gap: every pause eSpeak had between
words was incidental, coming from stop closures. `word_gap = 2` inserts
`phonPAUSE_SHORT` (`length 25`) instead.

| | gaps/word | median gap | gap ms/word |
|---|---|---|---|
| human (mbspeech) | 0.67 | 45 ms | 95.0 |
| `word_gap = 1` | 1.30 | 40 ms | 79.0 |
| **`word_gap = 2`** | **1.59** | **40 ms** | **98.5** |
| `word_gap = 3` | 1.58 | 70 ms | 144.0 |

Measured against the session baseline, `word_gap = 2` improved every metric:
MCD −0.366 (CI [−0.606, −0.126]), F1 distance −4.82 Hz (CI [−8.74, −1.54]),
F2 distance −9.05 Hz (CI [−16.82, −1.21]) — all significant.

**`word_gap = 3` scored better still and was rejected anyway.** It matches total
duration almost exactly (ratio 0.997), but only by doubling the median gap to
70 ms against the human's 45 and overshooting total gap time by 52%. What it
actually buys is DTW slack: MCD is averaged over the whole warping path
including silence, so padding every boundary makes silence-vs-silence frames
cheap and drags the mean down without the speech sounding any closer. The gap
*distribution* is the honest discriminator, and on it `word_gap = 2` wins.

Two limits worth recording:

- **Aggregate "quiet time" is not a usable metric.** eSpeak's silence is digital
  zero; a human recording's pauses carry breath and room tone. Human quiet/word
  measures 266 ms at a −20 dB threshold and 29 ms at −60 dB, while eSpeak's
  barely moves below −40 dB. The two are not comparable at any threshold. Only
  gap structure and threshold-free total duration were used for decisions.
- **eSpeak emits too many gaps** — 1.59 per word against the human's 0.67, and
  far too few long phrase-final breaks (1.0 vs 6.1 per 100 words above 400 ms).
  Human pause mass is chunked at phrase boundaries; eSpeak spreads it evenly.
  `word_gap` cannot fix that, since it is a single per-language constant applied
  at every boundary.

### Questions without a question mark

Mongolian marks a yes/no question with a clause-final clitic — **уу / үү / юу /
юү** — and writing such a sentence without `?` is normal. Wh-questions use
**вэ / бэ** after an interrogative word. eSpeak chooses the tune from
punctuation alone, so all of these would otherwise get the statement contour.

`LANGUAGE_OPTIONS.question_particles` (added in `translate.h`) holds a
NULL-terminated list of clause-final interrogative particles. After `ReadClause`,
a clause that would take the full-stop tune is upgraded to the question tune if
its final word is in that list. An explicit `?` or `!` still wins, and the check
returns immediately for any language that leaves the field NULL — which is every
language except Mongolian, so nothing else changes.

`Та Монгол хүн үү` without a question mark measures +33.0 Hz over the final
third, identical to the same sentence written with one.

---

## 8. Stress

Khalkha stress falls on the first syllable and is realised mostly as length and
pitch rather than loudness. `tr_languages.c` sets `STRESSPOSN_1L` with
`S_NO_AUTO_2 | S_NO_EOC_LENGTHEN` and `lengthen_tonic = 0`.

Because unstable-vowel deletion removes non-initial syllables, words routinely
surface with clusters the spelling does not show, so
`max_initial_consonants` is 4 rather than the more usual 3.

---

## 9. Numbers

Mongolian counts on the short scale — зуу, мянга, сая, тэрбум — so its cases sit
in the European-style section of `tests/language-numbers-cardinal.test`.
`tr_languages.c` sets `NUM_OMIT_1_HUNDRED | NUM_DFRACTION_6`, and the number
words themselves are in the Numbers block at the top of `mn_list`.

Units, teens, tens and hundreds are correct.

### Known defect: thousands use the wrong numeral form

Mongolian has **attributive** numeral forms that appear before a counted noun —
гурван, дөрвөн, таван, арван — rather than the plain cardinals гурав, дөрөв,
тав, арав. `mn_list` already supplies these for the hundreds through its `_NC`
entries, but the thousands multiplier takes a plain cardinal:

| Input | Output | Correct Mongolian |
|---|---|---|
| 300 | `qUrw@nts'U:` гурван зуу | ✓ |
| 3000 | `q'Uraw mjaNq@` гурав мянга | гур**ван** мянга |
| 4000 | `t'8r8w mjaNq@` дөрөв мянга | дөр**вөн** мянга |
| 10000 | `'araw mjaNq@` арав мянга | ар**ван** мянга |

This affects every thousands multiplier, not just one of them. eSpeak provides
`NUM2_THOUSANDS_VAR*` for exactly this situation, and `tr_languages.c` currently
sets no `langopts.numbers2` for `mn`.

The behaviour is pinned by the `"thousands"` case in
`tests/language-numbers-cardinal.test`, with a comment marking it as known-wrong.
Pinning it is deliberate: an untested wrong output can quietly become a
*differently* wrong output, whereas a pinned one makes the eventual fix show up
as a visible, intentional diff. Whoever sets `numbers2` must update that line.

One thing that is **not** a defect: compound numerals render as a single word
(`arw@nn'ek` for арван нэг). Every other language in the same test file does the
same, because `word_gap` inserts a pause only between orthographic words, and a
compound numeral is produced as one.

---

## 10. What belongs in `mn_list`

The rules handle harmony, unstable vowels, aspiration, palatalisation and the
г/х/л allophones, so ordinary words should **not** be listed. A stale copy in
`mn_list` silently overrides any future rule fix — several hundred such entries
were removed when the rules were able to derive them.

Four things still need listing:

1. **Numbers and symbol names** — spoken forms of digits and punctuation, not
   spellings.
2. **Clitics and particles** — these need prosodic flags (`$u`, `$pause`), not
   different pronunciations. Write them with the flag and *no* phoneme string,
   so they track the rules. The exceptions are **нь** and **ч**, which have no
   vowel of their own and cannot be syllabified without help.
3. **Loanwords** — these do not undergo unstable-vowel deletion. Without an
   entry, **редактор** becomes [retakʰtʰər] and **километр** loses its о
   entirely. Consonants still follow Khalkha phonology: **банк** is [paŋkʰ],
   not [baŋk].

   The sharpest case is a loanword ending in **-ан / -он**, where the final
   vowel reduces (§3) as it would in a native word. A borrowing keeps it full,
   so **ресторан** is `r'esthOraN` and not `r'esthO#ra#N` — the point is the
   unreduced `a`, not the final nasal. Any new loanword
   with that shape needs an entry. (Before G5 was corrected this was much worse
   than a reduced vowel — the word came out as [restʰɔrŋ̩], with no vowel there
   at all.)
4. **Abbreviations** — the spoken form is a convention (АНУ [anʊ], МУИС
   [mʊis]), not a reading of the letters.

   Unit abbreviations are read as the whole word: км is километр, not the
   letter names [kʰa.em] the spelling fallback produces. They are written out
   in phonemes rather than with `$text`, because **`$text` re-runs the
   replacement through `mn_rules` only and does not consult `mn_list`** — so
   `км километр $text` would say [kʰilɔ̆mĕtʰĕr] while the listed километр
   says [kʰilɔmetʰer]. The unit words are listed too, so both routes agree.

   Only unambiguous multi-letter forms are listed. Single letters are left
   alone on purpose: **л** is far more often the emphatic particle [ɬ] than
   "литр", and м, т, ц likewise. Expanding those would break ordinary
   sentences to fix a rare one.

**A known limitation: enclitics still take a word gap.** нь (3.05% of corpus
tokens) is an enclitic — it attaches to the preceding word, and ном нь is one
prosodic unit. eSpeak has no way to express that. `$combine` merges a word with
the word *after* it and has to be flagged on the first word, which for an
enclitic is unpredictable, and the `word_gap` insertion in `phonemelist.c` has
no per-word override. So нь renders as a bare `n^` with a pause on each side.
`$u` is set on it, which is as far as the format goes.

---

## 10a. Defects found by systematic probing

Three found by probing every declared phoneme for reachability, and every
Mongolian letter and number form for a sane reading. All are fixed; each is
worth knowing about because the same probe will find the next one.

**A voiced stop had leaked in.** `үгүй` was listed as `ygy:`, using the voiced
velar **g**. Mongolian has no voiced stops at all -- the language contrasts
aspiration, not voicing -- so this was a sound outside the inventory, in one of
the most frequent words in the language. Two neighbouring entries
(`ойлгохгүй`, `хамаагүй`) wrote the uvular `q` and a long `y:` where the rules
already produce the correct `k` plus the үй diphthong. All three entries were
removed and the words left to the rules, which handle болохгүй, мэдэхгүй,
байхгүй, хийхгүй and сайнгүй correctly. This is the "stale copy in `mn_list`
silently overrides a rule fix" failure §10 warns about, caught in the act.

**A minus sign was silently dropped.** `-5` read as "тав", not "хасах тав",
because `mn_rules` had no `-` group at all. In Mongolian weather text this
inverts the meaning of the most common numeric construction there is: `-30 хэм`
read as *plus* thirty. Now handled with the same three rules `en_rules` uses --
word-initial hyphen before a digit is хасах, digit-hyphen-digit is зураас
(2010-2020), anything else stays silent. Hyphenated words such as сайн-сайхан
are unaffected.

**№ read out two words.** `mn_emoji` mapped it to the text `дугаар, номер` --
both CLDR annotation names, comma included -- so `№5` said "дугаар номер тав".
Now just `дугаар`.

### Fixed: the `N-р` ordinal

Mongolian writes ordinals as a digit, a hyphen and р — `3-р сарын` is March,
`3-р хороо` the 3rd district. This used to read as the cardinal followed by a
stray consonant (`1-р` → `n'ekr`, `10-р сар` → аравр сар).

**This section previously recorded the defect as unfixable** — that the tokenizer
splits `1-р` before the number parser sees a suffix, so a fix would need changes
to `translate.c` affecting every language, "a feature, not a patch". That was
wrong, and the mistake is worth recording: `numbers.c` explicitly *reconstructs*
the suffix across the split, prepending the hyphen when `FLAG_HYPHEN_AFTER` is
set and reading the following word.

The proof is Crimean Tatar, a Turkic language with the identical `N-nci`
convention: `crh_list` carries `_#-nci  ndZ;'I` and produces `1-nci` →
`b,I*IndZ;'I`, byte-identical to the spelled-out *birinci*. Hungarian and
Esperanto also use hyphenated `_#-` suffixes. Russian happens not to (`1-й` →
"один и краткое"), which is probably where the impression of a general blocker
came from.

The split follows the orthography exactly. `_#-р` is the letter that is actually
written, and each `_No` carries the cardinal plus the harmonising **-дугаа**
(RTR) or **-дүгээ** (ATR) — so `10-р` is аравдугаа + р:

```
_#-р    r
_1o     n'ekty#ke:        // нэгдүгээр      _10o   'ara#wtU#qa:
_2o     X'OjO#rtU#qa:    // хоёрдугаар     _20o   X'Or;tU#qa:
...                                          _0Co   ts'U:tU#qa:   (зуудугаар)
```

Two details the mechanism forces:

- **The teens need their own entries.** Mongolian has explicit `_11`…`_19`
  cardinal words, so composition never reaches the units and `_12o` must exist
  in full. 11-р and 12-р are November and December.
- **Composition ordinalises only the final element**, which is Mongolian's rule
  too: `21-р` → хорин нэгдүгээр, `25-р` → хорин тавдугаар.

All twelve months, `21-р`, `25-р` and `30-р` are covered by `test_phon` cases in
`tests/language-pronunciation.test`, alongside the cardinals they must not
disturb.

Still not handled: `100-р` and `1000-р` produce the right stem but leave the
suffix as a separate token ("зуудугаа р") — the scale-word path appends it with
a word break. Rare enough to leave.

### Known limitations, deliberately not changed

**The decimal separator is the point, not the comma.** `1,5` drops the comma and
reads "нэг тав". Setting `decimal_sep = ','` would fix that but break `1,000`
and `10,000 төгрөг`, which currently read correctly as thousands. Russian, which
uses the decimal comma, gets `1,000` wrong in exactly that way. Without evidence
on which convention dominates real Mongolian text, the current setting handles
the more common case and is left alone.

**Space-separated thousands are not parsed.** `1 000` reads as "нэг тэгтэг тэг".
Russian has the same behaviour despite `NUM_ALLOW_SPACE`, so this is an eSpeak
limitation rather than a Mongolian configuration error.

---

## 10b. Consonant strength, measured

The original native-speaker report was that "pronouncing consonantal is weak".
That is measurable: every consonant token is compared to the human token DTW maps
it onto **in the same word**, so the flanking vowels are identical and cancel.
Level is RMS relative to the utterance, in dB.

Fixed — all three now within noise:

| phoneme | before | after | lever |
|---|---|---|---|
| `tSh` (ч) | −4.3 dB (5.3 SE) | −0.1 | `WAV(ustop/tsh)` 135 → 221 |
| `s` (с) | −1.7 dB (4.2 SE) | −0.0 | local declaration, WAV 100 → 122 |
| `S` (ш) | −2.7 dB (5.5 SE) | −0.4 | local declaration, WAV 100 → 136 |

`s` and `S` are declared locally in `ph_mongolian` *only* to raise their level;
everything else is copied verbatim from `phsource/phonemes`, which leaves `WAV`
at its default amplitude of 100.

**Not fixable, and why — do not retry these:**

- **`r` (−3.2 dB) and `X` (−2.6 dB)** are at the ceiling. `FMT` is already 255 and
  dominates the sum, so raising `addWav` does nothing: it was tried and measured
  (−3.2 → −3.5 dB, within noise) and reverted. These need louder source waveforms.
- **`n` (−3.1 dB), `m`, `j` (−3.2 dB)** cannot be reached from Mongolian data at
  all. Prevocalic nasals and `j` render through `VowelStart`, and the shipped
  definitions have **no fallback `FMT`** — that is eSpeak's design, not a defect,
  so the boosted `FMT` amplitudes on `N`/`m`/`n` apply only in the pause-adjacent
  and preconsonantal branches. `VowelStart`'s second argument is a *length*
  adjustment, not an amplitude ([synthesize.c:538](../../../src/libespeak-ng/synthesize.c#L538)).
  Fixing these needs Mongolian-specific copies of spect files shared by every
  language.

**Spectral centroid** is systematically high (mean 299 Hz over 16 consonants;
`n` +417 at 12.2 SE, `m` +402, `j` +519, `l#` +447). This survives matched
pairing, so it is not vowel spill. The worst offenders are the same
`VowelStart`-driven set above, so the same limitation applies.

### A negative result worth keeping: the shared amplitude envelope is fine

All 24 spect files carry byte-identical peak heights and widths, and this was
long suspected of flattening the voice. **It does not.** Measured 200–5000 Hz,
eSpeak's spectral tilt varies across vowels by 2.42 dB/oct against the speaker's
2.07, with a mean offset of −0.29 dB/oct and no vowel worse than 1.75.

Differing formant *frequencies* produce the correct tilt variation on their own.
**Do not rewrite the amplitude envelopes** — the shared envelope is not a defect,
and `scale_envelope()` exists for the cases where it genuinely is.

---

## 10c. Auditing the vowel space against a reference

The toolkit compared eSpeak against a Mongolian TTS reference
(`facebook/mms-tts-mon`). Before trusting any number below, read this section:
the reference's limits were measured, and they are tighter than they look.

### The reference's noise floor, measured

25 generations of each of 28 words (7 vowels x 4 consonantal frames):

| | F1 | F2 |
|---|---|---|
| test-retest SD (same text, regenerated) | 43 Hz | 68 Hz |
| cross-frame SD (same vowel, different consonants) | 49 Hz | **119 Hz** |

The cross-frame figure is the one that matters. It says the reference's F2 for a
given vowel moves by around 120 Hz depending only on which consonants surround
it. Since it does not shrink with more samples, it is a hard floor: no averaging
makes a 100 Hz F2 difference detectable.

One vowel is far worse. **э has a test-retest SD of 182 Hz in F1** -- the tracker
is failing on it outright, and every э-involving F1 contrast is unusable.

### Contrasts help, but not enough

Subtracting two of the reference's own vowels in the same frame cancels a
systematic bias in its voice or level, which is why contrastive comparison found
the ү defect when absolute comparison could not. Of the 21 possible pairs, the
contrast is stable across frames (|mean| > 2 SD) for **10 of 21 in F1 and 9 of
21 in F2**.

But stability is not determination. Anchoring each frame-stable contrast on
eSpeak's other vowel gives a target for ү F2:

| contrast | reference | implies ү F2 |
|---|---|---|
| э~ү | +554 ± 163 | 1339 |
| и~ү | +613 ± 79 | 1381 |
| ө~ү | +305 ± 99 | **928** |

Those span **452 Hz, or 3.8x the cross-frame noise**. Three individually stable
contrasts imply three different vowels. The reference does not determine a
single ү, and by the same construction it determines no other vowel's F2 either.

A caution about testing this: contrasts derived from the same per-frame means
are algebraically dependent, so a cycle-consistency check on them
(`(а~и) − (а~ө) + (и~ө)`) is identically zero and proves nothing. The spread
above is the real test.

### What the reference may and may not be used for

**May:** detecting that a contrast has the WRONG SIGN. That is a large,
qualitative error, well clear of the noise, and it is how the ү defect was
caught -- eSpeak had ү 190 Hz below у where every source puts it above.

**May not:** setting any absolute formant target, and setting contrast
magnitudes. Its absolute back-vowel F2 (о 1427, у 1287, ө 1628) sits roughly
300 Hz above real Khalkha, i.e. it does not render back vowels as back.

**Also may not:** anything about duration or question prosody. Its long/short
vowel ratio averages 1.13 with several pairs below 1, because VITS normalizes
duration; and its tokenizer never receives punctuation, so it cannot mark a
question.

Its character set is not Mongolian either. The tokenizer holds 34 Cyrillic
letters, **all lowercase**, and silently DROPS anything else -- so `өр`
tokenized to `р`, `шар` to `ар`, and any capitalised word lost its first letter.
It carries the archaic fita `ѳ` instead of `ө`, and `щ` instead of `ш`.
`ttsref.py` folds case, maps those two, and now RAISES on any character the
model cannot represent.

### Reduced vowels centralize, and now do so here too

eSpeak used to give every reduced vowel its full counterpart's spect file --
`a#` was literally `FMT(mongolian/a)` -- so reduction was a length change and
nothing else. §3 records the reasoning: reduction is harmony-preserving, and
collapsing everything to one central schwa measured worse. But PARTIAL
centralization had never been tested, because nothing had measured what the
speaker does.

Measured now, on mbspeech, comparing each vowel's stressed and unstressed tokens
**at matched duration** (60-140 ms, so a short-segment measurement artifact
cannot masquerade as centralization):

| vowel | ΔF1 | ΔF2 | |
|---|---|---|---|
| **а** | **−206** (8.3 SE) | −58 | 532 → 326 |
| **о** | −47 | **+204** (2.6 SE) | 1052 → 1256 |
| **ө** | −37 | **+371** (5.7 SE) | 1395 → 1766 |
| э | −19 | +13 | already central, does not move |
| и | −2 | +0 | already central, does not move |

That is textbook undershoot: the vowels far from the speaker's centroid move in,
the ones already near it stay. у and ү had too few reduced tokens (14 and 4) to
measure and are left alone.

The confound matters and was checked. Comparing raw full against raw reduced
gives a centroid-distance change of −54, but reduced tokens are shorter, and
restricting to matched durations shrinks it to −15…−50 with the sign reversing
in the longest band. The per-vowel figures above are the matched-duration ones.

This affects most syllables in connected speech, since non-initial syllables are
the majority.

#### The numbers above were contaminated, and have been re-measured

`reduced.py` calls `measure.formants()` with **no label-shift rejection**, so
every figure in the table above was computed over the mixed distribution
described in "The formant label shift". The error is large: reduced о measures
**F2 1817 raw against 1237 cleaned**, and reduced ө 1857 raw against 1482. The
"+371 Hz" centralization claimed for ө was mostly artifact, and `oe_red` was
built at F2 1744 on the strength of it.

Re-measured with shifted tokens rejected, the speaker centralizes **every**
vowel, consistently and in both directions:

| vowel | full F2 | reduced F2 | shift | n |
|---|---|---|---|---|
| а | 1442 | 1328 | −114 | 196 |
| э | 1924 | 1793 | −131 | 126 |
| и | 2062 | 1871 | −190 | 62 |
| о | 1021 | 1237 | +216 | 30 |
| ө | 1188 | 1482 | +294 | 19 |
| у | 875 | 1034 | +160 | 13 |

Front vowels fall, back vowels rise, all converging on ≈1250–1550. The earlier
conclusion that "э and и are already central and do not move" was an artifact —
they move as much as anything else.

**`reduced.py` is still uncorrected.** It will re-contaminate any future reduced
fit. Reject on F3 > 2900 before trusting it, exactly as for the full vowels.

Six files now carry measured values: `a_red`, `O_red`, `oe_red`, plus **`e_red`,
`i_red` and `u_red`**, which did not exist — `e#`, `i#` and `U#` pointed straight
at the full-vowel files and therefore could not centralize at all. That left
every unstressed syllable fully articulated, which a native speaker heard as the
voice not being smooth.

`y#` still points at full `y`: reduced ү had too few tokens to measure. Recorded
as a known gap rather than guessed at.

### Resolution: measured against real speakers

The reference's limits made it useless for magnitudes, so the vowel space was
re-measured against **real Mongolian speech** instead: one mbspeech shard
(1,923 utterances, single male speaker). Each sentence was aligned by DTW
against eSpeak's own phoneme events, and every stressed-vowel span mapped onto
the human timeline and measured there.

400 utterances give 192–789 tokens per vowel and standard errors of 10–41 Hz --
an order of magnitude better determined than anything the TTS reference could
offer:

| vowel | n | F1 | F2 | F2 SE |
|---|---|---|---|---|
| а | 789 | 546 | 1489 | 12.7 |
| э | 667 | 304 | 2008 | 10.3 |
| и | 263 | 283 | 2080 | 18.2 |
| о | 385 | 384 | 1070 | 24.2 |
| ө | 192 | 313 | 1321 | 40.4 |
| у | 267 | 337 | 1009 | 37.0 |
| ү | 253 | 293 | 1408 | 40.9 |

These are now the values in `phsource/mongolian/`, set by moving each file by the
measured synthesis-versus-human gap. eSpeak's synthesized vowels agree with the
speaker to a **mean of 8 Hz across F1 and F2**, every vowel inside 1.5 SE.

The ATR/RTR F2 offsets, the thing the native speaker originally reported as
unclear, land on the human values:

| pair | human | eSpeak |
|---|---|---|
| о → ө | +203 Hz | +256 Hz |
| у → ү | +186 Hz | +54 Hz |

**These offsets were revised after the label shift was found.** The values this
section previously reported (+398 Hz for у → ү, matched by eSpeak at +397) came
from the uncleaned distribution and overstated the offset by more than double.
Acting on them pushed ү to F2 1388, which measurement later showed to be 328 Hz
too front at 6.1 SE. ү now sits at 1035.

The direction of the original native-speaker report was right — ү began this
work at F2 800, below у, and the corpus does put it above — but the magnitude
was not, and the corrected magnitude is roughly half what was claimed.

`audit.py` scores the space; `reliability.py` measures the floor above and should
be re-run before any future number from this reference is believed.


Two things about that reference are load-bearing, and both were learned the
hard way.

**Only CONTRASTS are comparable, never absolute values.** The reference cannot
supply an absolute target: its isolated vowels are out of distribution (a bare ө
gave F2 1648, a front value), and its own у measures F1 378 in isolation but 602
across word contexts — it disagrees with itself by 224 Hz. Its back vowels are
not properly back either, rendering о at F2 1373 and у at 1357 where real
Khalkha male values are 900–1100. But subtracting two of its own vowels measured
the same way in the same frame cancels any systematic error in its voice, level
or vocal tract length. Every finding here is a contrast-to-contrast comparison.

**Its character set is not Mongolian.** The tokenizer holds only 34 Cyrillic
letters and silently DROPS anything else, so `өр` tokenized to `р` and `шар` to
`ар`, and every ө and ш measurement was of a word with the target sound missing.
It carries the archaic fita `ѳ` (U+0473) instead of `ө` (U+04E9), and `щ`
instead of `ш` — its training text used an older orthography. `ttsref.py` maps
these and now RAISES on any character the model cannot represent, rather than
letting it vanish.

`audit.py` scores the whole vowel space: every vowel in one uniform `тVт` frame,
all 21 pairwise contrasts against the reference's, reported as sign agreement
(are the two vowels ordered the right way round?) and mean contrast error in Hz.
`solve.py` least-squares fits formant values to the reference's whole contrast
matrix — useful, but its F2 answer must be sanity-checked, because fitting a
compressed reference space compresses eSpeak's.

Two audit results are the reference's fault, not eSpeak's, and should not be
"fixed": it puts э above и in F2 (и is the highest front vowel, so eSpeak is
right), and its о~ү ordering rests on back vowels it does not render properly.

---

## 10d. The clarity round: six silent phonemes and what else it turned up

The brief changed to **intelligibility over naturalness** — "robotic sound is
okay. As long it's clear." That inverts several earlier decisions, because
matching what a human speaker does (reduction, centralization) costs contrast,
and a formant synthesiser has no top-down context to spend it against.

### The whole oral stop series was silent

**б п д т г к produced no audio at all.** Measured relative to utterance level:
`t` −162.1 dB, `k` −112.9, `th` −116.4, against −0.8 to −11 dB for every other
consonant in the inventory. Not weak — absent. This is what a native speaker was
reporting as "I can't understand some words".

The cause is a compiler trap that reports **0 errors**:

```
phoneme t
  import_phoneme consonants/t-
  ipa t            <-- silences the phoneme
endphoneme
```

`ipa` is not an annotation. It compiles to a program instruction
(`compiledata.c`: `*prog_out++ = (i_IPA_NAME << 8) + value`). `ImportPhoneme()`
memcpy's the source `PHONEME_TAB` including its `program_index`, but the `ipa`
line then starts a fresh program buffer, and at `endphoneme` that new program —
an IPA name and no `WAV` — replaces the imported one. Removing `ipa t` alone
restored `t` to −13.7 dB.

All six are now declared in full locally. Context-controlled stop contrasts
(MFCC centroid distance ÷ within-phoneme spread, compared **within a matched
following vowel** — pooling across vowels produces false positives):

| pair | before | after |
|---|---|---|
| t/k | 0.79 | **5.33** |
| p/t | 0.84 | **6.86** |
| t/th | 0.95 | **1.18** |

**Any future audit should measure per-phoneme output energy.** A phoneme below
about −40 dB relative to the utterance is not quiet, it is missing, and nothing
in the build will say so.

### Other defects found and fixed in the same round

| defect | evidence | fix |
|---|---|---|
| `consonants` never set, so unvoiced consonants ran at espeak's default 90 | C:V −8.2 dB against the speaker's −7.0 | `consonants 115` → −6.9 |
| `r`'s `addWav` pointed at a **spectrum** file | `r3/r_trill` is `SPECTSQ2`; the waveform is `r_trill.wav` | added the extension; −6.3 → −2.6 dB |
| и blocked the г/х harmony lookback | борхи, тохир, унших, ногоохи all took the velar allophone in back-harmony words | `.L10` (consonants + и) in the gap positions, plus the missing zero-gap and three-gap rungs |
| ь after х/г was silently dropped | дах and дахь were byte-identical, likewise баг/багь, ах/ахь | explicit harmony-aware `х (ь` / `г (ь` rules that outrank the `L05` ladder |
| `n^` left out of the nasal level pass | −7.7 dB against the speaker while n/m/N were within 2.8 | local declaration at FMT 240 → −0.7 |
| no final lengthening | speaker lengthens the clause-final vowel 1.60×, eSpeak 1.04× | dropped `S_NO_EOC_LENGTHEN`, `lengthen_tonic` 0 → 35 → 1.53× |
| Latin acronyms had no vowels | `PDF` → `phtf`, `www` → `www` — unpronounceable | epenthetic vowel on a Latin consonant with no adjacent Latin vowel |

### Negative results — do not retry these

- **`breath` is not a defect.** Mongolian is the only espeak voice that sets it,
  and it injects noise at the formant frequencies, which looked bad. Removing it
  moved vowel HNR 18.2 → 18.4 dB, i.e. nothing: `breath[0]` holds the *count*, so
  the real per-formant amplitudes are just 2/3/1/1/1/1.
- **The ц/з affricate "inversion" is not real.** з is written 119/151 and ц
  81/104, which looks like the aspirated member sitting below the unaspirated
  one. They use *different source waveforms*, so the amplitude parameter is not
  comparable between them. Measured output has ц already louder, as the speaker
  has it. Raising ц measured +4.5 dB hotter than him and was reverted.
  **Never compare `WAV()` amplitudes across different source files.**
- **Sonorants are not confusable.** Pooled across vowel contexts n/r measures
  0.50; context-controlled it is 3.05. The pooled figure is an artifact.
- **The N-р ordinal blocker was NOT real** — this entry used to assert it was,
  "contrary to an audit claim that it was demonstrably false". The audit was
  right and this entry was wrong. `1-р` does tokenise as the number plus a
  separate `р`, but `numbers.c` reconstructs the suffix across exactly that
  split; no `punct_within_word` change is needed and ranges like `2010-2020`
  are untouched. Fixed as a dictionary addition — see §10a.

---

## 10e. The reference was stale, and mn-f was the voice nobody was testing

### The measurement cache silently held values from a broken build

The toolkit cached DTW-aligned corpus tokens, and every human-side comparison
in this project read from that cache. It had been collected **while the six
oral stops were emitting silence**. eSpeak produced nothing where a stop belonged,
so the alignment there mapped onto whatever was adjacent:

| | stale cache | fresh | error |
|---|---|---|---|
| `t` human duration | 56 ms | **106 ms** | −50 ms |
| `k` human level | −12.5 dB | **−7.0 dB** | −5.5 dB |

Everything else moved under 2 dB / 12 ms, so the vowel and non-stop conclusions
stood — but the stop ones did not, and this produced a **phantom defect**: against
the stale cache `t` looked 0.45× the speaker's duration, alarming for the most
frequent consonant in the language. Against a fresh alignment it is **1.03×**.

**The cache carries no record of which build produced it**, and `--report` reuses
it silently. Regenerate it after any change to `phsource/`, and treat any
stop-related figure from a cache older than the last phoneme edit as unusable.

### Stop levels, re-fitted against a correct reference

| | before | after |
|---|---|---|
| `k` | −6.8 dB vs speaker | **+0.5** |
| `t` | −4.6 dB | **+0.1** |
| `q` | +5.1 dB | **−0.7** |

`k` 100 → 219 (pause branch 85 → 186), `t` 100 → 170, `q` 213 → 118 / 170 → 94.
Clipping was checked first and is not a risk: `k`'s burst peaks at 0.259 against
an utterance peak of 0.820, and reaches 0.567 after the raise. Vowels set the
peak, not stops.

### mn-f: the voice most clients actually get, and it was never tested

Two independent defects, both invisible to every test in the tree.

**1. `mn-f` outranked `mn`.** Its header said `language mn 4`; `mn` has no number
and so defaults to 5. **Lower priority wins.** Measured through the API:

```
espeak_SetVoiceByProperties("mn")        -> Ankhmaa   xgn/mn-f
espeak_SetVoiceByProperties("mn", MALE)  -> Ankhmaa   xgn/mn-f
espeak_SetVoiceByName("mn")              -> Mongolian xgn/mn
```

Only the by-name path reached the male voice — and that is the CLI's `-v mn`,
which is what every test and every acoustic measurement in this project uses. So
Android TTS, NVDA, speech-dispatcher and the language bindings were all getting a
voice that had never been fitted to anything. Now `language mn 6`.

A language file also cannot declare `gender` (voices.c rejects it), which is why
even an explicit FEMALE request matched mn-f.

**2. `mn-f` never set `consonants`.** A language file inherits nothing from
another language file, so the `consonants 115` fix on `mn` never reached it and it
ran on espeak's default of 90. Measured against `mn` over the same sentences,
**all 14** unvoiced consonants sat 4.11 dB down (p t k th q s S x X ts tsh tS tSh
l#) while voiced segments stayed within 1.5 dB — precisely the band carrying the
s/ʃ/ts/tʃ/x place cues. Swept: 130 → −0.98 dB, 145 → −0.01, 160 → +0.85.
Now `consonants 145`.

### The uvulars fronted the vowels they exist to back

`X` and `q` are the back-harmony allophones of х and г. Their `Vowelin` locus sits
*above* the back vowels they precede (1500 and 1300, against о 1046 and у 843), so
the transition pulled the vowel onset **up**. Measured against the speaker:

| | eSpeak | speaker |
|---|---|---|
| X before о | **+165** | **−76** |
| X before у | **+197** | **−225** |
| q before о | **+267** | **−72** |

He lowers F2 after a uvular; eSpeak raised it. (This also corroborates the −81 Hz
figure §6 cites, measured independently here as −76.)

Lowering the locus does **not** fix it — `AdjustFormants` halves the difference, so
a locus change moves every vowel together, and 1500 is already near-optimal for а,
which is the most frequent back vowel and currently correct. The fix is the
**clamp**: `Vowelin`/`Vowelout` max 300 → **0**, so a uvular may lower a following
vowel's F2 but never raise it. Result for X: before о +165 → **+5**, before у
+197 → **−3**, before а unchanged at +3.

> Measurement limitation worth recording: this method cannot isolate the
> transition for a STOP. The first 30 ms after a stop's boundary contains its
> burst, not the vowel, so `q`, `t`, `th`, `k` and `p` all read a large positive
> "shift" that is really burst noise. `X` responds because it is a fricative.
> `q`'s clamp is correct by construction but was not verifiable acoustically.

### Cleared this round — measured, not defects

- **Durations.** Every phoneme within 35% of the speaker; utterance rate 0.94×.
- **Aspiration timing.** Aspirated stops run 1.23–1.92× longer than unaspirated in
  every pair, refuting the "VOT is rendered as zero" claim.
- **Vowel amplitude envelopes and bandwidths.** All 21 spect files still share one
  byte-identical envelope and one width set; the frequency writers did not disturb them.
- **F3/F4 short↔long consistency.** Six of seven pairs share F3 exactly.
- **mn-f formant scaling.** F2 ratio a consistent 1.15–1.17, and every vowel pair
  is *better* separated than in `mn` because scaling expands the space.
- **Punctuation.** Correctly graded: paragraph 740 ms > full stop 440 > semicolon
  340 > colon 300 > comma 240 > dash 190.
- **Consonant clusters.** Across 1183 two-, 174 three- and 15 four-consonant
  clusters, no member is under 20 ms or below −30 dB.
- **Stress placement.** Initial in all 27 word shapes tested.

---

## 10f. Numerals read from digits, and the gates that now guard all of this

### The digit forms disagreed with the spelled-out words in three ways

All 25 compound and tens entries in `mn_list` disagreed with what `mn_rules`
produces for the same word written out. `11` came out `arw@nn'ek` where
`арван нэг` gives `'arwa#N n'ek`:

- **stress** was on the SECOND element, so the compound landed on syllable 3 —
  Khalkha stress is fixed initial
- **`@`** stood where the rules give a harmony-preserving reduced vowel, which is
  exactly what G3 in `mn_rules` says not to do
- **word-final `n`** where Khalkha has velar `N`

The replacements were generated from the rules rather than written by hand: the
spelled-out words are handled correctly, so they are the reference.

**A constraint found while fixing it:** a space *inside* one `mn_list` numeral
entry truncates the output — `_11 'arwa#N n'ek` speaks only "арван". The number
engine composes multiple words across separate entries (`101` → `ts'U:N n'ek`)
but not within one. So the compounds have to stay single words, which costs the
second element its own stress.

**And a genuine trade-off, recorded so it is not "fixed" back:** giving the base
numerals `_0`.. `_10` explicit stress marks fixes them in composition (`1000` →
`n'ek mj'aNq@`) but the marks then leak into tens+unit compounds, so `47` becomes
`t'8tShi#Nt'OlO:` with two stresses. Measured both ways:

| | off syllable 1 | unstressed |
|---|---|---|
| original | 7 | 0 |
| base numerals stressed | 8 | 0 |
| **base numerals unstressed (shipped)** | **0** | **4** |

An entry's stress mark applies whether it is used standalone or concatenated, so
both cannot be had. Actively-wrong stress is worse than flat stress, so the base
numerals keep their vowel and nasal corrections but no stress mark.

### The gates are tests now, not a script

The gates were moved into the toolkit's test suite. They had existed only as a
scratchpad script,
which is how one went stale unnoticed: it still demanded that no reduced vowel
point at a full-vowel file, long after that was deliberately reverted. A gate
outside the suite cannot fail loudly, so it drifts.

Each encodes a defect that actually shipped: contrast collapse, missing
diphthong offglides, silent phonemes, `formant_factor` drift, and uvulars that
raise a following vowel. **They were mutation-tested** — merging у/ү, flattening
эй's glide, un-clamping a uvular and setting `pitch 55` each fail the matching
gate and nothing else.

### Cached measurements are stamped with the build that produced them

Each cache was stamped with a build fingerprint. A cache is only valid for the
build that made it, because the DTW alignment follows eSpeak's output — and one
was once reused across a build in which every oral stop was
silent, putting the stored human duration for `t` out by 50 ms and the level for
`k` out by 5.5 dB. Nothing recorded provenance and nothing complained.

The stamp covers `phontab`, `phondata`, `phonindex` **and `mn_dict`**. The
dictionary is in that list because a first version omitted it and a set of
`mn_list` numeral edits then sailed past the guard: lexicon changes never reach
`phondata`, but they certainly change what eSpeak emits. An unstamped or
mismatched cache is refused, not silently used.

---

## 10g. Wrong words: five rule defects and a regression I caused

The rounds above are about acoustics. This one is about the layer above them,
where a word comes out as the wrong phonemes entirely while every acoustic gate
stays green. All five were reproduced directly before being touched.

### The double [j] was a fix breaking another fix

`өгье` → `'8kjje#`, `ахье` → `'aXjje#`. The controls were clean — `өтье`
→ `'8thje#`, `асье` → `'asje#` — so only г and х doubled the glide.

The cause was the previous round's own work. Separating `дах` from `дахь`
needed a harmony-aware `г (ь` → `qj`/`kj` ladder, and those rules carry two to
four context items. The pre-existing one-item `г (ьL08 → k` guard, which exists
precisely to suppress the glide before an already-iotated vowel, was outranked
at every depth and never fired: the ladder emitted `kj` and then `е` added its
own `j`.

The guard now has the same ladder at each gap depth, so it wins wherever the
following vowel is iotated. `дах`/`дахь` stay distinct, which is the contrast
the ladder was added for.

### л was voiceless in fully voiced environments, twice

`алье` → `'al#je#` against `алаа` → `'ala:`. A vowel before and a /j/ after is
as voiced as an environment gets, and the same rule group already documents
`L04) л (L04 → l`.

The **geminate** case is the same gap seen from the other side:
in `байгууллага` and `долларын` both halves of the doubled л fell through to
the voiceless `l#`. The intervocalic rule needs a vowel on *both* sides, and a doubled letter puts a
consonant on one, so both halves fell through to the voiceless `l#`. Khalkha л
is /ɮ/ and devoices only beside a voiceless segment or at a word edge; a
geminate between two vowels has neither. Both now take `l`, while `алт` and
`сурталт` keep `l#` before т.

This was filed as "geminates render as two articulations". That part is
**refuted**: [phonemelist.c:382](../../../src/libespeak-ng/phonemelist.c#L382)
implements `X:` for fricatives, nasals and liquids *by inserting a duplicate
phoneme*, so `l:` compiles to exactly the `l#l#` already there. A duplicated
phoneme is eSpeak's canonical geminate. Only the voicing was wrong.

### ъ dropped the glide it exists to mark

`объект` → `'Ope#khth`, while `объём` → `'OpjO#m` and `адъютант` →
`'atjU#tha#nth` kept theirs. ё and ю were handled and е was not.

### One word read as two

`МОНГОЛын` → `m'ONqO# l#'iN` — two separately-stressed words with a mangled
suffix. `ТӨРийн` and `НҮБын` the same.

[translate.c:1445](../../../src/libespeak-ng/translate.c#L1445) starts a new
word at the last capital when an upper-case run is followed by lower case with
three or more letters. That is right for `McDonald` and wrong for Mongolian,
where an abbreviation or proper noun takes its case suffix in lowercase written
solid. New language option `caps_keep_suffix`, set only for `mn`; English,
Dutch and German CamelCase were re-checked and are unchanged.

### A name initial was a bare vowelless consonant

`Д. Нацагдорж` opened on a lone `t`, which is not audible as anything.

The plan recorded that the letter-name path "exists and works, it only needs
extending", because `Ч. Лодойдамба` gave `tSh'i`. That was **coincidence**:
`ч` is also the clitic *too*, listed in `mn_list` as `tShi $u`, and the real
letter name is `tShe`. There was no working path. A single letter spells itself
only when the letter-to-phoneme rules produce *nothing*
([translateword.c:446](../../../src/libespeak-ng/translateword.c#L446)), and
every Cyrillic letter produces something.

All 35 letters now carry `$hasdot $capital`, which fire only on an upper-case
letter followed by a dot — a name initial and nothing else. Running text is
untouched: `Би ч мэднэ` still gives the clitic `tShi`.

### The uvulars were lowering F1, against the harmony cue

`Vowelin`'s `f1=` is a **mode**, not a frequency: `AdjustFormants` reads 1 as
"lower the following vowel's F1 by 60–100 Hz" and 2 as "lower it by 150–300 and
drag peak 0 down too". Mongolian had `q`=1, `Q"`=2, `X`=0 — three settings
inside one series, while every velar was 0.

Uvulars are the back-harmony allophones and Khalkha marks RTR with a **higher**
F1, so lowering it pushed a back vowel toward its front partner exactly where
the listener reads the harmony class. Measured, F1 onset minus steady:

| | eSpeak before | eSpeak after | speaker | n |
|---|---|---|---|---|
| `q` before а | −49 | **+29** | +5 | 80 |

All three uvulars are `f1=0` now. The nasals `N`/`m`/`n` keep `f1=2`, which is
correct — a nasal really does lower F1 — so the gate names the uvulars rather
than sweeping the file.

### Peak 0 had been left above F1

Peak 0 is the sub-F1 shelf. It carries real amplitude — 3392 against F1's 7424
at the steady frame — and sits at a near-constant ~240 Hz in the template every
file inherits. Three vowels had their F1 fitted *below* that during the vowel
work and nobody re-fitted the shelf: и (235 = 235), и: (243 > 209), ү: (259 >
227). `f1_adj == 2` moves peak 0 with F1 precisely because the two are meant to
track. All three now sit 41 Hz below F1, the margin `e` has.

### Cleared this round — measured, not defects

- **`p` is not 10 dB quiet.** It reads −11.63 against the speaker's −1.94, the
  only phoneme outside ±6 dB. But the speaker's `p` spans −22.5…+4.5 (p10–p90)
  on n=34 with an IQR of 12.3, so the median sits high by sampling accident.
  Its aspirated partner `ph` — same articulator — agrees within 1.7 dB, and
  eSpeak's `p` is consistent with its own stop series.
- **`_` and `_:` are not 29–124 dB quiet.** eSpeak emits true digital silence
  where the speaker's pauses carry room noise. Comparing a synthesised pause to
  a recorded one measures the recording.

Every other phoneme now sits within ±5 dB of the speaker on relative level and
within 0.78–1.29× on duration, over 18,065 aligned phones.

### Gates

A 30-case suite guarded the symbolic layer, which
had no gates at all — the double [j] shows a fix in one direction silently
breaking another, which is exactly the class that needs holding. Two acoustic
gates were added for the uvular F1 mode and for peak 0.

---

## 10h. Reduction is an F1 change, and two findings that dissolved on inspection

This round audited every dimension that can be measured against the corpus:
vowel formants, reduced vowels, diphthong trajectories, consonant place cues,
consonant levels, durations, the phonemic length contrast, stress placement,
cluster generation, F0 level/range/shape/continuity, declination, spectral tilt,
and within-phoneme dynamics. One defect was real. Two looked large and were
artifacts of how they were measured.

### The reduced vowels had no reduced quality

Every `X#` pointed at its full counterpart's spect file, on the model that
reduction in this voice is a length change alone. The speaker says otherwise:
he lowers F1 on every reduced vowel, by an amount that scales with how high the
full vowel sits.

| | full F1 | reduced F1 | speaker lowers by | eSpeak lowered by |
|---|---|---|---|---|
| а | 536 | 332 | **−204** | 0 |
| о | 378 | 307 | −71 | 0 |
| ө | 312 | 277 | −35 | 0 |
| э | 310 | 284 | −26 | 0 |
| и | 277 | 279 | 0 | 0 |
| у | 352 | 339 | −13 | 0 |

`a#` therefore rendered **+201 Hz** off — the largest formant error in the voice.
Every unstressed а came out at full stressed quality, which flattens the
initial-syllable prominence that marks word boundaries in Khalkha. Reduced
vowels are ~28% of vowel tokens.

`a#`, `e#`, `O#` and `8#` now have their own files. `i#` and `U#` keep pointing
at their full ones, because a file that encodes a 0 Hz difference is a
difference with no measurement behind it.

**Why this is not the attempt that was reverted.** That one centralized F2 as
well, and F2 is where the harmony class lives — it collapsed у/ү to dF2 +32
against +192. Here F2, F3 and every amplitude are byte-identical to the full
vowel, and у/ү are not touched at all. The pairs also move *together*: fitting
`O#` alone to its own target inverts о/ө to −21 Hz, which is the same
per-phoneme-target failure as the у/ү collapse. Result:

| | before | after | speaker |
|---|---|---|---|
| a# F1 | 533 | **332** | 314 |
| O# F1 | 396 | 336 | 309 |
| a#−e# F1 | +215 | **+38** | +30 |
| O#−8# F1 | +68 | +45 | +39 |
| a#−e# F2 | −465 | −451 | −516 |

Four gates replace the old one, each mutation-tested: F2-and-amplitudes
identical to the full vowel, F1 never rising above it, and the reduced-pair F1
contrasts keeping the speaker's direction within 0.4–2.5×.

### Creak: why two large prosody findings were not real

Measured over 120 utterances, eSpeak looked badly wrong on two counts: its
utterance onset sat **+2.03 st** above its own median where the speaker's sat
**−3.33**, and its declination was **−0.72 st/s** against his **−0.10**. Both
survived a second, independent statistic. A tune rewrite was drafted.

Neither is real. **50% of the speaker's onset frames sit below 0.80× his
median** — creak, not pitch. Drop the sub-modal frames and his onset is
**+4.10 st**, *higher* than eSpeak's. Drop them across the whole utterance and
declination is eSpeak −0.16 against his −0.06, with every decile of the contour
matching within 1.6 st.

The tune comment already said the contour shape was right. It was.

The general form: **eSpeak has no creak, so any F0 statistic computed over raw
voiced frames compares two different things.** The speaker is creaky on 29% of
frames against eSpeak's 15%, concentrated at phrase edges — exactly where onset
and declination are measured. Every F0 comparison needs sub-modal frames
excluded on both sides.

An empirical check confirmed there was nothing to fix even if there had been:
sweeping `head`'s start pitch (98 → 55 → 40 → 25) and adding an `onset`
statement moved the *relative* onset by at most 0.5 st, because lowering the
head lowers the median with it — while costing 5–13 Hz of median pitch.

### `нь` is loud, but not by a lever that exists

`n^` measures +4.5 dB against the speaker, and `нь` is the single most frequent
word in the corpus (2.85% of tokens). Lowering both `FMT(n^/…, 240)` branches by
4 dB changes the output by **0.03 dB** — neither fires for a standalone `нь`,
which takes the `VowelStart` path shared with `нь` inside words, where no error
was measured. eSpeak's +0.3 dB also sits inside the speaker's own p25–p75
(−14.6 … +1.3). Left alone.

### Cleared this round — measured, not defects

- **The у/ү contrast is not collapsed.** It measured at ratio 0.50 (error
  −347 Hz) — the exact signature of the defect that shipped before — and was an
  artifact of the formant filter. Short back vowels merge F1 with F2, so the
  tracker relabels upward and reports the true F3 as F2, which *passes* an
  "F3 in 2000–4000" gate because the true F4 lands there. Gating F2 on a
  plausible male true-F3 band (2200–3200), a criterion independent of F2 itself,
  gives error **−13 Hz** and ratio **1.38**. Validation that the right tokens
  were dropped: ү's IQR fell 1174 → 786 while the already-clean long vowels
  barely moved. **Every formant claim about о/ө/у/ү needs this filter.**
- **Diphthongs are not stepped.** The spect files show three flat frames then a
  jump to 2150, which reads as a step. eSpeak interpolates between keyframes:
  measured in the audio, all six glide continuously, and the three control
  monophthongs come out flat (span −20, −64, +8), which is what proves the
  method rather than the assumption. All six do overshoot the offglide by
  ~270 Hz (2150 against the speaker's 1870–2030); overshooting toward /i/ makes
  the offglide more distinct, so under clarity-over-naturalness it stays.
- **Voice quality is correct.** Spectral tilt −6.00 dB/octave against −6.30.
- **Consonant place cues are correct.** F2 at vowel onset over 76
  consonant–vowel pairs: nearly all within ±150 Hz. Four exceed 250 Hz, all on
  ө/ү — the speaker's widest-spread vowels — on n=13–34, each contradicting its
  own siblings (`th+y` −528 against `t+y` −130; `x+8` +427 against `x+y` +2).
- **Stress is correct.** Over 2182 corpus words: 0 with stress off syllable 1,
  0 with spurious secondary stress.
- **Cluster generation is correct.** One vowelless word — `нь`, correct by
  design — and the longest runs are genuine Mongolian (`навчсыг`, `малчдын`).
- **The length contrast is not weak.** 2.02–2.24 against the speaker's
  1.68–1.97: over-separated, which helps clarity.
- **Flat dynamics are not a clarity defect.** Within-phoneme level IQR ~2 dB
  against the speaker's 16.
- **The `$u` function words are correct.** `ба/юм/бэ/вэ/мөн` render with a
  reduced vowel under a stress mark; that is how eSpeak writes a reduced
  monosyllable, and reduction is right for clitics.

---

## 10i. р was 10 dB too bright, and a harness bug that faked three results

This round audited what had never been measured: spectral centroid per phoneme,
sonorant spectral balance, formant bandwidths, amplitude envelope dynamics,
pause structure, question intonation, and every Mongolian amplitude parameter
against the same parameter in all other languages.

### The parameter-field scan

Comparing each `FMT`/`addWav`/`WAV` amplitude Mongolian sets against every other
language's use of the same source file found seven above the field maximum. Most
are harmless — a bare `WAV()` is the whole source, so raising it only moves
level, and consonant-to-vowel balance measures **−0.25 dB** against the speaker.

`addWav` is different. It adds noise on top of an `FMT`, so raising it alone
skews the **noise-to-formant ratio** rather than the level. Mongolian has only
three phonemes with a mixed source, and one of them carried `addWav` at 255:

| language | `r3/r_trill.wav` |
|---|---|
| Finnish | 50 |
| Belarusian, Catalan | 65 |
| Afrikaans | 100 |
| **Mongolian** | **255** |

It was raised to fix a level deficit of −6.3 dB. That measurement predates
`consonants 115` in the voice file, which then supplied the same level globally —
and the per-phoneme compensation was never rolled back. Level stayed correct, so
nothing caught it. The spectrum did not: measured against the speaker on the
middle 50% of each token, normalised to that utterance's own vowels so the
recording chain cancels, р read **+14.7 dB HF/LF against his +4.5**.

### The fix: raise the formant, not the noise

Removing the addWav fixed brightness (+4.2 dB) but cost 3.8 dB of level — a
straight trade, with no amplitude in between landing both:

| | level err | HF/LF | centroid |
|---|---|---|---|
| addWav 255 | −0.83 | +14.7 | 1153 |
| addWav 120 | −2.70 | — | — |
| addWav 0 | −4.63 | +4.2 | 565 |

Buying level with broadband noise was the wrong instrument. `FMT(…, 255)` was
already at the parameter ceiling, but the **spect file** had 9.9× of headroom, so
`phsource/mongolian/r_trill` and `r_` are local copies with every peak height
scaled 1.428× and frequencies untouched:

| | level err | HF/LF | centroid |
|---|---|---|---|
| **FMT ×1.428, no addWav** | **−0.66** | **+6.2** | 682 |
| speaker | — | +4.5 | 310 |

Level is better than it ever was and the brightness error falls from 10.2 dB to
1.7. Two gates follow: no Mongolian `addWav` above the field maximum without a
recorded justification (`ufric/xx` on `Q"` has one — it measures 3.8 dB *too
quiet* with both parameters at ceiling), and р specifically must have no addWav.

### The harness bug — three results were fake

`phtime.py` holds the shared library as a module-level singleton, and
`espeak_Initialize` reads `phondata` **once per process** with no way to reload.
A sweep that rebuilds data between variants and measures in the same process
therefore measures the **first** build every time, silently, with plausible
numbers.

It produced this: removing р's addWav entirely, swapping the waveform, and
dropping `FMT` from 255 to 40 all changed the output by under 0.1 dB. The
conclusion drawn was "the phoneme block is not being used" — the same shape as
the real `ipa`-after-`import_phoneme` defect, which is what made it credible.
Measured one variant per process, the same sweep spans 10 dB.

It also invalidated the previous round's `n^` test, which reported that neither
`FMT(n^/…)` branch fires. That conclusion should not be relied on.

`phtime.init()` now fingerprints the four compiled artefacts and raises
`StaleLibrary` when they change underneath a live process, telling the caller to
use a fresh one. Three tests cover it.

### Cleared this round — measured, not defects

- **Formant bandwidths are not too narrow.** eSpeak's B2 measured 31 Hz against
  the speaker's 221 and B3 42 against 352 — an 86–88% deficit, and exactly the
  sharp-resonance quality that reads as metallic. Measuring other eSpeak voices
  identically gives English B2 **36**, German **33**, Turkish **36**, Russian
  **62**, Mongolian **48**. Mongolian is mid-field; narrow bandwidths belong to
  the formant engine, and LPC inflates the estimate on natural speech besides.
- **The large centroid errors on voiceless segments are the silence artifact.**
  k +108%, t +80%, th +67%. eSpeak's closures are digital silence and its
  fricatives carry almost no low-frequency energy where the speaker has a voicing
  bar and room noise. On the noise itself: **s 0%**, S −7%, X +6%, tS +9%.
- **eSpeak does not fragment its pauses.** It showed 12.8 internal silences per
  utterance at 50 ms against the speaker's 7.6 at 120 ms — but attributing each
  to a phoneme put most of them *inside vowels*, which is impossible. It was
  peak-normalisation: against the utterance's 99th percentile eSpeak has **1.4%**
  of frames below −35 dB and the speaker **23.3%**.
- **Compressed dynamics are not a clarity defect.** Frame-energy range 12.4 dB
  against the speaker's 40.7.
- **Statement-final intonation is exact.** −0.98 st against −0.95, modal frames.

### Could not be measured

- **Question intonation (`mn_q`)** — 3 of 150 corpus utterances are questions.
  Unmeasured, not verified.
- **Nasal antiformants** — `n` +5.8 dB and `n^` +8.0 brighter than the speaker
  while `m` +1.0 and `N` −1.0 are clean, consistent with the alveolar
  antiformant falling in the measured band and pole-only synthesis being unable
  to make a zero. But the band-profile method carries ±12 dB noise on its own
  vowel control, and `phsource/n/` is shared with every language.

---

## 11. Working on Mongolian

```sh
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cmake --build build --target data     # needed after any dictsource/ or phsource/ change

ESPEAK_DATA_PATH=$(pwd)/build build/src/espeak-ng -xq -v mn "Улаанбаатар хотод оюутан байна"
ESPEAK_DATA_PATH=$(pwd)/build build/src/espeak-ng -X  -v mn "хотод"   # which rule fired

ctest --test-dir build -R language-pronunciation --output-on-failure
ctest --test-dir build -R language-phonemes --output-on-failure
```

Changing `ph_mongolian` changes the `test_phwav mn` and `test_phwav mn-f`
hashes in `tests/language-phonemes.test`. That is expected; regenerate them from
the reported actual values rather than suppressing the failure.

`tests/language-pronunciation.test` carries one `test_phon mn` case per
phenomenon described above, so a regression names its own cause rather than
just reporting a changed string.

Two traps specific to these test files:

- **Test text is passed as `argv`.** A text beginning with `-` is parsed as an
  option — `test_phon mn "..." "-5"` failed with `invalid option -- '5'`. The
  helpers in `tests/common` pass `--` before the text to end option parsing;
  keep that if you add a helper.
- **Non-ASCII argv is mangled on Windows.** Verify with
  `-f <utf8 file>` and strip `
` rather than passing the text as an argument.
  Note this means a local `-f` harness cannot reproduce the `argv` trap above;
  only the real shell tests catch it.

### The whole-vocabulary sweep

Both of the defects fixed most recently were found the same way, and the
technique is worth repeating after any rule change: run **every distinct word in
the corpus** through eSpeak and flag output that cannot be a well-formed Khalkha
word. Over mbspeech + fleurs that is 11,150 types / 70,674 tokens, and it takes
about a minute if the words are batched.

Batching detail that matters: a bare newline does **not** end an eSpeak clause,
so one word per line comes back as a single line and the run collapses into one
subprocess per word. Put a full stop after each word — verified not to change
the phonemes, since `mn` sets `S_NO_EOC_LENGTHEN`.

The checks that earned their place:

| Check | What it catches |
|---|---|
| `HARMONY` | RTR (`a O U`) and ATR (`e 8 y`) vowels in one word. Khalkha harmony forbids this, so every hit is a loanword, a foreign name, or a rule that failed to propagate. `i` and `@` are transparent and excluded. |
| `SYLLFIN` | Word ends in a syllabic consonant. This is what surfaced the 462-type G5 problem. |
| `CLUSTER` | Five or more consonants with no nucleus between them. |
| `NOVOWEL` / `EMPTY` / `RAWCYR` | No nucleus at all, no output, or a Cyrillic letter surviving into the phoneme string — each means a missing rule. |

`HARMONY` needs reading rather than acting on: the largest groups it returns are
legitimate. The **-гүй** negator and the **-жээ/-чээ** past are invariant
suffixes that genuinely do not harmonise, and biblical proper names
(мосе, иосеф, рахел) are foreign and outside the system. The check is a
filter for human attention, not a list of bugs.

---

## 12. Measuring the vowel space

The retuning in §6 was not done by ear. Two independent measurements agree to
within ~5%, and both are worth re-running after any change to
`phsource/mongolian/`:

**Read the source data.** The `.spect` files are documented by
`src/libespeak-ng/spect.c` (`LoadSpectSeq` / `LoadFrame`). Header, then per
frame: four 10-byte IEEE-754 extended floats, three `int16`, then nine peaks of
six `int16` each. `peaks[1..3].pkfreq` are the F1/F2/F3 targets the synthesiser
drives — `peaks[0]` is a fixed sub-F1 peak at ~240 Hz and is not a formant.
Whatever is in those fields is what you will hear; if two vowels are not
separated there, no rule or phoneme change can separate them.

**Measure the output.** LPC analysis (order `fs/1000 + 2`, pre-emphasis 0.97,
25 ms Hamming) on synthesized `[[mV]]` tokens, taking the median over the middle
of the vowel.

That the two agree is the point: the first says what was intended, the second
says what came out.

### Predictions worth testing

Structural, so they need no borrowed reference table — each is a falsifiable
ordering or ratio in the voice's own terms:

| | Prediction |
|---|---|
| P1 | F1(ө) < F1(о) — the rounded ATR contrast, carried by F1 |
| P2 | F1(ү) < F1(у) |
| P3 | F1(а) is the highest F1 in the inventory |
| P4 | F1 orders open > mid > close |
| P5 | long:short duration ≈ 2.1× |
| P6 | the schwa is interior to the vowel space and >100 Hz from every full vowel |

P1 and P2 were originally written as F2(ө) > F2(о) and F2(ү) > F2(у), on the
assumption that harmony was front/back. Measured against two corpora that is the
wrong test — the rounded pairs barely differ in F2 — so they now test F1, which
is where the contrast actually lives (§6).

**Measure P5 on bare vowels, not on `[mV]`.** The nasal onset and the fixed
formant ramps add a constant that does not scale with vowel length, which drags
the apparent ratio down to ~1.74 and looks like a synthesis defect when it is
an artifact of the analysis window.

### ASR round-trip scoring

Synthesizing a corpus and scoring an ASR transcript against the input text does
work as a relative measure, but only with a **human-speech control** to
establish the floor. Whisper large-v3 on 12 real Mongolian clips (FLEURS
`mn_mn`) scores CER 28.8% / WER 82.3%. Two things follow:

* **WER is useless here.** At 82% for real human speech it is saturated, so a
  synthetic arm scoring 100%+ tells you nothing.
* **CER is usable.** A 28.8% floor with synthetic arms in the 45–72% range
  leaves real headroom, and the arms separate cleanly.

Read every synthetic number as a distance above that floor, never as an
absolute. And note the structural bias: the reference is orthographic, so
correct unstable-vowel reduction (§3) makes orthographic recovery harder — an
accurate [xɔtəd] for хотод scores worse than an inaccurate [xɔtɔd]. Human
speech reduces too and still reaches 28.8%, so the bias does not excuse a large
gap, but it does mean a few points are not real.

For the harness: score each arm in its own process. Running all of them in one
interpreter repeatedly died partway through.

### What the round-trip actually showed

Every arm is the same 18-sentence corpus through the same model. Read the
numbers as distance above the 28.8% human floor.

| Arm | CER | vs previous |
|---|---|---|
| HUMAN Mongolian (FLEURS) | 28.8% | — floor |
| pre-overhaul `mn` | 45.2% | — |
| overhaul, original vowel data | 72.0% | +26.8 |
| + retuned vowel formants (§6) | 64.9% | **−7.1** |
| + Mongolian intonation tunes (§7) | 58.5% | **−6.4** |
| + harmony-preserving reduction (§3) | 57.4% | **−1.1** |

Single-variable ablations against the 58.5% build, each reverting one part of
the overhaul:

| Reverting | CER | Verdict |
|---|---|---|
| unstable-vowel deletion | 58.1% | −0.4, no effect |
| lateral fricative → plain [l] | 59.5% | +1.0, ɬ is better |
| aspiration series → voicing | 61.5% | +3.0, aspiration is better |

So the three reforms most likely to be blamed for the regression are each
neutral or beneficial, and the recovered 14.6 points came from the acoustic
data and the prosody, not from the phonology.

**The remaining ~12 points against the pre-overhaul build are not yet
explained.** Four hypotheses have been falsified; the largest untested
difference is the lexicon, since the old `mn_list` carried explicit
spelling-like transcriptions for many high-frequency words that appear in this
corpus, and those were deliberately removed (§10). Anyone continuing this work
should ablate that next.

---

## 13. The female voice (`mn-f`)

`mn-f` scales the male voice's formants by F1 112% / F2 117% / F3 111%. Those
figures sit within the general male-to-female scaling reported in the phonetics
literature, so they are not unreasonable — but they are **not measured from
Mongolian**, and an earlier comment in the voice file claiming they came from
"acoustic analysis of Ankhmaa recordings vs Ganbaa" is unsupported by anything
in this repository. That claim has been removed.

They also could not be verified here, because **every Mongolian corpus
available is male-only**:

| corpus | speakers | median F0 |
|---|---|---|
| mbspeech | one male | **131 Hz** |
| fleurs-mn test (450 clips measured) | all male | ~120 Hz, none above 180 Hz |
| fleurs-mn validation (300 clips measured) | all male | 119 Hz, none above 180 Hz |

An earlier revision of this table gave mbspeech as 102 Hz. That figure is wrong,
not a tracker-settings artifact: re-measured across five floor/ceiling
combinations (50–75 Hz floor, 300–600 Hz ceiling) the median stays within
129–134 Hz. The `mn` voice is tuned to the corrected value.

The `gender` column in fleurs-mn is uniformly 0, which these measurements
identify as male. Note also that the dataset's own `mean_f0_hz` column is
unreliable — it is populated for only 79 of 854 test clips and gives a median
of 146 Hz where direct measurement gives ~120 Hz.

Deriving real ratios requires a Mongolian corpus containing female speech. Until
then the existing values stand as a reasonable generic default, honestly
labelled as such.

The same reasoning governs `mn-f`'s pitch. With no female corpus to fit against,
the median is held at its previously tested value rather than invented:
`pitch 110 215` measures 204 Hz where the old `pitch 140 210` measured 199 Hz,
while widening the range from 3.22 to 4.77 semitones. A prior revision's
`pitch 120 300` measures **287 Hz** — far too high for a female voice; it never
took effect because voice files are staged at configure time (see §7), which is
the only reason it did not ship.

---

## 14. Acceptance: parity with eSpeak English

"Human level in espeak-ng capabilities" is not "indistinguishable from a
person" — eSpeak is a formant synthesiser and that ceiling is architectural. The
meaningful target is the ceiling the architecture actually reaches, and English
is eSpeak's best-developed voice. So the acceptance test is:

> mn's distance to a Mongolian human ≈ en's distance to an English human,
> measured by the identical pipeline.

Both sides are FLEURS test splits, male speakers, 70 clips each:

| | mn | en | gap |
|---|---|---|---|
| **F2 distance** | 390.9 | 385.8 | **+1% — at parity** |
| **F1 distance** | 163.2 | 218.5 | **−25% — Mongolian is closer than English** |
| MCD | 70.43 | 64.48 | +9% |

F2 is at parity and F1 is better than English — expected, since F1/F2 were just
fitted to measured Mongolian and English's never have been. MCD remains 9%
worse; MCD is holistic and includes voice timbre and spectral detail that two
decades of English tuning have shaped and that no measurement here addresses.

So on the vowel-space criteria the target is met.

**These figures predate the `word_gap` and G5 changes and were not re-measured
after them.** Both improved every metric against the mbspeech baseline on the
same instrument — combined, MCD −0.568 (CI [−0.825, −0.312]), F1 distance
−7.23 Hz (CI [−12.09, −3.43]), F2 distance −10.93 Hz (CI [−20.27, −2.10]) —
so the parity table above is, if anything, pessimistic. It should be regenerated
against FLEURS before being quoted as current.

The honest open items:

- **MCD is still ~9% worse than English.** MCD is holistic and includes timbre
  and spectral detail that no measurement here addresses.
- **Gap structure.** eSpeak emits 1.59 gaps per word against the human's 0.67
  and far too few long phrase-final pauses (§7). Fixing it needs phrase-level
  pause placement, not a per-language constant.
- **Enclitics take a word gap** (§10). нь is 3% of tokens and eSpeak's dictionary
  format cannot attach it to the preceding word.
- **-тан/-тэн** now reduces with everything else, on 11 tokens of evidence
  (§3).
- **Long-vowel quality** is still unmeasured (§6), and **`mn-f`** still has no
  female corpus behind it (§13).

---

## 15. The language block in `tr_languages.c`

Every setting in `case L('m', 'n')` and the measurement behind it. The C file
carries a one-line pointer to this section rather than the working, which is why
the working is here.

### `stress_amps` — flat, `{22 × 8}`

Was `{16, 16, 17, 17, 20, 20, 22, 18}`: every unstressed level 2–3 units *below*
eSpeak's default `{18, 18, 20, 20, 20, 22, 22, 20}`.

Mongolian is agglutinative — case, number, tense and possession all live in
unstressed suffixes — so attenuating them attacks the part of the word carrying
the grammar. The speaker's stressed-to-unstressed vowel level gap, on DTW-aligned
corpus tokens, is **−0.2 dB**: he barely marks stress by amplitude at all, where
eSpeak was at −3.0.

| stress_amps | gap |
|---|---|
| `{16,16,17,17,…}` | −3.0 dB |
| eSpeak default | −2.2 |
| `{20,20,21,21,…}` | −1.2 |
| `{21,21,22,22,…}` | −0.8 |
| **flat `{22×8}`** | **−0.5** |
| speaker | −0.2 |

Flattening costs nothing lexically: Khalkha stress is fixed initial and never
contrastive, so no word pair is distinguished by it. Closer to the speaker *and*
better for intelligibility, which is why it goes all the way to flat.

### `stress_lengths` — `{245, 240, 235, 235, 0, 0, 260, 250}`

Was `{190, 180, 220, 220, 0, 0, 260, 240}`. The grammar-bearing unstressed
suffixes were too short to carry it: reduced vowels measured **0.47** of a full
vowel's duration against the **0.667** their own phoneme definitions declare, and
against **0.61** for the speaker.

| unstressed pair | reduced:full |
|---|---|
| 190/180 | 0.47 |
| 220/215 | 0.54 |
| **245/240** | **0.58** |
| speaker | 0.61 |

Slowing here is safe: even after it, eSpeak runs at 0.91× the speaker's duration
— still 10% faster than he speaks.

### `lengthen_tonic = 35`, and `S_NO_EOC_LENGTHEN` removed

Both settings suppressed final lengthening, which the speaker uses heavily: his
clause-final vowel is **1.60×** a non-final one, where eSpeak was at 1.04 —
essentially no phrase-boundary cue at all. `S_NO_EOC_LENGTHEN` gates the
lengthen-last-syllable branch in `setlengths.c` outright; `lengthen_tonic` adds
the constant that keeps the tonic syllable from shrinking with speed.

| | final:non-final |
|---|---|
| flag set, tonic 0 | 1.04 |
| flag removed, tonic 0 | 1.20 |
| flag removed, tonic 20 (default) | 1.42 |
| **flag removed, tonic 35** | **1.53** |
| speaker | 1.60 |

35 is above the default and no other language sets it. It is the value the
measurement points at, and final lengthening is one of the strongest
phrase-boundary cues a listener has.

### `word_gap = 0x20` — juncture, no inserted pause

`0x20` makes a word-final vowel's length treat the boundary as "a pause plus the
next word's first phoneme" (`setlengths.c`), which keeps clusters from running
together across words. The low bits are deliberately 0 so `phonemelist.c` inserts
no silence.

This replaced `word_gap = 2`, which a native speaker heard as not smooth. That
value had been chosen by matching the **mean** gap per word (98.5 ms against 95.0)
— and the mean is not what matters, the distribution is. Over 40 utterances,
`word_gap = 2` produced 12.5 internal pauses per utterance at a 50 ms median
(20.4% of the signal silent) where the speaker produces 4.0 at a 125 ms median
(13.6%). He concentrates silence into a few syntactic pauses; eSpeak was
spreading the same total evenly between every pair of words, which is the
acoustic definition of staccato. Dropping the inserted pause takes eSpeak to
16.0% silent.

### `max_initial_consonants = 4`

Unstable-vowel deletion (§3) removes non-initial short vowels, so words surface
with clusters the spelling does not show: ажилтан is [atʃiɬtʰəŋ], оюутан
[ɔjʊːtʰŋ̩]. Four lets the syllabifier keep those together instead of splitting a
cluster across a boundary with no vowel to anchor it.

### `caps_keep_suffix = true`

An abbreviation or proper noun takes its case suffix in lowercase, written solid:
МОНГОЛын, НҮБын, ТӨРийн. eSpeak's CamelCase splitter starts a new word at the
last capital, which turned each into two separately-stressed words with a mangled
suffix (МОНГОЛын → `m'ONqO# l#'iN`). Cyrillic has no CamelCase convention to
lose. See §10g.

### `question_particles`

Yes/no questions are marked by a clause-final clitic, not by punctuation: "Та
Монгол хүн үү" is a question and is normally written without "?". Wh-questions
use вэ/бэ after an interrogative word and are listed too, since they also close a
question clause. See §7.

### The rest

`stress_rule = STRESSPOSN_1L` (fixed initial stress, §8); `stress_flags =
S_NO_AUTO_2` (no automatic secondary stress — with an accent on nearly every
word it reads as sing-song); `LOPT_SUFFIX = 1`; `vowel_pause = 1`; `numbers =
NUM_OMIT_1_HUNDRED | NUM_DFRACTION_6`; `SetLengthMods(tr, 3)`.

---

## 16. Records moved out of the phoneme table

`phsource/ph_mongolian` had grown to 45% comment, with four byte-identical
18-line blocks and a seven-times-repeated block that had gone stale — it still
asserted "reduction is a LENGTH change only, this points at the FULL vowel's
spect file" after §10h changed exactly that. The operative rules stayed in the
file; the experimental records are here.

### What was wrong with the diphthongs before they were rebuilt

Four of the six had no offglide at all. Measured F2 change across the frames was
ай +128, ой +240, өй +95, and эй **−273** — эй's offglide target sat *below* its
nucleus, gliding away from и. They had been built as "monophthong plus one small
step". Consequences: эй measured 24 from э and 26 from ээ, өй 38 from ө. ай, эй
and ой were already like this at git HEAD; өй was introduced by this project. In
all six, F1 *rose* through the offglide (ай 428→519) when the target is и, whose
F1 is 235.

The nuclei had also drifted: үй sat at F2 1300 against ү's 1035, and уй at 1000
against у's 843, so уй and үй started from nearly the same place.

Result against the git baseline, as trajectory distance (higher is more
distinct): pairwise minimum 54 → 62, diphthong-vs-monophthong minimum 37 → 40,
with every diphthong improved except эй. эй stays weakest at 40 and that is
inherent — its nucleus *is* э, so only the offglide and the length (220 ms
against 150) separate them. Distorting the nucleus to gain distance would make
эй wrong.

Converging both offglide F1s to 240 was tried and rejected: it erased the у/ү
distinction across half the diphthong's duration and measured уй/үй distance 41,
*below* the baseline's 54. Coarticulated F1 puts it at 62.

### The silent stop series, measured

Before the fix, relative to utterance level:

| | level |
|---|---|
| `t` | **−162.1 dB** |
| `th` | −116.4 dB |
| `k` | −112.9 dB |
| every other consonant | −0.8 to −11 dB |

That is digital silence, not weakness — б п д т г к, the entire oral stop series.
Removing `ipa t` alone restored `t` to −13.7 dB: one variable, decisive.

Three deliberate changes were made when the six were re-declared in full:
`voicingswitch` omitted (the base tables switch to b/d/g, which do not exist in
this table — Khalkha contrasts aspiration, not voicing, and `ImportPhoneme` was
discarding it anyway); `k`'s burst raised from 60 to 100 (it was "weaker" in the
base table while Mongolian's own uvular `q` sat at 213, so within one language
the uvular г was 3.5× louder than the velar к); and the aspirated series raised
to 120, above the unaspirated — it had been *quieter* (th 90 against t 100),
inverting the only cue that separates them in a language with no voicing
contrast.

### The uvular г, measured

Word-initial г retracts the following vowel's F2 onset by only about 70 Hz more
in RTR words than ATR (onset-minus-mid F2: −97 RTR against −27 ATR, mbspeech).
That is a modest retraction, not the hard Arabic qaf the phoneme had been built
from: `WAV(ustop/q)` is far too strong for Khalkha and made газар sound like
[qaʦar]. It is now the unaspirated velar burst with an F2 locus lowered to ~1300,
matching the measured onset of 1262 Hz.

The `ipa` label was also wrong — the phoneme is declared `vls` but was labelled
ɢ, which is voiced. Khalkha г is unaspirated, measured VOT 20–25 ms
word-initially: short-lag, i.e. phonetically voiceless unaspirated. Mongolianist
transcription writes ɢ; the label in the file reflects what is synthesised.

---

## 17. Final round: text handling, and the toolkit is gone

The acoustic work never touched number and date formats, punctuation, mixed
script or abbreviations. This round audited those, reviewed the comments the
previous rounds left behind, and removed the measurement toolkit.

### Fixed

**The `N-р` ordinal** — see §10a. Documented here as unfixable; it was a
dictionary addition. All twelve months now read correctly.

### A finding that was wrong

The audit flagged `АНУ` → "ану" and `НҮБ` → "нүб" as inconsistent, because 12 of
14 all-caps abbreviations spell by default (`УБ` → "у бэ", `ЭМЯ` → "э эм я",
`БНХАУ`, `ГХЯ`) and only those two have word-reading overrides. eSpeak's idiom
for an initialism elsewhere is `$abbrev $allcaps`, which spells it.

**It is not an inconsistency.** `mn_list` states the policy directly above the
block: *"Where an initialism has a settled word-like reading (АНУ [anʊ], МУИС
[mʊis]) that is given; otherwise the letter names are strung together the way a
speaker would say them."* A native-speaker editorial decision, naming АНУ by
hand. Left alone.

### Clean-code: the comments had outgrown the code

| file | before | after |
|---|---|---|
| `tr_languages.c` Mongolian block | 117 lines, **72%** comment | 60 lines |
| `phsource/ph_mongolian` | 1166 lines, **45%** comment | 942 lines, 32% |

For scale: the median comment share across the other 66 language blocks in
`tr_languages.c` is **0%**, and the next highest is 45% on an 11-line block.

Two blocks were duplicated verbatim — the reduced-vowel rationale four times (72
lines) and a "reduction is a LENGTH change only" block **seven** times (63
lines). The latter had also gone stale: it asserted that each `X#` points at the
full vowel's spect file, which §10h had changed. A comment repeated seven times
is a comment nobody can keep true.

The rule applied: the operative *why* stays at the decision point; the
experimental record moves to §15 and §16. Every step was verified by the mn and
mn-f wav hashes staying **unchanged**, which is what proves a comment move
changed no behaviour.

### Found and recorded, not fixed

- **Fractions.** `1/2` → "нэг зураас хоёр" (one dash two).
- **Prefix currency.** `$100` → "доллар зуун". `100$` → "зуун доллар" is right,
  so only the prefix order is wrong.
- **Roman numerals.** `XX зуун` → "экс экс зуун"; Mongolian writes centuries
  this way.
- **Numeral + case suffix.** `20-нд` → "хорин эн дэ". Same family as the
  ordinal, but `_#` forces `ordinal = 2`, so it cannot carry a plain case
  suffix.
- **Dotted dates.** `2026.05.20` → "…цэг тэг тав цэг хоёр тэг".
- **Attributive numerals.** `25000 төгрөг` → "хорин тав мянга төгрөг" rather
  than "хорин таван мянган төгрөг".
- **`100-р` / `1000-р`** leave the suffix as a separate token (§10a).

Verified correct, for the record: `-5` → "хасах тав", `50%` → "тавин хувь",
`№5` → "дугаар тав", `5кг` → "тав килограм", `3.14` → "гурав цэг нэг дөрөв",
`email@` → "ат тэмдэг", and stress placement over 2182 corpus words (0 words
stressed off syllable 1, 0 spurious secondary).

### The measurement toolkit is gone

`tools/mnlab` — 368 MB, 78 files, 148 tests — has been removed. It was untracked
and never meant to ship. What it did:

- drove the shared library for exact per-phoneme timings (`phtime.py`), read and
  patched the binary SPECTSQ2 vowel files (`spectio.py`), and DTW-aligned eSpeak
  against a 1,923-utterance single-male-speaker corpus (`sweepall.py`,
  `vox_min.py`, `human.py`);
- stamped every cached measurement with the build that produced it
  (`buildstamp.py`), after a stale cache once produced a phantom defect.

**What is lost.** There is now no way to re-measure this voice against the
corpus. Every acoustic number in §6 and §10a–§10i is a historical record, not
something a future change can re-verify. Any further acoustic work has to
rebuild the toolkit first — and should, because three separate findings in these
rounds were artifacts of measurement rather than defects in the voice (the у/ү
"collapse", the F0 onset and declination, and a stale-library A/B sweep).

**What survives.** The 30 symbolic gates were migrated into
`tests/language-pronunciation.test` as `test_phon mn` cases before the toolkit
was deleted, joining the existing ones for 88 in total. They run under the
project's own suite. The acoustic gates could not migrate — they read binary
spect files through `spectio.py` — so their content is only what is written
down here.

---

## 18. The normalization spec, triaged

A 70-category Mongolian text-normalization specification was checked against
what the engine can actually do. This section records the verdict for each
class so it is answered once rather than re-litigated.

**The spec is mostly not an espeak-ng spec.** eSpeak's text layer does numbers,
symbols and dictionary lookups. It has no URL parser, no date parser, no
transliteration tables and no sentence-boundary model. Those belong in a
normalization frontend that runs *before* eSpeak.

### Done in this round

- **Roman numerals** (§25 of the spec). `XXI зуун` read "экс экс и зуун".
  Mongolian uses them as ordinals — twenty-first century, third chapter — so
  `NUM_ROMAN | NUM_ROMAN_CAPITALS | NUM_ROMAN_ORDINAL` is correct, and they reuse
  the `_1o`…`_90o` entries added for `N-р`.

  Roman numerals reach those entries by a **different route**: `TranslateRoman`
  rebuilds the value as `"21 <roman_suffix>"` with a space, so the suffix arrives
  unhyphenated. `mn_list` therefore carries both `_#-р` (written form) and `_#р`
  (Roman route), and `tr_languages.c` sets `roman_suffix` to Cyrillic р.

  Cyrillic cannot be misread: `TranslateRoman` matches `"ixcmvld"`, Latin only,
  and Cyrillic х (U+0445) is not Latin x (U+0078). `Х`, `ХХ зуун`, `хот`, `их`
  and `ХИЙ` are all covered by tests.

  Known limit, recorded not endorsed: eSpeak refuses a single-letter Roman
  numeral unless a dot follows, so `V бүлэг` stays "вэ бүлэг".

- **`проф.`** now expands to профессор. `$dot` stops the dot being *pronounced*
  but not the clause break it causes — that decision is made in `readclause.c`
  before translation, so a pause remains.

### Tried and reverted — the tokenizer, not the dictionary

`м²`, `м³`, `кВт`, `куб.м` and `°C` were added as `mn_list` entries and **none of
them fired**, because the tokenizer splits them before any lookup happens: `²`
is not a letter, the dot in `куб.м` splits, and the internal capital in `кВт`
triggers the CamelCase split. An all-lowercase `квт` does work, but real text
writes `кВт`.

`д-р` is the same: `-X` shows it translated as two separate words, `д` and `р`,
so no dictionary key can ever match it.

The entries were removed rather than left in place. A dictionary line that never
fires is worse than no line — it implies coverage that does not exist.

This also confirms where the boundary lies: **these are tokenizer-level
problems, the same class as URLs and dates, and belong in a frontend.**

### Also done

- **Attributive numerals** — see below. This was listed as unfixable and was not.

### Not attempted, with the reason

| spec | why not |
|---|---|
| Fractions `1/2`, `2 1/2` | `numbers.c` has no slash handling of any kind |
| Distributive `5-аар`, collective `5-уул` | the `_#-` hook forces `ordinal = 2`, so it would emit "тавдугаа аар" |
| Dates, times, ranges, ratios, scores | no date or time parser; `09:15` reads "ес арван тав" with no цаг/минут |
| Phone numbers, ISBN, versions, legal refs | no digit-string mode |
| Prefix currency `$100` | **not Mongolian-specific** — English gives "dollar one hundred" too. Engine-wide; a separate cross-language change |
| URLs, emails, hashtags, emoji semantics, foreign-name transliteration, chemical formulas, scientific notation, sentence boundaries, code-switching, tables, poetry | frontend, not a TTS engine's job |

### Fixed: attributive numerals

A numeral modifying a noun takes a different form from the one used in
isolation: тав is "five", but "5 км" is **таван** километр. eSpeak emitted the
isolated form everywhere, and the specification hits this in six places.

This section previously said it could not be fixed here — that `numbers.c` emits
phonemes directly so `mn_rules` cannot post-process them, and the engine has no
"attributive before a noun" concept. The first half is true; the conclusion was
wrong. The engine already has the *key*: `_%de`, selected by `number_control & 1`
and restricted by `control & 2` to the **final tens and units** of a number —
which is exactly the scope Mongolian needs, because the tens are attributive in
`mn_list` already (хорин, not хорь).

Only the trigger was missing. `NUM2_ATTRIBUTIVE` sets `number_control |= 1` when
a word follows the number, right beside the existing Hungarian hook where `wtab`
is in scope. Every other language is unaffected: the flag is opt-in and the
lookup falls back to `_%d` when no `_%de` entry exists.

    5 км    → таван километр      3 хүн   → гурван хүн
    25 км   → хорин таван километр   9 сар   → есөн сар

**The trigger took two corrections, both caught by testing rather than by
thinking.**

First: a following *number* is not a following noun. Triggering on any following
word broke counts (`0 1 2 3` became "гурван дөрвөн") and ranges (`2010-2020`).
The trigger now skips the separator and requires the next character not to be a
digit.

Second: a following *particle* is not a noun either. Of nine probe cases, seven
were still wrong — `5 ба 6`, `5 юм`, `5 нь их`, `5 гэж хэлэв`, `5 бол их`,
`5 л байна` all went attributive. Conjunctions, clitics, quotatives and
sentence-final particles are a closed class, and the same one already listed as
particles in `mn_list`, so `langopts.attributive_stop_words` names them instead
of guessing. `5` alone still reads тав.

That list is a linguistic judgement, not a measurement, and it is the part of
this change most likely to need a native speaker's revision.

`mn_list` carries `_3e`…`_10e`; 1 and 2 do not change form and are absent.

Not covered: the scale words. `1000 хүн` reads "нэг мянга хүн" where Mongolian
wants мянган. `_%de` reaches the final unit, not the thousands word, so that
needs a separate variant.

### `mn-f`

Acoustic validation remains impossible: the reference corpus is one male speaker
and the toolkit is gone. What *is* checkable, and now checked: `mn-f` is
**phoneme-identical to `mn` across all 111 test texts**, which is the real
invariant — it is the same language with different voice parameters. Its
settings are internally coherent: `pitch 140 210` gives `formant_factor` 1.177,
which scales the coarticulation target, while `formant 1/2/3 = 112/117/111`
scale the vowel formants. Those are different mechanisms and do not compound.

---

## 19. What a native speaker heard, and what it changed

Section 18 answered a specification. This one answers a listener. A 5-minute
review file covering every phenomenon (`~/mn_samples/mn_full_review.wav`, with a
timestamped index) was played to a native speaker, who rated the voice 3/10 and
pointed at specific timestamps.

### The uvular fricative was wrong; the uvular allophony was not

Three of the six words he could not understand contained a uvular: бага
[paʁa], байгууллага, сургууль. `г` had three surface forms -- velar `k` in
front-harmony words, uvular `q` in back-harmony words, and uvular fricative
`Q"` [ʁ] between vowels.

The literature this voice is built on (Svantesson et al.) describes exactly that
allophony, so it could not be overturned on one reading. Instead the question was
put as a listening choice: the same fourteen words rendered three ways --

| | |
|---|---|
| A | current: uvular `q`, fricative `Q"` between vowels |
| B | uvular **stop** everywhere in back words, no fricative |
| C | velar everywhere, no uvular allophone at all |

He chose **B**. So the harmony-driven uvular allophone is correct and only its
fricative realisation was wrong: **г is a stop in every position**. бага is now
[paqa].

Worth recording that a Mongolian-specialist LLM had answered **C** -- that г is
identical in back and front words. The speaker's ear disagreed. The listening
test settled what neither the reference nor the model could.

`Q"` is gone from the rules, and the 24 `mn_list` entries that had it baked in
(the `-дугаар` ordinals, plus вагон) were regenerated.

### Word-final ь was audible

`морь` came out [mɔrj], with a real [j] segment. Final ь is palatalisation of
the preceding consonant and is itself silent.

The file was inconsistent with itself: five consonants had dedicated palatalised
phonemes (`t; th; ts; tsh; l;`) and every other one fell back to consonant + `j`.
Added `r; m; s; w;` on the same pattern. морьё keeps its glide, correctly -- that
is an iotated vowel, not a bare soft sign.

**`X;` and `x;` were tried and reverted.** Built the same way, they came out
*inert*: дах and дахь produced byte-identical audio, 771 ms and rms 2134 for
both, destroying a contrast that works today. `CALL X` overrides the palatalised
locus. Same family as the `ipa`-after-`import_phoneme` silence in section 10d --
a phoneme that compiles cleanly and changes nothing. дахь keeps its glide until
a real palatalised uvular exists.

### Still open, from the same session

- **э is too close to и.** "эт sounds like ит". Their separation is 59, the
  second-smallest of any pair: э sits at F1 276 against и's 235, where a mid [e]
  wants roughly 400-450.
- **о/ө is weak** (separation 132), and **у/ү weaker still** (79).
- **ө may be rounded the wrong way.** The speaker describes ө as *more* rounded
  than о, which means lower F2 -- but the file has ө at F2 1302 against о's 1046,
  and against this project's own measured value of 1082 (section 6).
- **улаан and долларын** were not understood either, and contain no uvular. The
  fault there is the vowel or the geminate `лл`.

### Whisper is not usable here

`faster-whisper tiny` transcribed Mongolian TTS output as "ipsolchitta mungkos
kinsling" and, on another sentence, Chinese characters. That is the model failing
at a low-resource language, not evidence about the voice; `small` could not be
downloaded on the available hardware. A Mongolian-specific ASR (Chimeg)
transcribed the same connected-speech sentence essentially correctly, which is
the more useful datum: the fault is not everywhere.

---

## 20. Four listening tests, and what they overturned

Section 19 recorded the first native-speaker session. This one records what four
A/B tests decided, and it is the part of this document most at odds with the
reference the voice was built on.

The method each time: change exactly one parameter, render the same words two or
three ways into one file with a tone between blocks and a timestamped index, and
ask only "which letter". No description required, no phonetic vocabulary. That
converts an ear into a decision.

### The vowel space was compressed

Individual vowels were tried first -- э's F1 at 276 / 360 / 440, and ө's F2 at
1302 / 1082 / 950. The speaker's verdict was **"none of them is correct"**, for
either. That is a more useful answer than a choice: it killed the hypothesis
that any single vowel was at fault.

Comparing the whole inventory against normal male values showed why. **Every F1
was low, median −114 Hz**, and the space spanned 235–567 where a male's spans
roughly 250–750 -- squashed toward the close end, so no pair separated from any
other. The open vowels were worst (а −133, о −163, э −174) and the close ones
nearly right (и −45, ү −47). It also matched what the speaker had said twice in
his own words: "о should be strong", "а should be strong".

Offered current / halfway / full male targets, he chose **halfway**:

| | was | now |
|---|---|---|
| а | 567 | 634 |
| э | 276 | 363 |
| и | 235 | 258 |
| о | 387 | 469 |
| ө | 286 | 343 |
| у | 299 | 350 |
| ү | 253 | 277 |

Short, long, reduced and diphthong nuclei all moved together. э/и went from 41 Hz
apart to 105.

Doing this exposed a latent bug: **`yi` had peak 0 above F1 in six of its frames**
(`ei` in three), which blocks any F1 change. The gate added in section 10f only
checked the steady frame, so it never saw them. Clamped per frame.

### л is a plain lateral

The reference analyses Khalkha л as a lateral fricative, and the voice
implemented it as one: `WAV(ufric/l#, 88)`. Two of the six words the speaker
could not understand were улаан and долларын, neither of which contains a uvular,
so nothing else explained them.

Asked twice in separate sessions, the specialist model said plain л both times.
Offered fricative / mostly plain / fully plain, the speaker chose **mostly
plain**: the noise source drops to 22, the lateral itself untouched.

### Word-final н is [n], not [ŋ]

Also contrary to the reference, and also confirmed twice by the model before
being put to the ear, which chose it on a straight A/B over хүн, ном, сайн,
эрдэмтэн. Assimilation before a velar is unaffected -- монгол, анги, банк and
ханх all keep [ŋ].

### сургууль is "сургуйл"

Word-final ль surfaces as a glide BEFORE the lateral, not as a palatalised
lateral after it: [surɢuːi̯ɬ], not [surɢuːʎ], which the speaker heard as
"сургуули". Scoped to word-final -- сургуульд and хаальга keep the palatalised
lateral, and сургуул is untouched.

### On disagreeing with the reference

Three of these contradict Svantesson: the uvular fricative (§19), plain л, and
final [n]. None was changed on one opinion. Each was confirmed twice by a
Mongolian-specialist model AND then chosen by a native speaker on a blind A/B
against the current behaviour. Where the model and the ear disagreed -- the model
said г is velar everywhere, the speaker chose the uvular stop -- **the ear won**.

That is the standard this section holds to, and it is worth stating plainly
because the measurements that justified the original values no longer exist: the
corpus and toolkit were removed in section 17, so nothing here can be
re-derived, only re-heard.

### Not acted on

The same model, asked eight short questions, produced four answers that do not
survive checking: that у and ү differ by LENGTH (both are short; the contrast is
harmony); that ор and оор differ by lip position (both rounded; the difference is
length); a third distinct set of ordinal tens across three askings; and that
final ж hardens to ч (eSpeak already distinguishes ж [tʃ] from ч [tʃʰ] by
aspiration, which is the Khalkha contrast -- it was describing correct
behaviour). Its claim that в is [v] rather than [w] was asked only once and
remains untested.

## 21. Hard-coded phonemes go stale: the rule that catches it

Changing `mn_rules` can silently invalidate `mn_list`. A list entry spells its
phonemes out, so it does not follow the rules -- that is the point of a list
entry. But when the entry exists for a *number-engine key* rather than to
override a rule, the same word can reach the listener by two paths, and the two
must agree.

Adding word-final palatalization (`р (ь` -> `r;`, `в (ь` -> `w;`) broke three
entries that still carried the old `j` glide:

| key | was | now | word |
|---|---|---|---|
| `%` | `XUwj` | `XUw;` | хувь |
| `_20o` | `X'OrjtU#qa:` | `X'Or;tU#qa:` | хорьдугаар |
| `_50o` | `th'awjtU#qa:` | `th'aw;tU#qa:` | тавьдугаар |

Audibly, `25%` and `хорин таван хувь` said хувь two different ways. Nothing
failed -- every test still passed, because the tests asserted the stale strings.

**The invariant.** Every `_`-prefixed entry whose comment names a Mongolian word
must equal what the rules derive for that word, modulo the `-р` suffix `r` that
`_#-р` appends. It is mechanically checkable:

```sh
# extract "key  phonemes  // word" triples, derive each word, compare
awk '/^_[0-9A-Za-z#%]+[ \t]+[^ \t]+[ \t]*\/\/[ \t]*[^ \t]+$/ {...}' dictsource/mn_list
espeak-ng -xq -v mn -f words.txt
```

All 22 commented entries agree as of this writing. Re-run it after any change to
`mn_rules`; the comments exist precisely so this check is possible, which is why
every number entry carries one.

The general lesson is broader than Mongolian: a test suite that asserts stored
output cannot detect that the stored output drifted from its source of truth.
The check has to compare the two derivations against each other, not either one
against a recorded constant.

## 22. з/ц: the contrast was in the wrong parameter

A native listener reported no distinction at all between заа and цаа. The rules
are right -- з is `ts`, ц is `tsh`, so the contrast is aspiration, which is the
Khalkha contrast. The defect was in the phoneme table.

**Not amplitude.** §10d already records that investigation: з is written 119/151
and ц 81/104, which looks inverted, but they use different source waveforms, the
rendered output already had ц louder as the speaker has it, and raising ц
measured +4.5 dB too hot and was reverted. That result still stands.

**Duration.** Nobody had compared the source *envelopes*. Every audible
aspiration pair in this table has a long unaspirated-to-aspirated duration ratio;
the affricate pair had almost none:

| pair | unasp | asp | ratio |
|---|---|---|---|
| т/т`h` | 13.9 ms | 48.6 ms | 3.50 |
| п/п`h` | 36.6 ms | 57.9 ms | 1.58 |
| к/х | 55.9 ms | 81.2 ms | 1.45 |
| ж/ч | 55.0 ms | 72.3 ms | 1.31 |
| **з/ц** | **98.1 ms** | **120.0 ms** | **1.22** |

Worse, `ustop/ts_unasp` and `ustop/ts` are both *rising* frication ramps --
51 46 50 51 55 58 61 60 57 and 48 53 55 57 58 61 60 62 65 63 65 64 in 10 ms
frames. Neither has a burst. Two slow crescendos of sibilance 22 ms apart are
not a stop contrast, which is exactly what the listener reported.

**Fix.** з moves to `ustop/ts2`: 55.6 ms, same alveolar sibilant family
(centroid 6227 Hz against ts_unasp's 5659), and what Armenian, Georgian and
Latvian use for plain [ts]. The ratio becomes 2.16.

Amplitude was then retuned *by measuring the rendered onset*, not by copying the
old parameter -- ts2 peaks above ts_unasp, so the first attempt (151 -> 107) came
out 3 dB hot and would have inverted the ц-louder relationship §10d validated
against the speaker. 151 -> 76 and 119 -> 60 hold the rendered level constant.

Measured before and after, `заа` / `цаа`:

| | before | after |
|---|---|---|
| duration gap | 21 ms | 61 ms |
| onset frame 1 | 58 / 53 dB | 60 / 53 dB |
| peak in first 60 ms | 65 / 65 dB | 65 / 65 dB |

The level relationship is preserved; the temporal contrast is created. з now
starts abruptly and reaches the vowel in 40 ms, ц climbs for 60 ms first.

**The general point.** §10d correctly ruled out amplitude and the table was left
alone, but "the parameter I checked is fine" is not "the phoneme is fine". When a
contrast is inaudible, enumerate every parameter that could carry it -- level,
duration, envelope shape, spectrum -- before concluding the pair is correct.

### Open: the back-harmony г burst

`ustop/k_unasp_` was chosen for its spectrum (centroid 2576 Hz, closest of any
candidate to the speaker's 1838) and that choice stands. Its envelope, though,
is 55 56 64 in 10 ms frames -- energy *peaks at the end* and rises into the
vowel, so there is no release transient. Every correctly-shaped burst decays
(`k_unasp` 60 58 56, `k_asp` 60 65 58 62 62 56 57 57, `q` a single frame). This
matches a listener report of г being "weak" without being able to say how.

Level is not the problem: q renders within 1.4 dB of both the front-harmony
allophone `k` and of х.

No other sample fits -- the dark ones (`k_asp2` 2171 Hz, `k_asp_a` 2240 Hz) are
98-112 ms aspirated, and every short one is bright (`k_unasp2` 5973,
`ki_unasp2` 6670, `c` 4774). A time-reversed copy of `k_unasp_` -- identical
magnitude spectrum, envelope 63 59 55 -- was built and tested against it. The
listener heard no improvement and the copy was deleted; §24 explains why the
test could not have worked. The envelope observation itself still stands and
may matter once word-initial voicing is solved.

## 23. г was voiceless, and a 20 ms hole after it

A native listener: "г sounds like K in all of it" -- in every position, and in
both arms of a burst-envelope A/B that had not touched voicing.

**The cause.** Every Mongolian г allophone was declared `vls`. `q` was
`vls uvl stp`, `k` was `vls vel stp`. Khalkha's stop contrast is aspiration and
not voicing, which is why the whole plain series was written voiceless -- but
the plain series is *lenis*, and voiced intervocalically, and a fully devoiced
unaspirated velar stop simply is a [k]. The listener was describing the feature
name in the table.

eSpeak has had a real voiced stop all along (`phsource/phonemes`, phoneme `g`):
`FMT(g/g) addWav(x/g2, 150)` with a `PreVoicing` branch. Mongolian never used it.

Chosen in a blind A/B/C -- A voiceless (current), B voiced stop, C voiced with
the back allophone as a continuant. B won. C revisited ground the listener had
already rejected once, but that earlier test compared a stop against a fricative
without varying voicing, so "stop beats fricative" and "voiced beats voiceless"
had never been separated. B winning means the earlier answer stood and only
needed voicing added.

### The 20 ms hole

B was better but "still needs something". Measuring it found a digital-silence
frame after every back-harmony г: `гал` rendered `62 62 59 57 -180 65` in 10 ms
frames. `гэр` did not.

The difference was one keyword. `q`'s `Vowelin` carried `gpaus`, which
`compiledata.c` encodes as transition flag 64 and `synthesize.c:653` executes as

```c
if (flags & 0x40)
    DoPause(20, 0); // add a short pause after the consonant
```

A 20 ms closure silence is correct for a voiceless [q] and wrong for a voiced
one. Removing it is the whole fix.

Measured as the longest run of *exact zero* samples inside the word:

| word | before | after |
|---|---|---|
| гал | 20 ms | none |
| аг | 20 ms | none |
| агаар | 20 ms | none |
| бага | 18 ms | 18 ms -- but at 17 ms, which is the initial **б**, not г |
| en `ago` (control) | 0.1 ms | 0.1 ms |

English was the control that made this legible: eSpeak's voiced stops produce a
*voice bar* through the closure (`ago` runs `58 58 58 58 58 61 60 58` and never
reaches zero), so the Mongolian silence was a Mongolian defect, not an engine
limit.

`богд` is unchanged and that is correct -- its г is `q` too, but it precedes a
consonant, and `Vowelin` only applies before a vowel, so the pause never fired.

### Two things that did not work

- **`lengthmod 2` -> `5`.** Tried here, appeared to change nothing, and was
  reverted. That was wrong: the metric in use could not see it. With a stable
  metric it is worth 4.5 dB and it is now applied -- see §24.
- **A burst-envelope A/B.** `k_unasp_` peaks at its *end* (`55 56 64`) where
  every correct burst decays, so a time-reversed copy was built and tested. The
  listener heard no improvement, because the defect was voicing and the test did
  not vary voicing. The copy has been deleted.

### What the language model got right, and wrong

Asked six one-line questions in Mongolian, no transcription: it said intervocalic
г does not fully block the airflow (1), that voicing continues through г (5), and
that voicing is the main thing separating г from к and х (6). Those three agreed
with each other, with the measurement, and with the listener, and are what
confirmed the direction.

It also said г is one single sound with no back/front difference (4). The
listener had already chosen the uvular/velar split by ear in an earlier test, so
that answer was not acted on. Same pattern as before: reliable on short
constrained questions about the language, unreliable the moment the question
turns phonetic.

### Still open

б and д are still `vls` with the same silent closures -- `аба` has 45 ms and
18 ms of exact zeros, `ада` 45 ms. If Khalkha's plain series is lenis as a
series, they have the defect that г just had. Nobody has reported them as wrong,
so they are recorded here rather than changed.

## 24. г: what is fixed, what is not, and a measurement that lied

The voicing change in §23 was not enough: "both are incorrect, it sounds like К
or Х". Getting from there to a defensible answer took throwing away two rounds
of numbers, so both the result and the failures are recorded.

### The metric was broken

The first attempt located the closure as `argmin` of the high band over a window,
then read the low band at that frame. When the high band has two near-equal
minima the index flips to an adjacent frame, worth about 7 dB. The same build --
verified by hashing `phondata` -- measured 35.1, 28.1 and 35.0 dB on consecutive
runs. Everything derived from it was discarded, including a reported
"41.3 -> 26.7 dB, +14 dB" for `lengthmod 5`, a claim that `brk` was harmful (it
helps), and a claim that the voice's `consonant_ampv` helped (it does nothing).

The fix is to stop letting the metric choose its own window. For a VCV token with
exactly one consonant, the deepest low-band dip anywhere inside the word *is*
that consonant, and no frame has to be picked:

```python
lo  = [low_band_energy(frame) for frame in word]
dip = max(lo) - min(lo[int(0.2*n):int(0.8*n)])
```

Stable to 0.1 dB across rebuilds. All figures below use it.

### The English comparison was also invalid

A second round compared Mongolian against English and concluded г sat 17 dB short
with 12 dB unexplained. That does not hold either. The dip is strongly
pitch-dependent: driving one voice with `-p` moves `ама` from 45.1 dB to 19.0 dB,
and English `amma` from 28.4 to 10.8. Mongolian runs at a different base pitch,
so any cross-voice figure measures the pitch difference as much as the phoneme.

**Only within-voice comparisons are valid.** The right reference is this voice's
own genuinely-voiced consonants.

### Where г stands

| mn token | dip |
|---|---|
| `ага` -- fully voiceless, before §23 | 41.3 dB |
| `ага` -- `lengthmod 5` | 36.8 dB |
| `ага` -- plus `brk` on `Vowelout` | **33.2 dB** |
| `ана` / `ама` / `ала` -- properly voiced | 33.0 / 34.9 / 35.7 dB |

г is no longer an outlier: it now sits with м, н and л in the same voice.

Every lever was rebuilt and measured separately. Only two moved it:

| change | dip |
|---|---|
| **`lengthmod 2` -> `5`** | 41.3 -> 36.8 dB |
| **`brk` on `Vowelout`** | 36.8 -> 33.2 dB |
| `FMT(g/g, 255)` | no change |
| `addWav` 150 -> 20, or removed entirely | no change |
| `f1=2`, dropping `rms=`, `voicingswitch` | no change (<=0.1 dB) |
| `Q"` continuant instead of a stop | worse |
| voice `consonants`, `breath`, `formant 1` | no change |

Note the metric's limit: `аха` also measures 33.9 dB, because a voiceless
fricative has continuous noise and never dips. It separates stops-with-silence
from everything else, not voiced from voiceless.

### Not fixed: word-initial г

Both applied changes act on the transition out of a **preceding vowel**.
Word-initial г has none, so neither reaches it. A rendered A/B confirms it:
`бага`, `агаар`, `аг`, `байгууллага` move by 2.1-3.5 dB, while `гал`, `газар`,
`гол`, `гэр`, `гүн` are byte-identical to the unchanged build.

That matters, because the original report was about `га`. Intervocalic and final
г are improved; **initial г is untouched** and needs a different mechanism --
most likely the `PreVoicing` / `FMT(g/xg)` branch, which fires but does not
carry.

### Process note

Three harness runs were killed by a timeout mid-variant, leaving mutated source
behind, because a `finally` block does not run when the process is killed. The
first went unnoticed until the next run adopted the mutated file as its baseline
and produced an A/B whose two halves were identical. Keep one known measurement
as an anchor and check the tree against it before trusting the next result --
`ага` is that anchor here.
