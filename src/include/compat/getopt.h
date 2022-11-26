/* Compatibility shim for <getopt.h>
 *
 * Copyright (C) 2006 to 2013 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2016 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#ifndef GETOPT_H_COMPAT_SHIM
#define GETOPT_H_COMPAT_SHIM

#if __has_include_next(<getopt.h>)
#pragma GCC system_header // Silence "warning: #include_next is a GCC extension"
#include_next <getopt.h>
#else

struct option {
	char *name;
	int has_arg;
	int *flag;
	int val;
};

extern int optind;
extern char *optarg;

#define no_argument 0
#define required_argument 1
#define optional_argument 2

int
getopt_long(int nargc, char * const *nargv, const char *options,
            const struct option *long_options, int *idx);

#endif

#endif
