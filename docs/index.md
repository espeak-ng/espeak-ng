# eSpeak NG: Speech Synthesizer

- [Features](#features)
- [History](#history)
- [Adding a Language](add_language.md)
  - [Text to Phoneme Translation](dictionary.md)
- [Voice Files](voices.md)
  - [MBROLA Voices](mbrola.md)
  - [Phoneme Tables](phontab.md)
- [Intonation](intonation.md)
- [Markup Tags](markup.md)
- [License](../COPYING)

----------

eSpeak NG is a compact open source software speech synthesizer for English and
other languages, for Linux and Windows.

eSpeak NG uses a "formant synthesis" method. This allows many languages to be
provided in a small size. The speech is clear, and can be used at high speeds,
but is not as natural or smooth as larger synthesizers which are based on human
speech recordings.

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

The eSpeak speech synthesizer supports over 70 languages, however in many cases
these are initial drafts and need more work to improve them. Assistance from
native speakers is welcome for these, or other new languages. Please contact me
if you want to help.

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
existing codebase, adding new features and adding and improving to the
supported languages.
