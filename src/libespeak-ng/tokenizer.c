/* Tokenizer APIs.
 *
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <ucd/ucd.h>

#include "encoding.h"
#include "tokenizer.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

#define ESPEAKNG_CLAUSE_TYPE_PROPERTY_MASK 0xFFF0000000000000ull

int clause_type_from_codepoint(uint32_t c)
{
	ucd_category cat = ucd_lookup_category(c);
	ucd_property props = ucd_properties(c, cat);

	switch (props & ESPEAKNG_CLAUSE_TYPE_PROPERTY_MASK)
	{
	case ESPEAKNG_PROPERTY_FULL_STOP:
		return CLAUSE_PERIOD;
	case ESPEAKNG_PROPERTY_FULL_STOP | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
		return CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_QUESTION_MARK:
		return CLAUSE_QUESTION;
	case ESPEAKNG_PROPERTY_QUESTION_MARK | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
		return CLAUSE_QUESTION | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_QUESTION_MARK | ESPEAKNG_PROPERTY_PUNCTUATION_IN_WORD:
		return CLAUSE_QUESTION | CLAUSE_PUNCTUATION_IN_WORD;
	case ESPEAKNG_PROPERTY_EXCLAMATION_MARK:
		return CLAUSE_EXCLAMATION;
	case ESPEAKNG_PROPERTY_EXCLAMATION_MARK | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
		return CLAUSE_EXCLAMATION | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_EXCLAMATION_MARK | ESPEAKNG_PROPERTY_PUNCTUATION_IN_WORD:
		return CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD;
	case ESPEAKNG_PROPERTY_COMMA:
		return CLAUSE_COMMA;
	case ESPEAKNG_PROPERTY_COMMA | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
		return CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_COLON:
		return CLAUSE_COLON;
	case ESPEAKNG_PROPERTY_COLON | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
		return CLAUSE_COLON | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_SEMI_COLON:
	case ESPEAKNG_PROPERTY_EXTENDED_DASH:
		return CLAUSE_SEMICOLON;
	case ESPEAKNG_PROPERTY_SEMI_COLON | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER:
	case ESPEAKNG_PROPERTY_QUESTION_MARK | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER | ESPEAKNG_PROPERTY_INVERTED_TERMINAL_PUNCTUATION:
	case ESPEAKNG_PROPERTY_EXCLAMATION_MARK | ESPEAKNG_PROPERTY_OPTIONAL_SPACE_AFTER | ESPEAKNG_PROPERTY_INVERTED_TERMINAL_PUNCTUATION:
		return CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_ELLIPSIS:
		return CLAUSE_SEMICOLON | CLAUSE_SPEAK_PUNCTUATION_NAME | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_PARAGRAPH_SEPARATOR:
		return CLAUSE_PARAGRAPH;
	}

	return CLAUSE_NONE;
}

struct espeak_ng_TOKENIZER_
{
	espeak_ng_TEXT_DECODER *decoder;
	char token[256];

	espeak_ng_TOKEN_TYPE (*read)(espeak_ng_TOKENIZER *tokenizer);
};

static espeak_ng_TOKEN_TYPE
tokenizer_state_end_of_buffer(espeak_ng_TOKENIZER *tokenizer)
{
	*tokenizer->token = '\0';
	return ESPEAKNG_TOKEN_END_OF_BUFFER;
}

static espeak_ng_TOKEN_TYPE
tokenizer_state_default(espeak_ng_TOKENIZER *tokenizer)
{
	if (text_decoder_eof(tokenizer->decoder)) {
		tokenizer->read = tokenizer_state_end_of_buffer;
		return tokenizer_state_end_of_buffer(tokenizer);
	}

	uint32_t c;
	char *current = tokenizer->token;

	switch (c = text_decoder_getc(tokenizer->decoder))
	{
	case '\r':
	case '\n':
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_NEWLINE;
	case '\0':
		tokenizer->read = tokenizer_state_end_of_buffer;
		return tokenizer_state_end_of_buffer(tokenizer);
	default:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_UNKNOWN;
	}

	return ESPEAKNG_TOKEN_END_OF_BUFFER;
}

espeak_ng_TOKENIZER *
create_tokenizer(void)
{
	espeak_ng_TOKENIZER *tokenizer = malloc(sizeof(espeak_ng_TOKENIZER));
	if (!tokenizer) return NULL;

	tokenizer->decoder = NULL;
	tokenizer->read = tokenizer_state_end_of_buffer;

	*tokenizer->token = '\0';
	return tokenizer;
}

void
destroy_tokenizer(espeak_ng_TOKENIZER *tokenizer)
{
	if (tokenizer) free(tokenizer);
}

int
tokenizer_reset(espeak_ng_TOKENIZER *tokenizer,
                espeak_ng_TEXT_DECODER *decoder)
{
	if (!tokenizer) return 0;

	tokenizer->decoder = decoder;
	tokenizer->read = decoder ? tokenizer_state_default : tokenizer_state_end_of_buffer;
	return 1;
}

espeak_ng_TOKEN_TYPE
tokenizer_read_next_token(espeak_ng_TOKENIZER *tokenizer)
{
	return tokenizer->read(tokenizer);
}

const char *
tokenizer_get_token_text(espeak_ng_TOKENIZER *tokenizer)
{
	return tokenizer->token;
}
