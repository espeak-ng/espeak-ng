# Phonemes

- [Phoneme Transcriptions](#phoneme-transcriptions)
- [Consonants](#consonants)
  - [Manner of Articulation](#manner-of-articulation)
  - [Place of Articulation](#place-of-articulation)
  - [Voice](#voice)
  - [Gemination](#gemination)
- [Vowels](#vowels)
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
  - [Length](#length)
- [References](#references)

----------

Evan Kirshenbaum created an ASCII transcription of the International Phonetic
Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\], \[<a href="#ref2">2</a>\]</sup>.
As well as using ASCII characters for specific IPA phonemes, this transcription
provides a set of 3-letter feature abbreviations allowing a phoneme to be
described as a sequence of features.

This document extends Evan Kirshenbaum's feature set to be able to describe the
different phonemes in the IPA and as are used in the various languages of the
world.

The goal of this document is not to provide a detailed guide on phonetics. Nor
is it intended to be able to accurately record differences in IPA diacritics.
Instead, it is designed to be a transcription guide for authors of espeak-ng
languages and voices on how to specify phonemes so that the IPA and feature
transcriptions are consistent.

## Phoneme Transcriptions

The following table lists phonetic transcription schemes based on the
International Phonetic Alphabet (IPA):

| BCP47 Variant Name              | Transcription Name                    | Encoding |
|---------------------------------|---------------------------------------|----------|
| foncxs                          | Conlang X-SAMPA (CXS)                 | ASCII    |
| [fonipa](phonemes/fonipa.md)    | International Phonetic Alphabet (IPA) | Unicode  |
| fonkirsh                        | Kirshenbaum (ASCII-IPA)               | ASCII    |
| fonxsampa                       | X-SAMPA                               | ASCII    |

__NOTE:__ The `foncxs` and `fonkirsh` variant names are not defined in the
[IANA language subtag registry](https://www.iana.org/assignments/language-subtag-registry/language-subtag-registry).
Instead, they are private use extensions defined in the
[bcp47-data](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
project.

## Consonants

### Manner of Articulation

| Feature   | Name           |
|-----------|----------------|
| `nas`     | nasal          |
| `stp`     | plosive (stop) |
| `afr`     | affricate      |
| `frc`     | fricative      |
| `flp`     | tap/flap       |
| `trl`     | trill          |
| `apr`     | approximant    |
| `clk`     | click          |
| `ejc`     | ejective       |
| `imp`     | implosive      |
| `vwl`     | vowel          |

The `vwl` phonemes are described using vowel height and backness features,
while consonants (the other manners of articulation) are described using
place of articulation features.

Additionally, the manner of articulation can be refined using the following
features:

| Feature | Name     |
|---------|----------|
| `lat`   | lateral  |
| `sib`   | sibilant |

### Place of Articulation

| Feature | Name                  |
|---------|-----------------------|
| `blb`   | bilabial              |
| `lbd`   | labiodental           |
| `bld`   | bilabial-labiodental  |
| `dnt`   | dental                |
| `alv`   | alveolar              |
| `pla`   | palato-alveolar       |
| `rfx`   | retroflex             |
| `alp`   | alveolo-palatal       |
| `pal`   | palatal               |
| `vel`   | velar                 |
| `lbv`   | labio-velar           |
| `uvl`   | uvular                |
| `phr`   | pharyngeal            |
| `glt`   | glottal               |

The `bld` place of articulation is used for `afr` consonants that have a `blb`
onset and a `lbd` release, e.g. in the German `[p͡f]` consonant.

__NOTE:__ The IPA charts make a distinction between pharyngeal and epiglottal
consonants, but Wikipedia does not. This model uses the Wikipedia descriptions.

### Voice

| Feature | Name      |
|---------|-----------|
| `vls`   | voiceless |
| `vcd`   | voiced    |

### Gemination

Gemination is found in several languages including Italian and Japanese.
It is also present in the suprasegmental phonology between words such as
"lamppost" and "evenness".

Some linguists use the [long](#length) suprasegmental for geminate consonants.
The eSpeak NG convention is to use consonant length for [phonation](#phonation)
when consonant length is distinct without gemination occurring.

The way gemination is represented in eSpeak NG is to duplicate the phonemes,
with the first phoneme using the `unx` feature. For example, `[n̚.n]` for a
geminated `[n]`. This describes how with the `stp` and `nas` consonants, the
mouth remains closed (`unx`) for the first of the geminated consonants.

## Vowels

### Height

| Feature | Name                   |
|---------|------------------------|
| `hgh`   | close (high)           |
| `smh`   | near-close (semi-high) |
| `umd`   | close-mid (upper-mid)  |
| `mid`   | mid                    |
| `lmd`   | open-mid (lower-mid)   |
| `sml`   | near-open (semi-low)   |
| `low`   | open (low)             |

### Backness

| Feature | Name            |
|---------|-----------------|
| `fnt`   | front           |
| `cnt`   | center          |
| `bck`   | back            |

__NOTE:__ The `smh` vowels (`[ɪ]`, `[ʏ]`, `[ʊ]`) are more `cnt` than the
other vowels. However, this distinction is not needed to classify these
vowels, so is not included in the phoneme model described in this document.

### Rounding

| Feature | Name      |
|---------|-----------|
| `unr`   | unrounded |
| `rnd`   | rounded   |

## Diacritics

### Articulation

| Feature | Name            |
|---------|-----------------|
| `lgl`   | linguolabial    |
| `idt`   | interdental     |
|         | dental          |
| `apc`   | apical          |
| `lmn`   | laminal         |
|         | advanced        |
|         | retracted       |
|         | centralized     |
|         | mid-centralized |
|         | raised          |
|         | lowered         |

The articulations that do not have a corresponding feature name are recorded
using the features of their new location in the consonant or vowel charts, not
using the features of the base phoneme.

### Air Flow

| Feature | Name       |
|---------|------------|
| `egs`   | egressive  |
| `igs`   | ingressive |

### Phonation

| Feature | Name            |
|---------|-----------------|
| `brv`   | breathy voice   |
| `slv`   | slack voice     |
| `stv`   | stiff voice     |
| `crv`   | creaky voice    |
| `glc`   | glottal closure |

### Rounding and Labialization

| Feature | Name       |
|---------|------------|
| `ptr`   | protruded  |
| `cmp`   | compressed |

The degree of rounding/labialization can be specified using the following
features:

| Feature | Name         |
|---------|--------------|
| `mrd`   | more rounded |
| `lrd`   | less rounded |

### Syllabicity

| Feature | Name            |
|---------|-----------------|
| `syl`   | syllabic        |
| `nsy`   | non-syllabic    |

### Consonant Release

| Feature | Name                            |
|---------|---------------------------------|
| `asp`   | aspirated                       |
| `nrs`   | nasal release                   |
| `lrs`   | lateral release                 |
| `unx`   | no audible release (unexploded) |

### Co-articulation

| Feature | Name           |
|---------|----------------|
| `pzd`   | palatalized    |
| `vzd`   | velarized      |
| `fzd`   | pharyngealized |
| `nzd`   | nasalized      |
| `rzd`   | rhoticized     |

### Tongue Root

The tongue root position can be specified using the following features:

| Feature | Name                  |
|---------|-----------------------|
| `atr`   | advanced tongue root  |
| `rtr`   | retracted tongue root |

### Fortis and Lenis

| Feature | Name   |
|---------|--------|
| `fts`   | fortis |
| `lns`   | lenis  |

The extended IPA<sup>\[<a href="#ref7">7</a>\]</sup> ◌͈ and ◌͉ diacritics
are used to specify lesser (`lns`) and greater (`fts`) oral pressure than
the unmodified voiced or voiceless phoneme. This distinction is made by
the Ewe, Tabasaran, Archi, and other languages<sup>\[<a href="#ref8">8</a>\]</sup>.

Where fortis and lenis are used to contrast consonant durations (e.g. in
the Jawoyn, Ojibwe, and Zurich German languages<sup>\[<a href="#ref8">8</a>\]</sup>),
the [length](#length) suprasegmentals are used instead.

## Suprasegmentals

### Length

| Feature | Name            |
|---------|-----------------|
| `est`   | extra short     |
| `hlg`   | half-long       |
| `lng`   | long            |
| `elg`   | extra long      |

## References

1. <a name="ref1"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](http://www.kirshenbaum.net/IPA/faq.html) (HTML). 1993.

2. <a name="ref2"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](http://www.kirshenbaum.net/IPA/ascii-ipa.pdf) (PDF). 2001.

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
