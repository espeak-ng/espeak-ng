6. ADDING OR IMPROVING A LANGUAGE {.western}
---------------------------------

Most of the work doesn't need any programming knowledge. Just an
understanding of the language, an awareness of its features, patience
and attention to detail. Wikipedia is a good source of basic phonetic
information, eg
[http://en.wikipedia.org/wiki/Vowel](http://en.wikipedia.org/wiki/Vowel).

In many cases it should be fairly easy to add a rough implementation of
a new language, hopefully enough to be intelligible. After that it's a
gradual process of improvement.

### 6.1 Language Code {.western}

Generally, the language's international [ISO
639-1](http://en.wikipedia.org/wiki/ISO_639-1) code is used to identify
the language. It is used in the filenames which contain the language's
data. In the examples below the code **"fr"** is used as an example.
Replace this with the code of your language.

If the language does not have a 2-letter ISO\_639-1 code, then use the
3-letter ISO\_639-3 code. Language codes may differ from country codes.

It is possible to have different variants of a language for different
dialects. For example the sound of some phonemes are changed, or some of
the pronunciation rules differ.

### 6.2 Language Files {.western}

The following files are needed for your language.

-   -   -   -   

The **fr\_rules** and **fr\_list** files are compiled to produce the
file **espeak-data/fr\_dict**, which eSpeak uses when it is speaking.

### 6.3 Voice File {.western}

Each language needs a voice file in **espeak-data/voices** or
**espeak-data/voices/test**. The filename of the default voice for a
language should be the same as the language code (eg. "fr" for French).

Details of the contents of voice files are given in
[voices.html](http://espeak.sf.net/voices.html).

The simplest voice file would contain just 2 lines to give the language
name and language code, eg:

~~~~ {.western}
  name french
  language fr
~~~~

This language code specifies which phoneme table and dictionary to use
(i.e. **phonemetable fr** and **espeak-data/fr\_dict**) to be used. If
needed, these can be overridden by **phonemes** and **dictionary**
attributes in the voice file. For example you may want to start the
implementation of a new language by using the phoneme table of an
existing language.

### 6.4 Phoneme Definition File {.western}

You must first decide on the set of phonemes (vowel and consonant
sounds) for the language. These should be defined in a phoneme
definition file **ph\_xxxx**, where "ph\_xxxx" is the name of your
language. A reference to this file is then included at the end of the
master phoneme file, **phsource/phonemes**, eg:

~~~~ {.western}
  phonemetable  fr  base
  include  ph_french
~~~~

This example defines a phoneme table **"fr"** which inherits the
contents of phoneme table **"base"**. Its contents are found in the file
**ph\_french**.

The **base** phoneme table contains definitions of a basic set of
consonants, and also some "control" phonemes such as stress marks and
pauses. These are defined in **phsource/phonemes**. The phoneme table
for a language will inherit these, or alternatively it may inherit the
phoneme table of another language which in turn inherits the **base**
phoneme table.

The phonemes file for the language defines those additional phonemes
which are not inherited (generally the vowels and diphthongs, plus any
additional consonants that are needed), or phonemes whose definitions
differ from the inherited version (eg. the redefinition of a consonant).

Details of phonemes files are given in
[phontab.html](http://espeak.sf.net/phontab.html).

The **Compile phoneme data** function of the **espeakedit** program
compiles the phonemes files of all languages to produce the files
**espeak-data/phontab**, **phonindex**, and **phondata** which are used
by eSpeak.

For many languages, the consonant phonemes which are already available
in eSpeak, together with the available vowel files which can be used to
define vowel phonemes, will be sufficient. At least for an initial
implementation.

### 6.5 Dictionary Files {.western}

Once the language's phonemes have been defined, then pronunciation
dictionary data can be produced in order to translate the language's
source text into phonemes. This consists of two source files:
**fr\_rules** (the spelling to phoneme rules) and **fr\_list** (an
exceptions list, and attributes of certain words). The corresponding
compiled data file is **espeak-data/fr\_dict** which is produced from
**fr\_rules** and **fr\_list** sources by the command:

> `espeak-ng --compile=fr`{.western}.

Or by using the **espeakedit** program.

Details of the contents of the dictionary files are given in
[dictionary.html](http://espeak.sf.net/dictionary.html).

The **fr\_list** file contains:

-   -   -   -   

### 6.6 Program Code {.western}

The behaviour of the eSpeak program is controlled by various options
such as:

-   -   -   -   

The function SetTranslator() at the start of the source code file
tr\_languages.cpp recognizes the language code and sets the appropriate
options. For a new language, you would add its language code and the
required options in SetTranslator(). However, this may not be necessary
during testing because most of the options can also be set in the voice
file in espeak-data/voices (see [Voice
files](http://espeak.sf.net/voices.html)).

### 6.7 Improving a Language {.western}

Listen carefully to the eSpeak voice. Try to identify what sounds wrong
and what needs to be improved.

-   -   -   -   -   

**If you are interested in working on a language, please contact me so
that I can set up the initial data and discuss the features of the
language.**

For most of the eSpeak voices, I do not speak or understand the
language, and I do not know how it should sound. I can only make
improvements as a result of feedback from speakers of that language. If
you want to help to improve a language, listen carefully and try to
identify individual errors, either in the spelling-to-phoneme
translation, the position of stressed syllables within words, or the
sound of phonemes, or problems with rhythm and vowel lengths.
