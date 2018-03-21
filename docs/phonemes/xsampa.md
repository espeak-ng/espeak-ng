# X-SAMPA Transcription Scheme

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

The Extended Speech Assessment Methods Phonetic Alphabet (X-SAMPA)
transcription scheme was created by John C. Wells in 1995 as an extension to
the various language-specific SAMPA phoneme sets available at that time. The
intention was to be able to encode the 1993 IPA chart in ASCII, while
preserving the scheme already developed by the language-specific phoneme sets
where there are no conflicts in the representation of that ASCII character.

This document uses a set of phoneme features based on the features originally
developed by Evan Kirshenbaum. These features are described in the
[phonemes](phonemes.md) document.

The `_` character is used as a tie bar to join two phonemes, as well as the
first character in several diacritics. It can be used for affricates, double
articulations and diphthongs, but can lead to ambiguity with the corresponding
diacritic. The X-SAMPA PDF document advises that `_` is only used for
affricates and double articulations. In that case, the `_^` non-syllabic marker
can be used for the second vowel in diphthongs.

The `-` character is described in the PDF version of X-SAMPA to differentiate
between affricates, double articulations, and diphthongs such as `ts` and
consonant clusters such as `t-s`.

## Phoneme Transcription Schemes

| BCP47 Subtag                   | Abbreviation | Transcription Scheme                                 | Encoding |
|--------------------------------|--------------|------------------------------------------------------|----------|
| [`fonipa`](../phonemes.md)     | IPA          | International Phonetic Alphabet                      | Unicode  |
| `fonxsamp`                     | X-SAMPA      | Extended Speech Assessment Methods Phonetic Alphabet | ASCII    |
| [`x-foncxs`](cxs.md)           | CXS          | Conlang X-SAMPA                                      | ASCII    |
| [`x-fonkirsh`](kirshenbaum.md) |              | Kirshenbaum (ASCII-IPA)                              | ASCII    |

1. `foncxs` and `fonkirsh` are private use extensions defined in
   the [bcp47-extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
   file, so have the `x-` private use specifier before their subtag names.

## Consonants

|                             | `blb` |       | `lbd` |       | `dnt` |       | `alv`  |       | `pla` |       | `rfx`      |           | `alp` |       | `pal` |        | `vel` |       | `uvl` |       | `phr`  |       | `glt` |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|--------|-------|-------|-------|------------|-----------|-------|-------|-------|--------|-------|-------|-------|-------|--------|-------|-------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls`  | `vcd` | `vls` | `vcd` | `vls`      | `vcd`     | `vls` | `vcd` | `vls` | `vcd`  | `vls` | `vcd` | `vls` | `vcd` | `vls`  | `vcd` | `vls` | `vcd` |
| `nas`                       |       | `m`   |       | `F`   |       |       |        | `n`   |       |       |            | `` n` ``  |       |       |       | `J`    |       | `N`   |       | `N\`  |        |       |       |       |
| `stp`                       | `p`   | `b`   |       |       |       |       | `t`    | `d`   |       |       | `` t` ``   | `` d` ``  |       |       | `c`   | `J\`   | `k`   | `g`   | `q`   | `G\`  | `>\`   |       | `?`   |       |
| `sib`&#xA0;`afr`            |       |       |       |       |       |       |        |       |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `afr`                       |       |       |       |       |       |       |        |       |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`afr`            |       |       |       |       |       |       |        |       |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `sib`&#xA0;`frc`            |       |       |       |       |       |       | `s`    | `z`   | `S`   | `Z`   | `` s` ``   | `` z` ``  | `s\`  | `z\`  |       |        |       |       |       |       |        |       |       |       |
| `frc`                       | `p\`  | `B`   | `f`   | `v`   | `T`   | `D`   |        |       |       |       |            |           |       |       | `C`   | `j\`   | `x`   | `G`   | `X`   | `R`   | `X\`   | `?\`  | `h`   | `h\`  |
| `lat`&#xA0;`frc`            |       |       |       |       |       |       | `K`    | `K\`  |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `apr`                       |       |       |       | `v\`  |       |       |        | `r\`  |       |       |            | `` r\` `` |       |       |       | `j`    |       | `M\`  |       |       |        |       |       |       |
| `lat`&#xA0;`apr`            |       |       |       |       |       |       |        | `l`   |       |       |            | `` l` ``  |       |       |       | `L`    |       | `L\`  |       |       |        |       |       |       |
| `flp`                       |       |       |       |       |       |       |        | `4`   |       |       |            | `` r` ``  |       |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`flp`            |       |       |       |       |       |       |        | `l\`  |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `trl`                       |       | `B\`  |       |       |       |       |        | `r`   |       |       |            |           |       |       |       |        |       |       |       | `R\`  | `H\`   | `<\`  |       |       |
| `clk`                       | `O\`  |       |       |       | `|\`  |       | `!\`   |       |       |       |            |           | `=\`  |       |       |        |       |       |       |       |        |       |       |       |
| `lat`&#xA0;`clk`            |       |       |       |       |       |       | `|\|\` |       |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |
| `imp`                       |       | `b_<` |       |       |       |       |        | `d_<` |       |       |            |           |       |       |       | `J\_<` |       | `g_<` |       | `G\`  |        |       |       |       |
| `ejc`                       | `p_>` |       |       |       |       |       | `t_>`  |       |       |       | `` t`_> `` |           |       |       | `c_>` |        | `k_>` |       | `q_>` |       | `>\_>` |       |       |       |
| `ejc`&#xA0;`frc`            |       |       | `f_>` |       | `T_>` |       | `s_>`  |       | `S_>` |       | `` s`_> `` |           |       |       |       |        | `x_>` |       | `X_>` |       |        |       |       |       |
| `lat`&#xA0;`ejc`&#xA0;`frc` |       |       |       |       |       |       | `K_>`  |       |       |       |            |           |       |       |       |        |       |       |       |       |        |       |       |       |

1. `J\` and `?` are defined in the PDF version of X-SAMPA, but not the HTML
   version.

1. `G\` is only mentioned in the opening description of the HTML version of
   X-SAMPA, but is defined properly in the PDF version.

1. `>\` is defined as an epiglottal plosive in X-SAMPA. In this document it
   is defined as a pharyngeal plosive to be consistent with the IPA
   transcription described in the [phonemes](../phonemes.md) document.

1. `P` can also be used for the labio-dental approximant `v\`.

1. `H\` and `<\` are defined in X-SAMPA as epiglottal fricatives. In this
   document they are defined as pharyngeal trills to be consistent with
   the IPA transcription described in the [phonemes](../phonemes.md)
   document.

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
| `hgh`  | `i`   | `y`   | `1`   | `}`   | `M`   | `u`   |
| `smh`  | `I`   | `Y`   |       |       |       | `U`   |
| `umd`  | `e`   | `2`   | `@\`  | `8`   | `7`   | `o`   |
| `mid`  |       |       | `@`   |       |       |       |
| `lmd`  | `E`   | `9`   | `3`   | `3\`  | `V`   | `O`   |
| `sml`  | `{`   |       | `6`   |       |       |       |
| `low`  | `a`   | `&`   |       |       | `A`   | `Q`   |

### Other Symbols

| Symbol   | Features                      |
|----------|-------------------------------|
| `` @` `` | `unr` `mid` `cnt` `rzd` `vwl` |
| `` 3` `` | `unr` `lmd` `cnt` `rzd` `vwl` |

1. `` @` `` and `` 3` `` are not explicitly listed in X-SAMPA. The rhoticized
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

1. The `{ptr}` (protruded) feature is described as labialized in X-SAMPA.

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
| `pzd`   | `◌'`, `◌_j`   | palatalized    |
| `vzd`   | `◌_G`, `◌_e`  | velarized      |
| `fzd`   | `◌_?\`, `◌_e` | pharyngealized |
| `nzd`   | `◌~`, `◌_~`   | nasalized      |
| `rzd`   | `` ◌` ``      | rhoticized     |

### Tongue Root

The tongue root position can be specified using the following features:

| Feature | Symbol | Name                  |
|---------|--------|-----------------------|
| `atr`   | `◌_A`  | advanced tongue root  |
| `rtr`   | `◌_q`  | retracted tongue root |

## Suprasegmentals

### Stress

| Symbol | Name             |
|--------|------------------|
| `"◌`   | primary stress   |
| `%◌`   | secondary stress |

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

1. <a name="ref1"></a> Wells, John C.,
   [Computer-coding the IPA: a proposed extension of SAMPA](http://www.phon.ucl.ac.uk/home/sampa/x-sampa.htm) (HTML).
   Revised <time datetime="2000-05-03T00:00">2000</time>.

2. <a name="ref2"></a> Wells, John C.,
   [Computer-coding the IPA: a proposed extension of SAMPA](http://www.phon.ucl.ac.uk/home/sampa/ipasam-x.pdf) (PDF).
   Revised <time datetime="1995-04-28T00:00">1995</time>.

3. <a name="ref3"></a> Wells, John C.,
   [SAMPA - computer readable phonetic alphabet](http://www.phon.ucl.ac.uk/home/sampa/index.html).
   Revised <time datetime="2005-10-25T00:00">2005</time>.

