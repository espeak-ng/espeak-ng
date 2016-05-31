# Change Log

## 8.0.0.1 - Tuesday May 31th 2016 \[May 2016\]

  *  Provide a C API in addition to the C++ API.
  *  Convert the implementation from C++ to C.

## 8.0.0 - Saturday June 6th 2015 \[Jun 2015\]

  *  Update to Unicode Character Data 8.0.0.
  *  Detect MacOSX `glibtoolize` in `autogen.sh`.
  *  Use UCD `PropertyValueAliases` instead of the IANA subtag registry for the
     script label to ISO 15924 mapping.
  *  Support enabling the CSUR data via the `--with-csur` configure flag.

## 7.0.0.1 - Saturday July 14th 2014 \[Jul 2014\]

  *  Remove the ConScript Unicode Registry data from the Script and Category tables.
  *  Fix mapping Script values to string.

## 7.0.0 - Saturday June 28th 2014 \[Jun 2014\]

  *  Update to Unicode Character Data 7.0.0.
  *  Updated the script list to support all ISO 15924 scripts.
  *  Add a make target for building the documentation.

## 6.3.0 - Wednesday October 16th 2013 \[Oct 2013\]

  *  Update to Unicode Character Data 6.3.0.

## 6.2.0 - Wednesday October 16th 2013 \[Oct 2013\]

  *  Use Unicode Character Data 6.2.0.
  *  General Category group and value lookup.
  *  Script lookup.
  *  `White_Space` property lookup provided by `isspace`.
  *  Uses compact data tables for minimal overhead.
  *  Support for Klingon \[F8D0-F8FF\] from the ConScript Unicode Registry.

wctype.h/wchar.h compatibility: 

  *  ctype-style API.
  *  case conversion API (including `totitle` to convert to title case).
