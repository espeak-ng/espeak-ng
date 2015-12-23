# Table of contents

  * [Languages](#languages)
      * [Help Needed](#help-needed)
      * [Character sets](#character-sets)
    * [Voice Files](#voice-files)
      * [Default Voice](#default-voice)
    * [English Voices](#english-voices)
    * [Voice Variants](#voice-variants)
    * [Other Languages](#other-languages)
    * [Provisional Languages](#provisional-languages)
    * [Mbrola Voices](#mbrola-voices)

# Languages

The eSpeak NG speech synthesizer supports several languages,
however in many cases these are initial drafts and need more work to
improve them. Assistance from native speakers is welcome for these, or
other new languages. Please contact me if you want to help.

eSpeak NG does text to speech synthesis for the following languages, some
better than others. Afrikaans, Albanian, Armenian, Cantonese, Catalan,
Croatian, Czech, Danish, Dutch, English, Esperanto, Finnish, French,
German, Greek, Hindi, Hungarian, Icelandic, Indonesian, Italian,
Kurdish, Latvian, Lojban, Macedonian, Mandarin, Norwegian, Polish,
Portuguese, Romanian, Russian, Serbian, Slovak, Spanish, Swahili,
Swedish, Tamil, Turkish, Vietnamese, Welsh.


### Help Needed

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
Language](add_language.md)

### Character sets

Languages recognise text either as UTF8 or alternatively in an 8-bit
character set which is appropriate for that language. For example, for
Polish this is Latin2, for Russian it is KOI8-R. This choice can be
overridden by a line in the voices file to specify an ISO 8859 character
set, eg. for Russian the line:

     charset 5

will mean that ISO 8859-5 is used as the 8-bit character set rather than
KOI8-R.

In the case of a language which uses a non-Latin character set (eg.
Greek or Russian) if the text contains a word with Latin characters then
that particular word will be pronounced using English pronunciation
rules and English phonemes. Speaking entirely English text using a Greek
or Russian voice will sound OK, but each word is spoken separately so it
won't flow properly.

Sample texts in various languages can be found at
[wikipedia](http://meta.wikimedia.org/wiki/List_of_Wikipedias)
and [gutenberg](http://www.gutenberg.org/)

## Voice Files

A number of Voice files are provided in the
`espeak-data/voices` directory. You can select one of these
with the `-v \<voice filename\>` parameter to the speak command, eg:

   espeak-ng -vaf

to speak using the Afrikaans voice.

Language voices generally start with the 2 letter [ISO 639-1
code](http://en.wikipedia.org/wiki/ISO_639-1) for the language. If the
language does not have an ISO 639-1 code, then the 3 letter [ISO 639-3
code](http://www.sil.org/iso639-3/codes.asp) can be used.

For details of the voice files see [Voices](voices.html).

### Default Voice

**default**  
  This voice is used if none is specified in the speak command. Copy your preferred voice to "default" so you can use the speak command without the need to specify a voice.

## English Voices

* **en**  
  is the standard default English voice.

* **en-us**  
  American English.

* **en-sc**  
  English with a Scottish accent.

* **en-n**  
  en-rp  
  en-wm**  
  are different English voices. These can be considered caricatures of various British accents: Northern, Received Pronunciation, West Midlands respectively.

## Voice Variants

To make alternative voices for a language, you can make additional voice
files in espeak-data/voices which contains commands to change various
voice and pronunciation attributes. See [voices](voices.md).

Alternatively there are some preset voice variants which can be applied
to any of the language voices, by appending **+** and a variant
name. Their effects are defined by files in
`espeak-data/voices/!v`.

The variants are `+m1 +m2 +m3 +m4 +m5 +m6 +m7` for male
voices, `+f1 +f2 +f3 +f4 +f5 `for female voices, and
`+croak +whisper` for other effects. For example:

   espeak-ng -ven+m3

The available voice variants can be listed with:

   espeak-ng --voices=variant

## Other Languages

The eSpeak NG speech synthesizer does text to speech for the following
additional langauges.

## Provisional Languages

These languages are only initial naive implementations which have had
little or no feedback and improvement from native speakers.

## Mbrola Voices

Some additional voices, whose name start with **mb-** (for example
**mb-en1**) use eSpeak NG as a front-end to Mbrola diphone voices. eSpeak NG
does the spelling-to-phoneme translation and intonation. See
[mbrola](mbrola.md).
