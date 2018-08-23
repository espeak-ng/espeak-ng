# Phoneme Model

- [Manner of Articulation](#manner-of-articulation)
  - [Air Flow](#air-flow)
  - [Initiator](#initiator)
  - [Target](#target)
    - [Co-articulation](#co-articulation)
  - [Manner](#manner)
- [Place of Articulation](#place-of-articulation)
  - [Active Articulators](#active-articulators)
  - [Passive Articulators](#passive-articulators)
  - [Co-articulation](#co-articulation-1)
- [Phonation](#phonation)
  - [Voice](#voice)
- [Vowel Height](#vowel-height)
- [Vowel Backness](#vowel-backness)
- [Rounding and Labialization](#rounding-and-labialization)
  - [Vowel Rounding](#vowel-rounding)
- [Syllabicity](#syllabicity)
- [Consonant Release](#consonant-release)
- [Tongue Root](#tongue-root)
- [Fortis and Lenis](#fortis-and-lenis)
- [Stress](#stress)
- [Length](#length)
- [Rhythm](#rhythm)
- [Intonation](#intonation)
- [Tone Stepping](#tone-stepping)
- [Tones](#tones)
- [References](#references)

----------

Evan Kirshenbaum's feature set used in his ASCII transcription of the
International Phonetic Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\],
\[<a href="#ref2">2</a>\]</sup> describes the phonemes in a way consistent with
how the phonemes are organised in the IPA code chart. That is the approach used
in the [Phonemes](phonemes.md) document to describe the phonemes in a phoneme
definition file.

Those phoneme features often represent the action of more than one articulatory
mechanism used to produce speech, or affect the same area. Internally, espeak-ng
makes use of the articulatory model, not the IPA descriptions. This document
describes how the feature-based IPA model is mapped to the articulatory model.

People working on adding new voices or languages do not need to read this
document, but should instead read the [Phonemes](phonemes.md) document. This is
intended for people working on the espeak-ng codebase, or people interested in
how espeak-ng works internally.

__NOTE:__ This model is in the process of being implemented. As such, the
current implementation does not reflect this document.

## Manner of Articulation

The manner of articulation is described in terms of several distinct feature
types. The possible manners of articulation are:

| Manner of Articulation | Feature   | Phoneme Model         |
|------------------------|-----------|-----------------------|
| nasal                  | `nas`     | `pmc egs nsl occ`     |
| plosive (stop)         | `stp`     | `pmc egs orl occ`     |
| affricate              | `afr`     | `pmc egs orl occ frr` |
| fricative              | `frc`     | `pmc egs orl frv`     |
| tap/flap               | `flp`     | `pmc egs orl fla`     |
| trill                  | `trl`     | `pmc egs orl tri`     |
| approximant            | `apr`     | `pmc egs orl app`     |
| click                  | `clk`     | `vlc igs orl`         |
| ejective               | `ejc`     | `vlc igs orl occ`     |
| implosive              | `imp`     | `gtc igs`             |
| vowel                  | `vwl`     | `pmc egs orl vow`     |

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

### Air Flow

| Feature | Name       | Description                                                       |
|---------|------------|-------------------------------------------------------------------|
| `egs`   | egressive  | The air flow is moving outwards from the initiator to the target. |
| `igs`   | ingressive | The air flow is moving inwards from the target to the initiator.  |

### Initiator

| Feature | Name       | Description                                                           |
|---------|------------|-----------------------------------------------------------------------|
| `pmc`   | pulmonic   | The diaphragm and lungs are used to generate the airstream.           |
| `gtc`   | glottalic  | The glottis is used to generate the airstream.                        |
| `vlc`   | velaric    | The velum is closed and the tongue is used to generate the airstream. |
| `pcv`   | percussive | There is no airstream used to produce this sound.                     |

### Target

| Feature | Name       | Description                                     |
|---------|------------|-------------------------------------------------|
| `nsl`   | nasal      | The air flows through the nose.                 |
| `orl`   | oral       | The air flows through the mouth.                |

#### Co-articulation

| Feature | Name      | Target |
|---------|-----------|--------|
| `nzd`   | nasalized | `nsl`  |

### Manner

| Feature | Name        | Description                                                                       |
|---------|-------------|-----------------------------------------------------------------------------------|
| `occ`   | occlusive   | The air flow is blocked within the vocal tract.                                   |
| `frv`   | fricative   | The air flow is constricted, causing turbulence.                                  |
| `fla`   | flap        | A single tap of the tongue against the secondary articulator.                     |
| `tri`   | trill       | A rapid vibration of the primary articulator against the secondary articulator.   |
| `app`   | approximant | The vocal tract is narrowed at the place of articulation without being turbulant. |
| `vow`   | vowel       | The phoneme is articulated as a vowel instead of a consonant.                     |

## Place of Articulation

The place of articulation is described in terms of an active articulator and
one or more passive articulators<sup>\[<a href="#ref9">9</a>\]</sup>. The
possible places of articulation are:

| Place of Articulation | Feature   | Active | Lips  | Teeth   | Passive |
|-----------------------|-----------|--------|-------|---------|---------|
| bilabial              | `blb`     | `lbl`  | `ulp` |         |         |
| linguolabial          | `lgl`     | `lmn`  | `ulp` |         |         |
| labiodental           | `lbd`     | `lbl`  |       | `utt`   |         |
| bilabial-labiodental  | `bld`     | `bld`  | `ulp` | `utt`   |         |
| interdental           | `idt`     | `lmn`  |       | `utt`   |         |
| dental                | `dnt`     | `apc`  |       | `utt`   |         |
| denti-alveolar        | `dta`     | `lmn`  |       | `utt`   | `alf`   |
| alveolar              | `alv`     | `lmn`  |       |         | `alf`   |
| apico-alveolar        | `apa`     | `apc`  |       |         | `alf`   |
| palato-alveolar       | `pla`     | `lmn`  |       |         | `alb`   |
| apical retroflex      | `arf`     | `sac`  |       |         | `alb`   |
| retroflex             | `rfx`     | `apc`  |       |         | `hpl`   |
| alveolo-palatal       | `alp`     | `dsl`  |       |         | `alb`   |
| palatal               | `pal`     | `dsl`  |       |         | `hpl`   |
| velar                 | `vel`     | `dsl`  |       |         | `spl`   |
| labio-velar           | `lbv`     | `dsl`  | `ulp` |         | `spl`   |
| uvular                | `uvl`     | `dsl`  |       |         | `uvu`   |
| pharyngeal            | `phr`     | `rdl`  |       |         | `prx`   |
| epiglotto-pharyngeal  | `epp`     | `lyx`  |       |         | `prx`   |
| (ary-)epiglottal      | `epg`     | `lyx`  |       |         | `egs`   |
| glottal               | `glt`     | `lyx`  |       |         | `gts`   |

### Active Articulators

| Feature | Name      | Articulator             |
|---------|-----------|-------------------------|
| `lbl`   | labial    | lower lip               |
| `lmn`   | laminal   | tongue blade            |
| `apc`   | apical    | tongue tip              |
| `sac`   | subapical | underside of the tongue |
| `dsl`   | dorsal    | tongue body             |
| `rdl`   | radical   | tongue root             |
| `lyx`   | laryngeal | larynx                  |

### Passive Articulators

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

### Co-articulation

| Feature | Name            | Articulator |
|---------|-----------------|-------------|
| `pzd`   | palatalized     | `hpl`       |
| `vzd`   | velarized       | `spl`       |
| `fzd`   | pharyngealized  | `prx`       |
| `nzd`   | nasalized       | `nsl`       |
| `rzd`   | rhoticized      | `apc hpl`   |

## Phonation

The phonation features describe the degree to which the glottis (vocal chords) are open or closed.

| Feature | Name            | Description                                                                 |
|---------|-----------------|-----------------------------------------------------------------------------|
| `vls`   | voiceless       | The glottis is fully open, such that the vocal chords do not vibrate.       |
| `brv`   | breathy voice   | The glottis is closed slightly, to produce a whispered or murmured sound.   |
| `slv`   | slack voice     | The glottis is opened wider than `mdv`, but not enough to be `brv`.         |
| `mdv`   | modal voice     | The glottis is opened to provide the optimal vibration of the vocal chords. |
| `stv`   | stiff voice     | The glottis is closed narrower than `mdv`, but not enough to be `crv`.      |
| `crv`   | creaky voice    | The glottis is closed to produce a vocal or glottal fry.                    |
| `glc`   | glottal closure | The glottis is fully closed.                                                |

### Voice

| Voice     | Feature | Phoneme Model |
|-----------|---------|---------------|
| voiceless | `vls`   | `vls`         |
| voiced    | `vcd`   | `mdv`         |

## Vowel Height

| Feature | Name                   |
|---------|------------------------|
| `hgh`   | close (high)           |
| `smh`   | near-close (semi-high) |
| `umd`   | close-mid (upper-mid)  |
| `mid`   | mid                    |
| `lmd`   | open-mid (lower-mid)   |
| `sml`   | near-open (semi-low)   |
| `low`   | open (low)             |

## Vowel Backness

| Feature | Name            |
|---------|-----------------|
| `fnt`   | front           |
| `cnt`   | center          |
| `bck`   | back            |

## Rounding and Labialization

| Feature | Name       | Rounded | Position                       |
|---------|------------|---------|--------------------------------|
| `unr`   | unrounded  | No      | Close to the jaw.              |
| `ptr`   | protruded  | Yes     | Protrude outward from the jaw. |
| `cmp`   | compressed | Yes     | Close to the jaw.              |

The degree of rounding/labialization is specified using the following features:

| Feature | Name         |
|---------|--------------|
| `mrd`   | more rounded |
| `lrd`   | less rounded |

### Vowel Rounding

| Rounding  | Feature | Phoneme Model                            |
|-----------|---------|------------------------------------------|
| unrounded | `unr`   | `unr`                                    |
| rounded   | `rnd`   | `ptr` if `bck` or `cnt`; `cmp` if `fnt`. |

## Syllabicity

| Feature | Name         |
|---------|--------------|
| `syl`   | syllabic     |
| `nsy`   | non-syllabic |

## Consonant Release

| Feature | Name                            |
|---------|---------------------------------|
| `frr`   | fricative release               |
| `asp`   | aspirated                       |
| `nrs`   | nasal release                   |
| `lrs`   | lateral release                 |
| `unx`   | no audible release (unexploded) |

## Tongue Root

The tongue root position can be specified using the following features:

| Feature | Symbol | Name                  |
|---------|--------|-----------------------|
| `atr`   | ◌̘      | advanced tongue root  |
| `rtr`   | ◌̙      | retracted tongue root |

## Fortis and Lenis

| Feature | Name   |
|---------|--------|
| `fts`   | fortis |
| `lns`   | lenis  |

## Stress

| Feature | Name             |
|---------|------------------|
| `st1`   | primary stress   |
| `st2`   | secondary stress |
| `st3`   | extra stress     |

## Length

| Feature | Name            |
|---------|-----------------|
| `est`   | extra short     |
| `hlg`   | half-long       |
| `lng`   | long            |

## Rhythm

| Feature | Name              |
|---------|-------------------|
| `sbr`   | syllable break    |
| `lnk`   | linked (no break) |

## Intonation

| Feature | Name                     |
|---------|--------------------------|
| `fbr`   | minor (foot) break       |
| `ibr`   | major (intonation) break |
| `glr`   | global rise              |
| `glf`   | global fall              |

## Tone Stepping

| Feature | Name        |
|---------|-------------|
| `ust`   | upstep      |
| `dst`   | downstep    |

## Tones

Tones are defined using the following 3 properties:

	tone_start  <value>
	tone_middle <value>
	tone_end    <value>

The `<value>` field for these properties is a number with one of the following
values:

| Tone               | `<value>` |
|--------------------|-----------|
| extra high (top)   | `5`       |
| high               | `4`       |
| mid                | `3`       |
| low                | `2`       |
| extra low (bottom) | `1`       |

A *level* tone can be specified by just using the `tone_start` value. A *raising*
or *falling* tone can be specified using the `tone_start` and `tone_end` values.
A *raising-falling* (*peaking*) or *falling-raising* (*dipping*) tone can be
specified using all three values.

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
