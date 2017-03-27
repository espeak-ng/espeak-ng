/*
 * Copyright (C) 2017 Reece H. Dunn
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

#include <espeak-ng/espeak_ng.h>

#include "speech.h"

// http://www.iana.org/assignments/character-sets/character-sets.xhtml
MNEM_TAB mnem_encoding[] = {
	{ "ANSI_X3.4-1968",   ESPEAKNG_ENCODING_US_ASCII },
	{ "ANSI_X3.4-1986",   ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM367",           ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO_646.irv:1991", ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO646-US",        ESPEAKNG_ENCODING_US_ASCII },
	{ "US-ASCII",         ESPEAKNG_ENCODING_US_ASCII },
	{ "cp367",            ESPEAKNG_ENCODING_US_ASCII },
	{ "csASCII",          ESPEAKNG_ENCODING_US_ASCII },
	{ "iso-ir-6",         ESPEAKNG_ENCODING_US_ASCII },
	{ "us",               ESPEAKNG_ENCODING_US_ASCII },
	{ NULL,               ESPEAKNG_ENCODING_UNKNOWN }
};

#pragma GCC visibility push(default)

espeak_ng_ENCODING
espeak_ng_EncodingFromName(const char *encoding)
{
	return LookupMnem(mnem_encoding, encoding);
}

#pragma GCC visibility pop
