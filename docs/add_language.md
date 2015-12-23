# Adding or Improving a Language

- [Language Code](#language-code)
- [Language Files](#language-files)
- [Voice File](#voice-file)
- [Phoneme Definition File](#phoneme-definition-file)
- [Dictionary Files](#dictionary-files)
- [Program Code](#program-code)
- [Improving a Language](#improving-a-language)

----------

Most of the work doesn't need any programming knowledge. Just an
understanding of the language, an awareness of its features, patience
and attention to detail. Wikipedia is a good source of basic phonetic
information, e.g.
[http://en.wikipedia.org/wiki/Vowel](http://en.wikipedia.org/wiki/Vowel).

In many cases it should be fairly easy to add a rough implementation of
a new language, hopefully enough to be intelligible. After that it's a
gradual process of improvement.

## Language Code

Generally, the language's international
[ISO 639-1](http://en.wikipedia.org/wiki/ISO_639-1) code is used to identify
the language. It is used in the filenames which contain the language's
data. In the examples below the code **"fr"** is used as an example.
Replace this with the code of your language.

If the language does not have a 2-letter ISO\_639-1 code, then use the
3-letter ISO\_639-3 code. Language codes may differ from country codes.

It is possible to have different variants of a language for different
dialects. For example the sound of some phonemes are changed, or some of
the pronunciation rules differ.

## Language Files

The following files are needed for your language.

  * `espeak-data/voices/fr`. The voice file. This gives the language name and
    may set some options.
  * `phsource/ph_french`. The phoneme definition file. This contains phoneme
    definitions for the vowels and consonants which the language uses. Usually
    it will contain mostly vowels. Most consonants will be inherited from the
    common phoneme definitions in the master phoneme file, `phsource/phonemes`.
    The master phoneme file needs to be edited to call your new `ph_french` file.
  * `dictsource/fr_rules`. This contains the spelling-to-phoneme translation
     rules.
  * `dictsource/fr_list`. This contains pronunciations for numbers, letter and
    symbol names, and words with exceptional pronunciations. It also gives
    attributes such as "unstressed" and "pause" to some common words.

The `fr_rules` and `fr_list` files are compiled to produce the
file `espeak-data/fr_dict`, which eSpeak uses when it is speaking.

## Voice File

Each language needs a voice file in `espeak-data/voices` or
`espeak-data/voices/test`. The filename of the default voice for a
language should be the same as the language code (eg. "fr" for French).

Details of the contents of voice files are given in [Voices](voices.md).

The simplest voice file would contain just 2 lines to give the language
name and language code, eg:

	name french
	language fr

This language code specifies which phoneme table and dictionary to use
(i.e. `phonemetable fr` and `espeak-data/fr_dict`) to be used. If
needed, these can be overridden by `phonemes` and `dictionary`
attributes in the voice file. For example you may want to start the
implementation of a new language by using the phoneme table of an
existing language.

## Phoneme Definition File

You must first decide on the set of phonemes (vowel and consonant
sounds) for the language. These should be defined in a phoneme
definition file `ph_xxxx`, where `ph_xxxx` is the name of your
language. A reference to this file is then included at the end of the
master phoneme file, `phsource/phonemes`, e.g.:

	phonemetable fr base
	include ph_french

This example defines a phoneme table `fr` which inherits the
contents of phoneme table `base`. Its contents are found in the file
`ph_french`.

The `base` phoneme table contains definitions of a basic set of
consonants, and also some "control" phonemes such as stress marks and
pauses. These are defined in `phsource/phonemes`. The phoneme table
for a language will inherit these, or alternatively it may inherit the
phoneme table of another language which in turn inherits the `base`
phoneme table.

The phonemes file for the language defines those additional phonemes
which are not inherited (generally the vowels and diphthongs, plus any
additional consonants that are needed), or phonemes whose definitions
differ from the inherited version (eg. the redefinition of a consonant).

Details of phonemes files are given in [Phoneme Tables](phontab.md).

To build the phoneme files, run:

	espeak-ng --compile-phonemes

or:

	make

For many languages, the consonant phonemes which are already available
in eSpeak, together with the available vowel files which can be used to
define vowel phonemes, will be sufficient. At least for an initial
implementation.

## Dictionary Files

Once the language's phonemes have been defined, then pronunciation
dictionary data can be produced in order to translate the language's
source text into phonemes. This consists of two source files:
`fr_rules` (the spelling to phoneme rules) and `fr_list` (an
exceptions list, and attributes of certain words). The corresponding
compiled data file is `espeak-data/fr_dict` which is produced from
the `fr_rules` and `fr_list` sources by the command:

	espeak-ng --compile=fr

or by:

	make fr

Details of the contents of the dictionary files are given in
[Dictionary](dictionary.md).

The `fr_list` file contains:

  * Pronunciations which exceptions to the rules in `fr_rules`, (e.g. foreign
    names).
  * Pronunciation of letter names, symbol names, and punctuation names.
  * Pronunciation of numbers.
  * Attributes for words. For example, common function words which should not
    be stressed, or conjunctions which should be preceded by a pause. 

## Program Code

The behaviour of the eSpeak program is controlled by various options
such as:

* Default rules for which syllable of a word has the main stress.
* Relative lengths and amplitude of vowels in stressed and unstressed syllables.
* Which intonation tunes to use.
* Rules for speaking numbers. 

The function `SetTranslator()` at the start of the source code file
`tr_languages.c` recognizes the language code and sets the appropriate
options. For a new language, you would add its language code and the
required options in `SetTranslator()`. However, this may not be necessary
during testing because most of the options can also be set in the voice
file in espeak-data/voices (see [Voice Files](voices.md)).

## Improving a Language

Listen carefully to the eSpeak voice. Try to identify what sounds wrong
and what needs to be improved.

* Make the spelling-to-phoneme translation rules more accurate, including
  the position of stressed syllables within words. Some languages are easier
  than others. I expect most are easier than English.

* Improve the sounds of the phonemes. It may be that a phoneme should sound
  different depending on adjacent sounds, or whether it's at the start or the
  end of a word, between vowels, in a stressed or unstressed syllable, etc.
  This may consist of making small adjustments to vowel and diphthong quality
  or length, or adjusting the strength of consonants. Phoneme definitions can
  include conditional statements which can be used to change the sound of a
  phoneme depending on its environment. Bigger changes may be recording new or
  replacement consonant sounds, or may even need program code to implement new
  types of sounds.

* Some common words should be added to the dictionary (the `fr_list` file for
  the language) with an "unstressed" attribute `$u` or `$u+` (e.g. in English,
  words such as "the", "is", "had", "my", "she", "of", "in", "some"), or should
  be preceded by a short pause (such as "and", "but", "which"), or have other
  attributes, in order to make the speech flow better.

* Improve the rhythm of the speech by adjusting the relative lengths of vowels
  in different contexts, e.g. stressed/unstressed syllable, or depending on the
  following phonemes. This is important for making the speech sound good for the
  language.

* Make new intonation "tunes" for statements or questions (see
  [Intonation](intonation.md)).

For most of the eSpeak voices, I do not speak or understand the language, and I
do not know how it should sound. I can only make improvements as a result of
feedback from speakers of that language. If you want to help to improve a
language, listen carefully and try to identify individual errors, either in
the spelling-to-phoneme translation, the position of stressed syllables within
words, or the sound of phonemes, or problems with rhythm and vowel lengths.
