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

typedef enum {
	ESPEAKNG_CTYPE_OTHER,
	ESPEAKNG_CTYPE_CARRIAGE_RETURN,
	ESPEAKNG_CTYPE_NEWLINE,
	ESPEAKNG_CTYPE_END_OF_STRING,
	ESPEAKNG_CTYPE_PARAGRAPH,
	ESPEAKNG_CTYPE_WHITESPACE,
	ESPEAKNG_CTYPE_LOWERCASE,
	ESPEAKNG_CTYPE_UPPERCASE,
	ESPEAKNG_CTYPE_FULL_STOP,
	ESPEAKNG_CTYPE_QUESTION_MARK,
	ESPEAKNG_CTYPE_EXCLAMATION_MARK,
	ESPEAKNG_CTYPE_COMMA,
	ESPEAKNG_CTYPE_COLON,
} espeakng_CTYPE;

#define ESPEAKNG_CTYPE_PROPERTY_MASK 0xF80000000000C001ull

// Reference: http://www.unicode.org/reports/tr14/tr14-32.html -- Unicode Line Breaking Algorithm
static espeakng_CTYPE codepoint_type(uint32_t c)
{
	// 1. Detect and classify specific codepoints.

	switch (c)
	{
	case 0x0000: return ESPEAKNG_CTYPE_END_OF_STRING; // NULL
	case 0x000A: return ESPEAKNG_CTYPE_NEWLINE; // LINE FEED (LF)
	case 0x000C: return ESPEAKNG_CTYPE_NEWLINE; // FORM FEED (FF)
	case 0x000D: return ESPEAKNG_CTYPE_CARRIAGE_RETURN; // CARRIAGE RETURN (CR)
	case 0x0085: return ESPEAKNG_CTYPE_NEWLINE; // NEW LINE (NEL)
	}

	// 2. Classify codepoints by their Unicode General Category.

	ucd_category cat = ucd_lookup_category(c);
	switch (cat)
	{
	case UCD_CATEGORY_Lu: return ESPEAKNG_CTYPE_UPPERCASE;
	case UCD_CATEGORY_Ll: return ESPEAKNG_CTYPE_LOWERCASE;
	case UCD_CATEGORY_Zl: return ESPEAKNG_CTYPE_NEWLINE;
	case UCD_CATEGORY_Zp: return ESPEAKNG_CTYPE_PARAGRAPH;
	case UCD_CATEGORY_Zs: return ESPEAKNG_CTYPE_WHITESPACE;
	}

	// 3. Classify codepoints by their Unicode properties.

	ucd_property props = ucd_properties(c, cat);
	switch (props & ESPEAKNG_CTYPE_PROPERTY_MASK)
	{
	case UCD_PROPERTY_WHITE_SPACE:
		return ESPEAKNG_CTYPE_WHITESPACE;
	case UCD_PROPERTY_OTHER_LOWERCASE:
		return ESPEAKNG_CTYPE_LOWERCASE;
	case UCD_PROPERTY_OTHER_UPPERCASE:
		return ESPEAKNG_CTYPE_UPPERCASE;
	case ESPEAKNG_PROPERTY_FULL_STOP:
		return ESPEAKNG_CTYPE_FULL_STOP;
	case ESPEAKNG_PROPERTY_QUESTION_MARK:
		return ESPEAKNG_CTYPE_QUESTION_MARK;
	case ESPEAKNG_PROPERTY_EXCLAMATION_MARK:
		return ESPEAKNG_CTYPE_EXCLAMATION_MARK;
	case ESPEAKNG_PROPERTY_COMMA:
		return ESPEAKNG_CTYPE_COMMA;
	case ESPEAKNG_PROPERTY_COLON:
		return ESPEAKNG_CTYPE_COLON;
	}

	// 4. Classify the remaining codepoints.

	return ESPEAKNG_CTYPE_OTHER;
}

#define ESPEAKNG_CODEPOINT_INVALID 0xFFFFFFFF

struct espeak_ng_TOKENIZER_
{
	espeak_ng_TEXT_DECODER *decoder;
	char token[256];
	uint32_t keepc;

	espeak_ng_TOKEN_TYPE (*read)(espeak_ng_TOKENIZER *tokenizer);
};

static espeak_ng_TOKEN_TYPE
tokenizer_state_end_of_buffer(espeak_ng_TOKENIZER *tokenizer)
{
	*tokenizer->token = '\0';
	return ESPEAKNG_TOKEN_END_OF_BUFFER;
}

static espeak_ng_TOKEN_TYPE
tokenizer_read_word_token(espeak_ng_TOKENIZER *tokenizer, char *current, espeak_ng_TOKEN_TYPE type)
{
	char *end = tokenizer->token + sizeof(tokenizer->token) - 5; // allow for UTF-8 trailing bytes
	int initial_state = 1;

	while (current < end && !text_decoder_eof(tokenizer->decoder)) {
		uint32_t c = text_decoder_getc(tokenizer->decoder);
		switch (codepoint_type(c))
		{
		case ESPEAKNG_CTYPE_LOWERCASE:
			current += utf8_out(c, current);
			switch (type)
			{
			case ESPEAKNG_TOKEN_WORD_LOWERCASE:
			case ESPEAKNG_TOKEN_WORD_MIXEDCASE:
			case ESPEAKNG_TOKEN_WORD_CAPITALIZED:
				break;
			case ESPEAKNG_TOKEN_WORD_UPPERCASE:
				type = initial_state
				     ? ESPEAKNG_TOKEN_WORD_CAPITALIZED
				     : ESPEAKNG_TOKEN_WORD_MIXEDCASE;
				break;
			}
			initial_state = 0;
			break;
		case ESPEAKNG_CTYPE_UPPERCASE:
			current += utf8_out(c, current);
			switch (type)
			{
			case ESPEAKNG_TOKEN_WORD_UPPERCASE:
			case ESPEAKNG_TOKEN_WORD_MIXEDCASE:
				break;
			case ESPEAKNG_TOKEN_WORD_LOWERCASE:
			case ESPEAKNG_TOKEN_WORD_CAPITALIZED:
				type = ESPEAKNG_TOKEN_WORD_MIXEDCASE;
				break;
			}
			initial_state = 0;
			break;
		default:
			tokenizer->keepc = c;
			*current = '\0';
			return type;
		}
	}

	*current = '\0';
	return type;
}

static espeak_ng_TOKEN_TYPE
tokenizer_state_default(espeak_ng_TOKENIZER *tokenizer)
{
	if (text_decoder_eof(tokenizer->decoder)) {
		tokenizer->read = tokenizer_state_end_of_buffer;
		return tokenizer_state_end_of_buffer(tokenizer);
	}

	char *current = tokenizer->token;
	char *end     = tokenizer->token + sizeof(tokenizer->token) - 5; // allow for UTF-8 trailing bytes

	uint32_t c;
	if (tokenizer->keepc != ESPEAKNG_CODEPOINT_INVALID) {
		c = tokenizer->keepc;
		tokenizer->keepc = ESPEAKNG_CODEPOINT_INVALID;
	} else {
		c = text_decoder_getc(tokenizer->decoder);
	}

	switch (codepoint_type(c))
	{
	case ESPEAKNG_CTYPE_END_OF_STRING: // '\0'
		tokenizer->read = tokenizer_state_end_of_buffer;
		return tokenizer_state_end_of_buffer(tokenizer);
	case ESPEAKNG_CTYPE_CARRIAGE_RETURN: // '\r'
		if (text_decoder_peekc(tokenizer->decoder) == '\n') {
			current += utf8_out(c, current);
			c = text_decoder_getc(tokenizer->decoder);
		}
		// fallthrough
	case ESPEAKNG_CTYPE_NEWLINE:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_NEWLINE;
	case ESPEAKNG_CTYPE_PARAGRAPH:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_PARAGRAPH;
	case ESPEAKNG_CTYPE_WHITESPACE:
		current += utf8_out(c, current);
		while (!text_decoder_eof(tokenizer->decoder) &&
		       current < end &&
		       codepoint_type(c = text_decoder_getc(tokenizer->decoder)) == ESPEAKNG_CTYPE_WHITESPACE)
		{
			current += utf8_out(c, current);
		}
		tokenizer->keepc = c;
		*current = '\0';
		return ESPEAKNG_TOKEN_WHITESPACE;
	case ESPEAKNG_CTYPE_LOWERCASE:
		current += utf8_out(c, current);
		return tokenizer_read_word_token(tokenizer, current, ESPEAKNG_TOKEN_WORD_LOWERCASE);
	case ESPEAKNG_CTYPE_UPPERCASE:
		current += utf8_out(c, current);
		return tokenizer_read_word_token(tokenizer, current, ESPEAKNG_TOKEN_WORD_UPPERCASE);
	case ESPEAKNG_CTYPE_FULL_STOP:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_FULL_STOP;
	case ESPEAKNG_CTYPE_QUESTION_MARK:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_QUESTION_MARK;
	case ESPEAKNG_CTYPE_EXCLAMATION_MARK:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_EXCLAMATION_MARK;
	case ESPEAKNG_CTYPE_COMMA:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_COMMA;
	case ESPEAKNG_CTYPE_COLON:
		current += utf8_out(c, current);
		*current = '\0';
		return ESPEAKNG_TOKEN_COLON;
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
	tokenizer->keepc = ESPEAKNG_CODEPOINT_INVALID;
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
                espeak_ng_TEXT_DECODER *decoder,
                espeak_ng_TOKENIZER_OPTIONS options)
{
	if (!tokenizer) return 0;

	tokenizer->decoder = decoder;
	tokenizer->keepc = ESPEAKNG_CODEPOINT_INVALID;
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
