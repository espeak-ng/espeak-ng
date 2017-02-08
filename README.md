# eSpeak NG Text-to-Speech

- [Windows](#windows)
  - [Binaries](#binaries)
  - [Building](#building)
- [Linux, Mac, BSD](#linux-mac-bsd)
  - [Dependencies](#dependencies)
  - [Building](#building-1)
    - [Cross Compilation](#cross-compilation)
    - [Audio Output Configuration](#audio-output-configuration)
    - [eSpeak NG Feature Configuration](#espeak-ng-feature-configuration)
    - [Extended Dictionary Configuration](#extended-dictionary-configuration)
  - [Testing](#testing)
  - [Installing](#installing)
- [Android](#android)
  - [Dependencies](#dependencies-1)
  - [Building with Gradle](#building-with-gradle)
  - [Signing the APK](#signing-the-apk)
  - [Installing](#installing-1)
- [Documentation](#documentation)
- [eSpeak Compatibility](#espeak-compatibility)
- [Historical Versions](#historical-versions)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The eSpeak NG (Next Generation) Text-to-Speech program is an open source speech
synthesizer that supports [99 languages and accents](docs/languages.md).
It is based on the eSpeak engine created by Jonathan Duddington. It uses
spectral formant synthesis by default which sounds robotic, but can be
configured to use Klatt formant synthesis or MBROLA to give it a more natural
sound.

See the [CHANGELOG](CHANGELOG.md) for a description of the changes in the
various releases and with the eSpeak project.

The following platforms are supported:

| Platform    | Minimum Version | Status |
|-------------|-----------------|--------|
| Linux       |                 | ![[Travis continuous integration builds on Linux](https://travis-ci.org/espeak-ng/espeak-ng)](https://secure.travis-ci.org/espeak-ng/espeak-ng.svg?branch=master) |
| BSD         |                 |        |
| Android     | 4.0             |        |
| Windows     | Windows 8       |        |
| Mac         |                 |        |

## Windows

### Binaries

The Windows version of eSpeak NG 1.49.1 is available as:

*  [espeak-ng-x64.msi](https://github.com/espeak-ng/espeak-ng/releases/download/1.49.1/espeak-ng-x64.msi) --
   64-bit Windows installer
*  [espeak-ng-x86.msi](https://github.com/espeak-ng/espeak-ng/releases/download/1.49.1/espeak-ng-x86.msi) --
   32-bit Windows installer

You also need to install the [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145).

__NOTE:__ SAPI 5 voices are not currently available in this release of eSpeak NG.
There is an [issue](https://github.com/espeak-ng/espeak-ng/issues/7) to track
support for this feature.

### Building

To build eSpeak NG on Windows, you will need:

1. a copy of Visual Studio 2013 or later, such as the Community Edition;
2. the Windows 8.1 SDK;
3. the [WiX installer](http://wixtoolset.org) plugin;
4. the [pcaudiolib](http://github.com/rhdunn/pcaudiolib) project checked out to
   `src` (as `src/pcaudiolib`).

You can then open and build the `src/windows/espeak-ng.sln` solution in Visual
Studio.

You can also use the Visual Studio tools to build espeak-ng with `msbuild` from
the `src/windows` directory:

	msbuild espeak-ng.sln

If building with Visual Studio 2003, you need to use:

	msbuild /p:PlatformToolset=v120 espeak-ng.sln

## Linux, Mac, BSD

### Dependencies

In order to build eSpeak NG, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c compiler that supports C99 (e.g. gcc or clang).

Optionally, you need:

1.  the [pcaudiolib](https://github.com/rhdunn/pcaudiolib) development library
    to enable audio output;
3.  the [sonic](https://github.com/waywardgeek/sonic) development library to
    enable sonic audio speed up support;
4.  the `ronn` man-page markdown processor to build the man pages.

To build the documentation, you need:

1.  the `kramdown` markdown processor.

On Debian-based systems such as Debian, Ubuntu and Mint, these dependencies can
be installed using the following commands:

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| autotools     | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c99 compiler  | `sudo apt-get install gcc`                                       |
| sonic         | `sudo apt-get install libsonic-dev`                              |
| ronn          | `sudo apt-get install ruby-ronn`                                 |
| kramdown      | `sudo apt-get install ruby-kramdown`                             |

### Building

The first time you build eSpeak NG, or when you want to change how to build
eSpeak NG, you need to run the following standard autotools commands:

	./autogen.sh
	./configure --prefix=/usr

__NOTE:__ The `--prefix` option above will install the files to the `/usr`
directory, instead of the default `/usr/local` location. You can use other
standard `configure` options to control the output. For more information,
you can run:

	./configure --help

To use a different compiler, or compiler flags, you can specify these before
the `configure` command. For example:

	CC=clang-3.5 CFLAGS=-Wextra ./configure --prefix=/usr

The `espeak-ng` and `speak-ng` programs, along with the espeak-ng voices, can
then be built with:

	make

__NOTE:__ Building the voice data does not work when using the `-jN` option.
If you want to use that option, you can run:

	make -j8 src/espeak-ng src/speak-ng
	make

The documentation can be built by running:

	make docs

Specific languages can be compiled by running:

	make LANG

where `LANG` is the language code of the given language. More information can
be found in the [Adding or Improving a Language](docs/add_language.md)
documentation.

#### Cross Compilation

Because the eSpeak NG build process uses the built program to compile the
language and voice data, you need to build it locally first. Once you have
built it locally you can perform the cross compilation using:

	./configure --build=... --host=... --target=...
	make -B src/espeak-ng src/speak-ng

#### Audio Output Configuration

The following `configure` options control which audio interfaces to use:

| Option              | Audio Interfaces | Default |
|---------------------|------------------|---------|
| `--with-pulseaudio` | PulseAudio       | yes     |
| `--with-portaudio`  | PortAudio        | yes     |
| `--with-sada`       | SADA (Solaris)   | no      |

If pulseaudio and portaudio are both enabled and available, eSpeak NG will choose
which one to use at runtime, trying pulseaudio first before falling back to
portaudio.

#### eSpeak NG Feature Configuration

The following `configure` options control which eSpeak NG features are enabled:

| Option          | Description                                  | Default |
|-----------------|----------------------------------------------|---------|
| `--with-klatt`  | Enable Klatt formant synthesis.              | yes     |
| `--with-mbrola` | Enable MBROLA voice support.                 | yes     |
| `--with-sonic`  | Use the sonic library to support higher WPM. | yes     |
| `--with-async`  | Enable asynchronous commands.                | yes     |

__NOTE:__ The `--with-sonic` option requires that the sonic library and header
is accessible on the system.

#### Extended Dictionary Configuration

The following `configure` options control which of the extended dictionary files
to build:

| Option               | Extended Dictionary | Default |
|----------------------|---------------------|---------|
| `--with-extdict-ru`  | Russian             | no      |
| `--with-extdict-zh`  | Mandarin Chinese    | no      |
| `--with-extdict-zhy` | Cantonese           | no      |

The extended dictionaries are taken from
[http://espeak.sourceforge.net/data/](http://espeak.sourceforge.net/data/) and
provide better coverage for those languages, while increasing the resulting
dictionary size.

### Testing

Before installing, you can test the built espeak-ng using the following command
from the top-level directory of this project:

    ESPEAK_DATA_PATH=`pwd` LD_LIBRARY_PATH=src:${LD_LIBRARY_PATH} src/espeak-ng ...

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak-ng data from
the source tree. Otherwise, espeak-ng will look in `$(HOME)` or
`/usr/share/espeak-ng-data`.

The `LD_LIBRARY_PATH` is set as `espeak` uses the `libespeak-ng.so` shared
library. This ensures that `espeak` uses the built shared library in the
`src` directory and not the one on the system (which could be an older
version).

### Installing

You can install eSpeak NG by running the following command:

    sudo make LIBDIR=/usr/lib/x86_64-linux-gnu install

__NOTE:__ The `LIBDIR` path may be different to the one on your system (the
above is for 64-bit Debian/Ubuntu releases that use the multi-arch package
structure -- that is, Debian Wheezy or later).

You can find out where espeak-ng is installed to on your system if you
already have an espeak-ng install by running:

    find /usr/lib | grep libespeak-ng

## Android

<div align="right"><a href="https://play.google.com/store/apps/details?id=com.reecedunn.espeak" title="eSpeak for Android on Google Play"><img src="https://developer.android.com/images/brand/en_app_rgb_wo_45.png"/></a></div>

The espeak-ng sources contain the code for the Android™ port of the application.
This is published as the [eSpeak for Android](http://reecedunn.co.uk/espeak-for-android)
program on the Google Play store. It is based on the eyes-free port of eSpeak
to the Android platform. This code was originally maintained in a separate
branch when the repository tracked eSpeak releases.

### Dependencies

In order to build the Android APK file, you need:

1.  the [Android SDK](http://developer.android.com/sdk/index.html) with API 21 support;
2.  the [Android NDK](http://developer.android.com/tools/sdk/ndk/index.html);
3.  Gradle 2.1 or later.

In order to use Android Studio, you will also need:

1.  [Android Studio](http://developer.android.com/sdk/installing/studio.html).

### Building with Gradle

1.  Set the location of the Android SDK:

        $ export ANDROID_HOME=<path-to-the-android-sdk>
2.  Build the project:

        $ ./autogen.sh
        $ ./configure --with-gradle=<path-to-gradle>
        $ make apk-release

This will create an `android/build/outputs/apk/espeak-release-unsigned.apk` file.

### Signing the APK

In order to install the built APK you need to self-sign the package. You can do
this by:

1.  Creating a certificate, if you do not already have one:

        $ keytool -genkey -keystore [YOUR_CERTIFICATE] -alias [ALIAS]
2. Sign the package using your certificate:

        $ jarsigner -sigalg MD5withRSA -digestalg SHA1 \
          -keystore [YOUR_CERTIFICATE] \
          android/build/outputs/apk/espeak-release-unsigned.apk [ALIAS]
3. Align the apk using the zipalign tool.

        $ zipalign 4 android/build/outputs/apk/espeak-release-unsigned.apk \
          android/build/outputs/apk/espeak-release-signed.apk

### Installing

Now, you can install the APK using the `adb` tool:

    $ adb install -r android/build/outputs/apk/espeak-release-signed.apk

After running, `eSpeakActivity` will extract the `espeakdata.zip` file into its
own data directory to set up the available voices.

To enable eSpeak, you need to:

1.  go into the Android `Text-to-Speech settings` UI;
2.  enable `eSpeak TTS` in the `Engines` section;
3.  select `eSpeak TTS` as the default engine;
4.  use the `Listen to an example` option to check if everything is working.

## Documentation

The [main documentation](docs/index.md) for eSpeak NG provides more information
on using and creating voices/languages for eSpeak NG.

The [espeak-ng](src/espeak-ng.1.ronn) and [speak-ng](src/speak-ng.1.ronn)
command-line documentation provide a reference of the different command-line
options available to these commands with example usage.

## eSpeak Compatibility

The *espeak-ng* binaries expose the same command-line options as *espeak*, with
several additions to expose new functionality from *espeak-ng* such as specifying
the output audio device name to use. The build creates symlinks of `espeak` to
`espeak-ng`, and `speak` to `speak-ng`.

The espeak `speak_lib.h` include file is provided in `espeak-ng/speak_lib.h` with
an optional symlink in `espeak/speak_lib.h`. This file contains the espeak API as
of 1.48.15, with a change to the `ESPEAK_API` macro to fix building on Windows,
and some minor changes to the documentation comments. Thus, the C API is API and
ABI compatible with espeak.

The `espeak-data` data has been moved to `espeak-ng-data` to avoid conflicts with
espeak. There have been various changes to the voice, dictionary and phoneme files
that make them incompatible with espeak.

The *espeak-ng* project does not include the *espeakedit* program. It has moved
the logic to build the dictionary, phoneme and intonation binary files into the
`libespeak-ng.so` file that is accessible from the `espeak-ng` command line and
C API.

## Historical Versions

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

## Bugs

Report bugs to the [espeak-ng issues](https://github.com/espeak-ng/espeak-ng/issues)
page on GitHub.

## License Information

eSpeak NG Text-to-Speech is released under the [GPL version 3](COPYING) or
later license.

The `ieee80.c` implementation is taken directly from
[ToFromIEEE.c.txt](http://www.realitypixels.com/turk/opensource/ToFromIEEE.c.txt)
which has been made available for use in Open Source applications per the
[license statement](COPYING.IEEE) on http://www.realitypixels.com/turk/opensource/.
The only modification made to the code is to comment out the `TEST_FP` define
to make it useable in the eSpeak NG library.

The `getopt.c` compatibility implementation for getopt support on Windows is
taken from the NetBSD `getopt_long` implementation, which is licensed under a
[2-clause BSD](COPYING.BSD2) license.

Android is a trademark of Google Inc.
