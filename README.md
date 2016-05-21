# eSpeak NG Text-to-Speech

<a href="https://travis-ci.org/espeak-ng/espeak-ng"><img src="https://secure.travis-ci.org/espeak-ng/espeak-ng.png?branch=master"/></a>

- [Getting source code](#getting-source-code)
- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Audio Output Configuration](#audio-output-configuration)
  - [eSpeak NG Feature Configuration](#espeak-ng-feature-configuration)
  - [Extended Dictionary Configuration](#extended-dictionary-configuration)
  - [Compiling on Windows](#compiling-on-windows)
- [Testing](#testing)
- [Installing](#installing)
- [Documentation](#documentation)
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

See the [CHANGELOG](CHANGELOG.md) for a description of the changes in the
various releases and with the eSpeak project.

## Getting source code

If necessary, install git package before. E.g., on Debian-based Linux:

	sudo apt-get install git

The source code repository can be cloned using git:

	git clone https://github.com/espeak-ng/espeak-ng.git

## Build Dependencies

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

### Debian

Core dependencies:

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| autotools     | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c99 compiler  | `sudo apt-get install gcc`                                       |

Optional dependencies:

| Dependency   | Install                                 |
|--------------|-----------------------------------------|
| sonic        | `sudo apt-get install libsonic-dev`     |
| ronn         | `sudo apt-get install ruby-ronn`        |

Documentation dependencies:

| Dependency    | Install                              |
|---------------|--------------------------------------|
| kramdown      | `sudo apt-get install ruby-kramdown` |

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

## Compiling on Windows

__NOTE:__ This is currently experimental. It does not build the voice data, and
may contain runtime errors and missing functionality.

To build eSpeak NG on Windows, you will need:

1. a copy of Visual Studio 2015, such as the Community Edition;
2. the Windows 8.1 SDK;
3. the [WiX installer](http://wixtoolset.org) plugin;
4. the [pcaudiolib](http://github.com/rhdunn/pcaudiolib) project checked out to
   `src` (as `src/pcaudiolib`).

You can then open and build the `src/windows/espeak-ng.sln` solution in Visual
Studio.

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

The [espeak-ng](src/espeak-ng.1.ronn) and [speak-ng](src/speak-ng.1.ronn)
command-line documentation provide a reference of the different command-line
options available to these commands with example usage.

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
