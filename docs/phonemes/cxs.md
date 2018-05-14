# Conlang X-SAMPA Transcription Scheme

- [Phoneme Transcription Schemes](#phoneme-transcription-schemes)
- [Consonants](#consonants)
  - [Other Symbols](#other-symbols)
  - [Manner of Articulation](#manner-of-articulation)
- [Vowels](#vowels)
  - [Other Symbols](#other-symbols-1)
- [Diacritics](#diacritics)
  - [Articulation](#articulation)
  - [Phonation](#phonation)
  - [Rounding and Labialization](#rounding-and-labialization)
  - [Syllabicity](#syllabicity)
  - [Consonant Release](#consonant-release)
  - [Co-articulation](#co-articulation)
  - [Tongue Root](#tongue-root)
- [Suprasegmentals](#suprasegmentals)
  - [Stress](#stress)
  - [Length](#length)
  - [Rhythm](#rhythm)
  - [Tones](#tones)
  - [Intonation](#intonation)
- [References](#references)

----------

The Conlang X-SAMPA (CXS) transcription scheme is documented by Henrik
Theiling. It is used on the conlang mailing list for transcribing the
pronunciation of conlangs, but can be used outside that domain. CXS derives
from X-SAMPA, making various incompatible changes to X-SAMPA with the
intention of making it easier to read by choosing some representations
that are closer to the IPA symbols.

This document uses a set of phoneme features based on the features originally
developed by Evan Kirshenbaum. These features are described in the
[phonemes](phonemes.md) document.

The `_` character is used as a tie bar to join two phonemes, as well as the
first character in several diacritics. It can be used for affricates and double
articulations. The `_^` non-syllabic marker can be used for the second vowel in
diphthongs.

The `)` character is used by CXS as an alternative tie bar that is placed after
the second phoneme. This is preferred by CXS as it does not clash with any
diacritics. For example, `ts)` is an affricate and `ts` is a consonant cluster
of `t` followed by `s`.

The X-SAMPA `-` character to differentiate between affricates, double
articulations, and diphthongs such as `ts` and consonant clusters such as
`t-s` is not supported in CXS.

## Phoneme Transcription Schemes

| BCP47 Subtag                   | Abbreviation | Transcription Scheme                                 | Encoding |
|--------------------------------|--------------|------------------------------------------------------|----------|
| [`fonipa`](../phonemes.md)     | IPA          | International Phonetic Alphabet                      | Unicode  |
| [`fonxsamp`](xsampa.md)        | X-SAMPA      | Extended Speech Assessment Methods Phonetic Alphabet | ASCII    |
| `x-foncxs`                     | CXS          | Conlang X-SAMPA                                      | ASCII    |
| [`x-fonkirsh`](kirshenbaum.md) |              | Kirshenbaum (ASCII-IPA)                              | ASCII    |

1. `foncxs` and `fonkirsh` are private use extensions defined in
   the [bcp47-extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
   file, so have the `x-` private use specifier before their subtag names.

## Consonants

|                             | `blb` |       | `lbd` |       | `dnt` |       | `alv`  |        | `pla` |       | `rfx`      |           | `alp` |       | `pal` |        | `vel` |       | `uvl` |       | `phr`  |       | `glt` |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|--------|--------|-------|-------|------------|-----------|-------|-------|-------|--------|-------|-------|-------|-------|--------|-------|-------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls`  | `vcd`  | `vls` | `vcd` | `vls`      | `vcd`     | `vls` | `vcd` | `vls` | `vcd`  | `vls` | `vcd` | `vls` | `vcd` | `vls`  | `vcd` | `vls` | `vcd` |
| `nas`                       |       | `m`   |       | `F`   |       |       |        | `n`    |       |       |            | `` n` ``  |       |       |       | `J`    |       | `N`   |       | `N\`  |        |       |       |       |
| `stp`                       | `p`   | `b`   |       |       |       |       | `t`    | `d`    |       |       | `` t` ``   | `` d` ``  |       |       | `c`   | `J\`   | `k`   | `g`   | `q`   | `G\`  | `>\`   |       | `?`   |       |
| `sib`&#xA0;`afr`            |       |       |       |       |       |       |        |        |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `afr`                       |       |       |       |       |       |       |        |        |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`afr`            |       |       |       |       |       |       |        |        |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `sib`&#xA0;`frc`            |       |       |       |       |       |       | `s`    | `z`    | `S`   | `Z`   | `` s` ``   | `` z` ``  | `s\`  | `z\`  |       |        |       |       |       |       |        |       |       |       |
| `frc`                       | `p\`  | `B`   | `f`   | `v`   | `T`   | `D`   |        |        |       |       |            |           |       |       | `C`   | `j\`   | `x`   | `G`   | `X`   | `R`   |        |       | `h`   | `h\`  |
| `lat`&#xA0;`frc`            |       |       |       |       |       |       | `K`    | `K\`   |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `apr`                       |       |       |       | `P`   |       |       |        | `r\`   |       |       |            | `` r\` `` |       |       |       | `j`    |       | `M\`  |       |       |        |       |       |       |
| `lat`&#xA0;`apr`            |       |       |       |       |       |       |        | `l`    |       |       |            | `` l` ``  |       |       |       | `L`    |       | `L\`  |       |       |        |       |       |       |
| `flp`                       |       |       |       |       |       |       |        | `4`    |       |       |            | `` r` ``  |       |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`flp`            |       |       |       |       |       |       |        | `l\`   |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `trl`                       |       | `B\`  |       |       |       |       |        | `r`    |       |       |            |           |       |       |       |        |       |       |       | `R\`  | `H\`   | `<\`  |       |       |
| `clk`                       | `O\`  |       |       |       | `|\`  |       | `!\`   |        |       |       | `` !\` ``  |           | `=\`  |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`clk`            |       |       |       |       |       |       | `|\|\` |        |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `imp`                       |       | `b_<` |       |       |       |       |        | `d_<`  |       |       |            |           |       |       |       | `J\_<` |       | `g_<` |       | `G\`  |        |       |       |       |
| `ejc`                       | `p_>` |       |       |       |       |       | `t_>`  |        |       |       | `` t`_> `` |           |       |       | `c_>` |        | `k_>` |       | `q_>` |       | `>\_>` |       |       |       |
| `ejc`&#xA0;`frc`            |       |       | `f_>` |       | `T_>` |       | `s_>`  |        | `S_>` |       | `` s`_> `` |           |       |       |       |        | `x_>` |       | `X_>` |       |        |       |       |       |
| `lat`&#xA0;`ejc`&#xA0;`frc` |       |       |       |       |       |       | `K_>`  |        |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |

1. The X-SAMPA `X\` and `?\` are not specified by CXS. Instead, the `H\` and
   `<\` consonants are transcribed as fricatives and are included in their
   places.

1. `\v` can also be used for the labio-dental approximant `P` like in X-SAMPA.

1. `` !\` `` is specific to CXS. It is not listed in the IPA chart either.

### Other Symbols

|                             | `bld` |       | `alv` |       | `pla` |       | `pal` |       | `lbv` |       | `vel` |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` |
| `nas`                       |       |       |       |       |       |       |       |       |       |       |       |       |
| `stp`                       |       |       |       |       |       |       |       |       |       |       |       |       |
| `afr`                       |       |       |       |       |       |       |       |       |       |       |       |       |
| `vzd`&#xA0;`frc`            |       |       |       |       | `x\`  |       |       |       |       |       |       |       |
| `ptr`&#xA0;`apr`            |       |       |       |       |       |       |       | `H`   |       |       | `W`   | `w`   |
| `fzd`&#xA0;`lat`&#xA0;`apr` |       |       |       | `5`   |       |       |       |       |       |       |       |       |

1. `5` is supported as an alternative to `l_e` due to its use in some language-
   specific SAMPA phoneme sets.

### Manner of Articulation

| Feature   | Symbol | Name      |
|-----------|--------|-----------|
| `ejc`     | `_>`   | ejective  |
| `imp`     | `_<`   | implosive |

## Vowels

|        | `fnt` |       | `cnt` |       | `bck` |       |
|--------|-------|-------|-------|-------|-------|-------|
|        | `unr` | `rnd` | `unr` | `rnd` | `unr` | `rnd` |
| `hgh`  | `i`   | `y`   | `i\`  | `u\`  | `M`   | `u`   |
| `smh`  | `I`   | `Y`   | `I\`  | `U\`  |       | `U`   |
| `umd`  | `e`   | `2`   | `@\`  | `8`   | `7`   | `o`   |
| `mid`  |       |       | `@`   |       |       |       |
| `lmd`  | `E`   | `9`   | `3`   | `3\`  | `V`   | `O`   |
| `sml`  | `&`   |       | `6`   |       |       |       |
| `low`  | `a`   | `&\`  |       |       | `A`   | `Q`   |

1. `i\` is in common usage in the conlang mailing list, but the X-SAMPA  `1`
   transcription is also supported.

1. In CXS, the following vowels are different to the X-SAMPA transcriptions:
   `}` is replaced by `u\`, `{` by `&`, and `&` by `&\`. This makes CXS
   incompatible with X-SAMPA.

1. `I\` and `U\` are not listed in the IPA chart.

### Other Symbols

| Symbol   | Features                      |
|----------|-------------------------------|
| `` @` `` | `unr` `mid` `cnt` `rzd` `vwl` |
| `` 3` `` | `unr` `lmd` `cnt` `rzd` `vwl` |

1. `` @` `` and `` 3` `` are not explicitly listed in CXS. The rhoticized
   diacritic is specified instead.

## Diacritics

### Articulation

| Feature | Symbol | Name            |
|---------|--------|-----------------|
| `lgl`   | `◌_N`  | linguolabial    |
| `idt`   |        | interdental     |
|         | `◌_d`  | dental          |
| `apc`   | `◌_a`  | apical          |
| `lmn`   | `◌_m`  | laminal         |
|         | `◌_+`  | advanced        |
|         | `◌_-`  | retracted       |
|         | `◌_"`  | centralized     |
|         |        | mid-centralized |
|         | `◌_r`  | raised          |
|         | `◌_l`  | lowered         |

The articulations that do not have a corresponding feature name are recorded
using the features of their new location in the consonant or vowel charts, not
using the features of the base phoneme.

### Phonation

| Feature | Symbol | Name            |
|---------|--------|-----------------|
| `brv`   | `◌_t`  | breathy voice   |
| `slv`   | `◌_0`  | slack voice     |
| `stv`   | `◌_v`  | stiff voice     |
| `crv`   | `◌_k`  | creaky voice    |
| `glc`   | `?_◌`  | glottal closure |

The IPA `_0` diacritic is also used to fill the `vls` spaces in the IPA consonant
charts. Thus, when `_0` is used with a `vcd` consonant that does not have an
equivalent `vls` consonant, the resulting consonant is `vls`, not `slv`.

### Rounding and Labialization

| Feature | Symbol | Name       |
|---------|--------|------------|
| `ptr`   | `◌_w`  | protruded  |
| `cmp`   |        | compressed |

The degree of rounding/labialization can be specified using the following
symbols:

| Feature | Symbol | Name         |
|---------|--------|--------------|
| `mrd`   | `◌_O`  | more rounded |
| `lrd`   | `◌_c`  | less rounded |

### Syllabicity

| Feature | Symbol | Name            |
|---------|--------|-----------------|
| `syl`   | `◌=`   | syllabic        |
| `nsy`   | `◌_^`  | non-syllabic    |


### Consonant Release

| Feature | Symbol | Name                            |
|---------|--------|---------------------------------|
| `asp`   | `◌_h`  | aspirated                       |
| `nrs`   | `◌_n`  | nasal release                   |
| `lrs`   | `◌_l`  | lateral release                 |
| `unx`   | `◌_}`  | no audible release (unexploded) |

### Co-articulation

| Feature | Symbol        | Name           |
|---------|---------------|----------------|
| `pzd`   | `◌_j`, `◌;`   | palatalized    |
| `vzd`   | `◌_G`, `◌_e`  | velarized      |
| `fzd`   | `◌_?\`, `◌_e` | pharyngealized |
| `nzd`   | `◌~`, `◌_~`   | nasalized      |
| `rzd`   | `` ◌` ``      | rhoticized     |

1. CXS does not support using `'` for palatalisation as it conflicts with the
   primary stress symbol. The `;` symbol is CXS specific.

### Tongue Root

The tongue root position can be specified using the following features:

| Feature | Symbol | Name                  |
|---------|--------|-----------------------|
| `atr`   | `◌_A`  | advanced tongue root  |
| `rtr`   | `◌_q`  | retracted tongue root |

## Suprasegmentals

### Stress

| Symbol           | Name             |
|------------------|------------------|
| `'◌`, `"◌`       | primary stress   |
| `"◌`, `,◌`, `%◌` | secondary stress |

1. The `"` primary and `%` secondary stress markers are from X-SAMPA. The other
   symbols are CXS specific.

### Length

| Feature | Symbol | Name        |
|---------|--------|-------------|
| `est`   | `◌_X`  | extra short |
| `hlg`   | `◌:\`  | half-long   |
| `lng`   | `◌:`   | long        |
| `elg`   | `◌::`  | extra long  |

1. The `::` symbol for `elg` is not listed in X-SAMPA, but is derived from the
   transcription for `lng`.

### Rhythm

| Symbol | Name               |
|--------|--------------------|
| `◌-\◌` | linking (no break) |

### Tones

| Symbol | Name            |
|--------|-----------------|
| `◌_T`  | extra high tone |
| `◌_H`  | high tone       |
| `◌_M`  | mid tone        |
| `◌_L`  | low tone        |
| `◌_B`  | extra low tone  |
| `!◌`   | downstep        |
| `^◌`   | upstep          |

X-SAMPA additionally defines various symbols for contour tones that can be
defined from the composite tone marks.

| Symbol  | Composite | Name                   |
|---------|-----------|------------------------|
| `◌_R`   | `◌_B_T`   | rising contour         |
| `◌_F`   | `◌_T_B`   | falling contour        |
| `◌_R_F` | `◌_M_H_M` | rising-falling contour |

### Intonation

| Symbol | Name            |
|--------|-----------------|
| `<R>`  | global rise     |
| `<F>`  | global fall     |

## References

1. <a name="ref1"></a> Theiling, Henrik,
   [Conlang X-Sampa: modified X-Sampa used on the Conlang Mailing List](http://www.theiling.de/ipa/).
   Revised <time datetime="2017-05-13T00:00">2017</time>.
