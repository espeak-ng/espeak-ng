# Adding or Improving a Language

- [Considerations Before Preparation](#considerations-before-preparation)
  - [Language Tag](#language-tag)
  - [Language Family](#language-family)
  - [Accent (optional)](#accent-optional)
- [Configuration Files](#configuration-files)
  - [Makefile.am file](#makefileam-file)
  - [Phonemes file](#phonemes-file)
  - [Language File](#language-file)
  - [Phoneme Definition File](#phoneme-definition-file)
  - [Dictionary Files](#dictionary-files)
  - [Building Phonemes](#building-phonemes)
  - [Adding tests](#adding-tests)
- [Program Code](#program-code)
- [Compiling Rules File for Debugging](#compiling-rules-file-for-debugging)
- [Improving a Language](#improving-a-language)

----------

## Considerations Before Preparation


Most of the work doesn't need any programming knowledge, but, to get immediate
feedback, by running and testing eSpeak,
you should be able to [build](../README.md#building) it.

You also have to understand the language main concepts, be aware of its features,
and have to have patience and attention to detail.
Wikipedia is a good source of basic phonetic information, e.g. about 
[vowels](http://en.wikipedia.org/wiki/Vowel) and
[consonants](https://en.wikipedia.org/wiki/Consonant).

In many cases it should be fairly easy to add a rough implementation of
a new language, hopefully enough to be intelligible. After that it's a
gradual process of improvement.

### Language Tag

The language is identified using the
[BCP 47](https://en.wikipedia.org/wiki/IETF_language_tag) language tag.
The list of valid tags originate from various standards and have been combined
into the
[IANA Language Subtag Registry](http://www.iana.org/assignments/language-subtag-registry/language-subtag-registry).

These language tags are used to specify the language, such as:

*  `fr` (French) -- The [ISO 639-1](https://en.wikipedia.org/wiki/ISO_639-1)
   2-letter language code for the language.

   __NOTE:__ BCP 47 uses ISO 639-1 codes for languages that are allocated
   2-letter codes (e.g. using `en` instead of `eng`).

*  `yue` (Cantonese) -- The [ISO 639-3](https://en.wikipedia.org/wiki/ISO_639-3)
   3-letter language codes for the language.

*  `ta-Arab` (Tamil written in the Arabic alphabet) -- The
   [ISO 15924](https://en.wikipedia.org/wiki/ISO_15924) 4-letter script code.

   __NOTE:__ Where the script is the primary script for the language, the script
   tag should be omitted.

### Language Family

The languages are grouped by the closest language family the language belongs.
These language families are defined in
[ISO 639-5](https://en.wikipedia.org/wiki/ISO_639-5). See also Wikipedia's
[List of language families](https://en.wiktionary.org/wiki/Wiktionary:List_of_families)
for more details.

For example, the Celtic languages (Welsh, Irish Gaelic, Scottish Gaelic, etc.)
are listed under the `cel` language family code.

### Accent (optional)

If necessary, the language tags are also used to specify the accent or dialect of a language,
such as:

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

## Configuration Files

To add new language, you have to create or edit following files:

|path/file                     |action                       |
|------------------------------|-----------------------------|
| Makefile.am                  |[edit](#makefileam-file)     |
| espeak-ng-data/lang/roa/fr   |[create](#language-file)     |
| phsource/phonemes            |[edit](#phonemes-file)       |
| phsource/ph_french           |[create](#phoneme-definition-file) (optional) |
| dictsource/fr_list           |[create](#dictionary-files)  |
| dictsource/fr_rules          |[create](#dictionary-files)  |
| dictsource/fr_extra          |create (optional)            |

where:

* __french__ is name of the newly created language
* __fr__ is the code of this language
* __roa__ is the family of this language


### Makefile.am File

`Makefile.am` is build configuration file. It should be edited if you add
new language text-to-phoneme translation rules or set of sounds (phonemes).
Usually you have to add necessary entries for new language, but may not need if
you add new dialect for existing language.

Search for configuration of existing languages (e.g. English)
and add similar lines for your language in following sections.
E.g. for French:

	dictionaries: \
	...
	espeak-ng-data/fr_dict \
	...

	fr: espeak-ng-data/fr_dict
	espeak-ng-data/fr_dict: dictsource/fr_list dictsource/fr_rules dictsource/fr_extra dictsource/fr_emoji
	...

Note, that you don't need to add `fr_extra` or `fr_emoji` reference in the last group, if your language doesn't have this file.

### Language File

E.g. `espeak-ng-data/lang/roa/fr` is the language file for French.
This gives the language name and may set some options.

Each language needs a language file in `espeak-ng-data/lang` grouped by the
[language family](#language-family). The filename of the default voice for a
language should be the same as the language code (e.g. `fr` for French).

The simplest voice file would contain just 2 lines to give the language
name (from the
[IANA Language Subtag Registry](https://github.com/rhdunn/bcp47-data/blob/master/language-subtag-registry)
and language code, e.g.:

	name French
	language fr

This language code specifies which phoneme table and dictionary to use
(i.e. `phonemetable fr` and `espeak-ng-data/fr_dict`) to be used. If
needed, these can be overridden by `phonemes` and `dictionary`
attributes in the voice file. For example you may want to start the
implementation of a new language by using the phoneme table of an
existing language.

Details of the contents of language files are given in [Voices](voices.md).

### Phonemes File

Open file `phsource/phonemes` and add following lines

1. Set of standard (already defined) phonemes for languge

```
	...
	phonemetable fr base
	...
```
  where:
        _..._ means other existing lines of the configuration file and
        _base_ is usually is `base1` or code of other (already defined) language:

  * `base` phoneme table contains only definitions of some "control" phonemes
    such as stress marks and pauses defined in top of the `phonemes` file.

  * `base1` includes `base` and also has basic set of vowels and consonants
    defined under `phonemetable base1` section in `phonemes` file.

  * `base2` has additional phonemes (mostly diphones) included from `ph_base2` file.

  * Other language e.g. `en` has all phonemes defined in its corresponding phoneme
    definition file e.g. `phsourc/ph_english` for English language.

2. Optionally, if you have customized phonemes in  [Phoneme Definition File](#phoneme-definition-file),
   add additional line: to include it:

```
	...
	include ph_french
	...
```
For example, if `phsource/phonemes` has following lines:

	phonemetable fr base
	include ph_french

it defines a phoneme table `fr` which inherits phoneme definitions from
`base` table and adds new definitions or overrides existing
definitions of `base` phonemes from the `phonemes/ph_french` file.

### Phoneme Definition File

If you decide to customize definitions of phonemes (vowel and consonant
sounds) for the language, then you have to create e.g. `phsource/ph_french`
phoneme definition file for French.

The phonemes file for the language defines those additional phonemes
which are not inherited (generally the vowels and diphthongs, plus any
additional consonants that are needed), or phonemes whose definitions
differ from the inherited version (eg. the redefinition of a consonant).

Usually it will contain mostly vowels. Most consonants will be inherited from the
common phoneme definitions in the _master phoneme file_: `phsource/phonemes` as
described in [Phonemes File](#phonemes-file) section above.

Details of phonemes files are given in [Phoneme Tables](phontab.md).

### Building Phonemes

To build the phoneme files, run:

	espeak-ng --compile-phonemes

or:

	make phsource/phonemes.stamp

For many languages, the consonant phonemes which are already available
in eSpeak, together with the available vowel files which can be used to
define vowel phonemes, will be sufficient. At least for an initial
implementation.

### Dictionary Files

There are usually two dictionary files, e.g. for French:

  * `dictsource/fr_list`. This contains pronunciations for numbers, letter and
    symbol names, and words with exceptional pronunciations. It also gives
    attributes such as "unstressed" and "pause" to some common words.
    The `fr_list` file contains:

      * Pronunciations which exceptions to the rules in `fr_rules`, (e.g. foreign
        names).
      * Pronunciation of letter names, symbol names, and punctuation names.
      * Pronunciation of numbers.
      * Attributes for words. For example, common function words which should not
        be stressed, or conjunctions which should be preceded by a pause. 

  * `dictsource/fr_rules`. This contains the spelling-to-phoneme translation
     rules.

Details of the contents of the dictionary files are given in
[Dictionary](dictionary.md).

The `fr_rules` and `fr_list` files are compiled to produce the
`espeak-ng-data/fr_dict` file, which eSpeak uses when it is speaking.

Once the language's phonemes have been defined, then pronunciation
dictionary data can be produced in order to translate the language's
source text into phonemes. This consists of two source files:
`fr_rules` (the spelling to phoneme rules) and `fr_list` (an
exceptions list, and attributes of certain words). The corresponding
compiled data file is `espeak-ng-data/fr_dict` which is produced from
the `fr_rules` and `fr_list` sources by the command:

	espeak-ng --compile=fr

or by:

	make fr

### Adding tests

All languages require automated tests to make sure accidental changes 
in program code don't change how the languages sound. 

Tests are run with `make check`. It will print an error if a language
is missing tests.

Most tests are bash scripts in tests/ and follow the syntax:
<test name> <language code> <expected result> <input>

Depending on the test, expected result can be a phoneme string, an md5 hash or something else.

To add a test:

1. First add a test line with arbitrary expected result. 
Example line in tests/language-phonemes.test:
test_phwav fi this_should_fail "ma na Na pa ba ta da"

2. run `make check` to see expected and actual results:
testing fi
1c1
< this_should_fail
---
> 7541590a29e2392b827d9f1563e6579774fb15ea

3. change the expected result in the test:
test_phwav fi 7541590a29e2392b827d9f1563e6579774fb15ea "ma na Na pa ba ta da"




## Program Code

The behaviour of the eSpeak program is controlled by various options
such as:

* Default rules for which syllable of a word has the main stress.
* Relative lengths and amplitude of vowels in stressed and unstressed syllables.
* Which intonation tunes to use.
* Rules for speaking numbers.

Most of these options can be set in the voice file in espeak-ng-data/lang folder (see [Voices](voices.md)).
However some advanced options may be needed to add in source code in the function `SelectTranslator()`
of the [tr_languages.c](https://github.com/espeak-ng/espeak-ng/blob/master/src/libespeak-ng/tr_languages.c)
file.

## Compiling Rules File for Debugging

When `espeak-ng` is invoked with `-X` parameter, it shows more detailed trace of 
chosen language rules for pronunciation. This trace can show line numbers also, 
if language is compiled with `--compile-debug` option.

To do this, go to `espeak-ng` project root folder, then:

	cd dictsource
	espeak-ng --compile-debug=en

When invoked in following way:

	espeak-ng -ven -X "Test."

It will show:

	Translate 'test'
	  1  5965:  t [t]
	
	  1  2104:  e [E]
	
	  1  5725:  s [s]
	
	  1  5965:  t [t]
	
	 t'Est

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

For most of the eSpeak languages, I do not speak or understand the language, and I
do not know how it should sound. I can only make improvements as a result of
feedback from speakers of that language. If you want to help to improve a
language, listen carefully and try to identify individual errors, either in
the spelling-to-phoneme translation, the position of stressed syllables within
words, or the sound of phonemes, or problems with rhythm and vowel lengths.
