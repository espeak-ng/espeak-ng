# Mbrola Voices

- [Voice Names](#voice-names)
- [Windows Installation](#windows-installation)
- [Linux Installation](#linux-installation)
- [Udage](#usage)
- [Mbrola Voice Files](#mbrola-voice-files)
- [Mbrola Phoneme Translation Data](#mbrola-phoneme-translation-data)

----------

The Mbrola project is a collection of diphone voices for speech
synthesis. They do not include any text-to-phoneme translation, so this
must be done by another program. The Mbrola voices are cost-free but are
not open source. They are available from the Mbrola website at:
[http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html)

eSpeak NG can be used as a front-end to Mbrola. It provides the
spelling-to-phoneme translation and intonation, which Mbrola then uses
to generate speech sound.

## Voice Names

To use a Mbrola voice, eSpeak NG needs information to translate from its
own phonemes to the equivalent Mbrola phonemes. This has been set up for
only some voices so far.

The eSpeak NG voices which use Mbrola are named as:

	mb-xxx

where `xxx` is the name of a Mbrola voice (e.g. `mb-en1` for the Mbrola
`en1` English voice). These voice files are in eSpeak NG's directory
`espeak-ng-data/voices/mbrola`.

The installation instructions below use the Mbrola voice "en1" as an
example. You can use other mbrola voices for which there is an
equivalent eSpeak NG voice in `espeak-ng-data/voices/mbrola`.

There are some additional eSpeak NG Mbrola voices which speak English text
using a Mbrola voice for a different language. These contain the name of
the Mbrola voice with a suffix `-en`. For example, the voice
`mb-de4-en` will speak English text with a German accent by using the
Mbrola `de4` voice.

## Windows Installation

The SAPI5 version of eSpeak NG uses the mbrola.dll.

1.  Install eSpeak. Include the voice `mb-en1` in the list of voices during
    the eSpeak installation.

2.  Install the PC/Windows version of Mbrola (MbrolaTools35.exe) from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe](http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe).

3.  Get the `en1` voice from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html).

4.  Unpack the archive, and copy the `en1` data file (not the whole "en1" directory) into
    `C:/Program Files/eSpeak/espeak-ng-data/mbrola`.

4.  Use the voice `espeak-MB-EN1` from the list of SAPI5 voices.

## Linux Installation

From eSpeak NG version 44 onwards, eSpeak NG calls the mbrola program
directly, rather than passing phoneme data to it using a pipe.

1.  To install the Linux Mbrola binary, download:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip](http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip).

2.  Unpack the archive, and copy and rename the file from: `mbrola-linux-i386` to `mbrola` somewhere in your executable path (eg. `/usr/bin/mbrola`).

3.  Get the `en1` voice from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html).

4.  Unpack the archive, and copy the `en1` data file (not the whole "en1" directory) to `/usr/share/mbrola/en1`.

    __NOTE:__ eSpeak will look for mbrola voices firstly in `espeak-ng-data/mbrola` and then in `/usr/share/mbrola`.

## Usage

If you use the eSpeak NG voice such as `mb-en1` then eSpeak NG will use the mbrola "en1" voice, e.g.:

	espeak-ng -v mb-en1 "Hello world"

To generate mbrola phoneme data (.pho file) you can use:

	espeak-ng -v mb-en1 -q --pho "Hello world"

or

	espeak-ng -v mb-en1 -q --pho --phonout=out.pho "Hello world"

## Mbrola Voice Files

eSpeak NG's voice files for Mbrola voices are in directory  `espeak-ng-data/voices/mbrola`.  
They contain a line: `mbrola <voice> <translation>`

e.g.

	mbrola en1 en1_phtrans

* \<voice\> is the name of the Mbrola voice.
* \<translation\> is a translation file to convert between eSpeak phonemes and
  the equivalent Mbrola phonemes.

These are kept in: `espeak-ng-data/mbrola_ph`

## Mbrola Phoneme Translation Data

Mbrola phoneme translation files specify translations from eSpeak NG
phoneme names to mbrola phoneme names. They are referenced from voice
files.

The source files are in `phsource/mbrola`. These are compiled using:

	espeak-ng --compile-mbrola=<mbrola voice>

Each line in the mbrola phoneme translation file contains:

	<control> <espeak ph1> <espeak ph2> <percent> <mbrola ph1> [<mbrola ph2>]

**\<control\>**

bit 0   skip the next phoneme  
bit 1   match this and Previous phoneme  
bit 2   only at the start of a word  
bit 3   don't match two phonemes across a word boundary

**\<espeak ph1\>**  

The eSpeak NG phoneme which is to be translated to an mbrola phoneme.

**\<espeak ph2\>**

If this field is not `NULL`, then the match only occurs if
this field matches the next phoneme. If control bit 1 is set, then the
*previous* rather than the *next* phoneme is matched. This field may
also have the following values:  

* `VWL` matches any Vowel phoneme.

**\<percent\>**

If this field is zero then only one mbrola phoneme is used. If this
field is non-zero, then two mbrola phonemes are used, and this value
gives the percentage length of the first mbrola phoneme.

**\<mbrola ph1\>**

The mbrola phoneme to which the eSpeak NG phoneme is translated. This
field may be `NULL`.

**\<mbrola ph2\>**

The second mbrola phoneme. This field is only used if the \<percent\>
field is not zero.

The list is searched from start to finish, until a match is found.
Therefore, a line with more specific match condition should appear
before a line which matches the same eSpeak NG phoneme but with a more
general condition.

The file `dictsource/dict_phonemes` lists the eSpeak NG phonemes
which are used for each language. Translations for all these should be
given in the mbrola phoneme translation file. In addition, some phonemes
which are referenced from phoneme files (e.g.
`phsource/ph_language, phsource/phonemes`) in lines such as:

	beforenotvowel   l/
	reduceto  a#  0

should also be included, even though they don't appear in
`dictsource/dict_phonemes`.

If the language's `*_list` or `*_rules` files includes rules to speak
words "as English" the mbrola phoneme translation file should include
rules which translate English phonemes into near equivalents, so that
they can spoken by the mbrola voice.
