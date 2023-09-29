# Voice and Language Files

- [Identification Attributes](#identification-attributes)
  - [name](#name)
  - [language](#language)
  - [gender](#gender)
- [Maintenance Attributes](#maintenance-attributes)
  - [maintainer](#maintainer)
- [Voice Attributes](#voice-attributes)
  - [pitch](#pitch)
  - [formant](#formant)
  - [freq\_add](#freq_add)
  - [echo](#echo)
  - [tone](#tone)
  - [flutter](#flutter)
  - [roughness](#roughness)
  - [voicing](#voicing)
  - [consonants](#consonants)
  - [breath](#breath)
  - [breathw](#breathw)
  - [speed](#speed)
  - [words](#words)
- [Language Attributes](#language-attributes)
  - [brackets](#brackets)
  - [bracketsAnnounced](#bracketsAnnounced)
  - [phonemes](#phonemes)
  - [dictionary](#dictionary)
  - [dictrules](#dictrules)
  - [lowercaseSentence](#lowercaseSentence)
  - [replace](#replace)
  - [spellingStress](#spellingStress)
  - [stressOpt](#stressopt)
  - [stressRule](#stressrule)
  - [stressLength](#stresslength)
  - [stressAdd](#stressadd)
  - [stressAmp](#stressamp)
  - [intonation](#intonation)
  - [dictmin](#dictmin)

----------

A voice file specifies a language (and possibly a language variant or
dialect or just voice) together with various attributes that affect the
characteristics of the voice quality and how the language is spoken.

Voice files are located in the `espeak-ng-data/` folder, and:

1. if it describes voice with language or dialect, in [lang](../../../tree/master/espeak-ng-data/lang) subfolder;
2. if it describes just voice, in [voices/!v](../../../tree/master/espeak-ng-data/voices/!v) subfolder.

Languages are grouped by the [ISO 639-5](https://en.wikipedia.org/wiki/ISO_639-5)
language family of the language being specified in the voice files.
See also Wikipedia's
[List of language families](https://en.wiktionary.org/wiki/Wiktionary:List_of_families)
for more details.

The `default` voice is used if none is specified in the speak command. You
can copy your preferred voice to "default" so you can use the speak command
without the need to specify a voice.

## Identification Attributes

### name

	name <name>

A mandatory name given to this voice. Should be single word.

### language

A mandatory identifier of the language or the voice. It can have one of
two options:

	language <language code> [<priority>]

_or_

	language variant

__NOTE:__ This attribute is mandatory and should appear before the other
attributes which are listed below.

It selects the default behaviour and characteristics for the language,
and sets default values for "phonemes", "dictionary" and other
attributes.

If value for `language` field is `variant`, then file describes only
phonetic features of the voice and it can be used as a [...+variant](../src/espeak-ng.1.ronn)
option for any language.

If `language` identifies actual language, then \<language code\> should be a valid
[BCP 47](https://en.wikipedia.org/wiki/IETF_language_tag) language tag. The
list of valid tags originate from various standards and have been combined
into the
[IANA Language Subtag Registry](http://www.iana.org/assignments/language-subtag-registry/language-subtag-registry).
For example:

*  `de` (German) -- The [ISO 639-1](https://en.wikipedia.org/wiki/ISO_639-1)
   2-letter language code for the language.

   __NOTE:__ BCP 47 uses ISO 639-1 codes for languages that are allocated
   2-letter codes (e.g. using `en` instead of `eng`).

*  `yue` (Cantonese) -- The [ISO 639-3](https://en.wikipedia.org/wiki/ISO_639-3)
   3-letter language codes for the language.

*  `ta-Arab` (Tamil written in the Arabic alphabet) -- The
   [ISO 15924](https://en.wikipedia.org/wiki/ISO_15924) 4-letter script code.

   __NOTE:__ Where the script is the primary script for the language, the script
   tag should be omitted.

*  `es-419` (Spanish (Latin America)) -- The
   [UN M.49](https://en.wikipedia.org/wiki/UN_M.49) 3-number region codes.

*  `fr-CA` (French (Canada)) -- Using the
   [ISO 3166-2](https://en.wikipedia.org/wiki/ISO_3166-2) 2-letter region codes.

*  `en-GB-scotland` (English (Scotland)) -- This is using the BCP 47 variant
   tags.

*  `en-GB-x-rp` (English (Received Pronunciation)) -- This is using the
   [bcp47-extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
   language tags for accents that cannot be described using the available
   BCP 47 language tags.

   __NOTE:__ If the accent you are trying to describe cannot be specified using
   the above system, raise an issue in the
   [bcp47-data](https://github.com/rhdunn/bcp47-data) project and a private use
   tag will be defined for that accent.

The optional \<priority\> value gives the preference of this voice
compared with others for the specified language. A low value indicates a
more preferred voice. The default value is 5.

More than one `language` line may be present. A voice may be selected
for other related languages (variants which have the same initial 2
letter language code as the specified language), but it will be less
preferred for these. Different language variants may be specified by
additional `language` lines in order to indicate that this is a
preferred voice for them also. E.g.

	language en-GB-x-gbclan
	language en

indicates that this is voice is for the `en-GB-x-gbclan` dialect, but it is
also a main choice when a general `en` language is specified. Without
the second `language` line, it would be disfavoured from `en` for being
a more specialised voice.

### gender

	gender <gender> [<age>]

This attribute is only a label for use in voice selection. It doesn't
change the sound of the voice.  

*  \<gender\> may be male, female, or unknown.  
*  \<age\> is optional and gives an age in years.

## Maintenance Attributes

### maintainer

	maintainer <maintaner>

Specifies the person responsible for updating the voice and associated language
files. If no maintainer is specified, the voice does not currently have an
active maintainer.

### status

	status <status level>

Specifies how advanced the support for the language is:

* `testing` -- Provides the basic level of support, with basic pronunciation
   rules. Requires feedback from a native speaker to help improve the voice.

* `mature` -- The language is well supported. It has been improved by, or with
  support from, a native speaker.

## Voice Attributes

### pitch

	pitch <base> <range>

Two integer values. The first gives a base pitch to the voice (value in
Hz) The second controls the range of pitches used by the voice. Setting
it equal to the base pitch will give a monotone. The default values are 82 118.  

### formant

	formant <number> <frequency> <strength> <width> <freq_add>

Systematically adjusts the frequency, strength, and width of the
resonance peaks of the voice. Values are percentages of the default
values. Changing these affects the tone/quality of the voice.

### freq\_add

Adds a constant value (in Hz) to the frequency of the formant peak. The value
may be negative.

* Formants 1,2,3 are the standard three formants which define vowels.
* Formant 0 is used to give a low frequency component to the sounds, of
  frequency lower than F1.
* Formants 4,5 are higher than F3. They affect the quality of the voice.
* Formants 6,7,8 are weak, high frequency, additions to vowels to give a
  clearer sound.

### echo

	echo <delay> <amplitude>

Parameter 1 gives the delay in mS (0 to 250mS).  

Parameter 2 gives the echo amplitude (0 to 100).  

Adding some echo can give a clearer or more interesting sound, especially when
listening through a domestic stereo sound system, rather than small computer
speakers.

### tone

Controls the tone of the sound.  

`tone` is followed by up to 4 pairs of `<frequency> <amplitude>`
which define a frequency response graph. Frequency is in Hz and
amplitude is in the range 0 to 255 The default is:

	tone 600 170  1200 135  2000 110

This means that from frequency 0Hz to 600Hz the amplitude is 170 From
600Hz to 1200Hz the amplitude decreases from 170 to 135, then decreases
to 110 at 2000Hz and remains at 110 at higher frequencies. This
adjustment applies only to voiced sounds such as vowels and sonorant
consonants (such as `[n]` and `[l]`). Unvoiced sounds such as `[s]` are
unaffected.

This `tone` statement can also appear in `espeak-ng-data/config`, in which case
it applies to all voices which don't have their own `tone` statement.

### flutter

	flutter <value>

Default value: 100.  

Adds pitch fluctuations to give a wavering or older-sounding voice. A
large value (eg. 20) makes the voice sound "croaky".

### roughness

	roughness <value>

Default value:  Range 0 - 7  

Reduces the amplitude of alternate waveform cycles in order to make the
voice sound creaky.

### voicing

	voicing <value>

Default value: 100

Adjusts the strength of formant-synthesized sounds (vowels and sonorant
consonants).

### consonants

	consonants <value> <value>

Default values: 100, 100

Adjusts the strength of noise sounds which are used in consonants. The
first value is the strength of unvoiced consonants such as "s" and "t".

The second value is the strength of the noise component of voiced
consonants such as "z" and "d".

### breath

	breath <up to 8 integer values>

Default values: 0.

Adds noise which corresponds to the formant frequency peaks. The values
give the strength of noise for each formant peak (formants 1 to 8).

Use together with a low or zero value of the `voicing` attribute to
make a "wisper". For example:

	breath   75 75 60 40 15 10
	breathw  150 150 200 200 400 400
	voicing  18
	flutter  20
	formant   0 100 0 100   // remove formant 0 

### breathw

	breathw <up to 8 integer values>

These values give bandwidths of the noise peaks of the `breath`
attribute. If `breathw` values are not given, then suitable default
values will be used.

### speed

	speed <value>

Default value 100

Adjusts the speaking speed by a percentage of the default rate. This
can be used if a language voice seems faster or slower compared to other
voices.

### words

	words <integer value> <integer value>

The first parameter  puts a pause between all words, the value can be between
`0` and `4`.

The second parameter adds a short pause if a word ends with a vowel and
the next words starts with a vowel. Values may be:

* `0`: no pause
* `1`: no pause, but the two vowels are kept separate
* `2`: short pause

For example:

	words 0 1

will put a short pause between two words where the first word end with a
vowel and the next start with a vowel.

## Language Attributes

### brackets

	brackets <value>

Default value: 4

Increases the pause when reading brackets. Example: "Pause (with brackets)".

### bracketsAnnounced

	bracketsAnnounced <value>

Default value: 2

Increases the pause when reading brackets when --punct is set to read bracket names. Example: "Pause (with brackets)".

### phonemes

	phonemes <name>

Specifies which set of phonemes to use from those contained in the
phontab, phonindex, and phondata data files. This is a `phonemetable`
name as given in the "phoneme" source file.

This parameter is usually not needed as it is set by default to the
first two letters of the "language" parameter. However, different voices
of the same language can use different phoneme sets, to give different
accents.

### dictionary

	dictionary <name>

Specifies which pair of dictionary files to use. For example, `en` indicates
that `speak-data/en_dict` should be used to translate from words to
phonemes. This parameter is usually not needed as it is set by default
to the value of the "language" parameter.

### dictrules

	dictrules <list of rule numbers>

Gives a list of conditional dictionary rules which are applied for this
voice. Rule numbers are in the range 0 to 31 and are specific to a
language dictionary. They apply to rules in the language's `*_rules`
dictionary file and also its `*_list` exceptions list. See
[Text to Phoneme Translation](dictionary.md#conditional-rules).

### lowercaseSentence

	lowercaseSentence <no arguments>

By default, a sentence end is detected if a period `.` is followed by an uppercase letter.
When lowercaseSentence is set, a period followed by a lowercase letter is also handled as end of sentence.

Note that other conditions, such as abbreviations, might override this setting.


### replace

	replace <flags> <phoneme> <replacement phoneme>

Replace a phoneme by another whenever it occurs.  
\<replacement phoneme\> may be NULL.  

Flags: bit 0: replacement only occurs on the final phoneme of a word.  
Flags: bit 1: replacement doesn't occur in stressed syllables.  

e.g.  

	replace  0  h  NULL      // drops h's
	replace  0  V  U         // replaces vowel in 'strut' by that in 'foot'
	                         // as occurs in northern British English
	replace  3  N  n         // change 'fishing' to 'fishin' etc.
	                         // (only the last phoneme of a word, only in unstressed syllables)

The phoneme mnemonics can be defined for each language, but some are
listed in [Phonemes](phonemes.md).

### spellingStress

	spellingStress

When set, stress first letter when reading abbreviations.

### stressOpt

	stressOpt <list of rule numbers>

Gives a list of stress options which are applied for this
language. (values are defined as bit numbers starting with "S_*" in [translate.h](../src/libespeak-ng/translate.h))

### stressRule

	stressRule <3 integer values>

Three integer parameters. These correspond to:

1. langopts->stress_rule (values in [translate.h](../src/libespeak-ng/translate.h))
3. langopts->unstressed_wd1 (stress for $u word of 1 syllable)
4. langopts->unstressed_wd2 (stress for $u word of >1 syllable) 

If a value is not given, it defaults to zero. For example:
"stressRule 2" is equal to "stressRule 2 0 0 0"

### stressLength

	stressLength <8 integer values>

Eight integer parameters. These control the relative lengths of the
vowels in stressed and unstressed syllables.

* 0  unstressed. Unstressed syllable in word
  (syllable with `%` mark or without any stress mark).
* 1 diminished. Its use depends on the language. In English it's used for
  unstressed syllables (marked with `%%`) within multisyllabic words.
  In other languages it is final unstressed syllable.
* 2 secondary stress (syllable with marked with `,`).
* 3 words marked as "unstressed" in the dictionary.
* 4   not currently used
* 5   not currently used
* 6 stressed syllable (the main syllable in stressed words marked with `'`).
* 7 tonic syllable (by default, the last stressed syllable in the clause).

### stressAdd

	stressAdd <8 integer values>

Eight integer parameters. These are added to the voice's corresponding
`stressLength` values. They are used in the voice variant files in
`espeak-ng-data/voices/!v` to give some variety. Negative values may be used.

Note that setting `stressLength` after `stressAdd`  will overwrite this value.
`stressLength` must be set before `stressAdd`.

### stressAmp

	stressAmp <8 integer values>

Eight integer parameters. These control the relative amplitudes of the
vowels in stressed and unstressed syllables (see stressLength above).
The general default values are: 16, 16, 20, 20, 20, 24, 24, 22, although
these defaults may be different for particular languages.

### intonation

	intonation <param1>

* 1 -- Default.  
* 2 -- Less intonation.  
* 3 -- Less intonation, and comma does not raise the pitch.  
* 4 -- Pitch rises (rather than falls) at the end of sentence.

### dictmin

	dictmin <value>

Used for some languages to detect if additional language data is
installed. If the size of the compiled dictionary data for the language
(the file `espeak-ng-data/*_dict`) is less than this size then a
warning is given.
