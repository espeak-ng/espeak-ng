# eSpeak Text-to-Speech

This branch contains changes to eSpeak to support building on a POSIX system
by using autotools.

This does the following things:

1.  compile the libespeak library;
2.  compile the speak and espeak command-line applications;
3.  compile the espeakedit application;
4.  compile the voice data, creating an espeak-data directory.

This branch also contains some bug fixes and improvements that get
sent back upstream.

## Building

The espeak and espeakedit programs, along with the espeak voices, can
be built via the standard autotools commands:

	$ ./autogen.sh
	$ ./configure
	$ make
	$ sudo make install

__NOTE:__ The configure command detects various platform differences that
the espeak makefiles don't cater for (e.g. different wxWidgets version)
and detect the available audio setup to use automatically.

## Testing

Before installing, you can test the built espeak using the following command:

    $ ESPEAK_DATA_PATH=`pwd` src/espeak hello

The `ESPEAK_DATA_PATH` variable needs to be set to use the espeak data from
the source tree. Otherwise, espeak will look in `$(HOME)` or
`/usr/share/espeak-data`.

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
