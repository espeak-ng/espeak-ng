# Phonemes

- [IPA Phonemes](#ipa-phonemes)
  - [Consonants (Pulmonic)](#consonants-pulmonic)
  - [Consonants (Non-Pulmonic)](#consonants-non-pulmonic)
  - [Other Symbols](#other-symbols)
  - [Gemination](#gemination)
  - [Vowels](#vowels)
  - [Positioning Diacritics](#positioning-diacritics)
- [Features](#features)
  - [Manner of Articulation](#manner-of-articulation)
    - [Air Flow](#air-flow)
    - [Initiator](#initiator)
    - [Target](#target)
    - [Manner](#manner)
  - [Phonation](#phonation)
  - [Place of Articulation](#place-of-articulation)
    - [Active Articulators](#active-articulators)
    - [Passive Articulators](#passive-articulators)
  - [Rounding and Labialization](#rounding-and-labialization)
  - [Vowel Height](#vowel-height)
  - [Vowel Backness](#vowel-backness)
  - [Syllabicity](#syllabicity)
  - [Consonant Release](#consonant-release)
  - [Fortis and Lenis](#fortis-and-lenis)
  - [Co-articulation](#co-articulation-1)
  - [Suprasegmentals](#suprasegmentals)
    - [Stress](#stress)
    - [Length](#length)
    - [Rhythm](#rhythm)
    - [Intonation](#intonation)
  - [Tone Stepping](#tone-stepping)
- [Properties](#properties)
  - [Tones](#tones)
- [References](#references)

----------

Evan Kirshenbaum created an ASCII transcription of the International Phonetic
Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\], \[<a href="#ref2">2</a>\]</sup>.
As well as using ASCII characters for specific IPA phonemes, this transcription
provides a set of 3-letter feature abbreviations allowing a phoneme to be
described as a sequence of features.

Kirshenbaum uses features that align with the way the IPA phoneme charts are
organised. This document uses the Kirshenbaum features as a base to work from,
but extends them to describe the underlying phonetic processes in a way that
is consistent between languages and voices.

This document is grouped into three sections. The first section displays the IPA
charts using the feature names instead of their names, showing the IPA phoneme
at that position in the chart. This makes it easier to look up the features for
a given IPA phoneme.

The second section lists the features and their associated name. The Wikipedia
IPA<sup>\[<a href="#ref4">4</a>\]</sup> article can be used as a starting point
into the various phonetic topics contained in this document.

The third section describes the properties (named values) used to describe the
phonemes. These, in addition to the features, should allow all possible phonemes
from any language to be described.

The goal of this document is not to provide a detailed guide on phonetics.
Instead, it is designed to be a transcription guide on how to specify phonemes
in a language or voice so that the narrow transcriptions are consistent between
the two.

## IPA Phonemes

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
    <td>ʡ</td><td> </td>
    <td>ʔ</td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>sib</code>&#xA0;<code>afr</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>t͡s</td><td>d͡z</td>
    <td>t͡ʃ</td><td>d͡ʒ</td>
    <td>ʈ͡ʂ</td><td>ɖ͡ʐ</td>
    <td>t͡ɕ</td><td>d͡ʑ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>afr</code></th>
    <td>p͡ɸ</td><td>b͡β</td>
    <td>p̪͡f</td><td>b̪͡v</td>
    <td>t͡θ</td><td>d͡ð</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>c͡ç</td><td>ɟ͡ʝ</td>
    <td>k͡x</td><td>ɡ͡ɣ</td>
    <td>q͡χ</td><td>ɢ͡ʁ</td>
    <td>ʡ͡ħ</td><td>ʡ͡ʕ</td>
    <td>ʔ͡h</td><td></td>
  </tr>
  <tr>
    <th align="right"><code>lat</code>&#xA0;<code>afr</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>t͡ɬ</td><td>d͡ɮ</td>
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
    <th align="right"><code>sib</code>&#xA0;<code>frc</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td>s</td><td>z</td>
    <td>ʃ</td><td>ʒ</td>
    <td>ʂ</td><td>ʐ</td>
    <td>ɕ</td><td>ʑ</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
  </tr>
  <tr>
    <th align="right"><code>frc</code></th>
    <td>ɸ</td><td>β</td>
    <td>f</td><td>v</td>
    <td>θ</td><td>ð</td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
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
    <th align="right"><code>lat</code>&#xA0;<code>flp</code></th>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td> </td>
    <td> </td><td>ɺ</td>
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
    <td>ʜ</td><td>ʢ</td>
    <td> </td><td> </td>
  </tr>
</table>

Symbols to the left have a `vls` phonation, and to the right have `mdv` phonation.

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
    <th align="right"><code>mdv</code>&#xA0;<code>imp</code></th>
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

| Symbol | Alternative | Features                |
|--------|-------------|-------------------------|
| ʍ      | ɰ̊ʷ           | `vls` `vel` `ptr` `apr` |
| w      | ɰʷ          | `mdv` `vel` `ptr` `apr` |
| ɥ      | jʷ          | `mdv` `pal` `ptr` `apr` |
| ɧ      |             | `vls` `vzd` `pla` `frc` |
| ɫ      |             | `mdv` `fzd` `alv` `lat` `apr` |
| ɚ      |             | `unr` `mid` `cnt` `rzd` `vwl` |
| ɝ      |             | `unr` `lmd` `cnt` `rzd` `vwl` |
| k͡p    |             | `vls` `lbv` `stp`       |
| ɡ͡b    |             | `mdv` `lbv` `stp`       |
| ŋ͡m    |             | `mdv` `lbv` `stp`       |
| p͡f    |             | `vls` `bld` `afr`       |
| b͡v    |             | `mdv` `bld` `afr`       |

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

#### Positioning Diacritics

The following IPA diacritics are only used by eSpeak NG to fill out positions
in the IPA consonant and vowel charts. As such those phonemes are transcribed
according to the features at that position, not using the features at the
location of the base phoneme with a feature for each of the positioning
diacritics.

| Symbol | Name            |
|--------|-----------------|
| ◌̟      | advanced        |
| ◌̠      | retracted       |
| ◌̈      | centralized     |
| ◌̽      | mid-centralized |
| ◌̝      | raised          |
| ◌̞      | lowered         |

## Features

### Manner of Articulation

The manner of articulation is described in terms of several distinct feature
types. The possible manners of articulation are:

| Manner of Articulation | Feature   | Symbol | Features              |
|------------------------|-----------|--------|-----------------------|
| nasal                  | `nas`     |        | `pmc egs nsl occ`     |
| plosive (stop)         | `stp`     |        | `pmc egs orl occ`     |
| affricate              | `afr`     |        | `pmc egs orl occ frr` |
| fricative              | `frc`     |        | `pmc egs orl frv`     |
| tap/flap               | `flp`     |        | `pmc egs orl fla`     |
| trill                  | `trl`     |        | `pmc egs orl tri`     |
| approximant            | `apr`     |        | `pmc egs orl app`     |
| click                  | `clk`     |        | `vlc igs orl`         |
| ejective               | `ejc`     |        | `vlc igs orl occ`     |
| implosive              | `imp`     | ◌ʼ     | `gtc igs`             |
| vowel                  | `vwl`     |        | `pmc egs orl vow`     |

The features for these manners of articulation are provided for convenience,
and to make it easier to describe the IPA consonants. Internally, the
distinct feature types are used.

For `imp` consonants, they use the features of the base phoneme except for
the `pmc` and `egs` features. Thus, a `nas imp` is a `gtc igs nsl occ`.

The `vwl` phonemes are described using vowel height and backness features,
while consonants (the other manners of articulation) are described using
place of articulation features.

Additionally, the manner of articulation can be refined using the following
features:

| Feature | Name     | Description                                                 |
|---------|----------|-------------------------------------------------------------|
| `lat`   | lateral  | The air flow is directed along the sides of the tongue.     |
| `sib`   | sibilant | The air flow is directed through the teeth with the tongue. |

#### Air Flow

| Feature | Symbol | Name       | Description                                                       |
|---------|--------|------------|-------------------------------------------------------------------|
| `egs`   | ↑      | egressive  | The air flow is moving outwards from the initiator to the target. |
| `igs`   | ↓      | ingressive | The air flow is moving inwards from the target to the initiator.  |

The ↑ and ↓ symbols are from the extended IPA<sup>\[<a href="#ref7">7</a>\]</sup>.
They only need to be used when the air flow is different to the base IPA
phoneme (e.g. using ↓ on pulmonic consonants).

#### Initiator

| Feature | Name       | Description                                                           |
|---------|------------|-----------------------------------------------------------------------|
| `pmc`   | pulmonic   | The diaphragm and lungs are used to generate the airstream.           |
| `gtc`   | glottalic  | The glottis is used to generate the airstream.                        |
| `vlc`   | velaric    | The velum is closed and the tongue is used to generate the airstream. |
| `pcv`   | percussive | There is no airstream used to produce this sound.                     |

#### Target

| Feature | Name       | Description                                     |
|---------|------------|-------------------------------------------------|
| `nsl`   | nasal      | The air flows through the nose.                 |
| `orl`   | oral       | The air flows through the mouth.                |

#### Manner

| Feature | Name        | Description                                                                       |
|---------|-------------|-----------------------------------------------------------------------------------|
| `occ`   | occlusive   | The air flow is blocked within the vocal tract.                                   |
| `frv`   | fricative   | The air flow is constricted, causing turbulence.                                  |
| `fla`   | flap        | A single tap of the tongue against the secondary articulator.                     |
| `tri`   | trill       | A rapid vibration of the primary articulator against the secondary articulator.   |
| `app`   | approximant | The vocal tract is narrowed at the place of articulation without being turbulant. |
| `vow`   | vowel       | The phoneme is articulated as a vowel instead of a consonant.                     |

### Phonation

The phonation features describe the degree to which the glottis (vocal chords) are open or closed.

| Feature | Symbol | Name            | Description                                                                 |
|---------|--------|-----------------|-----------------------------------------------------------------------------|
| `vls`   |        | voiceless       | The glottis is fully open, such that the vocal chords do not vibrate.       |
| `brv`   | ◌̤      | breathy voice   | The glottis is closed slightly, to produce a whispered or murmured sound.   |
| `slv`   | ◌̥      | slack voice     | The glottis is opened wider than `mdv`, but not enough to be `brv`.         |
| `mdv`   |        | modal voice     | The glottis is opened to provide the optimal vibration of the vocal chords. |
| `stv`   | ◌̬      | stiff voice     | The glottis is closed narrower than `mdv`, but not enough to be `crv`.      |
| `crv`   | ◌̰      | creaky voice    | The glottis is closed to produce a vocal or glottal fry.                    |
| `glc`   | ʔ͡◌    | glottal closure | The glottis is fully closed.                                                |

The IPA ◌̥ diacritic is also used to fill the `vls` spaces in the IPA consonant
charts. Thus, when ◌̥ is used with a `mdv` consonant that does not have an
equivalent `vls` consonant, the resulting consonant is `vls`, not `slv`.

### Place of Articulation

The place of articulation is described in terms of an active articulator and
one or more passive articulators<sup>\[<a href="#ref9">9</a>\]</sup>. The
possible places of articulation are:

| Place of Articulation | Feature   | Symbol | Active | Lips  | Teeth   | Passive |
|-----------------------|-----------|--------|--------|-------|---------|---------|
| bilabial              | `blb`     |        | `lbl`  | `ulp` |         |         |
| linguolabial          | `lgl`     | ◌̼      | `lmn`  | `ulp` |         |         |
| labiodental           | `lbd`     | ◌̪      | `lbl`  |       | `utt`   |         |
| bilabial-labiodental  | `bld`     |        | `bld`  | `ulp` | `utt`   |         |
| interdental           | `idt`     | ◌̪͆      | `lmn`  |       | `utt`   |         |
| dental                | `dnt`     | ◌̪      | `apc`  |       | `utt`   |         |
| denti-alveolar        | `dta`     |        | `lmn`  |       | `utt`   | `alf`   |
| alveolar              | `alv`     |        | `lmn`  |       |         | `alf`   |
| apico-alveolar        | `apa`     | ◌̺      | `apc`  |       |         | `alf`   |
| palato-alveolar       | `pla`     |        | `lmn`  |       |         | `alb`   |
| apical retroflex      | `arf`     |        | `sac`  |       |         | `alb`   |
| retroflex             | `rfx`     | ◌̺      | `apc`  |       |         | `hpl`   |
| alveolo-palatal       | `alp`     |        | `dsl`  |       |         | `alb`   |
| palatal               | `pal`     |        | `dsl`  |       |         | `hpl`   |
| velar                 | `vel`     |        | `dsl`  |       |         | `spl`   |
| labio-velar           | `lbv`     |        | `dsl`  | `ulp` |         | `spl`   |
| uvular                | `uvl`     |        | `dsl`  |       |         | `uvu`   |
| pharyngeal            | `phr`     |        | `rdl`  |       |         | `prx`   |
| epiglotto-pharyngeal  | `epp`     |        | `lyx`  |       |         | `prx`   |
| (ary-)epiglottal      | `epg`     |        | `lyx`  |       |         | `egs`   |
| glottal               | `glt`     |        | `lyx`  |       |         | `gts`   |

The features for these places of articulation are provided for convenience, and
to make it easier to describe the IPA consonants. Internally, the active and
passive articulators are used.

The ◌̪ diacritic is `lbd` when used on `blb` consonants, and `dnt` when used on
`alv` consonants.

The `bld` place of articulation is used for `afr` consonants that have a `blb`
onset and a `lbd` release, e.g. in the p͡f consonant.

The `alv` consonant is `lmn` as found in French and Spanish, while `apa` is
`apc` as found in English, as such ◌̻ (*laminal*) is not needed.

__NOTE:__ The IPA charts make a distinction between pharyngeal and epiglottal
consonants, but Wikipedia does not. This model uses the Wikipedia descriptions.

#### Active Articulators

| Feature | Name      | Articulator             |
|---------|-----------|-------------------------|
| `lbl`   | labial    | lower lip               |
| `lmn`   | laminal   | tongue blade            |
| `apc`   | apical    | tongue tip              |
| `sac`   | subapical | underside of the tongue |
| `dsl`   | dorsal    | tongue body             |
| `rdl`   | radical   | tongue root             |
| `lyx`   | laryngeal | larynx                  |

#### Passive Articulators

| Feature | Articulator            |
|---------|------------------------|
| `ulp`   | upper lip              |
| `utt`   | upper teeth            |
| `alf`   | alveolar ridge (front) |
| `alb`   | alveolar ridge (back)  |
| `hpl`   | hard palate            |
| `spl`   | soft palate (velum)    |
| `uvu`   | uvular                 |
| `prx`   | pharynx                |
| `egs`   | epiglottis             |
| `gts`   | glottis                |

### Rounding and Labialization

| Feature | Symbol | Name       | Rounded | Position                                |
|---------|--------|------------|---------|-----------------------------------------|
| `unr`   |        | unrounded  | No      | Close to the jaw.                       |
| `ptr`   | ◌ʷ, ◌ᶣ | protruded  | Yes     | Protrude outward from the jaw.          |
| `cmp`   | ◌ᵝ     | compressed | Yes     | Close to the jaw.                       |
| `rnd`   |        | rounded    | Yes     | `ptr` if `bck` or `cnt`; `cmp` if `fnt` |

The `unr` and `rnd` features are used for vowels to describe their default
labialization. Consonants are `unr` by default, and can use the ◌ʷ, ◌ᶣ and ◌ᵝ
annotations to specify the type of labialization. Vowels can use these to
change their labialization from the default one specified by `rnd`.

Additionally, the degree of rounding/labialization can be specified using the
following features:

| Feature | Symbol | Name         |
|---------|--------|--------------|
| `mrd`   | ◌̹      | more rounded |
| `lrd`   | ◌̜      | less rounded |

### Vowel Height

| Feature | Name                   |
|---------|------------------------|
| `hgh`   | close (high)           |
| `smh`   | near-close (semi-high) |
| `umd`   | close-mid (upper-mid)  |
| `mid`   | mid                    |
| `lmd`   | open-mid (lower-mid)   |
| `sml`   | near-open (semi-low)   |
| `low`   | open (low)             |

### Vowel Backness

| Feature | Name            |
|---------|-----------------|
| `fnt`   | front           |
| `cnt`   | center          |
| `bck`   | back            |

### Syllabicity

| Feature | Symbol | Name            |
|---------|--------|-----------------|
| `syl`   | ◌̩      | syllabic        |
| `nsy`   | ◌̯      | non-syllabic    |

### Consonant Release

| Feature | Symbol | Name                            |
|---------|--------|---------------------------------|
| `frr`   |        | fricative release               |
| `asp`   | ◌ʰ     | aspirated                       |
| `nrs`   | ◌ⁿ     | nasal release                   |
| `lrs`   | ◌ˡ     | lateral release                 |
| `unx`   | ◌̚      | no audible release (unexploded) |

### Co-articulation

| Feature | Symbol | Name            | Co-Articulator | Type                  |
|---------|--------|-----------------|----------------|-----------------------|
| `pzd`   | ◌ʲ     | palatalized     | `hpl`          | Passive Articulator   |
| `vzd`   | ◌ˠ     | velarized       | `spl`          | Passive Articulator   |
| `fzd`   | ◌ˤ     | pharyngealized  | `prx`          | Passive Articulator   |
| `nzd`   | ◌̃      | nasalized       | `nsl`          | Target                |
| `rzd`   | ◌˞     | rhoticized      | `rfx`          | Place of Articulation |

Additionally, the tongue root position can be specified using the following
features:

| Feature | Symbol | Name                  |
|---------|--------|-----------------------|
| `atr`   | ◌̘      | advanced tongue root  |
| `rtr`   | ◌̙      | retracted tongue root |

### Fortis and Lenis

| Feature |Symbol | Name   |
|---------|-------|--------|
| `fts`   | ◌͈     | fortis |
| `lns`   | ◌͉     | lenis  |

The extended IPA<sup>\[<a href="#ref7">7</a>\]</sup> ◌͈ and ◌͉ diacritics
are used to specify lesser (`lns`) and greater (`fts`) oral pressure than
the unmodified voiced or voiceless phoneme. This distinction is made by
the Ewe, Tabasaran, Archi, and other languages<sup>\[<a href="#ref8">8</a>\]</sup>.

Where fortis and lenis are used to contrast consonant durations (e.g. in
the Jawoyn, Ojibwe, and Zurich German languages<sup>\[<a href="#ref8">8</a>\]</sup>),
the [length](#length) suprasegmentals are used instead.

### Suprasegmentals

#### Stress

| Feature | Symbol | Name             |
|---------|--------|------------------|
| `st1`   | ˈ◌     | primary stress   |
| `st2`   | ˌ◌     | secondary stress |
| `st3`   | ˈˈ◌    | extra stress     |

#### Length

| Feature | Symbol | Name            |
|---------|--------|-----------------|
| `est`   | ◌̆      | extra short     |
| `hlg`   | ◌ˑ     | half-long       |
| `lng`   | ◌ː     | long            |

#### Rhythm

| Feature | Symbol | Name              |
|---------|--------|-------------------|
| `sbr`   | ◌.◌    | syllable break    |
| `lnk`   | ◌‿◌    | linked (no break) |

#### Intonation

| Feature | Symbol | Name                     |
|---------|--------|--------------------------|
| `fbr`   | &#124; | minor (foot) break       |
| `ibr`   | ‖      | major (intonation) break |
| `glr`   | ↗      | global rise              |
| `glf`   | ↘      | global fall              |

#### Tone Stepping

| Feature | Symbol | Name        |
|---------|--------|-------------|
| `dst`   | ꜛ◌     | downstep    |
| `ust`   | ꜜ◌     | upstep      |

## Properties

### Tones

Tones are defined using the following 3 properties:

	tone_start  <value>
	tone_middle <value>
	tone_end    <value>

The `<value>` field for these properties is a number with one of the following
values:

| Tone               | Symbol | `<value>` |
|--------------------|--------|-----------|
| extra high (top)   | ◌˥     | `5`       |
| high               | ◌˦     | `4`       |
| mid                | ◌˧     | `3`       |
| low                | ◌˨     | `2`       |
| extra low (bottom) | ◌˩     | `1`       |

A *level* tone can be specified by just using the `tone_start` value. A *raising*
or *falling* tone can be specified using the `tone_start` and `tone_end` values.
A *raising-falling* (*peaking*) or *falling-raising* (*dipping*) tone can be
specified using all three values.

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
