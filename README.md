# eSpeak NG Text-to-Speech

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Audio Output Configuration](#audio-output-configuration)
  - [eSpeak NG Feature Configuration](#espeak-ng-feature-configuration)
  - [Extended Dictionary Configuration](#extended-dictionary-configuration)
  - [Cross-Compiling For Windows](#cross-compiling-for-windows)
- [Testing](#testing)
- [Installing](#installing)
- [Documentation](#documentation)
- [Praat Changes](#praat-changes)
- [eSpeak for Android](#espeak-for-android)
- [Historical Versions](#historical-versions)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The eSpeak NG (Next Generation) Text-to-Speech program is an open source speech
synthesizer that supports over 70 languages. It is based on the eSpeak engine
created by Jonathan Duddington. It uses spectral formant synthesis by default
which sounds robotic, but can be configured to use Klatt formant synthesis or
MBROLA to give it a more natural sound.

## Build Dependencies

In order to build eSpeak NG, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c compiler that supports C11.

Optionally, you need:

1.  the pulseaudio development library to enable pulseaudio output;
2.  the portaudio development library to enable portaudio output;
3.  the sonic development library to enable sonic audio speed up support.

To build the documentation, you need:

1.  the `kramdown` markdown processor.
2.  the `ronn` man-page markdown processor.

### Debian

Core dependencies:

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| autotools     | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c11 compiler  | `sudo apt-get install gcc`                                       |

Optional dependencies:

| Dependency | Install                                 |
|------------|-----------------------------------------|
| pulseaudio | `sudo apt-get install libpulse-dev`     |
| portaudio  | `sudo apt-get install libportaudio-dev` |
| sonic      | `sudo apt-get install libsonic-dev`     |

Documentation dependencies:

| Dependency    | Install                              |
|---------------|--------------------------------------|
| kramdown      | `sudo apt-get install ruby-kramdown` |
| ronn          | `sudo apt-get install ruby-ronn`     |

Cross-compiling for windows:

| Dependency              | Install                                     |
|-------------------------|---------------------------------------------|
| 32-bit Windows compiler | `sudo apt-get install mingw-w64-i686-dev`   |
| 64-bit Windows compiler | `sudo apt-get install mingw-w64-x86-64-dev` |

## Building

The first time you build eSpeak NG, or when you want to change how to build
eSpeak NG, you need to run the following standard autotools commands:

	./autogen.sh
	./configure --prefix=/usr

__NOTE:__ The `--prefix` option above will install the files to the `/usr`
directory, instead of the default `/usr/local` location. You can use other
standard `configure` options to control the output. For more information,
you can run:

	./configure --help

The `espeak-ng` and `speak-ng` programs, along with the espeak-ng voices, can
then be built with:

	make

The documentation can be built by running:

	make docs

Specific languages can be compiled by running:

	make LANG

where `LANG` is the language code of the given language. More information can
be found in the [Adding or Improving a Language](docs/add_language.md)
documentation.

### Audio Output Configuration

The following `configure` options control which audio interfaces to use:

| Option              | Audio Interfaces | Default |
|---------------------|------------------|---------|
| `--with-pulseaudio` | PulseAudio       | yes     |
| `--with-portaudio`  | PortAudio        | yes     |
| `--with-sada`       | SADA (Solaris)   | no      |

If pulseaudio and portaudio are both enabled and available, eSpeak NG will choose
which one to use at runtime, trying pulseaudio first before falling back to
portaudio.

### eSpeak NG Feature Configuration

The following `configure` options control which eSpeak NG features are enabled:

| Option          | Description                                  | Default |
|-----------------|----------------------------------------------|---------|
| `--with-klatt`  | Enable Klatt formant synthesis.              | yes     |
| `--with-mbrola` | Enable MBROLA voice support.                 | yes     |
| `--with-sonic`  | Use the sonic library to support higher WPM. | yes     |
| `--with-async`  | Enable asynchronous commands.                | yes     |

__NOTE:__ The `--with-sonic` option requires that the sonic library and header
is accessible on the system.

### Extended Dictionary Configuration

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

### Cross-Compiling For Windows

To prepare the build, run:

	./autogen.sh
	export ac_cv_func_realloc_0_nonnull=yes
	export ac_cv_func_malloc_0_nonnull=yes

To build the 32-bit Windows executable, run:

	./configure --host=i686-w64-mingw32 --with-mbrola=no --with-async=no
	make

To build the 64-bit Windows executable, run:

	./configure --host=x86_64-w64-mingw32 --with-mbrola=no --with-async=no
	make

__NOTE:__ This currently fails to build `espeak-ng.exe`, but does build
`speak-ng.exe`.

## Testing

Before installing, you can test the built espeak-ng using the following command
from the top-level directory of this project:

    ESPEAK_DATA_PATH=`pwd` LD_LIBRARY_PATH=src:${LD_LIBRARY_PATH} src/espeak-ng ...

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak-ng data from
the source tree. Otherwise, espeak-ng will look in `$(HOME)` or
`/usr/share/espeak-data`.

The `LD_LIBRARY_PATH` is set as `espeak` uses the `libespeak-ng.so` shared
library. This ensures that `espeak` uses the built shared library in the
`src` directory and not the one on the system (which could be an older
version).

## Installing

You can install eSpeak NG by running the following command:

    sudo make LIBDIR=/usr/lib/x86_64-linux-gnu install

__NOTE:__ The `LIBDIR` path may be different to the one on your system (the
above is for 64-bit Debian/Ubuntu releases that use the multi-arch package
structure -- that is, Debian Wheezy or later).

You can find out where espeak-ng is installed to on your system if you
already have an espeak-ng install by running:

    find /usr/lib | grep libespeak-ng

## Documentation

The [main documentation](docs/index.md) for eSpeak NG provides more information
on using and creating voices/languages for for eSpeak NG.

The [espeak-ng](src/espeak-ng.1.ronn) command-line documentation provides a
reference of the different command-line options available, with example usage.

## Praat Changes

The `praat` program is used to create and edit the spectral data files for
espeak-ng. The changes to that program to support this are located on the
*espeak* branch of the [praat](https://github.com/rhdunn/praat) mirror.

## eSpeak for Android

The *android* branch contains the sources for the
[eSpeak for Android](http://reecedunn.co.uk/espeak-for-android) program, based
on the eyes-free port of eSpeak to Android.

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

eSpeak NG Text-to-Speech is released under the [GPL version 3](COPYING) or later license.
