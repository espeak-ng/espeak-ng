# Phoneme Features

- [Phoneme Type](#phoneme-type)
- [Consonants](#consonants)
  - [Voicing](#voicing)
  - [Place of Articulation](#place-of-articulation)
  - [Manner of Articulation](#manner-of-articulation)
- [Vowels](#vowels)
  - [Height](#height)
  - [Backness](#backness)
  - [Rounding](#rounding)
- [Additional Features](#additional-features)
- [References](#references)

----------

Evan Kirshenbaum created an ASCII transcription of the International Phonetic
Alphabet<sup> \[<a href="#ref1">1</a>\]</sup>. As well as using ASCII
characters for specific IPA phonemes, this transcription provides a set of
3-letter feature abbreviations allowing a phoneme to be described as a sequence
of features.

This document describes the features used by Kirshenbaum.

## Phoneme Type

| Feature | Name            |
|---------|-----------------|
| `vwl`   | vowel           |

## Consonants

### Voicing

| Feature | Name            |
|---------|-----------------|
| `vcd`   | voiced          |
| `vls`   | voiceless       |

### Place of Articulation

| Feature | Name            |
|---------|-----------------|
| `blb`   | bilabial        |
| `lbd`   | labio-dental    |
| `dnt`   | dental          |
| `alv`   | alveolar        |
| `rfx`   | retroflex       |
| `pla`   | palato-alveolar |
| `pal`   | palatal         |
| `vel`   | velar           |
| `lbv`   | labio-velar     |
| `uvl`   | uvular          |
| `phr`   | pharyngeal      |
| `glt`   | glottal         |

### Manner of Articulation

| Feature | Name            |
|---------|-----------------|
| `stp`   | stop            |
| `frc`   | fricative       |
| `nas`   | nasal           |
| `orl`   | oral            |
| `apr`   | approximant     |
| `lat`   | lateral         |
| `ctl`   | central         |
| `trl`   | trill           |
| `flp`   | tap/flap        |
| `clk`   | click           |
| `ejc`   | ejective        |
| `imp`   | implosive       |

## Vowels

### Height

| Feature | Name            |
|---------|-----------------|
| `hgh`   | high            |
| `smh`   | semi-high       |
| `umd`   | upper-mid       |
| `mid`   | mid             |
| `lmd`   | lower-mid       |
| `low`   | low             |

### Backness

| Feature | Name            |
|---------|-----------------|
| `fnt`   | front           |
| `cnt`   | center          |
| `bck`   | back            |

### Rounding

| Feature | Name            |
|---------|-----------------|
| `unr`   | unrounded       |
| `rnd`   | rounded         |

## Additional Features

| Feature | Name            |
|---------|-----------------|
| `asp`   | aspirated       |
| `unx`   | unexploded      |
| `syl`   | syllabic        |
| `mrm`   | murmured        |
| `lng`   | long            |
| `vzd`   | velarized       |
| `lzd`   | labialized      |
| `pzd`   | palatalized     |
| `rzd`   | rhoticized      |
| `nzd`   | nasalized       |
| `fzd`   | pharyngealized  |

## References

1. <a name="ref1"></a> Kirshenbaum, Evan,
   [http://www.kirshenbaum.net/IPA/ascii-ipa.pdf](Representing IPA phonetics in ASCII). 2001.
