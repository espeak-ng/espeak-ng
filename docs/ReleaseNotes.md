Release Notes
=============

Version 7.0.0: Saturday June 28th 2014 \[Jun 2014\]
---------------------------------------------------

  *  Update to Unicode Character Data 7.0.0.
  *  Updated the script list to support all ISO 15924 scripts.
  *  Add a make target for building the documentation.

Version 6.3.0: Wednesday October 16th 2013 \[Oct 2013\]
-------------------------------------------------------

  *  Update to Unicode Character Data 6.3.0.

Version 6.2.0: Wednesday October 16th 2013 \[Oct 2013\]
-------------------------------------------------------

  *  Use Unicode Character Data 6.2.0.
  *  General Category group and value lookup.
  *  Script lookup.
  *  `White_Space` property lookup provided by `isspace`.
  *  Uses compact data tables for minimal overhead.
  *  Support for Klingon [F8D0-F8FF] from the ConScript Unicode Registry.

wctype.h/wchar.h compatibility: 

  *  ctype-style API.
  *  case conversion API (including `totitle` to convert to title case).

  *  Supports the following information from the Unicode Character Data:
     *  General Category Value (`UnicodeData.txt`);
     *  General Category Group (`UnicodeData.txt`).
  *  Supports the following properties from `PropertyList.txt`:
  *  ctype-style API compatible with `wctype.h`.
  *  case-conversion API for 
