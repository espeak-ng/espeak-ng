# eSpeak Text-to-Speech

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Audio Output Configuration](#audio-output-configuration)
  - [eSpeak Feature Configuration](#espeak-feature-configuration)
  - [Extended Dictionary Configuration](#extended-dictionary-configuration)
- [Testing](#testing)
- [Installing](#installing)
- [Building Voices](#building-voices)
- [Adding New Voices](#adding-new-voices)
- [Branching Structure and Changes](#branching-structure-and-changes)
  - [upstream](#upstream)
    - [Setting Up git-svn](#setting-up-git-svn)
  - [development](#development)
    - [Praat Modifications](#praat-modifications)
  - [master](#master)
    - [Build System](#build-system)
    - [espeakedit](#espeakedit)
    - [Bug Fixes](#bug-fixes)
  - [android](#android)
  - [historical](#historical)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The [eSpeak Text-to-Speech](http://espeak.sourceforge.net/) program is an open
source speech synthesizer that supports over 70 languages. It uses spectral
formant synthesis by default which sounds robotic, but can be configured to
use Klatt formant synthesis or MBROLA to give it a more natural sound.

This is a mirror of the eSpeak sources with the following improvements on the
*master* branch:

1.  An autotools-based build system that supports building the phoneme data and
    language dictionaries from source on POSIX systems.

2.  Optional support for the extended Russian and Chinese dictionaries from
    [http://espeak.sourceforge.net/data/](http://espeak.sourceforge.net/data/).

3.  Detection of the supported audio APIs via the `configure` script — no need to
    modify the build files.

4.  The NVDA voice variants are included in the `espeak-data/voices/!v` directory.

5.  Add support for the Oromo language (`om`, contributed by [gude432](https://github.com/gude432)).

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

### Audio Output Configuration

The following `configure` options control which audio interfaces to use:

| Option              | Audio Interfaces | Default |
|---------------------|------------------|---------|
| `--with-pulseaudio` | PulseAudio       | yes     |
| `--with-portaudio`  | PortAudio        | yes     |
| `--with-sada`       | SADA (Solaris)   | no      |

If pulseaudio and portaudio are both enabled and available, eSpeak will choose
which one to use at runtime, trying pulseaudio first before falling back to
portaudio.

### eSpeak Feature Configuration

The following `configure` options control which eSpeak features are enabled:

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

If the option has the value `no`, the extended dictionary is not built; instead,
the version from upstream espeak is build. If the option has the value `yes`,
the extended dictionary is built; that is, the extra data from
[http://espeak.sourceforge.net/data/](http://espeak.sourceforge.net/data/)
is used.

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

## Branching Structure and Changes

The following sections describe the different branches in this repository,
what they are used for and how they relate to the upstream version of eSpeak
(including details of any changes made to eSpeak).

Changes are merged in the following order:

    upstream => development => master => android

### upstream

The *upstream* branch contains the unmodified eSpeak sources imported from
the [espeak]() subversion repository using the `git svn` command.

#### Setting Up git-svn

In order to synchronize with the SVN repository, you need to initialize git-svn:

	$ git svn init https://svn.code.sf.net/p/espeak/code/trunk

Next, you need to make git-svn look for commits on the upstream branch:

	$ git update-ref refs/remotes/git-svn refs/remotes/origin/upstream

The eSpeak SVN repository moved, so you need to inform git-svn to rewrite the
URLs to the old location:

	$ git config svn-remote.svn.rewriteRoot https://espeak.svn.sourceforge.net/svnroot/espeak/trunk

Now you can update from SVN by running:

	$ git checkout upstream && git svn rebase

### development

The *development* branch replaces the code from subversion with the code
from [http://espeak.sourceforge.net/test/latest.html](http://espeak.sourceforge.net/test/latest.html).
It combines the *eSpeak* and *espeakedit* zip files containing the Linux
binaries with source code to match the subversion code.

The motivation for using these development releases is that the source
code from the subversion repositories is often incomplete -- either
missing several files, or not including certain changes. This is also
why the *master* branch pulls changes from *development*, not *upstream*.

The following changes are performed when merging the two zip files:

1.  the `src/Makefile` in the *espeakedit* code has been renamed to
    `src/Makefile.espeakedit` to match the name in subversion;

2.  the *espeakedit* files have been merged into the *eSpeak* files
    as the subversion repository does not split out the two;

3.  the `platforms/riscos/Makefile,fe1` file is moved to `platforms/riscos/Makefile`;

4.  the `platforms/riscos/copysource,feb` file is moved to `platforms/riscos/copysource`;

5.  the `phsource/vnasal/.directory` file is removed — this is used by the
    `Dolphin` file browser and should not be included in the sources;

6.  the `docs` folder contains an older version of the documentation, so these
    changes are reverted;

7.  the `espeak-data/voices/asia/ko` file is missing from the development
    sources, but none of the other related Korean voice files have been removed
    so the file is restored to the upstream version;

8.  the executable bit is removed from the source files — this is because
    there are no files that need to be executable in the source tree.

The following files are removed (excluded via `.gitignore`) from the sources:

1.  the Linux binaries (including the voice/phoneme data) — these are not
    part of the source code;

2.  generated files such as `dictsource/dict_phonemes` — these are not part
    of the source code;

3.  `praat-mod` — this is not included as part of the subversion code (see
    [Praat Modifications](#praat-modifications) for where to get these
    changes);

4.  `Makefile` — this is a copy of `src/Makefile`;

5.  `ReadMe` — this is not provided in the subversion codebase, instead a
    more informative version is located in this `README.md` file;

6.  `ChangeLog.txt` — this is not provided in the subversion codebase, is
    incomplete and there is a more detailed change log available in the
    subversion and git repositories;

7.  `License.txt` — this is a copy of the GPLv3 license; it is not provided
    in the subversion repository; for this git mirror, the GPLv3 license is
    located in the `COPYING` file as per GNU guidelines.

**NOTE:** The generated files `phsource/compile_report` and
`dictsource/dict_phonemes` are included in the subversion repository. They
have been removed from the git repository as well as being added to the
`.gitignore` file; as such, they will cause merge conflicts when merging
from *upstream* to *development*.

#### Praat Modifications

The `praat-mod` directory in the development zip files provides modifications
of the `praat` program to support generating spectral data for use with
espeakedit. This is not available in the subversion releases.

Instead of adding the `praat-mod` folder to the sources here, to better track
the changes to the `praat` program the modifications are included in the
*espeak* branch of the [praat](https://github.com/rhdunn/praat) mirror. This
mirror currently includes support upto version 5.3.23 of praat.

### master

The *master* branch contains changes to the build system, espeakedit and bug
fixes detailed in the sections below. In addition, it:

1.  includes the NVDA voice variants in the `espeak-data/voices/!v` directory;

2.  supports the Oromo language (`om`, contributed by [gude432](https://github.com/gude432)).

3.  Optional support for the extended Russian (`ru`), Mandarin Chinese (`zh`)
    and Cantonese Chinese (`zhy`) dictionaries from
    [http://espeak.sourceforge.net/data/](http://espeak.sourceforge.net/data/).

4.  Use `/i/` instead of `/ɪ/` for `en` transcription of the `/i/` phoneme;
    `en-n` still uses the accent-specific `/ɪ/` transcription.

#### Build System

The build system for eSpeak has been changed on the *master* branch to use
autotools and to support building the phoneme data and language dictionaries
on POSIX systems.

1.  Use autotools (`autoconf`, `automake` and `libtool`) to build espeak.

2.  Use the portaudio header on the build machine, not the one in espeak. This
    avoids the need to link to the correct portaudio header file.

3.  Detect wxWidgets using `wx-config`.

4.  Add `configure` options and detection for the `pulse`, `portaudio`,
    `runtime` and `sada` audio API modes.

5.  Allow `CXXFLAGS` to be overwritten from the environment. This is based
    on the patch for Debian bug #707925 by Jason White.

6.  *Klatt*, *MBROLA*, *libsonic* and asynchronous command support was made
    optional. This covers:
    *  adding an option to the `configure` script;
    *  only building the source files that are enabled;
    *  removing the guards around the relevant source files;
    *  removing the relevant conditional compilation defines from `speech.h`;
    *  fixing the sources if needed to build without support for the feature.

#### espeakedit

The following changes have been made to `espeakedit` to make it usable from
the command line, especially for use in the build process:

1.  Build the intonation data as well as the phoneme data when the `--compile`
    option is passed.

2.  Don't compile the dictionaries when using the `--compile` command.

3.  Only call `DrawEnvelopes` if `MAKE_ENVELOPES` is defined. This prevents the
    `phsource/envelopes.png` file being updated every build.

4.  Don't load the phoneme data before compiling it, as the phoneme data may
    not exist.

5.  Use the path set by `ESPEAK_DATA_PATH` if that environment variable exists,
    otherwise use the `HOME` environment variable.

6.  Don't use the paths from the configuration file when running the `--compile`
    command.

#### Bug Fixes

The following bugs have been fixed on the *master* branch:

*  all bug fixes have been applied upstream.

The bug fixes are sent upstream for inclusion in the upstream version of eSpeak.

### android

The *android* branch contains the sources for the
[eSpeak for Android](http://reecedunn.co.uk/espeak-for-android) program, based
on the eyes-free port of eSpeak to Android.

### historical

The *historical* branch contains the available older releases of eSpeak that
are not contained in the subversion repository.

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

Report bugs to the [espeak issues](https://github.com/rhdunn/espeak/issues)
page on GitHub.

## License Information

eSpeak Text-to-Speech is released under the GPL version 3 or later license.
