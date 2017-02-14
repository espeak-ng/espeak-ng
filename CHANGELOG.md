# Change Log

1.  [espeak-ng](#espeak-ng)
2.  [espeak](#espeak)

## espeak-ng

The espeak-ng project is a fork of the espeak project.

### 1.49.2 - (In Development)

*  Support describing all IPA phonemes in the phoneme tables.
*  Provide more validation when reading phoneme tables and voice/language files.
*  Removed support for phoneme equivalence tables. These were disabled in the
   French and German language files.
*  Allow pre- as well as post-jump rules in dictionary files.
*  Support building the Windows version with Visual Studio 2013.
*  Use language and accent names consistently across the language files.

bug fixes:

*  Fix running `make clean ; make`.
*  Fix reading stdin buffers larger than 1000.

new languages:

*  kok (Konkani) -- Vardhan
*  mni (Manipuri) -- Vardhan
*  sd (Sindhi, Arabic script) -- Vardhan

updated languages:

*  af (Afrikaans) -- Christo de Klerk
*  du (Dutch) -- Leonard de Ruijter
*  fr (French) -- Valdis Vitolins
*  gu (Gujarati) -- Vardhan
*  hi (Hindi) -- Vardhan
*  it (Italian) -- chrislm
*  ky (Krygyz) -- JRMeyer
*  mr (Marathi) -- Vardhan
*  or (Oriya) -- Vardhan
*  ur (Urdu) -- Ejaz Shah

### 1.49.1 - 2017-01-21

*  Vim syntax support for rule files.
*  Replace `ieee80.c` with the implementation at
   http://www.realitypixels.com/turk/opensource/ToFromIEEE.c.txt for Debian
   open source license compliance.
*  Documentation updates.
*  Emscripten support.
*  Merged the Android port into the main espeak-ng codebase.
*  Extend `--compile-phoneme` to support specifying the source directory for
   phoneme files.
*  Support using any directory as the data home, not just `espeak-ng-data`.
*  Don't install the `default` voice: use `en` as the default voice.

__NOTE:__ The merger of the Android code is currently experimental. It is
missing support for recognising Unicode characters.

bug fixes:

*  Fix `.Lnn` rule groups to allow groups above 62.
*  Fix reporting the eSpeak NG version in the `--version` string and Windows
   installer.
*  Fix a crash when calling `LoadDictionary` when using clang.
*  Threading fixes and Mac OSX portability for the fifo and event code.
*  Fixes for running the spect code on big-endian architectures.
*  Fix determining the voice directory when installing the 32-bit Windows
   binaries on a 64-bit Windows system.
*  Fix a regression with the saved parameter logic.
*  Reduce the default buffer length to 60mS to improve latency.
*  Don't override buffer length when using espeak-ng for audio.
*  Fix detecting doubled consonants when using Unicode characters.
*  Fix speaking `1,,2`, etc. in languages that use `,` as a decimal separator.

updated languages:

*  af (Afrikaans) -- Christo de Klerk
*  en (English) -- Valdis Vitolins
*  fa (Farsi/Persian) -- Shadyar Khodayari
*  it (Italian) -- chrislm
*  ky (Krygyz) -- JRMeyer
*  lv (Latvian) -- Valdis Vitolins
*  tr (Turkish) -- Valdis Vitolins

new languages:

*  ar (Arabic) -- Taha Zerrouki
*  jp (Japanese) -- Reece Dunn (NOTE: Hiragana and Karakana only.)

### 1.49.0 - 2016-09-10

*  Support the `--compile-mbrola` command-line option.
*  Support the `--compile-phonemes` command-line option.
*  Support the `--compile-intonations` command-line option.
*  Support SSML &lt;phoneme alphabet="espeak" ph="..."&gt; tags.
*  Added man files for the `speak-ng` and `espeak-ng` command-line programs.
*  Created a companion espeak-ng API to provide more detailed error codes and
   provide access to the new espeak-ng functionality.
*  Fixed many logic and security issues reported by clang scan-build, Coverity
   and msvc /analyze.
*  Group languages by their language family and use BCP47 compliant names.
*  Support for Windows and BSD platforms.
*  Removed support for WinCE, MS-DOS and RiscOS.
*  Add support for `maintainer` and `status` field in voice files for tracking
   voice maintenance.
*  Vim syntax highlighting for espeak dictionary (list and rules) files.
*  Support reading input from named pipes.
*  Fix wav file truncation when reading multiline text from stdin or a named pipe.

build:

*  Build the code with a C99 compiler, instead of a C++ compiler.
*  Provide a pkg-config file (patch by Luke Yelavich).
*  Use -fPIC to support sparc/sparc64 architectures.
*  Removed the local portaudio header files.
*  Use the system's sonic library and header files.
*  Output phoneme compilation errors to stderr.
*  Generate build failures if building phoneme, intonation or dictionary files
   contain errors.
*  Provide modern Visual Studio project files to build eSpeak NG on Windows,
   with a WiX-based project to create an MSI installer.
*  Use the NetBSD `getopt_long` implementation on Windows.

restructuring:

*  Moved the library code to `src/libespeak-ng`.
*  Renamed `espeak` to `espeak-ng`.
*  Renamed `speak` to `speak-ng`.
*  Use the `libespeak-ng` API in `speak-ng` using a shared implementation with
   `espeak-ng`.
*  Moved the code to build the mbrola voice data, phoneme tables and intonation
   data to libespeak-ng.
*  Removed the `espeakedit` program and the associated wxWidgets dependency.
*  Removed the platforms directory and approaching portability in a similar way
   to how libressl handles portability.
*  Converted the documentation to markdown.
*  Group the Windows and POSIX mbrowrap code to provide the `mbrowrap.h`
   implementation in a single place.
*  Replaced the audio APIs with PCAudioLib to improve portability of the audio
   and to share that across different projects.
*  Reworked the synchronous audio to share the code paths with asynchronous
   audio.

cleanup:

*  Removed unused/empty internal header files.
*  Removed unused and commented out code.
*  Reformatted the code to use a consistent style and indentation.
*  Fixed many GCC and clang warnings.
*  Improved the error handling within the codebase to report the underlying
   error where possible.
*  Inlined several wrapper methods that were adding little/no value.

updated languages:

*  en (English) -- Thanks to Kendell Clark for identifying mispronunciations.
*  el (Greek) : improved polytonic Greek support
*  es (Spanish) : ChrisLeo (improved intonations)
*  fa (Persian) -- Shadyar Khodayari
*  fr (French) -- Thomas Guillory
*  ga (Irish Gaelic) -- Jim Regan
*  it (Italian) -- ChrisLeo
*  lv (Latvian) -- Valdis Vitolins

new languages:

*  gn (Guarani) -- ChrisLeo
*  ky (Kyrgyz) -- JRMeyer
*  mb-br2 (Brazillian Portuguese)
*  mb-de\* (German) : extend support coverage of the German MBROLA voices
*  mb-lt1 (Lithuanian) -- embar
*  mb-lt2 (Lithuanian) -- embar
*  mt (Maltese)
*  my (Myanmar/Burmese) -- Min Maung, Lwin Moe
*  tn (Setswana)
*  tt (Tatar)

## espeak

The espeak project was developed by Jonathan Duddington.

This history log is for a fork of the espeak project maintained by Reece H.
Dunn. This project was designed to:

1.  Make it easier to build espeak on POSIX systems.
2.  Keep track of the source code releases that differed from the releases
    in the subversion repository.

### 1.48.11 - 2014-08-31

*  Support building the MBROLA voice files.
*  mbrola/de6 support for syllabic m and syllabic n.

### 1.47.14 - 2013-12-03

*  Support building with the extended Chinese and Russian dictionary data.

### 1.47.13 - 2013-10-22

updated languages:

*  om (Oromo)

### 1.47.12 - 2013-10-12

*  Added the NVDA voice variants.
*  Do not crash if `espeak_SetPunctuationList` is called with a NULL
   punctuation list.
*  Fix a segfault in `GetTranslatedPhonemeString`.

new languages:

*  om (Oromo)

build:

*  Support the `--with-async` configure option.
*  Support the `--with-sonic` configure option.
*  Support the `--with-mbrola` configure option.
*  Support the `--with-klatt` configure option.
*  Support the `--with-sada` configure option.
*  More build improvements.

### 1.46.23 - 2012-09-11

*  Converted the build to use autotools.

### 1.46.11 - 2011-12-31

*  Support building all the voice dictionaries.
*  More build improvements.

### 1.43.46 - 2010-06-28

*  Initial build changes to make it easier to build espeak on POSIX systems.
