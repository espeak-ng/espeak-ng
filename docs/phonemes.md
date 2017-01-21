# Phonemes

- [IPA Charts](#ipa-charts)
  - [Consonants (Pulmonic)](#consonants-pulmonic)
  - [Consonants (Non-Pulmonic)](#consonants-non-pulmonic)
  - [Other Symbols](#other-symbols)
  - [Vowels](#vowels)
- [Features](#features)
  - [Consonants](#consonants)
    - [Voicing](#voicing)
    - [Place of Articulation](#place-of-articulation)
      - [Co-articulation](#co-articulation)
    - [Manner of Articulation](#manner-of-articulation)
  - [Vowels](#vowels-1)
    - [Height](#height)
    - [Backness](#backness)
    - [Rounding](#rounding)
  - [Diacritics](#diacritics)
    - [Syllabicity](#syllabicity)
    - [Consonant Release](#consonant-release)
      - [Gemination](#gemination)
    - [Phonation](#phonation)
    - [Articulation](#articulation)
    - [Co-articulation](#co-articulation-1)
      - [Precomposed Characters](#precomposed-characters)
  - [Suprasegmentals](#suprasegmentals)
    - [Stress](#stress)
    - [Length](#length)
    - [Rhythm](#rhythm)
    - [Intonation](#intonation)
  - [Tones](#tones)
    - [Tone Stepping](#tone-stepping)
  - [Unused](#unused)
- [References](#references)

----------

Evan Kirshenbaum created an ASCII transcription of the International Phonetic
Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\], \[<a href="#ref2">2</a>\]</sup>.
As well as using ASCII characters for specific IPA phonemes, this transcription
provides a set of 3-letter feature abbreviations allowing a phoneme to be
described as a sequence of features.

This document describes the IPA phonemes using the features used by Kirshenbaum.
Where Kirshenbaum does not specify a feature name, the feature name from
Cainteoir Text-to-Speech<sup>\[<a href="#ref5">5</a>\]</sup> is used. This is
to provide a consistent naming scheme for the extended feature set. Where there
is still no feature available, a custom 3-letter feature name is chosen.

The aim of the feature set described in this document is to specify the underlying
phonetics and phonemics of the sounds being produced in a way that is consistent
between languages and voices. While this feature set is modelled on the IPA, it
is not meant to be able to preserve phoneme transcriptions when using a
transcription as both the input and output phoneme sets. This document provides
commentary on the intended usage of these features where there is ambiguity from
the associated IPA usage between authors.

This document is grouped into two sections. The first section displays the IPA
charts using the feature names instead of their names, showing the IPA phoneme
at that position in the chart. This makes it easier to look up the features for
a given IPA phoneme.

The second section lists the features and their associated name. This section
does not describe what these mean. Their meaning is described in phonetics
articles, books and Wikipedia. The Wikipedia IPA<sup>\[<a href="#ref4">4</a>\]</sup>
article can be used as a starting point, as it links to topics and descriptions
of the various phonemes.

The diacritics and suprasegmental feature lists also show their corresponding
IPA symbol. This is to avoid duplicating the lists in the IPA chart and Feature
sections.

## IPA Charts

## Consonants (Pulmonic)

<table>
  <tr>
    <td></td>
    <th colspan="2"><code>blb</code></th>
    <th colspan="2"><code>lbd</code></th>
    <th colspan="2"><code>dnt</code></th>
    <th colspan="2"><code>alv</code></th>
    <th colspan="2"><code>pla</code></th>
    <th colspan="2"><code>rfx</code></th>
    <th colspan="2"><code>alp</code></th>
    <th colspan="2"><code>pal</code></th>
    <th colspan="2"><code>vel</code></th>
    <th colspan="2"><code>uvl</code></th>
    <th colspan="2"><code>phr</code></th>
    <th colspan="2"><code>glt</code></th>
  </tr>
  <tr>
    <th align="right"><code>nas</code></th>
    <td> </td><td>m</td>
    <td> </td><td>ɱ</td>
    <td> </td><td> </td>
    <td> </td><td>n</td>
    <td> </td><td> </td>
    <td> </td><td>ɳ</td>
    <td> </td><td> </td>
    <td> </td><td>ɲ</td>
    <td> </td><td>ŋ</td>
    <td> </td><td>ɴ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>stp</code></th>
    <td>p</td><td>b</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>t</td><td>d</td>
    <td> </td><td> </td>
    <td>ʈ</td><td>ɖ</td>
    <td> </td><td> </td>
    <td>c</td><td>ɟ</td>
    <td>k</td><td>ɡ</td>
    <td>q</td><td>ɢ</td>
    <td> </td><td> </td>
    <td>ʔ</td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>frc</code></th>
    <td>ɸ</td><td>β</td>
    <td>f</td><td>v</td>
    <td>θ</td><td>ð</td>
    <td>s</td><td>z</td>
    <td>ʃ</td><td>ʒ</td>
    <td>ʂ</td><td>ʐ</td>
    <td>ɕ</td><td>ʑ</td>
    <td>ç</td><td>ʝ</td>
    <td>x</td><td>ɣ</td>
    <td>χ</td><td>ʁ</td>
    <td>ħ</td><td>ʕ</td>
    <td>h</td><td>ɦ</td>
  </tr>
  <tr>
    <th align="right"><code>lat</code>&#xA0;<code>frc</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>ɬ</td><td>ɮ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>apr</code></th>
    <td> </td><td> </td>
    <td> </td><td>ʋ</td>
    <td> </td><td> </td>
    <td> </td><td>ɹ</td>
    <td> </td><td> </td>
    <td> </td><td>ɻ</td>
    <td> </td><td> </td>
    <td> </td><td>j</td>
    <td> </td><td>ɰ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>lat</code>&#xA0;<code>apr</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td>l</td>
    <td> </td><td> </td>
    <td> </td><td>ɭ</td>
    <td> </td><td> </td>
    <td> </td><td>ʎ</td>
    <td> </td><td>ʟ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>flp</code></th>
    <td> </td><td> </td>
    <td> </td><td>ⱱ</td>
    <td> </td><td> </td>
    <td> </td><td>ɾ</td>
    <td> </td><td> </td>
    <td> </td><td>ɽ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>trl</code></th>
    <td> </td><td>ʙ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td>r</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td>ʀ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
</table>

Symbols to the left are `vls`, and to the right are `vcd`.

### Consonants (Non-Pulmonic)

<table>
  <tr>
    <td></td>
    <th><code>blb</code></th>
    <th><code>lbd</code></th>
    <th><code>dnt</code></th>
    <th><code>alv</code></th>
    <th><code>pla</code></th>
    <th><code>rfx</code></th>
    <th><code>pal</code></th>
    <th><code>vel</code></th>
    <th><code>uvl</code></th>
    <th><code>phr</code></th>
    <th><code>glt</code></th>
  </tr>
  <tr>
    <th align="right"><code>clk</code></th>
    <td>ʘ</td>
    <td> </td>
    <td>ǀ</td>
    <td>ǃ</td>
    <td> </td>
    <td> </td>
    <td>ǂ</td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <th align="right"><code>lat</code>&#xA0;<code>clk</code></th>
    <td> </td>
    <td> </td>
    <td> </td>
    <td>ǁ</td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <th align="right"><code>vcd</code>&#xA0;<code>imp</code></th>
    <td>ɓ</td>
    <td> </td>
    <td> </td>
    <td>ɗ</td>
    <td> </td>
    <td> </td>
    <td>ʄ</td>
    <td>ɠ</td>
    <td>ʛ</td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <th align="right"><code>ejc</code></th>
    <td>pʼ</td>
    <td> </td>
    <td> </td>
    <td>tʼ</td>
    <td> </td>
    <td>ʈʼ</td>
    <td>cʼ</td>
    <td>kʼ</td>
    <td>qʼ</td>
    <td>ʡʼ</td>
    <td> </td>
  </tr>
  <tr>
    <th align="right"><code>ejc</code>&#xA0;<code>frc</code></th>
    <td>fʼ</td>
    <td> </td>
    <td>θʼ</td>
    <td>sʼ</td>
    <td>ʃʼ</td>
    <td>ʂʼ</td>
    <td> </td>
    <td>xʼ</td>
    <td>χʼ</td>
    <td> </td>
    <td> </td>
  </tr>
  <tr>
    <th align="right"><code>lat</code>&#xA0;<code>ejc</code>&#xA0;<code>frc</code></th>
    <td> </td>
    <td> </td>
    <td> </td>
    <td>ɬʼ</td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
    <td> </td>
  </tr>
</table>

### Other Symbols

| Symbol | Features                |
|--------|-------------------------|
| ʍ      | `vls` `lbv` `apr`       |
| w      | `vcd` `lbv` `apr`       |
| ɥ      | `vcd` `lbp` `apr`       |
| ʜ      | `vls` `epg` `frc`       |
| ʢ      | `vcd` `epg` `frc`       |
| ʡ      | `vls` `epg` `stp`       |
| ɺ      | `vcd` `alv` `lat` `flp` |
| ɧ      | `vls` `vzd` `pla` `frc` |

### Vowels

<table>
  <tr>
    <td></td>
    <th colspan="2"><code>fnt</code></th>
    <th colspan="2"><code>cnt</code></th>
    <th colspan="2"><code>bck</code></th>
  </tr>
  <tr>
    <th align="right"><code>hgh</code></th>
    <td>i</td><td>y</td>
    <td>ɨ</td><td>ʉ</td>
    <td>ɯ</td><td>u</td>
  </tr>
  <tr>
    <th align="right"><code>smh</code></th>
    <td>ɪ</td><td>ʏ</td>
    <td> </td><td> </td>
    <td> </td><td>ʊ</td>
  </tr>
  <tr>
    <th align="right"><code>umd</code></th>
    <td>e</td><td>ø</td>
    <td>ɘ</td><td>ɵ</td>
    <td>ɤ</td><td>o</td>
  </tr>
  <tr>
    <th align="right"><code>mid</code></th>
    <td> </td><td> </td>
    <td>ə</td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>lmd</code></th>
    <td>ɛ</td><td>œ</td>
    <td>ɜ</td><td>ɞ</td>
    <td>ʌ</td><td>ɔ</td>
  </tr>
  <tr>
    <th align="right"><code>sml</code></th>
    <td>æ</td><td> </td>
    <td>ɐ</td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>low</code></th>
    <td>a</td><td>ɶ</td>
    <td> </td><td> </td>
    <td>ɑ</td><td>ɒ</td>
  </tr>
</table>

Symbols to the left are `unr`, and to the right are `rnd`.

__NOTE:__ The `smh` vowels are more `cnt` than the other vowels. However, this
distinction is not needed to classify these vowels, so is not included in the
above table.

## Features

### Consonants

#### Voicing

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `vcd`   | `vcd`       | voiced          |
| `vls`   | `vls`       | voiceless       |

#### Place of Articulation

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `blb`   | `blb`       | bilabial        |
| `lbd`   | `lbd`       | labio-dental    |
| `dnt`   | `dnt`       | dental          |
| `alv`   | `alv`       | alveolar        |
| `pla`   | `pla`       | post-alveolar (palato-alveolar) |
| `rfx`   | `rfx`       | retroflex       |
| `alp`   |             | alveolo-palatal |
| `pal`   | `pal`       | palatal         |
| `vel`   | `vel`       | velar           |
| `uvl`   | `uvl`       | uvular          |
| `phr`   | `phr`       | pharyngeal      |
| `epg`   |             | epiglottal      |
| `glt`   | `glt`       | glottal         |

##### Co-articulation

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `lbv`   | `lbv`       | labial-velar    |
| `lbp`   |             | labial-palatal  |

#### Manner of Articulation

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `stp`   | `stp`       | plosive (stop)  |
| `nas`   | `nas`       | nasal           |
| `trl`   | `trl`       | trill           |
| `flp`   | `flp`       | tap/flap        |
| `frc`   | `frc`       | fricative       |
| `apr`   | `apr`       | approximant     |
| `clk`   | `clk`       | click           |
| `ejc`   | `ejc`       | ejective        |
| `imp`   | `imp`       | implosive       |

The following features can be combined with other manners of articulation:

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `lat`   | `lat`       | lateral         |

### Vowels

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `vwl`   | `vwl`       | vowel           |

#### Height

| Feature | Kirshenbaum | Name                   |
|---------|-------------|------------------------|
| `hgh`   | `hgh`       | close (high)           |
| `smh`   | `smh`       | near-close (semi-high) |
| `umd`   | `umd`       | close-mid (upper-mid)  |
| `mid`   | `mid`       | mid                    |
| `lmd`   | `lmd`       | open-mid (lower-mid)   |
| `sml`   |             | near-open (semi-low)   |
| `low`   | `low`       | open (low)             |

#### Backness

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `fnt`   | `fnt`       | front           |
| `cnt`   | `cnt`       | center          |
| `bck`   | `bck`       | back            |

#### Rounding

| Feature | Kirshenbaum | Name            |
|---------|-------------|-----------------|
| `unr`   | `unr`       | unrounded       |
| `rnd`   | `rnd`       | rounded         |

### Diacritics

#### Syllabicity

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `syl`   | `syl`       | ◌̩   | syllabic        |
| `nsy`   |             | ◌̯   | non-syllabic    |

#### Consonant Release

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `asp`   | `asp`       | ◌ʰ  | aspirated       |
| `nrs`   |             | ◌ⁿ  | nasal release   |
| `lrs`   |             | ◌ˡ  | lateral release |
| `unx`   | `unx`       | ◌̚   | no audible release (unexploded) |

__NOTE:__ The `nrs` and `lrs` features are not defined in Cainteoir Text-to-Speech.

##### Gemination

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

#### Phonation

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `brv`   |             | ◌̤   | breathy voiced  |
| `crv`   |             | ◌̰   | creaky voiced   |
| `fts`   |             | ◌͈   | fortis          |
| `lns`   |             | ◌͉   | lenis           |

The IPA ◌̬ and ◌̥ diacritics (*voiced* and *voiceless*) are used for both
filling spaces in the IPA consonants chart and making the distinction
between *fortis* and *lenis* consonants. Cainteoir Text-to-Speech uses `slv`
(slack voice) and `stv` (stiff voice) for these diacritics. For eSpeak NG,
these diacritics are only used for controlling their voiced/voiceless
property (and thus their position on the IPA code chart), so `vls` and `vcd`
are used for these diacritics.

The extended IPA<sup>\[<a href="#ref7">7</a>\]</sup> ◌͈ and ◌͉ diacritics
are used to specify lesser (`lns`) and greater (`fts`) oral pressure than
the unmodified voiced or voiceless phoneme. This distinction is made by
the Ewe, Tabasaran, Archi, and other languages<sup>\[<a href="#ref8">8</a>\]</sup>.

Where fortis and lenis are used to contrast consonant durations (e.g. in
the Jawoyn, Ojibwe, and Zurich German languages<sup>\[<a href="#ref8">8</a>\]</sup>),
the [length](#length) suprasegmentals are used instead.

#### Articulation

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `dzd`   |             | ◌̪   | dental          |
| `apc`   |             | ◌̺   | apical          |
| `lmn`   |             | ◌̻   | laminal         |
| `lgl`   |             | ◌̼   | linguolabial    |

The following IPA diacritics are only used by eSpeak NG to fill out positions
in the IPA consonant and vowel charts. As such those phonemes are transcribed
according to the features at that position, not using the features at the
location of the base phoneme with a feature for each of the positioning
diacritics.

| IPA | Name            |
|-----|-----------------|
| ◌̟   | advanced        |
| ◌̠   | retracted       |
| ◌̈   | centralized     |
| ◌̽   | mid-centralized |
| ◌̝   | raised          |
| ◌̞   | lowered         |

#### Co-articulation

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `mrd`   |             | ◌̹   | more rounded    |
| `lrd`   |             | ◌̜   | less rounded    |
| `lzd`   | `lzd`       | ◌ʷ  | labialized or labio-velarized |
| `pzd`   | `pzd`       | ◌ʲ  | palatalized     |
| `vzd`   | `vzd`       | ◌ˠ  | velarized       |
| `fzd`   | `fzd`       | ◌ˤ  | pharyngealized  |
| `atr`   |             | ◌̘   | advanced tongue root  |
| `rtr`   |             | ◌̙   | retracted tongue root |
| `nzd`   | `nzd`       | ◌̃   | nasalized       |
| `rzd`   | `rzd`       | ◌˞  | rhoticized      |

__NOTE:__ The IPA supports ◌̴ for velarized or pharynealized consonants. Unicode
has deprecated this combining character, while keeping the combined forms. As
such, only the combined forms are supported, using the `fzd` feature. Cainteoir
Text-to-Speech uses `vfz` for this combining character, but eSpeak NG does not
preserve the distinction between ◌ˤ and ◌̴.

##### Precomposed Characters

These characters are represented as a single Unicode character. They can be
used in place of the base character followed by a co-articulation combining
character.

| Symbol | Features                      |
|--------|-------------------------------|
| ɫ      | `vcd` `fzd` `alv` `lat` `apr` |
| ɚ      | `unr` `mid` `cnt` `rzd` `vwl` |
| ɝ      | `unr` `lmd` `cnt` `rzd` `vwl` |

### Suprasegmentals

#### Stress

| Feature | Kirshenbaum | IPA | Name             |
|---------|-------------|-----|------------------|
| `st1`   |             | ˈ◌  | primary stress   |
| `st2`   |             | ˌ◌  | secondary stress |
| `st3`   |             | ˈˈ◌ | extra stress     |

#### Length

| Feature | Kirshenbaum | IPA | Name            |
|---------|-------------|-----|-----------------|
| `est`   |             | ◌̆   | extra short     |
| `hlg`   |             | ◌ˑ  | half-long       |
| `lng`   | `lng`       | ◌ː  | long            |

#### Rhythm

| Feature | Kirshenbaum | IPA | Name              |
|---------|-------------|-----|-------------------|
| `sbr`   |             | ◌.◌ | syllable break    |
| `lnk`   |             | ◌‿◌ | linked (no break) |

#### Intonation

| Feature | Kirshenbaum | IPA    | Name                     |
|---------|-------------|--------|--------------------------|
| `fbr`   |             | &#124; | minor (foot) break       |
| `ibr`   |             | ‖      | major (intonation) break |
| `glr`   |             | ↗      | global rise              |
| `glf`   |             | ↘      | global fall              |

### Tones

| Tone               | IPA | Start | Middle | End   |
|--------------------|-----|-------|--------|-------|
| extra high (top)   | ◌˥  | `ts5` | `tm5`  | `te5` |
| high               | ◌˦  | `ts4` | `tm4`  | `te4` |
| mid                | ◌˧  | `ts3` | `tm3`  | `te3` |
| low                | ◌˨  | `ts2` | `tm2`  | `te2` |
| extra low (bottom) | ◌˩  | `ts1` | `tm1`  | `te1` |

#### Tone Stepping

| Feature | Kirshenbaum | IPA | Name        |
|---------|-------------|-----|-------------|
| `dst`   |             | ꜛ◌  | downstep    |
| `ust`   |             | ꜜ◌  | upstep      |

### Unused

| Feature   | Kirshenbaum | Name            |
|-----------|-------------|-----------------|
|           | `ctl`       | central         |
|           | `mrm`       | murmured        |
|           | `orl`       | oral            |

__NOTE:__ The `ctl` and `orl` features are specified by Evan Kirshenbaum in
Appendix A of his ASCII/IPA transcription, but are not used in the rest of
the specification.

__NOTE:__ Evan Kirshenbaum transcribes ɦ as `h<?>` (`{mrm,glt,frc}`), while
Wikipedia also lists this as a `{vcd,glt,frc}`<sup>\[<a href="#ref6">6</a>\]</sup>.
This is the only phoneme that Kirshenbaum uses the `mrm` feature for. As such,
the `mrm` feature is not used within this document.

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
