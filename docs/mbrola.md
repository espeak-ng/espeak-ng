# Mbrola Voices

- [Voice Names](#voice-names)
- [Windows Installation](#windows-installation)
- [Linux Installation](#linux-installation)
  - [Installation of standard packages](#installation-of-standard-packages)
  - [Installation of latest Mbrola packages](#installation-of-latest-mbrola-packages)
- [Usage](#usage)
- [Adding new Mbrola voice entry to eSpeak NG](#adding-new-mbrola-voice-entry-to-espeak-ng)
  - [1. Add Mbrola voice definition file](#1-add-mbrola-voice-definition-file)
  - [2. Add Mbrola phoneme translation file](#2-add-mbrola-phoneme-translation-file)
  - [3. Compile voice and update Makefile.am file](#3-compile-voice-and-update-makefileam-file)

----------

The Mbrola project is a collection of diphone voices for speech
synthesis. They do not include any text-to-phoneme translation, so this
must be done by another program. The Mbrola voices are cost-free but are
not open source. Voice files are available at
[http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html).

eSpeak NG can be used as a front-end to Mbrola. It provides the
spelling-to-phoneme translation and intonation, which Mbrola then uses
to generate speech sound.

## Voice Names

To use a Mbrola voice, eSpeak NG needs information to translate from its
own phonemes to the equivalent Mbrola phonemes.

The eSpeak NG voices which use Mbrola are named as:

	mb-xxN

where `xxN` is the name of a Mbrola voice (e.g. `mb-en1` for the Mbrola
`en1` English voice). These voice files are in eSpeak NG's folder
`.../espeak-ng-data/voices/mb` (where `...` is usually `/usr/share`,
but could be somewhere else).

The installation instructions below use the Mbrola voice `en1` as an
example. You can use other mbrola voices for which there is an
equivalent eSpeak NG voice in `espeak-ng-data/voices/mb`.

There are some additional eSpeak NG Mbrola voices, which speak English text
using a Mbrola voice for a different language. These contain the name of
the Mbrola voice with a suffix `-en`. For example, the voice
`mb-de4-en` will speak English text with a German accent by using the
Mbrola `de4` voice.

## Windows Installation

The SAPI5 version of eSpeak NG uses the `mbrola.dll`.

1.  Install eSpeak NG. Include the voice `mb-en1` in the list of voices during
    the eSpeak NG installation.

2.  Install the PC/Windows version of Mbrola (MbrolaTools35.exe) from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe](http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe).

3.  Get the `en1` voice from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html).

4.  Unpack the archive, and copy the `en1` data file (not the whole "en1" folder) into
    `C:/Program Files/eSpeak/espeak-ng-data/mbrola`.

4.  Use the voice `espeak-MB-EN1` from the list of SAPI5 voices.

## Linux Installation

### Installation of standard packages

There are standard packages prepared for Mbrola binary and voices on different Linux distributions.
On Debian/Ubuntu-like Linux, you can install mbrola using `apt-get` package manager:

	sudo apt-get install mbrola mbrola-en1

where:

* `mbrola` is package containing Mbrola executable,
* `mbrola-en1` is mbrola data files for **en1** Mbrola voice.

You can check other available voices searching with command:

	apt-cache search mbrola

### Installation of latest Mbrola packages

1.  To install the latest Mbrola binary for Linux, download:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip](http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pclinux/mbr301h.zip).

2.  Unpack the archive, and copy and rename the file from: `mbrola-linux-i386` to `mbrola` somewhere in your executable path (eg. `/usr/bin/mbrola`).

3.  Get for example `en1` voice from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html](http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html).

4.  Unpack the archive, and copy the `en1` data file (not the whole "en1" folder) to `/usr/share/mbrola/en1` folder.

5.  Remember to make `mbrola` file executable.

__Notes:__

* eSpeak NG will look for mbrola voices firstly in `espeak-ng-data/mbrola` and then in `/usr/share/mbrola`.
* You can download latest development versions of MBROLA binaries from [https://github.com/mbrola/mbrola-bin/](https://github.com/mbrola/mbrola-bin/)

## Usage

If you use the eSpeak NG voice such as `mb-en1` then eSpeak NG will use the mbrola "en1" voice, e.g.:

	espeak-ng -v mb-en1 "Hello world"

To generate mbrola phoneme data (.pho file) you can use:

	espeak-ng -v mb-en1 -q --pho "Hello world"

or

	espeak-ng -v mb-en1 -q --pho --phonout=out.pho "Hello world"

If you have issues with sound system integration, you can pass Mbrola's output to standard output,
which then can be played by ALSA (`aplay`) or PulseAudio (`paplay`) player throug the pipe:

        espeak-ng -vmb-en1 --stdout "Hello world"|aplay

## Adding new Mbrola voice entry to eSpeak NG

To add new Mbrola voice entry for eSpeak NG you have to:

1. add Mbrola voice definition file,
2. add Mbrola phoneme translation file,
3. update Makefile.am file and compile voice.

These steps are described in details in following sections.

### 1. Add Mbrola voice definition file

eSpeak NG's voice files for Mbrola voices are in `espeak-ng-data/voices/mb` folder.
Voice definition file is in form `mb-xxN` and have to contain at least this line:
`mbrola <voice> <translation>`, e.g.

	mbrola en1 en1_phtrans

Where:
* `en1` is the name of the Mbrola voice.
* `en1_phtrans` is a translation file to convert between eSpeak NG phonemes and
  the equivalent Mbrola phonemes.

Binary `xxN_phtrans` files are kept in `espeak-ng-data/mbrola_ph` folder
and are generated from `phsource/mb/xxN` text files, during Mbrola
voice compilation.

Additionaly Mbrola voice definition file can have other optional parameters,
similar to eSpeak NG voices, which are described [Voices](voices.md) file.

### 2. Add Mbrola phoneme translation file

Mbrola phoneme translation files specify translations from eSpeak NG
phoneme names to mbrola phoneme names.

The source phoneme translation files are in `.../phsource/mbrola` folder and
their name is in form `xxN` of referenced Mbrola voice.

_Note:_

* eSpeak NG phonemes are referenced from voice files in `phsource` folder of particular language e.g.
`ph_english` and/or general `phonemes` file.

* Mbrola phonemes are usualy listed in `xxN.txt` file of Mbrola voice.

Each line in the mbrola phoneme translation file contains:

	<control> <espeak ph1> <espeak ph2> <percent> <mbrola ph1> [<mbrola ph2>]

* `<control>` \
  bit 0 (+1)  skip the next phoneme \
  bit 1 (+2)  match this and previous phoneme \
  bit 2 (+4)  only at the start of a word \
  bit 3 (+8)  don't match two phonemes across a word boundary \
  bit 4 (+16) add this phoneme name as a prefix to the next phoneme name (used for de4 phoneme prefix '?') \
  bit 5 (+32) only in stressed syllable \
  bit 6 (+64) only at the end of a word

* `<espeak ph1>` \
  The eSpeak NG phoneme which is to be translated to an mbrola phoneme.

* `<espeak ph2>` \
  If this field is not `NULL`, then the match only occurs if
  this field matches the next phoneme. If control bit `1` is set, then the
  _previous_ rather than the _next_ phoneme is matched. This field may
  also have the following values:
  `VWL` \
  matches any Vowel phoneme.

* `<percent>` \
  If this field is zero then only one mbrola phoneme is used. If this
  field is non-zero, then two mbrola phonemes are used, and this value
  gives the percentage length of the first mbrola phoneme.

* `<mbrola ph1>` \
  The mbrola phoneme to which the eSpeak NG phoneme is translated. This
  field may be `NULL`.

* `<mbrola ph2>` \
  The second mbrola phoneme. This field is only used if the \<percent\>
  field is not zero.

The list is searched from start to finish, until a match is found.
Therefore, a line with more specific match condition should appear
before a line which matches the same eSpeak NG phoneme but with a more
general condition.

__Note:__

You can get list (and descriptions) of defined phonemes for particular eSpeak NG language
by entering command in `phsource` folder:

	egrep "^phoneme " phonemes ph_english|cut -d$' ' -f2-|sort

where `ph_english` is phoneme definition for particular language

Note that `ph_language` file can extend or override phoneme definitions
in `phonemes` file. Translations for all defined phonemes should be
given in the mbrola phoneme translation file.

If the language's `*_list` or `*_rules` files includes rules to speak
words "as English" the mbrola phoneme translation file should include
rules which translate English phonemes into near equivalents, so that
they can spoken by the Mbrola voice.

For latest Linux verson of `mbrola` you can get list of diphones for particular
language by entering command:

      mbrola -d /usr/share/mbrola/en1/en1

where `/usr/share/mbrola/en1/en1` is location of the voice file.

When phoneme translation source file is compiled (look at next section)
`espeak-ng-data/mbrola_ph/xxN_phtrans` file is created.

### 3. Compile voice and update Makefile.am file

Separate Mbrola voice can be compiled using comand:

	espeak-ng --compile-mbrola=<xxN>

where `xxN` is Mbrola voice name.

`Makefile.am` is build configuration file which should be extended, to include automatic compilation
of newly added Mbrola voice for eSpeak NG.

Search for  `mbrola: \` line in `Makefile.am` and add additional line for newly created Mbrola voice, e.g.:

	mbrola: \
	...
	espeak-ng-data/mbrola_ph/xxN_phtrans \
	...
	espeak-ng-data/mbrola_ph/xxN_phtrans: phsource/mbrola/xxN src/espeak-ng
		mkdir -p espeak-ng-data/mbrola_ph
		ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/xxN

Note that it could be that several voices share the same translation file. Then translation file
is named just `xx`.

When `Makefile.am` is update, when espek-ng will be compiled, e.g. by
entering command `automake; make -B` newly added Mbrola voice will be compiled automatically.

