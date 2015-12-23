# Table of contents

* [Linux and other Posix systems](#linux-and-other-posix-systems)
* [Dependencies](#dependencies)
* [Windows](#windows)
* [Examples](#examples)
* [The Command Line Options](#the-command-line-options)
* [The Input Text](#the-input-text)

# INSTALLATION

## Linux and other Posix systems

There are two versions of the command line program. They both have the
same command parameters (see below).


1. **espeak** uses speech engine in the **libespeak** shared library. The libespeak library must first be installed.

1. **speak** is a stand-alone version which includes its own copy of the speech engine. 

Place the **espeak-ng** or **speak-ng** executable file in the command
path, e.g. in **/usr/local/bin**

Place the **espeak-data** directory in **/usr/share** as **/usr/share/espeak-data**.

Alternatively if it is placed in the user's home directory (e.g. **/home/\<user\>/espeak-data**) then that will be used instead.

## Dependencies

**espeak-ng** uses the PortAudio sound library (version 18), so you will need to have the **libportaudio0** library package installed. It may be already, since it' used by other software, such as OpenOffice.org and the Audacity sound editor.

Some Linux distrubitions (eg. SuSe 10) have version 19 of PortAudio which has a slightly different API. The speak program can be compiled to use version 19 of PortAudio by copying the file portaudio19.h to portaudio.h before compiling.

The speak program may be compiled without using PortAudio, by removing
the line

    #define USE_PORTAUDIO

in the file speech.h.

## Windows

The installer: **setup\_espeak.exe** installs the SAPI5 version of
eSpeak NG. During installation you need to specify which voices you want to
appear in SAPI5 voice menus.

It also installs a command line program **espeak-ng** in the espeak-ng
program directory.

## Examples

To use at the command line, type:
```
espeak-ng "This is a test"
```
or
```
espeak-ng -f <text file>
```
or just type
```
espeak-ng
```
followed by text on subsequent lines. Each line is spoken when RETURN
is pressed.

Use **espeak-ng -x** to see the corresponding phoneme codes.

## The Command Line Options

* **espeak-ng [options] ["text words"]**  
  Text input can be taken either from a file, from a string in the command, or from stdin.

* **-f \<text file\>**  
  Speaks a text file.

* **--stdin**  
  Takes the text input from stdin. If neither -f nor --stdin is given, then the text input is taken from "text words" (a text string within double quotes). If that is not present then text is taken from stdin, but each line is treated as a separate sentence.

* **-a \<integer\>**  
  Sets amplitude (volume) in a range of 0 to 200. The default is 100.

* **-p \<integer\>**  
  Adjusts the pitch in a range of 0 to 99. The default is 50.

* **-s \<integer\>**  
  Sets the speed in words-per-minute (approximate values for the
default English voice, others may differ slightly). The default
value is 175. I generally use a faster speed of 260. The lower limit
is 80. There is no upper limit, but about 500 is probably a
practical maximum.

* **-b \<integer\>**  
  Input text character format.
  1   UTF-8. This is the default.
  2   The 8-bit character set which corresponds to the language (eg. Latin-2 for Polish).
  4   16 bit Unicode.
  Without this option, eSpeak NG assumes text is UTF-8, but will
automatically switch to the 8-bit character set if it finds an
illegal UTF-8 sequence.

* **-g \<integer\>**  
  Word gap. This option inserts a pause between words. The value is
the length of the pause, in units of 10 mS (at the default speed of
170 wpm).

* **-h** or **--help**  
  The first line of output gives the eSpeak NG version number.

* **-k \<integer\>**  
  Indicate words which begin with capital letters.
  1   eSpeak NG uses a click sound to indicate when a word starts with a
 apital letter, or double click if word is all capitals.
  2   eSpeak NG speaks the word "capital" before a word which begins with
  capital letter.
  Other values:   eSpeak NG increases the pitch for words which begin
with a capital letter. The greater the value, the greater the
increase in pitch. Try -k20.

* **-l \<integer\>**  
  Line-break length, default value 0. If set, then lines which are
shorter than this are treated as separate clauses and spoken
separately with a break between them. This can be useful for some
text files, but bad for others.

* **-m**  
  Indicates that the text contains SSML (Speech Synthesis Markup
Language) tags or other XML tags. Those SSML tags which are
supported are interpreted. Other tags, including HTML, are ignored,
except that some HTML tags such as \<hr\> \<h2\> and \<li\> ensure a
break in the speech.

* **-q**  
  Quiet. No sound is generated. This may be useful with options such
as -x and --pho.

* **-v \<voice filename\>[+\<variant\>]**  
  Sets a Voice for the speech, usually to select a language. eg:
`espeak-ng -vaf` To use the Afrikaans voice. A modifier after the voice name can be used to vary the tone of the voice, eg:
`espeak-ng -vaf+3`
The variants are `+m1 +m2 +m3 +m4 +m5 +m6 +m7` for male voices and `+f1 +f2 +f3 +f4 `which simulate female voices by using higher pitches. Other variants include `+croak` and `+whisper`.
\<voice filename\> is a file within the `espeak-data/voices` directory.\
\<variant\> is a file within the `espeak-data/voices/!v` directory.
Voice files can specify a language, alternative pronunciations or phoneme sets, different pitches, tonal qualities, and prosody for the voice. See the [voices](voices.md) file.
Voice names which start with **mb-** are for use with Mbrola diphone voices, see [mbrola](mbrola.md)
Some languages may need additional dictionary data, see [languages](languages.md)

* **-w \<wave file\>**  
  Writes the speech output to a file in WAV format, rather than speaking it.

* **-x**  
  The phoneme mnemonics, into which the input text is translated, are written to stdout. If a phoneme name contains more than one letter (eg. [tS]), the --sep or --tie option can be used to distinguish this from separate phonemes.

* **-X**  
  As -x, but in addition, details are shown of the pronunciation rule and dictionary list lookup. This can be useful to see why a certain pronunciation is being produced. Each matching pronunciation rule is listed, together with its score, the highest scoring rule being used in the translation. "Found:" indicates the word was found in the dictionary lookup list, and "Flags:" means the word was found with only properties and not a pronunciation. You can see when a word has been retranslated after removing a prefix or suffix.

* **-z**  
  The option removes the end-of-sentence pause which normally occurs at the end of the text.

* **--stdout**  
  Writes the speech output to stdout as it is produced, rather than speaking it. The data starts with a WAV file header which indicates the sample rate and format of the data. The length field is set to zero because the length of the data is unknown when the header is produced.

* **--compile [=\<voice name\>]**  
  Compile the pronunciation rule and dictionary lookup data from their source files in the current directory. The Voice determines which language's files are compiled. For example, if it's an English voice, then *en\_rules*, *en\_list*, and *en\_extra* (if present), are compiled to replace *en\_dict* in the *speak-data* directory. If no Voice is specified then the default Voice is used.

* **--compile-debug [=\<voice name\>]**  
  The same as **--compile**, but source line numbers from the \*\_rules file are included. These are included in the rules trace when the **-X** option is used.


* **--ipa**  
  Writes phonemes to stdout, using the International Phonetic Alphabet (IPA). If a phoneme name contains more than one letter (eg. [tS]), the --sep or --tie option can be used to distinguish this from separate phonemes.

* **--path [="\<directory path\>"]**  
  Specifies the directory which contains the espeak-data directory.

* **--pho**  
  When used with an mbrola voice (eg. -v mb-en1), it writes mbrola phoneme data (.pho file format) to stdout. This includes the mbrola phoneme names with duration and pitch information, in a form which is suitable as input to this mbrola voice. The --phonout option can be used to write this data to a file.

* **--phonout [="\<filename\>"]**  
  If specified, the output from -x, -X, --ipa, and --pho options is written to this file, rather than to stdout.

* **--punct [="\<characters\>"]**  
  Speaks the names of punctuation characters when they are encountered in the text. If \<characters\> are given, then only those listed punctuation characters are spoken, eg. `--punct=".,;?"`
 
* **--sep [=\<character\>]**  
  The character is used to separate individual phonemes in the output which is produced by the -x or --ipa options. The default is a space character. The character z means use a ZWNJ character (U+200c).

* **--split [=\<minutes\>]**  
  Used with **-w**, it starts a new WAV file every `<minutes>` minutes, at the next sentence boundary.

* **--tie [=\<character\>]**  
  The character is used within multi-letter phonemes in the output which is produced by the -x or --ipa options. The default is the tie character  ͡ U+361. The character z means use a ZWJ character (U+200d).

* **--voices [=\<language code\>]**  
  Lists the available voices.
  If =\<language code\> is present then only those voices which are suitable for that language are listed.
  `--voices=mbrola` lists the voices which use mbrola diphone voices. These are not included in the default `--voices` list.
  `--voices=variant` lists the available voice variants (voice modifiers).

## The Input Text

* **HTML Input**  
  If the -m option is used to indicate marked-up text, then HTML can
be spoken directly.

* **Phoneme Input**  
  As well as plain text, phoneme mnemonics can be used in the text
input to **espeak-ng**. They are enclosed within double square
brackets. Spaces are used to separate words and all stressed
syllables must be marked explicitly.
eg:  
  `espeak-ng -v en "[[D,Is Iz sVm f@n'EtIk t'Ekst 'InpUt]]"`
  This command will speak: "This is some phonetic text input".

