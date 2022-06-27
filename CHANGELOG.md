# Change Log

1.  [espeak-ng](#espeak-ng)
2.  [espeak](#espeak)

## espeak-ng

The espeak-ng project is a fork of the espeak project.

### 1.52 (In Development)

updated languages:
*  be (Belarusian) -- Andiv06
*  en (English) -- Bill Dengler
*  es (Spanish) -- Sukil Etxenike
*  fa (Persian) -- MH

### 1.51

*  Add support for speechPlayer.
*  Add more tests to check the various parts of espeak-ng.
*  Various changes to clean up the codebase (Juho Hiltunen)
*  Remove support for l_length_mods language option
*  Restructure "option brackets" language option to "brackets" and "bracketsAnnounced"
*  Restructure "stressLength" and "stressAdd" language option
*  New Language option: "lowercaseSentence" for ending a sentence if a period is followed by a lower case letter
*  Add voice variants
*  Rename zh to cmn (Mandarin)
*  Rename zhy to yue (Cantonese)
*  cmn (Mandarin) now assumes all latin characters all English text
   Use cmn-latn-pinyin for interpreting latin characters as pinyin
*  Update list of voice varants for Android application
*  Add Chromium extension (guest271314)
*  Replace `ieee80.c` by a free software implementation (Ulrich Müller)

bug fixes:
*  Fix reading malformed SSML (Christopher Brannon)
*  Fix memory allocation (Christopher Brannon)
*  Fix building and phoneme compilation (John Bowler)
*  Fix build scripts for Android (Peter Vágner, Minas Tirith Citizen)

documentation:
* Add documentation about voice and language options.
* Add documentation about dictionary flags.

updated languages:
*  ar (Arabic) -- king-dahmanus
*  ba (Bashkir) -- boracasli98, Valdis Vitolins, Juho Hiltunen
*  cmn (Mandarin) -- Silas S. Brown, Rongcui Dong, Icenowy Zheng, Juho Hiltunen
*  de (German) -- Karl Eick, Henry Krumb
*  el (Modern Greek) -- Reece Dunn (support for variant Greek letter forms)
*  en (English) -- Steven Presser, Ben Talagan
*  eo (Esperanto) -- AlsoScratch
*  eu (Basque) -- agonzalezd
*  fa (Farsi/Persian) -- Shadyar Khodayari
*  ga (Gaelic (Irish)) Chen, Chien-ting
*  grc (Ancient Greek) -- Reece Dunn (support for variant Greek letter forms)
*  hak (Hakka Chinese) -- Juho Hiltunen
*  haw (Hawaiian) -- Juho Hiltunen
*  ka (Georgian) -- Juho Hiltunen
*  kok (Konkani) -- Juho Hiltunen
*  nb (Norwegian Bokmål) -- Juho Hiltunen
*  nci (Classical Nahuatl) -- Juho Hiltunen
*  hy (Armenian) -- tigransimonyan
*  ia (Interlingua) -- nesrad
*  it (Italian) -- Christian Leo
*  ja (Japanese) -- fukuen, Juho Hiltunen
*  jbo (Lojban) -- Juho Hiltunen, xunsku
*  lv (Latvian) -- Valdis Vitolins
*  mi (Māori) -- boracasli98
*  py (Pyash) -- Andrii Logan Zvorygin
*  shn (Shan Tay Yai) -- ronaldaug
*  tr (Turkish) -- boracasli98, SeanTolstoyevski
*  ur (Urdu) -- Ejaz Shah
*  uz (Uzbek) -- boracasli98, Valdis Vitolins, Andiv06
*  yue (Cantonese) -- Andiv06

new languages:
*  be (Belarusian) -- Sergei B
*  chr (Cherokee) -- Michael Conrad
*  cv (Chuvash) -- boracasli98, Valdis Vitolins
*  en-us-nyc (English, New York City) -- Richard Calvi
*  haw (Hawaiian) -- Valdis Vitolins
*  he (Hebrew) -- boracasli98, Valdis Vitolins
*  io (Ido) -- José Miguel López, Valdis Vitolins
*  lb (Luxembourgish) -- Marco Barnig, Valdis Vitolins
*  ltg (Latgalian) -- Valdis Vitolins
*  nog (Nogai) -- boracasli98, Valdis Vitolins
*  piqd (Klingon) -- Valdis Vitolins
*  qu (Quechua) -- Valdis Vitolins
*  qdb (Lang Belta) -- Da Def, Valdis Vitolins
*  qya (Quenya) -- Lucas Werkmeister
*  sjn (Sindarin) -- Lucas Werkmeister
*  smj (Lule Saami) -- Katri Hiovain, Sebastien Le Maguer
*  th (Thai) -- Valdis Vitolins
*  tk (Turkmen) -- boracasli98, Valdis Vitolins
*  ug (Uyghur) -- boracasli98, Valdis Vitolins
*  uk (Ukrainian) -- Valdis Vitolins

### 1.50 - 2019-11-29

*  Add a `--disable-rpath` option to prevent libtool hardcoding rpaths in the executable.
*  Renamed the `hy-arevmda` language to `hyw`, following the 2018-03-30 change to the
   BCP 47 language subtag registry making the newly registered `hyw` language code the
   preferred value for `hy-arevmda`. This change keeps support for detecting the
   `hy-arevela` and `hy-arevmda` language tags.
*  Support any length replacement rule strings for the source part of the rule (replacing
   from the 'source' string to the 'target' string).
*  Add more tests to check the various parts of espeak-ng.
*  Various changes to clean up the codebase.
*  Fix various compiler warnings (`-Winitialized`, `-Wmissing-prototypes`, `-Wreturn-type`,
   and `-Wunused`).

security:

*  Fix several crashes in the emoji support.
*  Fix several static analysis issues detected by Coverity Scan.
*  Fix several static analysis issues detected by Microsoft Visual C++ `/analyze`.
*  `oss-fuzz` support for the SSML logic.
*  Fix running `make check` with `-fsanitize=address` (LLVM AddressSanitizer).

documentation:

*  document the Kirshenbaum phoneme transcription scheme (used as the basis of espeak phonemes).
*  document the X-SAMPA phoneme transcription scheme.
*  document the Conlang X-SAMPA phoneme transcription scheme.

emoji:

*  Update the emoji to Unicode 11.0.
*  Update the emoji translations to CLDR 33.1.
*  gd (Scottish Gaelic)
*  sd (Sindhi)
*  yue (Chinese (Cantonese))

new languages:

*  ba (Bashkir) -- tc-dl, Valdis Vitolins
*  hak (Hakka Chinese) -- Chen Chien-ting
*  ht (Haitian Creole) -- Valdis Vitolins
*  kk (Kazakh) -- boracasli14, Valdis Vitolins, Andiv06
*  py (Pyash) -- Logan Streondj
*  quc (K'iche') -- Valdis Vitolins
*  ru-lv (Russian Latvia) -- Valdis Vitolins
*  shn (Shan Tay Yai) -- ronaldaug
*  uz (Uzbek) -- Valdis Vitolins

updated languages:

*  af (Afrikaans) -- Christo de Klerk
*  ar (Arabic) -- Valdis Vitolins, Mark Readdie, Hesham Assabahi
*  cmn (Chinese Mandarin) -- materia-cccp
*  de (German) -- Reece Dunn, Karl Eick
*  en (English) -- Reece Dunn
*  fa (Farsi/Persian) -- Shadyar Khodayari
*  fi (Finnish) -- Juho Hiltunen
*  fr (French) -- Shallowmallow, Karl Eick, maiido, Reece Dunn
*  ku (Kurdish) -- rojanu
*  it (Italian) -- chrislm
*  lv (Latvian) -- Valdis Vitolins
*  my (Myanmar/Burmese) -- Min Maung
*  ru (Russian) -- Valdis Vitolins, Reece Dunn
*  sd (Sindhi) -- Ejaz Shah
*  ur (Urdu) -- Ejaz Shah
*  vi (Vietnamese) -- Anh Hoang Nguyen

### 1.49.2 - 2017-09-24

*  Provide more validation when reading phoneme tables and voice/language files.
*  Removed support for phoneme equivalence tables. These were disabled in the
   French and German language files.
*  Allow pre- as well as post-jump rules in dictionary files.
*  Support building the Windows version with Visual Studio 2013.
*  Use language and accent names consistently across the language files.
*  Group the encoding handling into a single place with a clean interface and tests.
*  Support all ISO 8859 encodings.
*  Support for multi-word text replacements in language dictionaries.
*  Support compiling `_emoji` dictionary files if present.

bug fixes:

*  Fix running `make clean ; make`.
*  Fix reading stdin buffers larger than 1000.
*  Fixed various language and parent BCP 47 codes (`jp` is now `ja`).
*  Fixed several crashes and bugs in `espeak_SetVoiceByName/Properties`.

new languages:

*  bpy (Bishnupriya Manipuri) -- Vardhan
*  fr-CH (French (Switzerland)) -- Claude Beazley
*  kok (Konkani) -- Vardhan
*  mi (Māori) -- Graham Oliver
*  sd (Sindhi, Arabic script) -- Vardhan, Ejaz Shah

updated languages:

*  af (Afrikaans) -- Christo de Klerk
*  de (German) -- Valdis Vitolins
*  du (Dutch) -- Leonard de Ruijter
*  en (English) -- Reece Dunn. Thanks to Kendell Clark for identifying mispronunciations.
*  fa (Farsi/Persian) -- Shadyar Khodayari
*  fr (French) -- Valdis Vitolins; Reece Dunn
*  gu (Gujarati) -- Vardhan
*  hi (Hindi) -- Vardhan
*  it (Italian) -- chrislm
*  ky (Kyrgyz) -- JRMeyer
*  lv (Latvian) -- Valdis Vitolins
*  mk (Macedonian) -- Reece Dunn. Updated the romanization support:
   *  Don't map đ and ć to Serbian ђ and ћ (use Macedonian ѓ and ќ instead).
   *  Support additional romanizations: ISO 9, BGN/PCGN, Cadastre, and MJMS/SSO.
*  mr (Marathi) -- Vardhan
*  or (Oriya) -- Vardhan
*  ur (Urdu) -- Ejaz Shah

Unicode Emoji support (from the Unicode Emoji 5.0, and CLDR 31.0.1 data files):

*  af (Afrikaans)
*  am (Amharic)
*  ar (Arabic)
*  az (Azerbaijani)
*  bg (Bulgarian)
*  bn (Bengali)
*  bs (Bosnian)
*  ca (Catalan)
*  cs (Czech)
*  cy (Welsh)
*  da (Danish)
*  de (German)
*  el (Greek)
*  en (English) -- American
*  es (Spanish)
*  et (Estonian)
*  eu (Basque)
*  fa (Farsi/Persian)
*  fi (Finnish)
*  fr (French)
*  ga (Irish Gaelic)
*  gu (Gujarati)
*  hi (Hindi)
*  hr (Croatian)
*  hu (Hungarian)
*  hy (Armenian)
*  id (Indonesian)
*  is (Icelandic)
*  it (Italian)
*  ja (Japanese)
*  ka (Georgian)
*  kn (Kannada)
*  ko (Korean)
*  ky (Kyrgyz)
*  lt (Lithuanian)
*  lv (Latvian)
*  mk (Macedonian)
*  ml (Malayalam)
*  mr (Marathi)
*  ms (Malay)
*  my (Myanmar/Burmese)
*  ne (Nepali)
*  nl (Dutch)
*  or (Oriya)
*  pa (Punjabi)
*  pl (Polish)
*  pt (Portuguese) -- Brazil
*  ro (Romanian)
*  ru (Russian)
*  si (Sinhala)
*  sk (Slovak)
*  sl (Slovenian)
*  sq (Albanian)
*  sr (Serbian)
*  sv (Swedish)
*  sw (Swahili)
*  ta (Tamil)
*  te (Telugu)
*  tr (Turkish)
*  ur (Urdu)
*  vi (Vietnamese)
*  zh (Chinese)

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
*  ky (Kyrgyz) -- JRMeyer
*  lv (Latvian) -- Valdis Vitolins
*  tr (Turkish) -- Valdis Vitolins

new languages:

*  ar (Arabic) -- Taha Zerrouki
*  jp (Japanese) -- Reece Dunn (NOTE: Hiragana and Katakana only.)

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
