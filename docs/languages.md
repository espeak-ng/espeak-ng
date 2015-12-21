3. LANGUAGES {.western}
------------

**Languages**. The eSpeak speech synthesizer supports several languages,
however in many cases these are initial drafts and need more work to
improve them. Assistance from native speakers is welcome for these, or
other new languages. Please contact me if you want to help.

eSpeak does text to speech synthesis for the following languages, some
better than others. Afrikaans, Albanian, Armenian, Cantonese, Catalan,
Croatian, Czech, Danish, Dutch, English, Esperanto, Finnish, French,
German, Greek, Hindi, Hungarian, Icelandic, Indonesian, Italian,
Kurdish, Latvian, Lojban, Macedonian, Mandarin, Norwegian, Polish,
Portuguese, Romanian, Russian, Serbian, Slovak, Spanish, Swahili,
Swedish, Tamil, Turkish, Vietnamese, Welsh.


#### Help Needed {.western}

Many of these are just experimental attempts at these languages,
produced after a quick reading of the corresponding article on
wikipedia.org. They will need work or advice from native speakers to
improve them. Please contact me if you want to advise or assist with
these or other languages.

The sound of some phonemes may be poorly implemented, particularly [r]
since I'm English and therefore unable to make a "proper" [r] sound.

A major factor is the rhythm or cadance. An Italian speaker told me the
Italian voice improved from "difficult to understand" to "good" by
changing the relative length of stressed syllables. Identifying
unstressed function words in the xx\_list file is also important to make
the speech flow well. See [Adding or Improving a
Language](add_language.html)

#### Character sets {.western}

Languages recognise text either as UTF8 or alternatively in an 8-bit
character set which is appropriate for that language. For example, for
Polish this is Latin2, for Russian it is KOI8-R. This choice can be
overridden by a line in the voices file to specify an ISO 8859 character
set, eg. for Russian the line:

~~~~ {.western style="margin-bottom: 0.5cm"}
     charset 5
~~~~

will mean that ISO 8859-5 is used as the 8-bit character set rather than
KOI8-R.

In the case of a language which uses a non-Latin character set (eg.
Greek or Russian) if the text contains a word with Latin characters then
that particular word will be pronounced using English pronunciation
rules and English phonemes. Speaking entirely English text using a Greek
or Russian voice will sound OK, but each word is spoken separately so it
won't flow properly.

Sample texts in various languages can be found at
[http://\<language\>.wikipedia.org](http://meta.wikimedia.org/wiki/List_of_Wikipedias)
and [www.gutenberg.org](http://www.gutenberg.org/)

### 3.1 Voice Files {.western}

A number of Voice files are provided in the
`espeak-data/voices`{.western} directory. You can select one of these
with the **-v \<voice filename\>** parameter to the speak command, eg:

~~~~ {.western style="margin-bottom: 0.5cm"}
   espeak-ng -vaf
~~~~

to speak using the Afrikaans voice.

Language voices generally start with the 2 letter [ISO 639-1
code](http://en.wikipedia.org/wiki/ISO_639-1) for the language. If the
language does not have an ISO 639-1 code, then the 3 letter [ISO 639-3
code](http://www.sil.org/iso639-3/codes.asp) can be used.

For details of the voice files see [Voices](voices.html).

#### Default Voice {.western}

### 3.2 English Voices {.western}

### 3.3 Voice Variants {.western}

To make alternative voices for a language, you can make additional voice
files in espeak-data/voices which contains commands to change various
voice and pronunciation attributes. See [voices.html](voices.html).

Alternatively there are some preset voice variants which can be applied
to any of the language voices, by appending `+`{.western} and a variant
name. Their effects are defined by files in
`espeak-data/voices/!v`{.western}.

The variants are `+m1 +m2 +m3 +m4 +m5 +m6 +m7`{.western} for male
voices, `+f1 +f2 +f3 +f4 +f5 `{.western}for female voices, and
`+croak +whisper`{.western} for other effects. For example:

~~~~ {.western style="margin-bottom: 0.5cm"}
   espeak-ng -ven+m3
~~~~

The available voice variants can be listed with:

~~~~ {.western style="margin-bottom: 0.5cm"}
   espeak-ng --voices=variant
~~~~

### 3.4 Other Languages {.western}

The eSpeak speech synthesizer does text to speech for the following
additional langauges.

### 3.5 Provisional Languages {.western}

These languages are only initial naive implementations which have had
little or no feedback and improvement from native speakers.

### 3.6 Mbrola Voices {.western}

Some additional voices, whose name start with **mb-** (for example
**mb-en1**) use eSpeak as a front-end to Mbrola diphone voices. eSpeak
does the spelling-to-phoneme translation and intonation. See
[mbrola.html](mbrola.html).
