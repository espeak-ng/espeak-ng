# Change Log

## (In Development)

*  Support the `--compile-mbrola` command-line option.
*  Support the `--compile-phonemes` command-line option.
*  Support the `--compile-intonations` command-line option.
*  Support SSML &lt;phoneme alphabet="espeak" ph="..."&gt; tags.
*  Use -fPIC to support sparc/sparc64 architectures.
*  Use the system's portaudio header files.
*  Use the system's sonic library and header files.
*  Added man files for the `speak-ng` and `espeak-ng` command-line programs.
*  Output phoneme compilation errors to stderr.
*  Generate build failures if building phoneme, intonation or dictionary files
   contain errors.

restructuring:

*  Build the code with a C11 compiler, instead of a C++ compiler.
*  Moved the library code to `src/libespeak-ng`.
*  Renamed `espeak` to `espeak-ng`.
*  Renamed `speak` to `speak-ng`.
*  Moved the code to build the mbrola voice data, phoneme tables and intonation
   data to libespeak-ng.
*  Removed the `espeakedit` program and the associated wxWidgets dependency.
*  Removed the platforms directory.
*  Converted the documentation to markdown.

cleanup:

*  Removed unused/empty internal header files.
*  Removed unused and commented out code.
*  Reformatted the code to use a consistent style and indentation.
*  Fixed many GCC and clang warnings.

updated languages:

*  fr (French) -- Thomas Guillory
*  ga (Irish Gaelic) -- Jim Regan
*  it (Italian) -- ChrisLeo
*  lv (Latvian) -- Valdis Vitolins

new languages:

*  mb-br2 (Brazillian Portuguese)
*  mb-lt1 (Lithuanian) -- embar
*  mb-lt2 (Lithuanian) -- embar
*  mt (Maltese)
*  tn (Setswana)
*  tt (Tatar)

## Historic

These are a condensed set of releases that were maintained in step with the
upstream eSpeak releases. The releases containing minor build fixes, or only
incorporating upstream changes have been ommitted.

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
