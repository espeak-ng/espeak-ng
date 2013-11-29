# eSpeak Text-to-Speech

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
- [Testing](#testing)
- [Installing](#installing)
- [Building Voices](#building-voices)
- [Adding New Voices](#adding-new-voices)
- [Praat Modifications](#praat-modifications)
- [Historical Releases](#historical-releases)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

This is a mirror of the eSpeak sources with changes to make it build on POSIX
systems using autotools. It contains the following branches:

*  *upstream* — a mirror of the eSpeak subversion repository using `git svn`;
*  *development* — the eSpeak sources from
   [http://espeak.sourceforge.net/test/latest.html](http://espeak.sourceforge.net/test/latest.html);
*  *master* — build system changes and bug fixes;
*  *historical* — historical eSpeak releases from
   [http://sourceforge.net/projects/espeak/files/espeak/](http://sourceforge.net/projects/espeak/files/espeak/)
   not contained in the subversion codebase;
*  *android* — the source code of
   [eSpeak for Android](http://reecedunn.co.uk/espeak-for-android).

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

## Praat Modifications

The eSpeak sources from
[http://espeak.sourceforge.net/test/latest.html](http://espeak.sourceforge.net/test/latest.html)
contains a `praat-mod` folder. This provides modifications of the `praat` program
to support generating spectral data for use with espeakedit. This is not
available in the subversion releases.

Instead of adding the `praat-mod` folder to the sources here, to better track
the changes to the `praat` program the modifications are included in the
*espeak* branch of the [praat](https://github.com/rhdunn/praat) mirror. This
mirror currently includes support upto version 5.3.23 of praat.

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

## Bugs

Report bugs to the [espeak issues](https://github.com/rhdunn/espeak/issues)
page on GitHub.

## License Information

eSpeak Text-to-Speech is released under the GPL version 3 or later license.
