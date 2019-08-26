# eSpeak NG Text-to-Speech

- [Features](#features)
- [Windows](#windows)
  - [Binaries](#binaries)
  - [Building](#building)
- [Linux, Mac, BSD](#linux-mac-bsd)
  - [Dependencies](#dependencies)
  - [Building](#building-1)
    - [Cross Compilation](#cross-compilation)
    - [Sanitizer Flag Configuration](#sanitizer-flag-configuration)
    - [LLVM Fuzzer Support](#llvm-fuzzer-support)
    - [eSpeak NG Feature Configuration](#espeak-ng-feature-configuration)
    - [Extended Dictionary Configuration](#extended-dictionary-configuration)
  - [Testing](#testing)
  - [Installing](#installing)
- [Android](#android)
  - [Dependencies](#dependencies-1)
  - [Building with Gradle](#building-with-gradle)
  - [Signing the APK](#signing-the-apk)
  - [Opening project in Android Studio](#opening-project-in-android-studio)
  - [Installing](#installing-1)
- [Documentation](#documentation)
- [eSpeak Compatibility](#espeak-compatibility)
- [History](#history)
- [Feedback](#feedback)
- [License Information](#license-information)

----------

The eSpeak NG is a compact open source software text-to-speech synthesizer for 
Linux, Windows, Android and other operating systems. It supports 
[107 languages and accents](docs/languages.md). It is based on the eSpeak engine
created by Jonathan Duddington.

eSpeak NG uses a "formant synthesis" method. This allows many languages to be
provided in a small size. The speech is clear, and can be used at high speeds,
but is not as natural or smooth as larger synthesizers which are based on human
speech recordings. It also supports Klatt formant synthesis, and the ability
to use [MBROLA](https://github.com/numediart/MBROLA) [voices](https://github.com/numediart/MBROLA-voices).

eSpeak NG is available as:

*  A command line program (Linux and Windows) to speak text from a file or
   from stdin.
*  A shared library version for use by other programs. (On Windows this is
   a DLL).
*  A SAPI5 version for Windows, so it can be used with screen-readers and
   other programs that support the Windows SAPI5 interface.
*  eSpeak NG has been ported to other platforms, including Solaris and Mac
   OSX.

## Features

*  Includes different Voices, whose characteristics can be altered.
*  Can produce speech output as a WAV file.
*  SSML (Speech Synthesis Markup Language) is supported (not complete),
   and also HTML.
*  Compact size.  The program and its data, including many languages,
   totals about 1.4 Mbytes.
*  Can be used as a front-end to [MBROLA diphone voices](mbrola.md).
   eSpeak NG converts text to phonemes with pitch and length information.
*  Can translate text into phoneme codes, so it could be adapted as a
   front end for another speech synthesis engine.
*  Potential for other languages. Several are included in varying stages
   of progress. Help from native speakers for these or other languages is
   welcome.
*  Written in C.

See the [CHANGELOG](CHANGELOG.md) for a description of the changes in the
various releases and with the eSpeak NG project.

The following platforms are supported:

| Platform    | Minimum Version | Status |
|-------------|-----------------|--------|
| Linux       |                 | ![[Travis continuous integration builds on Linux](https://travis-ci.org/espeak-ng/espeak-ng)](https://secure.travis-ci.org/espeak-ng/espeak-ng.svg?branch=master) |
| BSD         |                 |        |
| Android     | 4.0             |        |
| Windows     | Windows 8       |        |
| Mac         |                 |        |

## Documentation

1.  [User guide](src/espeak-ng.1.ronn) provides reference and examples for command-line options.
2.  [Language guide](docs/add_language.md) provides step-by-step instructions, how to add/improve support for language.
3.  [Index](docs/index.md) provides entries to more detailed information for contributors and developers.

## eSpeak Compatibility

The *espeak-ng* binaries use the same command-line options as *espeak*, with
several additions to provide new functionality from *espeak-ng* such as specifying
the output audio device name to use. The build creates symlinks of `espeak` to
`espeak-ng`, and `speak` to `speak-ng`.

The espeak `speak_lib.h` include file is located in `espeak-ng/speak_lib.h` with
an optional symlink in `espeak/speak_lib.h`. This file contains the espeak 1.48.15
API, with a change to the `ESPEAK_API` macro to fix building on Windows
and some minor changes to the documentation comments. This C API is API and ABI
compatible with espeak.

The `espeak-data` data has been moved to `espeak-ng-data` to avoid conflicts with
espeak. There have been various changes to the voice, dictionary and phoneme files
that make them incompatible with espeak.

The *espeak-ng* project does not include the *espeakedit* program. It has moved
the logic to build the dictionary, phoneme and intonation binary files into the
`libespeak-ng.so` file that is accessible from the `espeak-ng` command line and
C API.

## History

The program was originally known as __speak__ and originally written
for Acorn/RISC\_OS computers starting in 1995 by Jonathan Duddington. This was
enhanced and re-written in 2007 as __eSpeak__, including a relaxation of the
original memory and processing power constraints, and with support for additional
languages.

In 2010, Reece H. Dunn started maintaining a version of eSpeak on GitHub that
was designed to make it easier to build eSpeak on POSIX systems, porting the
build system to autotools in 2012. In late 2015, this project was officially
forked to a new __eSpeak NG__ project. The new eSpeak NG project is a significant
departure from the eSpeak project, with the intention of cleaning up the
existing codebase, adding new features, and adding to and improving the
supported languages.


The *historical* branch contains the available older releases of the original
eSpeak that are not contained in the subversion repository.

1.24.02 is the first version of eSpeak to appear in the subversion
repository, but releases from 1.05 to 1.24 are available at
[http://sourceforge.net/projects/espeak/files/espeak/](http://sourceforge.net/projects/espeak/files/espeak/).

These early releases have been checked into the historical branch,
with the 1.24.02 release as the last entry. This makes it possible
to use the replace functionality of git to see the earlier history:

	git replace 8d59235f 63c1c019

__NOTE:__ The source releases contain the `big_endian`, `espeak-edit`,
`praat-mod`, `riskos`, `windows_dll` and `windows_sapi` folders. These
do not appear in the source repository until later releases, so have
been excluded from the historical commits to align them better with
the 1.24.02 source commit.

## Feedback

Test latest development version at [eSpeakNG online](https://odo.lv/Espeak).

Report bugs to the [espeak-ng issues](https://github.com/espeak-ng/espeak-ng/issues)
page on GitHub.

Look at and subscribe to [eSpeakNG mailing list](https://groups.io/g/espeak-ng)
to view and discuss other related topics.

## License Information

eSpeak NG Text-to-Speech is released under the [GPL version 3](COPYING) or
later license.

The `ieee80.c` implementation is taken directly from
[ToFromIEEE.c.txt](http://www.realitypixels.com/turk/opensource/ToFromIEEE.c.txt)
which has been made available for use in Open Source applications per the
[license statement](COPYING.IEEE) on http://www.realitypixels.com/turk/opensource/.
The only modifications made to the code is to comment out the `TEST_FP` define
to make it useable in the eSpeak NG library, and to fix compiler warnings.

The `getopt.c` compatibility implementation for getopt support on Windows is
taken from the NetBSD `getopt_long` implementation, which is licensed under a
[2-clause BSD](COPYING.BSD2) license.

Android is a trademark of Google Inc.
