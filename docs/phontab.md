# Phoneme Tables

- [Phoneme files](#phoneme-files)
- [Phoneme Definitions](#phoneme-definitions)
- [Phoneme Properties](#phoneme-properties)
  - [Type](#type)
  - [Properties](#properties)
  - [starttype](#starttype)
  - [endtype](#endtype)
  - [lengthmod](#lengthmod)
  - [voicingswitch](#voicingswitch)
- [Phoneme Instructions](#phoneme-instructions)
  - [length](#length)
  - [ipa](#ipa)
  - [WAV](#wav)
  - [FMT](#fmt)
  - [VowelStart](#vowelstart)
  - [VowelEnding](#vowelending)
  - [Vowelin](#vowelin)
  - [Vowelout](#vowelout)
  - [ChangePhoneme](#changephoneme)
  - [ChangeIfDiminished](#changeifdiminished)
  - [ChangeIfUnstressed](#changeifunstressed)
  - [ChangeIfNotStressed](#changeifnotstressed)
  - [ChangeIfStressed](#changeifstressed)
  - [IfNextVowelAppend](#ifnextvowelappend)
  - [RETURN](#return)
  - [CALL](#call)
- [Conditional Statements](#conditional-statements)
  - [Conditions](#conditions)
  - [Attributes](#attributes)
- [Sound Specifications](#sound-specifications)
- [Vowel Transitions](#vowel-transitions)
- [Customization of sound source files](#customization-of-sound-source-files)

----------

A phoneme table defines all the phonemes which are used by a language,
together with their properties and the data for their production as
sounds.

Generally each language has its own phoneme table, although additional
phoneme tables can be used for different voices within the language.
These alternatives are referenced from Voice files.

A phoneme table does not need to define all the phonemes used by a
language. It can inherit the phonemes from a previously defined phoneme
table. For example, a phoneme table may redefine (or add) some of the
vowels that it uses, but inherit most of its consonants from a standard
set.

The source files for the phoneme data are in the "phsource" directory.
"Vowel files", which are referenced in `FMT()`, `VowelStart()`, and `VowelEnding()`
instructions are made using the `espeakedit` program which is not part of this project.
See more in [Customization of sound source files](#customization-of-sound-source-files).

## Phoneme Files

The phoneme tables are defined in a master phoneme file, named
`phonemes`. This starts with the `base` phoneme table followed by
phoneme tables for other languages and voices. These inherit phonemes
from the `base` table or previously defined tables.

In addition to phoneme definitions, the phoneme file can contain the
following:

	include <filename>

Includes the text of the specified file at this point. This allows
different phoneme tables to be kept in different text files, for
convenience. \<filename\> is a relative path. The included file can
itself contain `include` statements.

	phonemetable <name> <parent>

Starts a new phoneme table, and ends the previous table.

\<name\> Is the name of this phoneme table. This name is used in Voice files.

\<parent\> Is the name of a previously defined phoneme table whose phoneme
definitions are inherited by this one. The name `base` indicates the first
(base) phoneme table. The name `_` indicates no parent phoneme table.

## Phoneme Definitions

A phoneme table contains a list of phoneme definitions. Each starts with
the keyword `phoneme` and the phoneme name (this is the name used in
the pronunciation rules in a language's \*\_rules and \*\_list files),
and ends with the keyword `endphoneme`.

For example:

	phoneme aI
	  vwl
	  starttype #a endtype #i
	  length 230
	  FMT(vowels/ai)
	endphoneme

	phoneme s
	  vls alv sib frc
	  voicingswitch z
	  lengthmod 3
	  Vowelin  f1=0  f2=1700 -300 300  f3=-100 80
	  Vowelout f1=0  f2=1700 -300 250  f3=-100 80  rms=20

	  IF nextPh(isPause) THEN
	    WAV(ufric/s_)
	  ELIF nextPh(p) OR nextPh(t) OR nextPh(k) THEN
	    WAV(ufric/s!)
	  ENDIF
	  WAV(ufric/s)
	endphoneme

A phoneme definition contains both static properties and executed
instructions. The instructions may contain conditional statements, so
that the effect of the phoneme may be different depending on adjacent
phonemes, whether the syllable is stressed, etc.

The instructions of a phoneme are interpreted in two different phases.
In the first phase, the instructions may change the phoneme and replace
it by a different phoneme. In the second phase, instructions are used to
produce the sound for the phoneme.

The `import_phoneme` statement can be used to copy a previously
defined phoneme from a specified phoneme table. For example:

	phoneme t
	  import_phoneme base/t[
	endphoneme

means: `phoneme t` in this phoneme table is a copy of`phoneme t[` from phoneme
table `base`. A `length` instruction can be used after `import\_phoneme` to
vary the length from the original.

## Phoneme Properties

The phoneme features are described in the [Phonemes](phonemes.md) document. These
should be included within the `phoneme` to fully describe that phoneme.

Within the phoneme definition the following lines may also occur: (`(V)` indicates
only for vowels, `(C)` only for consonants).

### Type

One of these or a manner of articulation from the [Phonemes](phonemes.md) document
must be present.

| Type      | Description |
|-----------|-------------|
| `liquid`  | semi-vowels, such as:  `r`, `l`, `j`, `w` |
| `pause`   |             |
| `stress`  | Used for stress symbols, eg: `'` `,` `=` `%` |
| `virtual` | Used to represent a class of phonemes. |

### Properties

| Property     | Type | Description |
|--------------|------|-------------|
| `rhotic`     | (C)  | An `r` type consonant. |
| `unstressed` | (V)  | This vowel is always unstressed, unless explicitly marked otherwise. |
| `nolink`     |      | Prevent any linking from the previous phoneme. |
| `nopause`    |      | Used in a `liquid` or `nasal` phoneme to prevent eSpeak NG inserting a short pause if a word starts with this phoneme and the previous word ends with a vowel. |
| `trill`      | (C)  | Apply trill to the voicing. |

### starttype

	starttype <phoneme>

Allocates this phoneme to a group so that conditions such as `nextPh(#e)` can
test for any of a group of phonemes. Pre-defined groups for use for vowels are:
`#@` `#a` `#e` `#i` `#o` `#u`. Additional groups can be defined as phonemes
with type `virtual`.

### endtype

	endtype <phoneme>

Allocates this phoneme to a group so that conditions such as `prevPh(#e)` can
test for any of a group of phonemes. Pre-defined groups for use for vowels are:
`#@` `#a` `#e` `#i` `#o` `#u`. Additional groups can be defined as phonemes
with type `virtual`.

### lengthmod

	lengthmod <integer>

(C) Determines how this consonant affects the length of the previous vowel.

This value is used as index into the `length_mods` table in the `CalcLengths()`
function in the eSpeak NG program.

### voicingswitch

	voicingswitch <phoneme>

This is used for some languages to change between voiced and unvoiced phonemes.

## Phoneme Instructions

Phoneme Instructions may be included within conditional statements.

During the first phase of phoneme interpretation, an instruction which
causes a change to a different phoneme will terminate the instructions.
During the second phase, `FMT()` and `WAV()` instructions will terminate the
instructions.

### length

	length <length>

The relative length of the phoneme in milliseconds. Typical values are about 
140 for a short vowel and from 200 to 300 for a long vowel or a diphthong. 
A `length()` instruction is needed for vowels. It is optional for consonants.

### ipa

	ipa <ipa string>

In many cases, eSpeak NG makes IPA (International Phonetic Alphabet) phoneme
names automatically from eSpeak NG phoneme names. If this is not correct, then
the phoneme definition can include an `ipa` instruction to specify the correct
IPA name. IPA strings may include non-ascii characters. They may also include
characters specified by their character codes in the form `U+` followed by 4
hexadecimal digits. For example a string: `aU+0303` indicates 'a' with a
'combining tilde'.

### WAV

	WAV(<wav file>, <amplitude>)

\<wav file\> is a path to a WAV file (22 kHz, 16 bits, mono) within `phsource/`
which will be played to produce the sound. This method is used for unvoiced
consonants. \<wavefile\> does not include a .WAV filename extension, although
the file to which it refers may or may not have one.

\<amplitude\> is optional. It is a percentage change to the amplitude of the
WAV file. So, `WAV(ufric/s, 50)` means: play file 'ufric/s.wav' at 50% amplitude.
Default value is 100.

### FMT

	FMT(<vowel file>, <amplitude>)

\<vowel file\> is a path to a file (within `phsource/`) which defines how to
generate the sound (a vowel or voiced consonant) from a sequence of formant
values. Vowel files are made using the `espeakedit` program which is not part
of this project.

\<amplitude\> is optional. It is a percentage change to the amplitude of the
sound which is synthesized from the `FMT()` instruction. Default value is 100.

	FMT(<vowel file>, <amplitude>) addWav(<wav file>, <amplitude>)

For voiced consonants, a `FMT()` instruction may be followed by an `addWav()`
instruction. `addWav()` has the same format as a `WAV()` instruction, but the
WAV file is mixed with the sound which is synthesized from the `FMT()` instruction.

### VowelStart

	VowelStart(<vowel file>, <length adjust>)

This is used to modify the start of a vowel when it follows after a sonorant consonant
(such as `[l]` or `[j]`). It replaces the first frame of the \<vowel file\> which
is specified in a `FMT()` instruction by this \<vowel file\>, and adjusts the
length of the original by a signed value \<length adjust\>. The `VowelStart()`
instruction may be specified either in the phoneme definition of the vowel, or
in the phoneme definition of the sonorant consonant which precedes the vowel.
The former takes precedence.

### VowelEnding

	VowelEnding(<vowel file>, <length adjust>)

This is used to modify the end of a vowel when it is followed by a sonorant
consonant (such as `[l]` or `[j]`). It is appended to the \<vowel file\> which
is specified in a `FMT()` instruction by this \<vowel file\>, and adjusts the
length of the original by a signed value \<length adjust\>. The `VowelEnding()`
instruction may be specified either in the phoneme definition of the vowel, or
in the phoneme definition of the sonorant consonant which follows the vowel.
The former takes precedence.

### Vowelin

	Vowelin <vowel transition data>

(C) Specifies the effects of this consonant on the formants of a following
vowel. See [vowel transitions](#vowel-transitions).

### Vowelout

	Vowelout <vowel transition data>

(C) Specifies the effects of this consonant on the formants of a preceding
vowel. See [vowel transitions](#vowel-transitions).

### ChangePhoneme

	ChangePhoneme(<phoneme>)

Change to the specified phoneme.

### ChangeIfDiminished

	ChangeIfDiminished(<phoneme>)

Change to the specified phoneme (such as schwa, `@`) if this syllable has
"diminished" stress.

### ChangeIfUnstressed

	ChangeIfUnstressed(<phoneme>)

Change to the specified phoneme if this syllable has "diminished" or
"unstressed" stress.

### ChangeIfNotStressed

	ChangeIfNotStressed(<phoneme>)

Change to the specified phoneme if this syllable does not have "primary" stress.

### ChangeIfStressed

	ChangeIfStressed(<phoneme>)

Change to the specified phoneme if this syllable has "primary" stress.

### IfNextVowelAppend

	IfNextVowelAppend(<phoneme>)

If the following phoneme is a vowel then this additional phoneme will be
inserted before it. Usually it is short pause to distinguish two vowels from
diphthongs.

### RETURN

Ends executions of instructions.

### CALL

	CALL <phoneme table>/<phoneme>

Executes the instructions of the specified phoneme.

## Conditional Statements

Phoneme definitions can contain conditional statements such as:

	IF <condition> THEN
	    <statements>
	ENDIF

or more generally:

	IF <condition> THEN
	    <statements>
	ELIF <condition> THEN
	    <statements>
	...
	ELSE
	    <statements>
	ENDIF

where the `ELSE` and multiple `ELSE` parts are optional.

Multiple conditions may be joined with `AND` or `OR`, but not a mixture of
`AND`s and `OR`s.

A condition may be preceded by `NOT`. For example:

	IF <condition> AND NOT <condition> THEN
	    <statements>
	ENDIF

### Conditions

Conditions can be:

	thisPh(<attribute>)

Test this current phoneme

	prevPh(<attribute>)

Test the previous phoneme

	prevPhW(<attribute>)

Test the previous phoneme, but only within the same word. Returns false if
there is no previous phoneme in the word.

	prev2PhW(<attribute>)

Test the phoneme before the previous phoneme, but only within the same word.
Returns false if it is not in this word.

	nextPh(<attribute>)

Test the following phoneme

	next2Ph(<attribute>)

Test the phoneme after the next phoneme.

	nextPhW(<attribute>)

Test the next phoneme, but only within the same word. Returns false if there
is no following phoneme in the word.

	next2PhW(<attribute>)

Test the phoneme after the next phoneme, but only within the same word. Returns
false if not found before the word end.

	next3PhW(<attribute>)

Test the third phoneme after the current phoneme, but only within the same word.
Returns false if not found before the word end.

	nextVowel(<attribute>)

Test the next vowel after the current phoneme, but only within the same word.
Returns false if there is none.

	prevVowel(<attribute>)

Test the previous vowel before the current phoneme, but only within the same
word. Returns false if there is none.

	PreVoicing()

This is used as part of the instructions for voiced stop consonants (e.g. `[d]`
and `[g]`). If true then produce a voiced murmur before the stop.

	KlattSynth()

Returns true if the voice is using the Klatt synthesizer rather than the eSpeak synthesizer.

### Attributes

	<phoneme name>

True if the phoneme has this phoneme name.

	<phoneme group>

True if the phoneme has this `starttype` (or if it has this `endtype` if it is
used in `prevPh()`). The pre-defined phoneme groups are `#@`, `#a`, `#e`, `#i`,
`#o`, `#u`.

	isPause

True if the phoneme is a pause.

	isPause2

`nextPh(isPause2)` is used to test whether the next phoneme is not a vowel or
liquid consonant within the same word.

	isVowel
	isNotVowel
	isLiquid
	isNasal
	isVFricative

These test the phoneme type.

	isPalatal
	isRhotic

These test whether the phoneme has this property.

	isWordStart
	notWordStart

These text whether this is the first phoneme in a word.

	isWordEnd

True if this is the final phoneme in a word.

	isFirstVowel
	isSecondVowel
	isFinalVowel

True if this is the First, Second, or Last vowel in a word.

	isAfterStress

True if this phoneme is after the stressed vowel in a word.

	isVoiced

True if this phoneme is a vowel or a voiced consonant.

	isDiminished

True if the syllable stress is "diminished"

	isUnstressed

True if the syllable stress is "diminished" or "unstressed"

	isNotStressed

True if the syllable stress is not "primary stress".

	isStressed

True if the syllable stress is "primary stress".

	isMaxStress

True if this is the highest stressed syllable in the word.

## Sound Specifications

There are three ways to produce sounds:

* Playing a WAV file, by using a `WAV()` instruction. This is used for unvoiced
  consonants such as `[p]`, `[t]` and `[s]`.
* Generating a wave from a sequence of formant parameters, by using a `FMT()`
  instruction. This is used for vowels and also for sonorants such as `[l]`,
  `[j]` and `[n]`.
* A mixture of these. A stored `WAV` file is mixed with a wave generated from
  formant parameters. Use a `FMT()` instruction followed by `addWav()`. This is
  used for voiced stops and fricatives such as `[b]`, `[g]`, `[v]` and `[z]`.

## Vowel Transitions

These specify how a consonant affects an adjacent vowel. A consonant may
cause a transition in the vowel's formants as the mouth changes shape
between the consonant and the vowel. The following attributes may be
specified. Note that the maximum rate of change of formant frequencies
is limited by the program.

	len=<integer>

Nominal length of the transition in milliseconds. If omitted a default value is used.

	rms=<integer>

Adjusts the amplitude of the vowel at the end of the transition. If omitted
a default value is used.

	f1=<integer>

0: f1 formant frequency unchanged.

1: f1 formant frequency decreases.

2: f1 formant frequency decreases more.

	f2=<freq> <min> <max>

\<freq\>: The frequency towards which the f2 formant moves (Hz).

\<min\>: Signed integer (Hz).  The minimum f2 frequency change.

\<max\>: Signed integer (Hz).  The maximum f2 frequency change.

	f3=<change> <amplitude>

\<change\>: Signed integer (Hz).  Frequency change of f3, f4, and f5 formants.

\<amplitude\>: Amplitude of the f3, f4, and f5 formants at the end of the
transition. 100 = no change.

	brk

Break. Do not merge the synthesized wave of the consonant into the vowel. This
will produce a discontinuity in the formants.

	rate

Allow a greater maximum rate of change of formant frequencies.

	glstop

Indicates a glottal stop.


## Customization of sound source files

* You need [Espeakedit](http://espeak.sourceforge.net/editor.html) to change sound parameters of vowel definition files.

On Debian/Ubuntu based systems you can install it with command:

```
sudo apt-get install espeakedit
```

If your 64-bit Linux system doesn't have this package, you can download just compiled binary file from
[here](https://github.com/valdisvi/espeak-ng-espeakedit/blob/master/bin/espeakedit?raw=true).

To edit *.wav files for sonorant/consonant phonemes you may also need:

* [Audacity](https://www.audacityteam.org/) can be used as sound editor for consonant sounds (*.wav) files.
Audacity may be also used to analyse [sound spectrum](https://manual.audacityteam.org/man/spectrogram_view.html)
(use maximum window size to see harmonics clearly).

On Debian/Ubuntu based systems you can install it with command:

```
sudo apt-get install audacity
```

* [Praat](http://www.fon.hum.uva.nl/praat/) can be used to do more detailed analysis of recorded voice files.

On Debian/Ubuntu based systems you can install it with command:

```
sudo apt-get install praat
```

`Praat` can also provide initial settings for vowel definitions to `espeakedit`.

