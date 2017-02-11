# Unicode Character Database Tools

- [Data Files](#data-files)
  - [Unicode Character Database](#unicode-character-database)
  - [ConScript Unicode Registry](#conscript-unicode-registry)
- [C Library](#c-library)
  - [Querying Properties](#querying-properties)
  - [Case Conversion](#case-conversion)
  - [wctype Compatibility](#wctype-compatibility)
- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
- [Updating the UCD Data](#updating-the-ucd-data)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Unicode Character Database (UCD) Tools is a set of Python tools and a C
library with a C++ API binding. The Python tools are designed to support
extracting and processing data from the text-based UCD source files, while
the C library is designed to provide easy access to this information within
a C or C++ program.

## Data Files

The `ucd-tools` project provides support for UCD formatted data files from
several different sources.

### Unicode Character Database

The following [Unicode Character Database](http://www.unicode.org/Public/9.0.0/ucd/)
files are supported:

*  Blocks
*  DerivedAge
*  PropList
*  PropertyValueAliases
*  Scripts
*  UnicodeData

### ConScript Unicode Registry

If enabled, the following data from the
[ConScript Unicode Registry](http://www.evertype.com/standards/csur/) (CSUR) is
added:

| Code Range   | Script  |
|--------------|---------|
| `F8D0-F8FF`  | [Klingon](http://www.evertype.com/standards/csur/klingon.html) |

This data is located in the `data/csur` directory in a form compatible with the
Unicode Character Data files.

## C Library

The C library provides several different facilities that make use of the UCD
data. It provides a compact and efficient representation of the different data
tables.

Detailed documentation is provided in the `src/include/ucd/ucd.h` file in the
Doxygen documentation format.

### Querying Properties

The library exposes the following properties from the UCD data files:

| Property           | Description |
|--------------------|-------------|
| `General_Category` | A [General Category Value](http://www.unicode.org/reports/tr44/#General_Category_Values), including the higher-level grouping. |
| `Script`           | An [ISO 15924](http://www.unicode.org/iso15924/iso15924-codes.html) script code. |

### Case Conversion

The following character conversion functions are provided:

| C API         | C++ API        | Description |
|---------------|----------------|-------------|
| `ucd_tolower` | `ucd::tolower` | convert letters to lower case |
| `ucd_totitle` | `ucd::totitle` | convert letters to title case (UCD extension) |
| `ucd_toupper` | `ucd::toupper` | convert letters to upper case |

__NOTE:__ These functions use the simple case mapping algorithm. That is, they
only ever map to a single character. This is to provide a compatible signature
to the standard C `wctype.h` APIs.

### wctype Compatibility

To facilitate working on platforms that don't have a useable wide-character
ctypes library, or to provide a more consistent behaviour, the `ucd-tools`
C library provides a set of APIs that are compatible with `wctype.h`.

The following character classification functions are provided:

| C API          | C++ API         |
|----------------|-----------------|
| `ucd_isalnum`  | `ucd::isalnum`  |
| `ucd_isalpha`  | `ucd::isalpha`  |
| `ucd_isblank`  | `ucd::isblank`  |
| `ucd_iscntrl`  | `ucd::iscntrl`  |
| `ucd_isdigit`  | `ucd::isdigit`  |
| `ucd_isgraph`  | `ucd::isgraph`  |
| `ucd_islower`  | `ucd::islower`  |
| `ucd_isprint`  | `ucd::isprint`  |
| `ucd_ispunct`  | `ucd::ispunct`  |
| `ucd_isspace`  | `ucd::isspace`  |
| `ucd_isupper`  | `ucd::isupper`  |
| `ucd_isxdigit` | `ucd::isxdigit` |

## Build Dependencies

In order to build ucd-tools, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake` and `libtool`);
2.  a functional C and C++ compiler.

__NOTE__: The C++ compiler is used to build the test for the C++ API.

To build the documentation, you need:

1.  the doxygen program to build the api documentation;
2.  the dot program from the graphviz library to generate graphs in the api documentation.

### Debian

Core Dependencies:

| Dependency       | Install                                               |
|------------------|-------------------------------------------------------|
| autotools        | `sudo apt-get install make autoconf automake libtool` |
| C++ compiler     | `sudo apt-get install gcc g++`                        |

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

Additionally, you can use the `UCD_FLAGS` option to control how the data is
generated. The following flags are supported:

| Flag        | Description |
|-------------|-------------|
| --with-csur | Add ConScript Unicode Registry data. |

## Bugs

Report bugs to the [ucd-tools issues](https://github.com/rhdunn/ucd-tools/issues)
page on GitHub.

## License Information

UCD Tools is released under the GPL version 3 or later license.

The UCD data files in `data/ucd` are downloaded from the UCD website and are
licensed under the [Unicode Terms of Use](COPYING.UCD). These data files are
used in their unmodified form. They have the following Copyright notice:

    Copyright © 1991-2014 Unicode, Inc. All rights reserved.

The files in `data/csur` are based on the information from the ConScript
Unicode Registry maintained by John Cowan and Michael Everson.
