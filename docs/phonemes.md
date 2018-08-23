# Phoneme Features and the International Phonetic Alphabet

- [Phoneme Transcription Schemes](#phoneme-transcription-schemes)
- [Consonants](#consonants)
  - [Other Symbols](#other-symbols)
  - [Gemination](#gemination)
  - [Manner of Articulation](#manner-of-articulation)
  - [Place of Articulation](#place-of-articulation)
  - [Voice](#voice)
- [Vowels](#vowels)
  - [Other Symbols](#other-symbols-1)
  - [Rounding](#rounding)
  - [Height](#height)
  - [Backness](#backness)
- [Diacritics](#diacritics)
  - [Articulation](#articulation)
  - [Air Flow](#air-flow)
  - [Phonation](#phonation)
  - [Rounding and Labialization](#rounding-and-labialization)
  - [Syllabicity](#syllabicity)
  - [Consonant Release](#consonant-release)
  - [Co-articulation](#co-articulation)
  - [Tongue Root](#tongue-root)
  - [Fortis and Lenis](#fortis-and-lenis)
- [Suprasegmentals](#suprasegmentals)
  - [Stress](#stress)
  - [Length](#length)
  - [Rhythm](#rhythm)
  - [Tones](#tones)
  - [Intonation](#intonation)
- [References](#references)

----------

Evan Kirshenbaum created an ASCII transcription of the International Phonetic
Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\], \[<a href="#ref2">2</a>\]</sup>.
As well as using ASCII characters for specific IPA phonemes, this transcription
provides a set of 3-letter feature abbreviations allowing a phoneme to be
described as a sequence of features.

This document extends Evan Kirshenbaum's feature set to be able to describe the
different phonemes in the IPA and as are used in the various languages of the
world. The origin column is used to describe where the feature originated from:

1.  `kirshenbaum` -- The feature originated from Evan Kirshenbaum's ASCII-IPA
     Feature Abbreviation table in Appendix A of his ASCII-IPA document
     <sup>\[<a href="#ref1">1</a>\], \[<a href="#ref2">2</a>\]</sup>.
1.   `cainteoir` -- The feature originated from the Cainteoir Text-to-Speech
     phoneme features <sup>\[<a href="#ref5">5</a>\]</sup>.
1.   `espeak-ng` -- The features originate from the eSpeak NG Text-to-Speech
     program, and are defined in this document.

Not all the Cainteoir Text-to-Speech phoneme features are listed here, only
the ones that are relevant to eSpeak NG.

The goal of this document is not to provide a detailed guide on phonetics. Nor
is it intended to be able to accurately record differences in IPA diacritics.
Instead, it is designed to be a transcription guide for authors of espeak-ng
languages and voices on how to specify phonemes so that the IPA and feature
transcriptions are consistent.

## Phoneme Transcription Schemes

| BCP47 Subtag                            | Abbreviation | Transcription Scheme                                 | Encoding |
|-----------------------------------------|--------------|------------------------------------------------------|----------|
| `fonipa`                                | IPA          | International Phonetic Alphabet                      | Unicode  |
| [`fonxsamp`](phonemes/xsampa.md)        | X-SAMPA      | Extended Speech Assessment Methods Phonetic Alphabet | ASCII    |
| [`x-foncxs`](phonemes/cxs.md)           | CXS          | Conlang X-SAMPA                                      | ASCII    |
| [`x-fonkirsh`](phonemes/kirshenbaum.md) |              | Kirshenbaum (ASCII-IPA)                              | ASCII    |

1. `foncxs` and `fonkirsh` are private use extensions defined in
   the [bcp47-extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
   file, so have the `x-` private use specifier before their subtag names.

## Consonants

|                             | `blb` |       | `lbd` |       | `dnt` |       | `alv` |       | `pla` |       | `rfx` |       | `alp` |       | `pal` |       | `vel` |       | `uvl` |       | `phr` |       | `glt` |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` |
| `nas`                       | m̥     | m     |       | ɱ     |       |       | n̥     | n     |       |       | ɳ̊     | ɳ     | ɲ̟̊     | ɲ̟     | ɲ̊     | ɲ     | ŋ̊     | ŋ     | ɴ̥     | ɴ     |       |       |       |       |
| `stp`                       | p     | b     | p̪     | b̪     | t̪     | d̪     | t     | d     |       |       | ʈ     | ɖ     |       |       | c     | ɟ     | k     | ɡ     | q     | ɢ     | ʡ     |       | ʔ     |       |
| `sib`&#xA0;`afr`            |       |       |       |       |       |       | t͡s    | d͡z    | t͡ʃ    | d͡ʒ    | ʈ͡ʂ    | ɖ͡ʐ    | t͡ɕ    | d͡ʑ    |       |       |       |       |       |       |       |       |       |       |
| `afr`                       | p͡ɸ    | b͡β    | p̪͡f    | b̪͡v    | t͡θ    | d͡ð    |       |       |       |       |       |       |       |       | c͡ç    | ɟ͡ʝ    | k͡x    | ɡ͡ɣ    | q͡χ    | ɢ͡ʁ    | ʡ͡ħ    | ʡ͡ʕ    | ʔ͡h    |       |
| `lat`&#xA0;`afr`            |       |       |       |       |       |       | t͡ɬ    | d͡ɮ    |       |       | ʈ͡ɭ̊˔   |       |       |       | c͡ʎ̥˔   |       | k͡ʟ̝̊    | ɡ͡ʟ̝    |       |       |       |       |       |       |
| `sib`&#xA0;`frc`            |       |       |       |       |       |       | s     | z     | ʃ     | ʒ     | ʂ     | ʐ     | ɕ     | ʑ     |       |       |       |       |       |       |       |       |       |       |
| `frc`                       | ɸ     | β     | f     | v     | θ     | ð     |       |       |       |       |       |       |       |       | ç     | ʝ     | x     | ɣ     | χ     | ʁ     | ħ     | ʕ     | h     | ɦ     |
| `lat`&#xA0;`frc`            |       |       |       |       |       |       | ɬ     | ɮ     |       |       | ɭ̊˔    |       |       |       | ʎ̥˔    | ʎ̝     | ʟ̝̊     | ʟ̝     |       |       |       |       |       |       |
| `apr`                       |       |       | ʋ̥     | ʋ     |       |       | ɹ̥     | ɹ     |       |       | ɻ̊     | ɻ     |       |       | j̊     | j     | ɰ̊     | ɰ     |       |       |       |       |       |       |
| `lat`&#xA0;`apr`            |       |       |       |       |       |       | l̥     | l     |       |       | ɭ̊     | ɭ     |       |       | ʎ̥     | ʎ     | ʟ̥     | ʟ     |       | ʟ̠     |       |       |       |       |
| `flp`                       |       | ⱱ̟     |       | ⱱ     |       |       | ɾ̥     | ɾ     |       |       | ɽ̊     | ɽ     |       |       |       |       |       |       |       | ɢ̆     |       | ʡ̮     |       |       |
| `lat`&#xA0;`flp`            |       |       |       |       |       |       |       | ɺ     |       |       |       | ɭ̆     |       |       |       | ʎ̮     |       | ʟ̆     |       |       |       |       |       |       |
| `trl`                       |       | ʙ     |       |       |       |       | r̥     | r     |       |       | ɽ͡r̥    | ɽ͡r    |       |       |       |       |       |       | ʀ̥     | ʀ     | ʜ     | ʢ     |       |       |
| `clk`                       | ʘ     |       |       |       | ǀ     |       | ǃ     |       |       |       |       |       | ǂ     |       |       |       |       |       |       |       |       |       |       |       |
| `lat`&#xA0;`clk`            |       |       |       |       |       |       | ǁ     |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
| `imp`                       |       | ɓ     |       |       |       |       |       | ɗ     |       |       |       |       |       |       |       | ʄ     |       | ɠ     |       | ʛ     |       |       |       |       |
| `ejc`                       | pʼ    |       |       |       |       |       | tʼ    |       |       |       | ʈʼ    |       |       |       | cʼ    |       | kʼ    |       | qʼ    |       | ʡʼ    |       |       |       |
| `ejc`&#xA0;`frc`            |       |       | fʼ    |       | θʼ    |       | sʼ    |       | ʃʼ    |       | ʂʼ    |       |       |       |       |       | xʼ    |       | χʼ    |       |       |       |       |       |
| `lat`&#xA0;`ejc`&#xA0;`frc` |       |       |       |       |       |       | ɬʼ    |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |

### Other Symbols

|                             | `bld` |       | `alv` |       | `pla` |       | `pal` |       | `lbv` |       | `vel` |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` |
| `nas`                       |       |       |       |       |       |       |       |       |       | ŋ͡m    |       |       |
| `stp`                       |       |       |       |       |       |       |       |       | k͡p    | ɡ͡b    |       |       |
| `afr`                       | p͡f    | b͡v    |       |       |       |       |       |       |       |       |       |       |
| `vzd`&#xA0;`frc`            |       |       |       |       | ɧ     |       |       |       |       |       |       |       |
| `ptr`&#xA0;`apr`            |       |       |       |       |       |       |       | ɥ     |       |       | ʍ     | w     |
| `fzd`&#xA0;`lat`&#xA0;`apr` |       |       |       | ɫ     |       |       |       |       |       |       |       |       |

### Gemination

Gemination is found in several languages including Italian and Japanese.
It is also present in the suprasegmental phonology between words such as
"lamppost" and "evenness".

Some linguists use the [long](#length) suprasegmental for geminate consonants.
The eSpeak NG convention is to use consonant length for [phonation](#phonation)
when consonant length is distinct without gemination occurring.

The way gemination is represented in eSpeak NG is to duplicate the phonemes,
with the first phoneme using the `unx` feature. For example, n̚.n for a
geminated n. This describes how with the `stp` and `nas` consonants, the
mouth remains closed (`unx`) for the first of the geminated consonants.

### Manner of Articulation

| Feature   | Symbol | Name           | Origin        |
|-----------|--------|----------------|---------------|
| `nas`     |        | nasal          | `kirshenbaum` |
| `stp`     |        | plosive (stop) | `kirshenbaum` |
| `afr`     |        | affricate      | `espeak-ng`   |
| `frc`     |        | fricative      | `kirshenbaum` |
| `flp`     |        | tap/flap       | `kirshenbaum` |
| `trl`     |        | trill          | `kirshenbaum` |
| `apr`     |        | approximant    | `kirshenbaum` |
| `clk`     |        | click          | `kirshenbaum` |
| `ejc`     | ◌ʼ     | ejective       | `kirshenbaum` |
| `imp`     |        | implosive      | `kirshenbaum` |
| `vwl`     |        | vowel          | `kirshenbaum` |

The `vwl` phonemes are described using vowel height and backness features,
while consonants (the other manners of articulation) are described using
place of articulation features.

__NOTE:__ Evan Kirshenbaum defines an `orl` (oral) feature which is not
used. From context, it looks like `{orl,stp}` was indended to be used for
plosives, and `{nas,stp}` for nasals. That feature is not defined in this
document, but is defined in the [phoneme model](phoneme_model.md).

The manner of articulation can be refined using the following features:

| Feature | Name     | Origin        |
|---------|----------|---------------|
| `lat`   | lateral  | `kirshenbaum` |
| `sib`   | sibilant | `cainteoir`   |

__NOTE:__ Evan Kirshenbaum defines a `ctl` (central) feature which is not
used. From context, it looks like it was intended to explicitly annotate
consonants as having a central release, similar to how the `lat` feature
is used for lateral release. As consonants are implicitly central, the
`ctl` feature is not needed and as such is not defined in this document.

### Place of Articulation

| Feature | Name                  | Origin        |
|---------|-----------------------|---------------|
| `blb`   | bilabial              | `kirshenbaum` |
| `lbd`   | labio-dental          | `kirshenbaum` |
| `bld`   | bilabial-labio-dental | `espeak-ng`   |
| `dnt`   | dental                | `kirshenbaum` |
| `alv`   | alveolar              | `kirshenbaum` |
| `pla`   | palato-alveolar       | `kirshenbaum` |
| `rfx`   | retroflex             | `kirshenbaum` |
| `alp`   | alveolo-palatal       | `cainteoir`   |
| `pal`   | palatal               | `kirshenbaum` |
| `vel`   | velar                 | `kirshenbaum` |
| `lbv`   | labio-velar           | `kirshenbaum` |
| `uvl`   | uvular                | `kirshenbaum` |
| `phr`   | pharyngeal            | `kirshenbaum` |
| `glt`   | glottal               | `kirshenbaum` |

The `bld` place of articulation is used for `afr` consonants that have a `blb`
onset and a `lbd` release, e.g. in the German p͡f consonant.

__NOTE:__ The IPA charts make a distinction between pharyngeal and epiglottal
consonants, but Wikipedia does not. This model uses the Wikipedia descriptions.

### Voice

| Feature | Name      | Origin        |
|---------|-----------|---------------|
| `vls`   | voiceless | `kirshenbaum` |
| `vcd`   | voiced    | `kirshenbaum` |

## Vowels

|        | `fnt` |       | `cnt` |       | `bck` |       |
|--------|-------|-------|-------|-------|-------|-------|
|        | `unr` | `rnd` | `unr` | `rnd` | `unr` | `rnd` |
| `hgh`  | i     | y     | ɨ     | ʉ     | ɯ     | u     |
| `smh`  | ɪ     | ʏ     |       |       |       | ʊ     |
| `umd`  | e     | ø     | ɘ     | ɵ     | ɤ     | o     |
| `mid`  |       |       | ə     |       |       |       |
| `lmd`  | ɛ     | œ     | ɜ     | ɞ     | ʌ     | ɔ     |
| `sml`  | æ     |       | ɐ     |       |       |       |
| `low`  | a     | ɶ     |       |       | ɑ     | ɒ     |

### Other Symbols

| Symbol | Features                      |
|--------|-------------------------------|
| ɚ      | `unr` `mid` `cnt` `rzd` `vwl` |
| ɝ      | `unr` `lmd` `cnt` `rzd` `vwl` |

### Height

| Feature | Name                   | Origin        |
|---------|------------------------|---------------|
| `hgh`   | close (high)           | `kirshenbaum` |
| `smh`   | near-close (semi-high) | `kirshenbaum` |
| `umd`   | close-mid (upper-mid)  | `kirshenbaum` |
| `mid`   | mid                    | `kirshenbaum` |
| `lmd`   | open-mid (lower-mid)   | `kirshenbaum` |
| `sml`   | near-open (semi-low)   | `cainteoir`   |
| `low`   | open (low)             | `kirshenbaum` |

### Backness

| Feature | Name            | Origin        |
|---------|-----------------|---------------|
| `fnt`   | front           | `kirshenbaum` |
| `cnt`   | center          | `kirshenbaum` |
| `bck`   | back            | `kirshenbaum` |

__NOTE:__ The `smh` vowels are more `cnt` than the other vowels. However, this
distinction is not needed to classify these vowels so there are no features
for front-central and back-central. The `fnt` and `bck` features are used
instead.

### Rounding

| Feature | Name      | Origin        |
|---------|-----------|---------------|
| `unr`   | unrounded | `kirshenbaum` |
| `rnd`   | rounded   | `kirshenbaum` |

## Diacritics

### Articulation

| Feature | Symbol | Name            | Origin        |
|---------|--------|-----------------|---------------|
| `lgl`   | ◌̼      | linguolabial    | `cainteoir`   |
| `idt`   | ◌̪͆      | interdental     | `espeak-ng`   |
|         | ◌̪      | dental          |               |
| `apc`   | ◌̺      | apical          | `espeak-ng`   |
| `lmn`   | ◌̻      | laminal         | `cainteoir`   |
|         | ◌̟      | advanced        |               |
|         | ◌̠      | retracted       |               |
|         | ◌̈      | centralized     |               |
|         | ◌̽      | mid-centralized |               |
|         | ◌̝      | raised          |               |
|         | ◌̞      | lowered         |               |

The articulations that do not have a corresponding feature name are recorded
using the features of their new location in the consonant or vowel charts, not
using the features of the base phoneme.

### Air Flow

| Feature | Symbol | Name       | Origin        |
|---------|--------|------------|---------------|
| `egs`   | ↑      | egressive  | `espeak-ng`   |
| `igs`   | ↓      | ingressive | `espeak-ng`   |

The ↑ and ↓ symbols are from the extended IPA<sup>\[<a href="#ref7">7</a>\]</sup>.
They only need to be used when the air flow is different to the base IPA
phoneme (e.g. using ↓ on pulmonic consonants).

### Phonation

| Feature | Symbol | Name            | Origin        |
|---------|--------|-----------------|---------------|
| `brv`   | ◌̤      | breathy voice   | `espeak-ng`   |
| `slv`   | ◌̥      | slack voice     | `cainteoir`   |
| `stv`   | ◌̬      | stiff voice     | `cainteoir`   |
| `crv`   | ◌̰      | creaky voice    | `cainteoir`   |
| `glc`   | ʔ͡◌     | glottal closure | `espeak-ng`   |

The IPA ◌̥ diacritic is also used to fill the `vls` spaces in the IPA consonant
charts. Thus, when ◌̥ is used with a `vcd` consonant that does not have an
equivalent `vls` consonant, the resulting consonant is `vls`, not `slv`.

__NOTE:__ Evan Kirshenbaum uses the `mrm` (murmured) feature for breathy voice,
using it for `[ɦ]` instead of `vcd`, following the way the phoneme is pronounced
instead of how it is annotated on the IPA chart.

### Rounding and Labialization

| Feature | Symbol | Name       | Origin        |
|---------|--------|------------|---------------|
| `ptr`   | ◌ʷ, ◌ᶣ | protruded  | `espeak-ng`   |
| `cmp`   | ◌ᵝ     | compressed | `espeak-ng`   |

__NOTE:__ Evan Kirshenbaum uses the `lzd` (labialized) feature for protruded.

The degree of rounding/labialization can be specified using the following
features:

| Feature | Symbol | Name         | Origin        |
|---------|--------|--------------|---------------|
| `mrd`   | ◌̹      | more rounded | `cainteoir`   |
| `lrd`   | ◌̜      | less rounded | `cainteoir`   |

### Syllabicity

| Feature | Symbol | Name            | Origin        |
|---------|--------|-----------------|---------------|
| `syl`   | ◌̩      | syllabic        | `kirshenbaum` |
| `nsy`   | ◌̯      | non-syllabic    | `cainteoir`   |

### Consonant Release

| Feature | Symbol | Name                            | Origin        |
|---------|--------|---------------------------------|---------------|
| `asp`   | ◌ʰ     | aspirated                       | `kirshenbaum` |
| `nrs`   | ◌ⁿ     | nasal release                   | `espeak-ng`   |
| `lrs`   | ◌ˡ     | lateral release                 | `espeak-ng`   |
| `unx`   | ◌̚      | no audible release (unexploded) | `kirshenbaum` |

### Co-articulation

| Feature | Symbol | Name           | Origin        |
|---------|--------|----------------|---------------|
| `pzd`   | ◌ʲ     | palatalized    | `kirshenbaum` |
| `vzd`   | ◌ˠ, ◌̴  | velarized      | `kirshenbaum` |
| `fzd`   | ◌ˤ, ◌̴  | pharyngealized | `kirshenbaum` |
| `nzd`   | ◌̃      | nasalized      | `kirshenbaum` |
| `rzd`   | ◌˞     | rhoticized     | `kirshenbaum` |

The combining ◌̴ mark is used for velarized or pharyngealized consonants.
Wikipedia recommends precomposed letters for this as this combining mark
is deprecated, and the font may not render the composed form correctly.

### Tongue Root

The tongue root position can be specified using the following features:

| Feature | Symbol | Name                  | Origin        |
|---------|--------|-----------------------|---------------|
| `atr`   | ◌̘      | advanced tongue root  | `cainteoir`   |
| `rtr`   | ◌̙      | retracted tongue root | `cainteoir`   |

### Fortis and Lenis

| Feature | Symbol | Name   | Origin        |
|---------|--------|--------|---------------|
| `fts`   | ◌͈      | fortis | `espeak-ng`   |
| `lns`   | ◌͉      | lenis  | `espeak-ng`   |

The extended IPA<sup>\[<a href="#ref7">7</a>\]</sup> ◌͈ and ◌͉ diacritics
are used to specify lesser (`lns`) and greater (`fts`) oral pressure than
the unmodified voiced or voiceless phoneme. This distinction is made by
the Ewe, Tabasaran, Archi, and other languages<sup>\[<a href="#ref8">8</a>\]</sup>.

Where fortis and lenis are used to contrast consonant durations (e.g. in
the Jawoyn, Ojibwe, and Zurich German languages<sup>\[<a href="#ref8">8</a>\]</sup>),
the [length](#length) suprasegmentals are used instead.

## Suprasegmentals

### Stress

| Symbol | Name             |
|--------|------------------|
| `ˈ◌`   | primary stress   |
| `ˌ◌`   | secondary stress |

### Length

| Feature | Symbol | Name            | Origin        |
|---------|--------|-----------------|---------------|
| `est`   | ◌̆      | extra short     | `cainteoir`   |
| `hlg`   | ◌ˑ     | half-long       | `cainteoir`   |
| `lng`   | ◌ː     | long            | `kirshenbaum` |
| `elg`   | ◌ːː    | extra long      | `espeak-ng`   |

### Rhythm

| Symbol | Name               |
|--------|--------------------|
| `.`    | syllable break     |
| `◌‿◌`  | linking (no break) |

### Tones

| Symbol | Name            |
|--------|-----------------|
| `◌˥`   | extra high tone |
| `◌˦`   | high tone       |
| `◌˧`   | mid tone        |
| `◌˨`   | low tone        |
| `◌˩`   | extra low tone  |
| `ꜛ◌`   | upstep          |
| `ꜜ◌`   | downstep        |

### Intonation

| Symbol | Name                     |
|--------|--------------------------|
| `|`    | minor (foot) break       |
| `‖`    | major (intonation) break |
| `↗︎`    | global rise              |
| `↘︎`    | global fall              |

## References

1. <a name="ref1"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](https://web.archive.org/web/20160419125856/http://www.kirshenbaum.net/IPA/faq.html) (HTML). 1993.

2. <a name="ref2"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](https://web.archive.org/web/20160419125856/http://www.kirshenbaum.net/IPA/ascii-ipa.pdf) (PDF). 2001.

3. <a name="ref3"></a> International Phonetic Association,
   [The International Phonetic Alphabet and the IPA Chart](https://www.internationalphoneticassociation.org/content/ipa-chart). 2015.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

4. <a name="ref4"></a> Wikipedia.
   [International Phonetic Alphabet](https://en.wikipedia.org/wiki/International_Phonetic_Alphabet). 2017.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

5. <a name="ref5"></a> Dunn, R. H.,
   [Cainteoir Text-to-Speech Phoneme Features](https://raw.githubusercontent.com/rhdunn/cainteoir-engine/master/src/libcainteoir/phoneme/phoneme.cpp). 2013-2015.

6. <a name="ref6"></a> Wikipedia.
   [Voiced glottal fricative](https://en.wikipedia.org/wiki/Voiced_glottal_fricative). 2017,
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

7. <a name="ref7"></a> Wikipedia.
   [Extensions to the International Phonetic Alphabet](https://en.wikipedia.org/wiki/Extensions_to_the_International_Phonetic_Alphabet). 2017,
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

8. <a name="ref8"></a> Wikipedia.
   [Fortis and lenis](https://en.wikipedia.org/wiki/Fortis_and_lenis). 2017,
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

9. <a name="ref9"></a> Wikipedia.
   [Place of articulation](https://en.wikipedia.org/wiki/Place_of_articulation). 2017,
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).
