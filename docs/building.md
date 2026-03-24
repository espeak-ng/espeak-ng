# Building eSpeak NG

- [Windows](#windows)
  - [Dependencies](#dependencies)
  - [Building](#building)
- [Linux, Mac, BSD](#linux-mac-bsd)
  - [Dependencies](#dependencies-1)
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

## Windows

### Dependencies

To build eSpeak NG on Windows, you will need:

1. a copy of [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) or later, such as the Community Edition;
2. the Windows 8.1 SDK;
3. the [WiX installer](http://wixtoolset.org) plugin;
4. the [pcaudiolib](https://github.com/espeak-ng/pcaudiolib) project checked out to
   `src` (as `src/pcaudiolib`).

Optionally, you need:

1. sox (http://sox.sourceforge.net/) to enable audio output for SSML <audio> tag

__NOTE:__ SAPI 5 voices are not currently available in this release of eSpeak NG.
There is an [issue](https://github.com/espeak-ng/espeak-ng/issues/7) to track
support for this feature.

### Building

You can then open and build the `src/windows/espeak-ng.sln` solution in Visual
Studio.

You can also use the Visual Studio tools to build espeak-ng with `msbuild` from
the `src/windows` directory:

	msbuild espeak-ng.sln

To build with a specific version of Visual Studio, you need to use:

	msbuild /p:PlatformToolset=v120 espeak-ng.sln

replacing `v120` with the appropriate value for the target Visual Studio version:

| PlatformToolset | Visual Studio |
|-----------------|---------------|
| v120            | 2013          |
| v140            | 2015          |
| v141 (default)  | 2017          |

## Linux, Mac, BSD

### Dependencies

In order to build eSpeak NG, you need:

1.  [CMake](https://cmake.org/) 3.10 or later;
2.  a functional c compiler that supports C99 (e.g. gcc or clang). Note: if building with speechPlayer, a C++ compiler is required.

Optionally, you need:

1.  the [pcaudiolib](https://github.com/espeak-ng/pcaudiolib) development library
    to enable audio output;
2.  the speechPlayer development library to
    enable the speechPlayer Klatt implementation;
3.  the [sonic](https://github.com/espeak-ng/sonic) development library to
    enable sonic audio speed up support;
4.  the `ronn` man-page markdown processor to build the man pages.
5. sox (http://sox.sourceforge.net/) to enable audio output for SSML <audio> tag

To build the documentation, you need:

1.  the `kramdown` markdown processor.

On Debian-based systems such as Debian, Ubuntu and Mint, these dependencies can
be installed using the following commands:

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| cmake         | `sudo apt-get install cmake`                                     |
| c99 compiler  | `sudo apt-get install gcc g++`                                   |
| sonic         | `sudo apt-get install libsonic-dev`                              |
| ronn          | `sudo apt-get install ronn`                                      |
| kramdown      | `sudo apt-get install kramdown`                                  |

For recent Debian or Ubuntu >= 18.04 you should also install:

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| pcaudiolib    | `sudo apt-get install libpcaudio-dev`                            |

### Building

Configure the build using CMake:

	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=/usr

__NOTE:__ The `-DCMAKE_INSTALL_PREFIX` option above will install the files to the `/usr`
directory, instead of the default `/usr/local` location. For more information
on available options, you can run:

	cmake -Bbuild -LH

To use a different compiler, or compiler flags, you can specify these before
the `cmake` command. For example:

	CC=clang CFLAGS=-Wextra cmake -Bbuild -DCMAKE_INSTALL_PREFIX=/usr

The `espeak-ng` and `speak-ng` programs, along with the espeak-ng voices, can
then be built with:

	cmake --build build

The data (language dictionaries, phoneme tables, intonation) can be built with:

	cmake --build build --target data

Specific languages can be compiled by running the built binary directly:

	ESPEAK_DATA_PATH=`pwd`/build build/src/espeak-ng --compile=LANG

where `LANG` is the language code of the given language (e.g. `fr` for French).
More information can be found in the [Adding or Improving a Language](add_language.md)
documentation.

#### Cross Compilation

Because the eSpeak NG build process uses the built program to compile the
language and voice data, you need to build it natively first. Once you have
built it natively you can perform the cross compilation by pointing CMake
to the native build:

	cmake -Bbuild-cross -DCMAKE_TOOLCHAIN_FILE=... -DNativeBuild_DIR=build/src
	cmake --build build-cross

#### Sanitizer Flag Configuration

It is possible to build eSpeak NG with the gcc or clang sanitizer by passing
the appropriate flags to CMake. For example:

	CC=clang cmake -Bbuild \
		-DCMAKE_C_FLAGS="-fsanitize=address,undefined -g" \
		-DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
	cmake --build build
	ctest --test-dir build --output-on-failure

#### LLVM Fuzzer Support

To enable libFuzzer support you need clang 6.0 or later. It is enabled with
the following:

	CC=clang cmake -Bbuild -DWITH_FUZZER=ON
	cmake --build build
	ctest --test-dir build --output-on-failure

#### eSpeak NG Feature Configuration

The following CMake options control which eSpeak NG features are enabled:

| Option               | Description                                  | Default |
|----------------------|----------------------------------------------|---------|
| `-DUSE_KLATT`        | Enable Klatt formant synthesis.              | ON      |
| `-DUSE_SPEECHPLAYER` | Enable the speechPlayer Klatt implementation.| ON      |
| `-DUSE_MBROLA`       | Enable MBROLA voice support.                 | ON (if available) |
| `-DUSE_LIBSONIC`     | Use the sonic library to support higher WPM. | ON (if available) |
| `-DUSE_ASYNC`        | Enable asynchronous commands.                | ON (if pthread available) |

__NOTE:__ The `-DUSE_LIBSONIC` option requires that the sonic library and header
is accessible on the system.

#### Extended Dictionary Configuration

Extended dictionaries are automatically enabled when an `extra/<lang>_listx`
file is present in `dictsource/`. They can be toggled per-language with CMake
options:

| Option                  | Extended Dictionary | Default |
|-------------------------|---------------------|---------|
| `-DEXTRA_ru`            | Russian             | ON      |
| `-DEXTRA_cmn`           | Mandarin Chinese    | ON      |
| `-DEXTRA_yue`           | Cantonese           | ON      |

Set to `OFF` to exclude an extended dictionary (e.g. `-DEXTRA_ru=OFF`).

The extended dictionaries are taken from
[http://espeak.sourceforge.net/data/](http://espeak.sourceforge.net/data/) and
provide better coverage for those languages, while increasing the resulting
dictionary size.

### Testing

Before installing, you can test the built espeak-ng using the following command
from the top-level directory of this project:

    ESPEAK_DATA_PATH=`pwd`/build build/src/espeak-ng ...

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak-ng data from
the build tree. Otherwise, espeak-ng will look in `$(HOME)` or
`/usr/share/espeak-ng-data`.

If you are building with `-DBUILD_SHARED_LIBS=ON`, you also need to set
`LD_LIBRARY_PATH` to the shared library output directory:

    LD_LIBRARY_PATH=build/src/libespeak-ng:${LD_LIBRARY_PATH}

You can run the test suite with:

    ctest --test-dir build -j$(nproc) --output-on-failure

### Installing

You can install eSpeak NG by running the following command:

    cmake --install build

__NOTE:__ You may need to run with `sudo` depending on the install prefix.

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

1.  the [Android Studio](https://developer.android.com/studio/) with API 34 support;
2.  the [Android NDK](http://developer.android.com/tools/sdk/ndk/index.html);
3.  Gradle 8.13+
4.  JDK 17

### Building with Gradle

1.  Set the location of the Android SDK:

        $ export ANDROID_HOME=<path-to-the-android-sdk>
(where `<path-to-the-android-sdk>` is your actual path of SDK folder e.g. `/home/user/Android/Sdk`)

2. Build the project from the `android` directory:

        $ cd android
        $ ./gradlew assembleRelease

This will create an `android/build/outputs/apk/release/espeak-release-unsigned.apk` file.

### Signing the APK

In order to install the built APK you need to self-sign the package. You can do
this by:

1.  Creating a certificate, if you do not already have one:

        $ keytool -genkey -keystore [YOUR_CERTIFICATE] -alias [ALIAS] -keyalg RSA -storetype PKCS12
2. Align the apk using the zipalign tool.

        $ zipalign 4 android/build/outputs/apk/release/espeak-release-unsigned.apk \
          android/build/outputs/apk/release/espeak-release-zipalign.apk
3. Sign the package using your certificate:

        $ apksigner sign --ks [YOUR_CERTIFICATE] --ks-key-alias [ALIAS] \
          --out android/build/outputs/apk/release/espeak-release-signed.apk \
          android/build/outputs/apk/release/espeak-release-zipalign.apk


### Opening project in Android Studio

To open project in **Android Studio** select **Import project (Gradle, Eclipse ADT, etc)**
and  select `android` folder of the `espeak-ng` project.

Then select menu **File — Project Structure...**, tab **SDK Location**, field **Android NDK location**
and set your location of NDK, e.g. `/home/user/Android/Ndk`.

### Installing

Now, you can install the APK using the `adb` tool:

    $ adb install -r android/build/outputs/apk/release/espeak-release-signed.apk

After running, `eSpeakActivity` will extract the `espeakdata.zip` file into its
own data directory to set up the available voices.

To enable eSpeak, you need to:

1.  go into the Android `Text-to-Speech settings` UI;
2.  select `eSpeak TTS` as the default engine;
3.  use the `Listen to an example` option to check if everything is working.
