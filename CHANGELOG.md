# Change Log

## 9.0.0.1 - (In Progress)

*  Add `iswblank` and `iswxdigit` compatibility.
*  Improve ctype compatibility.

## 9.0.0 - 2016-12-28

*  Update to Unicode Character Data 9.0.0.

## 8.0.0.1 - 2016-05-31

  *  Provide a C API in addition to the C++ API.
  *  Convert the implementation from C++ to C.

## 8.0.0 - 2015-06-06

  *  Update to Unicode Character Data 8.0.0.
  *  Detect MacOSX `glibtoolize` in `autogen.sh`.
  *  Use UCD `PropertyValueAliases` instead of the IANA subtag registry for the
     script label to ISO 15924 mapping.
  *  Support enabling the CSUR data via the `--with-csur` configure flag.

## 7.0.0.1 - 2014-07-14

  *  Remove the ConScript Unicode Registry data from the Script and Category tables.
  *  Fix mapping Script values to string.

## 7.0.0 - 2014-06-28

  *  Update to Unicode Character Data 7.0.0.
  *  Updated the script list to support all ISO 15924 scripts.
  *  Add a make target for building the documentation.

## 6.3.0 - 2013-10-16

  *  Update to Unicode Character Data 6.3.0.

## 6.2.0 - 2013-10-16

  *  Use Unicode Character Data 6.2.0.
  *  General Category group and value lookup.
  *  Script lookup.
  *  `White_Space` property lookup provided by `isspace`.
  *  Uses compact data tables for minimal overhead.
  *  Support for Klingon \[F8D0-F8FF\] from the ConScript Unicode Registry.

wctype.h/wchar.h compatibility: 

  *  ctype-style API.
  *  case conversion API (including `totitle` to convert to title case).
