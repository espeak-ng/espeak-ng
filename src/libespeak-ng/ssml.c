/*
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2017 Reece H. Dunn
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


#include "config.h"

#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>


#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>
#include <ucd/ucd.h>

#include "error.h"
#include "speech.h"
#include "phoneme.h"
#include "voice.h"
#include "synthesize.h"
#include "translate.h"

int attrcmp(const wchar_t *string1, const char *string2);
int attrlookup(const wchar_t *string1, const MNEM_TAB *mtab);
int attrnumber(const wchar_t *pw, int default_value, int type);
int attrcopy_utf8(char *buf, const wchar_t *pw, int len);

int attrcmp(const wchar_t *string1, const char *string2)
{
	int ix;

	if (string1 == NULL)
		return 1;

	for (ix = 0; (string1[ix] == string2[ix]) && (string1[ix] != 0); ix++)
		;
	if (((string1[ix] == '"') || (string1[ix] == '\'')) && (string2[ix] == 0))
		return 0;
	return 1;
}


int attrlookup(const wchar_t *string1, const MNEM_TAB *mtab)
{
	int ix;

	for (ix = 0; mtab[ix].mnem != NULL; ix++) {
		if (attrcmp(string1, mtab[ix].mnem) == 0)
			return mtab[ix].value;
	}
	return mtab[ix].value;
}

int attrnumber(const wchar_t *pw, int default_value, int type)
{
	int value = 0;

	if ((pw == NULL) || !IsDigit09(*pw))
		return default_value;

	while (IsDigit09(*pw))
		value = value*10 + *pw++ - '0';
	if ((type == 1) && (ucd_tolower(*pw) == 's')) {
		// time: seconds rather than ms
		value *= 1000;
	}
	return value;
}

int attrcopy_utf8(char *buf, const wchar_t *pw, int len)
{
	// Convert attribute string into utf8, write to buf, and return its utf8 length
	unsigned int c;
	int ix = 0;
	int n;
	int prev_c = 0;

	if (pw != NULL) {
		while ((ix < (len-4)) && ((c = *pw++) != 0)) {
			if ((c == '"') && (prev_c != '\\'))
				break; // " indicates end of attribute, unless preceded by backstroke
			n = utf8_out(c, &buf[ix]);
			ix += n;
			prev_c = c;
		}
	}
	buf[ix] = 0;
	return ix;
}

