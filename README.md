# eSpeak Text-to-Speech

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
- [Testing](#testing)
- [Installing](#installing)
- [Building Voices](#building-voices)
- [Adding New Voices](#adding-new-voices)
- [Historical Releases](#historical-releases)

----------

This branch contains changes to eSpeak to support building on a POSIX system
by using autotools.

This does the following things:

1.  compile the libespeak library;
2.  compile the speak and espeak command-line applications;
3.  compile the espeakedit application;
4.  compile the voice data, creating an espeak-data directory.

This branch also contains some bug fixes and improvements that get
sent back upstream.

## Build Dependencies

In order to build eSpeak, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c++ compiler;
3.  wxWidgets development libraries (needed to build and run espeakedit
    to compile the phoneme data).

Optionally, you need:

1.  the pulseaudio development library to enable pulseaudio output;
2.  the portaudio development library to enable portaudio output.

### Debian

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| autotools     | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c++ compiler  | `sudo apt-get install gcc g++`                                   |
| wxWidgets     | `sudo apt-get install libwxgtk2.8-dev`                           |
| pulseaudio    | `sudo apt-get install libpulse-dev`                              |
| portaudio     | `sudo apt-get install libportaudio-dev`                          |

## Building

The espeak and espeakedit programs, along with the espeak voices, can
be built via the standard autotools commands:

	$ ./autogen.sh
	$ ./configure --prefix=/usr
	$ make

__NOTE:__ The configure command detects various platform differences that
the espeak makefiles don't cater for (e.g. different wxWidgets version)
and detect the available audio setup to use automatically.

## Testing

Before installing, you can test the built espeak using the following command:

    $ ESPEAK_DATA_PATH=`pwd` LD_LIBRARY_PATH=src:${LD_LIBRARY_PATH} src/espeak ...

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak data from
the source tree. Otherwise, espeak will look in `$(HOME)` or
`/usr/share/espeak-data`.

The `LD_LIBRARY_PATH` is set as `espeak` uses the `libespeak.so` shared
library. This ensures that `espeak` uses the built shared library in the
`src` directory and not the one on the system (which could be an older
version).

## Installing

You can install eSpeak by running the following command:

    $ sudo make LIBDIR=/usr/lib/x86_64-linux-gnu install

The `LIBDIR` path may be different to the one on your system (the above
is for 64-bit Debian/Ubuntu releases that use the multi-arch package
structure -- that is, Debian Wheezy or later).

You can find out where espeak is installed to on your system if you
already have an espeak install by running:

    $ find /usr/lib | grep libespeak

## Building Voices

If you are modifying a language's phoneme, voice or dictionary files, you
can just build that voice by running:

    $ make <lang-code>

For example, if you add entries in the `dictsource/en_extra` file, you can
run:

    $ make en

to build an English voice file with those changes in without rebuilding
all the voices. This will make it easier to spot errors.

## Adding New Voices

Once you have added the necessary files to eSpeak to support the new voice,
you can then run:

    $ ./mkdictlist Makefile.am

This will update the build system so that `make` will build the new voice
in addition to building everything else, and add a `<lang-code>` target
for building just that voice.

## Historical Releases

1.24.02 is the first version of eSpeak to appear in the subversion
repository, but releases from 1.05 to 1.24 are available on the
sourceforge website at http://sourceforge.net/projects/espeak/files/espeak/.

These early releases have been checked into the historical branch,
with the 1.24.02 release as the last entry. This makes it possible
to use the replace functionality of git to see the earlier history:

	$ git replace 8d59235f 63c1c019

__NOTE:__ The source releases contain the big_endian, espeak-edit, praat-mod,
riskos, windows_dll and windows_sapi folders. These do not appear in the
source repository until later releases, so have been excluded from the
historical commits to align them better with the 1.24.02 source commit.
