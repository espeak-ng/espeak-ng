# Unicode Character Data Tools

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
- [Updating the UCD Data](#updating-the-ucd-data)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Unicode Character Data (UCD) Tools is a library for working with the
Unicode Character Data from unicode.org.

It provides a compact replacement for various wide-character C APIs. These can
be used in Android applications, as the Android C library does not have full
wide-character support.

In addition to this it provides APIs for:
-  querying the [Unicode General Category](http://www.unicode.org/reports/tr44/) values and groups;
-  querying the [ISO 15924](http://www.unicode.org/iso15924/iso15924-codes.html) script;
-  converting to upper, lower and title case.

The following data sets are used for the data tables:
-  [Unicode Character Data 7.0.0](http://www.unicode.org/Public/7.0.0/ucd/).

## Build Dependencies

In order to build ucd-tools, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake` and `libtool`);
2.  a functional c++ compiler.

To build the documentation, you need:

1.  the doxygen program to build the api documentation;
2.  the dot program from the graphviz library to generate graphs in the api documentation.

### Debian

Core Dependencies:

| Dependency       | Install                                               |
|------------------|-------------------------------------------------------|
| autotools        | `sudo apt-get install make autoconf automake libtool` |
| c++ compiler     | `sudo apt-get install gcc g++`                        |

Documentation Dependencies:

| Dependency | Install                         |
|------------|---------------------------------|
| doxygen    | `sudo apt-get install doxygen`  |
| graphviz   | `sudo apt-get install graphviz` |

## Building

UCD Tools supports the standard GNU autotools build system. The source code
does not contain the generated `configure` files, so to build it you need to
run:

	./autogen.sh
	./configure --prefix=/usr
	make

The tests can be run by using:

	make check

The program can be installed using:

	sudo make install

The documentation can be built using:

	make html

## Updating the UCD Data

To re-generate the source files from the UCD data when a new version of
unicode is released, you need to run:

	./configure --prefix=/usr --with-unicode-version=VERSION
	make ucd-update

where `VERSION` is the Unicode version (e.g. `6.3.0`).

## Bugs

Report bugs to the [ucd-tools issues](https://github.com/rhdunn/ucd-tools/issues)
page on GitHub.

## License Information

UCD Tools is released under the GPL version 3 or later license.
