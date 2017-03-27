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

#include <stdint.h>
#include <stdlib.h>

#include <espeak-ng/espeak_ng.h>

#include "speech.h"
#include "encoding.h"

// http://www.iana.org/assignments/character-sets/character-sets.xhtml
MNEM_TAB mnem_encoding[] = {
	{ "ANSI_X3.4-1968",   ESPEAKNG_ENCODING_US_ASCII },
	{ "ANSI_X3.4-1986",   ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM367",           ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM819",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO_646.irv:1991", ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO_8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO646-US",        ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO-8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-1:1987",  ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "US-ASCII",         ESPEAKNG_ENCODING_US_ASCII },
	{ "cp367",            ESPEAKNG_ENCODING_US_ASCII },
	{ "cp819",            ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csASCII",          ESPEAKNG_ENCODING_US_ASCII },
	{ "csISOLatin1",      ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "iso-ir-6",         ESPEAKNG_ENCODING_US_ASCII },
	{ "iso-ir-100",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "latin1",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "l1",               ESPEAKNG_ENCODING_ISO_8859_1 },
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

struct espeak_ng_TEXT_DECODER_
{
	const char *current;
	const char *end;

	uint32_t (*get)(espeak_ng_TEXT_DECODER *decoder);
};

static uint32_t
text_decoder_getc_us_ascii(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t c = *decoder->current++ & 0xFF;
	return (c >= 0x80) ? 0xFFFD : c;
}

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT
static uint32_t
text_decoder_getc_iso_8859_1(espeak_ng_TEXT_DECODER *decoder)
{
	return *decoder->current++ & 0xFF;
}

espeak_ng_TEXT_DECODER *
create_text_decoder(void)
{
	espeak_ng_TEXT_DECODER *decoder = malloc(sizeof(espeak_ng_TEXT_DECODER));
	if (!decoder) return NULL;

	decoder->current = NULL;
	decoder->end = NULL;
	decoder->get = NULL;
	return decoder;
}

void
destroy_text_decoder(espeak_ng_TEXT_DECODER *decoder)
{
	if (decoder) free(decoder);
}

static int
initialize_encoding(espeak_ng_TEXT_DECODER *decoder,
                    espeak_ng_ENCODING encoding)
{
	switch (encoding)
	{
	case ESPEAKNG_ENCODING_US_ASCII:
		decoder->get = text_decoder_getc_us_ascii;
		break;
	case ESPEAKNG_ENCODING_ISO_8859_1:
		decoder->get = text_decoder_getc_iso_8859_1;
		break;
	default:
		return 0;
	}
	return 1;
}

espeak_ng_STATUS
text_decoder_decode_string(espeak_ng_TEXT_DECODER *decoder,
                           const char *string,
                           int length,
                           espeak_ng_ENCODING encoding)
{
	if (!initialize_encoding(decoder, encoding))
		return ENS_UNKNOWN_TEXT_ENCODING;

	decoder->current = string;
	decoder->end = string + length;
	return ENS_OK;
}

int
text_decoder_eof(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->current == decoder->end;
}

uint32_t
text_decoder_getc(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->get(decoder);
}
