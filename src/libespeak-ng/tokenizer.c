/* Tokenizer APIs.
 *
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
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

#include "encoding.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

// punctuations symbols that can end a clause
static const unsigned short punct_chars[] = {
	0x00a1, // inverted exclamation
	0x00bf, // inverted question
	0
};

// indexed by entry num. in punct_chars
static const unsigned int punct_attributes[] = {
	CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER,  // inverted exclamation
	CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER,  // inverted question
	0
};

#define ESPEAKNG_CLAUSE_TYPE_PROPERTY_MASK 0xFFF0000000000000ull

int clause_type_from_codepoint(uint32_t c)
{
	ucd_category cat = ucd_lookup_category(c);
	ucd_property props = ucd_properties(c, cat);

	for (int ix = 0; punct_chars[ix] != 0; ++ix) {
		if (punct_chars[ix] == c)
			return punct_attributes[ix];
	}

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
		return CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_ELLIPSIS:
		return CLAUSE_SEMICOLON | CLAUSE_SPEAK_PUNCTUATION_NAME | CLAUSE_OPTIONAL_SPACE_AFTER;
	case ESPEAKNG_PROPERTY_PARAGRAPH_SEPARATOR:
		return CLAUSE_PARAGRAPH;
	}

	return CLAUSE_NONE;
}
