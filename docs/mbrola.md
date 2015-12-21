MBROLA VOICES {.western}
-------------

The Mbrola project is a collection of diphone voices for speech
synthesis. They do not include any text-to-phoneme translation, so this
must be done by another program. The Mbrola voices are cost-free but are
not open source. They are available from the Mbrola website at:\

[http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html)

eSpeak can be used as a front-end to Mbrola. It provides the
spelling-to-phoneme translation and intonation, which Mbrola then uses
to generate speech sound.

### Voice Names {.western}

To use a Mbrola voice, eSpeak needs information to translate from its
own phonemes to the equivalent Mbrola phonemes. This has been set up for
only some voices so far.

The eSpeak voices which use Mbrola are named as:\
   **mb-**xxx

where xxx is the name of a Mbrola voice (eg. **mb-en1** for the Mbrola
"**en1**" English voice). These voice files are in eSpeak's directory
`espeak-data/voices/mbrola`{.western}.

The installation instructions below use the Mbrola voice "en1" as an
example. You can use other mbrola voices for which there is an
equivalent eSpeak voice in `espeak-data/voices/mbrola`{.western}.

There are some additional eSpeak Mbrola voices which speak English text
using a Mbrola voice for a different language. These contain the name of
the Mbrola voice with a suffix **-en**. For example, the voice
**mb-de4-en** will speak English text with a German accent by using the
Mbrola **de4** voice.

### Windows Installation {.western}

The SAPI5 version of eSpeak uses the mbrola.dll.

1.  2.  3.  4.  

### Linux Installation {.western}

From eSpeak version 1.44 onwards, eSpeak calls the mbrola program
directly, rather than passing phoneme data to it using a pipe.

1.  2.  3.  

### Mbrola Voice Files {.western}

eSpeak's voice files for Mbrola voices are in directory
`espeak-data/voices/mbrola`{.western}. They contain a line:\
   `mbrola <voice> <translation>`{.western} \
 eg.\
   `mbrola en1 en1_phtrans`{.western}

-   -   

They are binary files which are compiled, using espeakedit, from source
files in `phsource/mbrola`{.western}, see below.

### Mbrola Phoneme Translation Data {.western}

Mbrola phoneme translation files specify translations from eSpeak
phoneme names to mbrola phoneme names. They are referenced from voice
files.

The source files are in `phsource/mbrola`{.western}. These are compiled
using the `espeakedit`{.western} program
(`Compile->Compile mbrola phonemes list`{.western}) to produce data
files in `espeak-data/mbrola_ph`{.western} which are used by eSpeak.

Each line in the mbrola phoneme translation file contains:

`<control> <espeak ph1> <espeak ph2> <percent> <mbrola ph1> [<mbrola ph2>] `{.western}

**\<control\>**

-   -   -   -   

**\<espeak ph1\>**\
 The eSpeak phoneme which is to be translated to an mbrola phoneme.

**\<espeak ph2\>**\
 If this field is not `NULL`{.western}, then the match only occurs if
this field matches the next phoneme. If control bit 1 is set, then the
*previous* rather than the *next* phoneme is matched. This field may
also have the following values:\
 `VWL`{.western}   matches any Vowel phoneme.

**\<percent\>**\
 If this field is zero then only one mbrola phoneme is used. If this
field is non-zero, then two mbrola phonemes are used, and this value
gives the percentage length of the first mbrola phoneme.

**\<mbrola ph1\>**\
 The mbrola phoneme to which the eSpeak phoneme is translated. This
field may be `NULL`{.western}.

**\<mbrola ph2\>**\
 The second mbrola phoneme. This field is only used if the \<percent\>
field is not zero.

The list is searched from start to finish, until a match is found.
Therefore, a line with more specific match condition should appear
before a line which matches the same eSpeak phoneme but with a more
general condition.

The file `dictsource/dict_phonemes`{.western} lists the eSpeak phonemes
which are used for each language. Translations for all these should be
given in the mbrola phoneme translation file. In addition, some phonemes
which are referenced from phoneme files (eg.
`phsource/ph_language, phsource/phonemes`{.western}) in lines such as:

~~~~ {.western}
   beforenotvowel   l/
   reduceto  a#  0
~~~~

should also be included, even though they don't appear in
`dictsource/dict_phonemes`{.western}.

If the language's \*\_list or \*\_rules files includes rules to speak
words "as English" the mbrola phoneme translation file should include
rules which translate English phonemes into near equivalents, so that
they can spoken by the mbrola voice.
