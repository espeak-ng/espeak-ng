// The Visual C++ C Runtime deprecates several standard library functions in
// preference for _s variants that are specific to Visual C++. This removes
// those deprecation warnings.
#define _CRT_SECURE_NO_WARNINGS

// The Visual C++ C Runtime deprecates standard POSIX APIs that conflict with
// reserved ISO C names (like strdup) in favour of non-portable conforming
// variants that start with an '_'. This removes those deprecation warnings.
#define _CRT_NONSTDC_NO_DEPRECATE

// This is created by autoconf on POSIX-compatibe build environments.
#define PACKAGE_VERSION "1.52.0"
