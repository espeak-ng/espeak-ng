# MBROLA Voices

- [Voice Names](#voice-names)
- [Windows Installation](#windows-installation)
- [Linux Installation](#linux-installation)
  - [Installation of standard packages](#installation-of-standard-packages)
  - [Installation of MBROLA package from source](#installation-of-mbrola-package-from-source)
- [Usage](#usage)
- [Adding new MBROLA voice entry to eSpeak NG](#adding-new-mbrola-voice-entry-to-espeak-ng)
  - [1. Add MBROLA voice definition file](#1-add-mbrola-voice-definition-file)
  - [2. Add MBROLA phoneme translation file](#2-add-mbrola-phoneme-translation-file)
  - [3. Compile voice and update Makefile.am file](#3-compile-voice-and-update-makefileam-file)

----------

The [MBROLA](https://github.com/numediart/MBROLA) is an open source speech engine
with collection of diphone voices for speech synthesis. Voices are cost-free for
non-commerical purposes, but are not open source.

MBROLA does not include any text-to-phoneme translation, so this
must be done by another program. eSpeak NG can be used as a front-end to MBROLA,
to provide spelling-to-phoneme translation and intonation, which MBROLA then uses
to generate speech sound.

## Voice Names

To use a MBROLA voice, eSpeak NG needs information to translate from its
own phonemes to the equivalent MBROLA phonemes.

The eSpeak NG voices which use MBROLA are named as:

	mb-xxN

where `xxN` is the name of a MBROLA voice (e.g. `mb-en1` for the MBROLA
`en1` English voice). These voice files are in eSpeak NG's folder
`.../espeak-ng-data/voices/mb` (where `...` is usually `/usr/share`,
but could be somewhere else).

There are some additional eSpeak NG MBROLA voices, which speak English text
using a MBROLA voice for a different language. These contain the name of
the MBROLA voice with a suffix `-en`. For example, the voice
`mb-de4-en` will speak English text with a German accent by using the
MBROLA `de4` voice.

The following MBROLA voices are available in eSpeak NG<sup>[1](#footnote1)</sup>.

| MBROLA Voice | Language             | Gender | eSpeak   | Debian Package |
|--------------|----------------------|--------|----------|----------------|
| `af1`        | Afrikaans            | male   | `mb-af1` | `mbrola-af1`   |
| `ar1`        | Arabic               | male   | `mb-ar1` |                |
| `ar2`        | Arabic               | male   | `mb-ar2` |                |
| `br1`        | Brazillian Portugues | male   | `mb-br1` | `mbrola-br1`   |
| `br2`        | Brazillian Portugues | male   | `mb-br2` | `mbrola-br2`   |
| `br3`        | Brazillian Portugues | male   | `mb-br3` | `mbrola-br3`   |
| `br4`        | Brazillian Portugues | female | `mb-br4` | `mbrola-br4`   |
| `bz1`        | Breton               | female |          |                |
| `ca1`        | Canadian French      | male   | `mb-ca1` |                |
| `ca2`        | Canadian French      | male   | `mb-ca2` |                |
| `cn1`        | Mandarin Chinese     | female | `mb-cn1` |                |
| `cr1`        | Croatian             | male   | `mb-cr1` | `mbrola-cr1`   |
| `cz1`        | Czech                | female | `mb-cz1` |                |
| `cz2`        | Czech                | male   | `mb-cz2` | `mbrola-cz2`   |
| `de1`        | German               | female | `mb-de1` | `mbrola-de1`   |
| `de2`        | German               | male   | `mb-de2` | `mbrola-de2`   |
| `de3`        | German               | female | `mb-de3` | `mbrola-de3`   |
| `de4`        | German               | male   | `mb-de4` | `mbrola-de4`   |
| `de5`        | German               | female | `mb-de5` | `mbrola-de5`   |
| `de6`        | German               | male   | `mb-de6` | `mbrola-de6`   |
| `de7`        | German               | female | `mb-de7` | `mbrola-de7`   |
| `de8`        | German-Bavarian      | male   | `mb-de8` |                |
| `ee1`        | Estonian             | male   | `mb-ee1` | `mbrola-ee1`   |
| `en1`        | British English      | female | `mb-en1` | `mbrola-en1`   |
| `es1`        | Spanish              | male   | `mb-es1` | `mbrola-es1`   |
| `es2`        | Spanish              | male   | `mb-es2` | `mbrola-es2`   |
| `es3`        | Spanish              | female | `mb-es3` |                |
| `es4`        | Spanish              | male   | `mb-es4` |                |
| `fr1`        | French               | male   | `mb-fr1` | `mbrola-fr1`   |
| `fr2`        | French               | female | `mb-fr2` |                |
| `fr3`        | French               | male   | `mb-fr3` |                |
| `fr4`        | French               | female | `mb-fr4` | `mbrola-fr4`   |
| `fr5`        | Belgian French       | male   | `mb-fr5` |                |
| `fr6`        | French               | male   | `mb-fr6` |                |
| `fr7`        | Belgian French       | male   | `mb-fr7` |                |
| `gr1`        | Greek                | male   | `mb-gr1` | `mbrola-gr1`   |
| `gr2`        | Greek                | male   | `mb-gr2` | `mbrola-gr2`   |
| `hb1`        | Hebrew               | male   | `mb-hb1` |                |
| `hb2`        | Hebrew               | female | `mb-hb2` |                |
| `hn1`        | Korean               | male   |          |                |
| `hu1`        | Hungarian            | female | `mb-hu1` | `mbrola-hu1`   |
| `ic1`        | Icelandic            | male   | `mb-ic1` | `mbrola-ic1`   |
| `in1`        | Hindi                | male   | `mb-in1` |                |
| `in2`        | Hindi                | female | `mb-in2` |                |
| `id1`        | Indonesian           | male   | `mb-id1` | `mbrola-id1`   |
| `ir1`        | Iranian Persian      | male   | `mb-ir1` | `mbrola-ir1`   |
| `it1`        | Italian              | male   | `mb-it1` |                |
| `it2`        | Italian              | female | `mb-it2` |                |
| `it3`        | Italian              | male   | `mb-it3` | `mbrola-it3`   |
| `it4`        | Italian              | female | `mb-it4` | `mbrola-it4`   |
| `jp1`        | Japanese             | male   | `mb-jp1` |                |
| `jp2`        | Japanese             | female | `mb-jp2` |                |
| `jp3`        | Japanese             | female | `mb-jp3` |                |
| `la1`        | Classical Latin      | male   | `mb-la1` | `mbrola-la1`   |
| `lt1`        | Lithuanian           | male   | `mb-lt1` | `mbrola-lt1`   |
| `lt2`        | Lithuanian           | male   | `mb-lt2` | `mbrola-lt2`   |
| `ma1`        | Malay                | female | `mb-ma1` |                |
| `mx1`        | Mexican Spanish      | male   | `mb-mx1` | `mbrola-mx1`   |
| `mx2`        | Mexican Spanish      | male   | `mb-mx2` | `mbrola-mx2`   |
| `nl1`        | Dutch                | male   | `mb-nl1` <sup>[2](#footnote2)</sup> |   |
| `nl2`        | Dutch                | male   | `mb-nl2` | `mbrola-nl2`   |
| `nl3`        | Dutch                | female | `mb-nl3` |                |
| `nz1`        | Maori                | male   | `mb-nz1` |                |
| `pl1`        | Polish               | female | `mb-pl1` | `mbrola-pl1`   |
| `pt1`        | Portuguese           | female | `mb-pt1` | `mbrola-pt1`   |
| `ro1`        | Romanian             | male   | `mb-ro1` | `mbrola-ro1`   |
| `sw1`        | Swedish              | male   | `mb-sw1` | `mbrola-sw1`   |
| `sw2`        | Swedish              | female | `mb-sw2` | `mbrola-sw2`   |
| `tl1`        | Telugu               | female | `mb-tl1` |                |
| `tr1`        | Turkish              | male   | `mb-tr1` | `mbrola-tr1`   |
| `tr2`        | Turkish              | female | `mb-tr2` | `mbrola-tr2`   |
| `us1`        | American English     | female | `mb-us1` | `mbrola-us1`   |
| `us2`        | American English     | male   | `mb-us2` | `mbrola-us2`   |
| `us3`        | American English     | male   | `mb-us3` | `mbrola-us3`   |
| `vz1`        | Venezuelan Spanish   | male   | `mb-vz1` | `mbrola-vz1`   |

eSpeak NG will look for mbrola voices firstly in `espeak-ng-data/mbrola` and then in `/usr/share/mbrola`.

The installation instructions below use the MBROLA voice `en1` as an
example. You can use other mbrola voices for which there is an
equivalent eSpeak NG voice in `espeak-ng-data/voices/mb`.

## Windows Installation

The SAPI5 version of eSpeak NG uses the `mbrola.dll`.

1.  Install eSpeak NG. Include the voice `mb-en1` in the list of voices during
    the eSpeak NG installation.

2.  Install the PC/Windows version of MBROLA (`MbrolaTools35.exe`) from:
    [http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe](http://www.tcts.fpms.ac.be/synthesis/mbrola/bin/pcwin/MbrolaTools35.exe).

3.  Download the `en1` or other voice from:
    [https://github.com/numediart/MBROLA-voices](https://github.com/numediart/MBROLA-voices)

4.  Save downloaded `en1` data file into `C:/Program Files/eSpeak/espeak-ng-data/mbrola`.

4.  Use the voice `espeak-MB-EN1` from the list of SAPI5 voices.

## Linux Installation

### Installation of standard packages

There are standard packages prepared for MBROLA binary and voices on different Linux distributions.
On Debian/Ubuntu-like Linux, you can install mbrola using `apt-get` package manager:

	sudo apt-get install mbrola mbrola-en1

where:

* `mbrola` is package containing MBROLA executable,
* `mbrola-en1` is mbrola data files for **en1** MBROLA voice.

You can check other available voices searching with command:

	apt-cache search mbrola

If some MBROLA voice is not available as Debian package, you can download data files
from [https://github.com/numediart/MBROLA-voices](https://github.com/numediart/MBROLA-voices)
and save them in `/usr/share/mbrola/xxN/xxN` file, where `xxN` is code and number of language file.

### Installation of MBROLA package from source

1. Install necessary packages:
```
sudo apt-get install git make gcc
```

2.  Clone MBROLA git repository:
```
git clone https://github.com/numediart/MBROLA.git
cd MBROLA
```

3. Compile MBROLA:
```
make
```

4. Install MBROLA:
```
sudo cp Bin/mbrola /usr/bin/mbrola
```

5.  Download the `en1` or other voice from
[https://github.com/numediart/MBROLA-voices](https://github.com/numediart/MBROLA-voices)

6.  Copy the `en1` data file to `/usr/share/mbrola/en1` folder.

## Usage

If you use the eSpeak NG voice such as `mb-en1` then eSpeak NG will use the mbrola "en1" voice, e.g.:

	espeak-ng -v mb-en1 "Hello world"

To generate mbrola phoneme data (.pho file) you can use:

	espeak-ng -v mb-en1 -q --pho "Hello world"

or

	espeak-ng -v mb-en1 -q --pho --phonout=out.pho "Hello world"

If you have issues with sound system integration, you can pass MBROLA's output to standard output,
which then can be played by ALSA (`aplay`) or PulseAudio (`paplay`) player throug the pipe:

        espeak-ng -vmb-en1 --stdout "Hello world"|aplay

## Adding new MBROLA voice entry to eSpeak NG

To add new MBROLA voice entry for eSpeak NG you have to:

1. add MBROLA voice definition file,
2. add MBROLA phoneme translation file,
3. update Makefile.am file and compile voice.

These steps are described in details in following sections.

### 1. Add MBROLA voice definition file

eSpeak NG's voice files for MBROLA voices are in `espeak-ng-data/voices/mb` folder.
Voice definition file is in form `mb-xxN` and have to contain at least this line:
`mbrola <voice> <translation>`, e.g.

	mbrola en1 en1_phtrans

Where:
* `en1` is the name of the MBROLA voice.
* `en1_phtrans` is a translation file to convert between eSpeak NG phonemes and
  the equivalent MBROLA phonemes.

If language code differs between MBROLA voice and eSpeak NG language,
then additional line is necessary:

        language xx

where `xx` is code of the language in eSpeak NG.

Binary `xxN_phtrans` files are kept in `espeak-ng-data/mbrola_ph` folder
and are generated from `phsource/mb/xxN` text files, during MBROLA
voice compilation.

Additionally MBROLA voice definition file can have other optional parameters,
similar to eSpeak NG voices, which are described [Voices](voices.md) file.

### 2. Add MBROLA phoneme translation file

MBROLA phoneme translation files specify translations from eSpeak NG
phoneme names to mbrola phoneme names.

The source phoneme translation files are in `.../phsource/mbrola` folder and
their name is in form `xxN` of referenced MBROLA voice.

_Note:_

* eSpeak NG phonemes are referenced from voice files in `phsource` folder of particular language e.g.
`ph_english` and/or general `phonemes` file.

* MBROLA phonemes are usualy listed in `README.txt` file of MBROLA voice.

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
they can spoken by the MBROLA voice.

For latest Linux verson of `mbrola` you can get list of diphones for particular
language by entering command:

      mbrola -d /usr/share/mbrola/en1/en1

where `/usr/share/mbrola/en1/en1` is location of the voice file.

When phoneme translation source file is compiled (look at next section)
`espeak-ng-data/mbrola_ph/xxN_phtrans` file is created.

### 3. Compile voice and update Makefile.am file

Separate MBROLA voice can be compiled using comand:

	espeak-ng --compile-mbrola=<xxN>

where `xxN` is MBROLA voice name.

`Makefile.am` is build configuration file which should be extended, to include automatic compilation
of newly added MBROLA voice for eSpeak NG.

Search for  `mbrola: \` line in `Makefile.am` and add additional line for newly created MBROLA voice, e.g.:

	mbrola: \
	...
	espeak-ng-data/mbrola_ph/xxN_phtrans \

Note that it could be that several voices share the same translation file. Then translation file
is named just `xx`.

When `Makefile.am` is update, when espek-ng will be compiled, e.g. by
entering command `automake; make -B` newly added MBROLA voice will be compiled automatically.

<a name="footnote1">1</a>: You can get list of configured entries for MBROLA synthesizer with following command
in espeak-ng project folder:
```
cd espeak-ng-data/voices/mb/; for i in $(ls); do printf "$i "; grep name $i|awk '{print $2}'; done
```
<a name="footnote2">2</a>: This voice has very limited set of diphones and is usable only for reading numbers.

