# eSpeak NG Text-to-Speech

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Audio Output Configuration](#audio-output-configuration)
  - [eSpeak NG Feature Configuration](#espeak-ng-feature-configuration)
  - [Extended Dictionary Configuration](#extended-dictionary-configuration)
- [Testing](#testing)
- [Installing](#installing)
- [Building Voices](#building-voices)
- [Adding New Voices](#adding-new-voices)
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
2.  a functional c compiler that supports C11;
3.  a functional c++ compiler;
4.  wxWidgets development libraries (needed to build and run espeakedit
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

The espeak-ng and espeakedit programs, along with the espeak-ng voices, can
be built via the standard autotools commands:

	$ ./autogen.sh
	$ ./configure --prefix=/usr
	$ make

__NOTE:__ The configure command detects various platform differences that
the espeak-ng makefiles don't cater for (e.g. different wxWidgets version)
and detect the available audio setup to use automatically.

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

## Testing

Before installing, you can test the built espeak-ng using the following command:

    $ ESPEAK_DATA_PATH=`pwd` LD_LIBRARY_PATH=src:${LD_LIBRARY_PATH} src/espeak-ng ...

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak-ng data from
the source tree. Otherwise, espeak-ng will look in `$(HOME)` or
`/usr/share/espeak-data`.

The `LD_LIBRARY_PATH` is set as `espeak` uses the `libespeak-ng.so` shared
library. This ensures that `espeak` uses the built shared library in the
`src` directory and not the one on the system (which could be an older
version).

## Installing

You can install eSpeak NG by running the following command:

    $ sudo make LIBDIR=/usr/lib/x86_64-linux-gnu install

The `LIBDIR` path may be different to the one on your system (the above
is for 64-bit Debian/Ubuntu releases that use the multi-arch package
structure -- that is, Debian Wheezy or later).

You can find out where espeak-ng is installed to on your system if you
already have an espeak-ng install by running:

    $ find /usr/lib | grep libespeak-ng

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

Once you have added the necessary files to eSpeak NG to support the new voice,
you can then run:

    $ ./mkdictlist Makefile.am

This will update the build system so that `make` will build the new voice
in addition to building everything else, and add a `<lang-code>` target
for building just that voice.

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

	$ git replace 8d59235f 63c1c019

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
