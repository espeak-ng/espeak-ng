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
	',', '.', '?', '!', ':', ';',

	0x00a1, // inverted exclamation
	0x00bf, // inverted question
	0x2013, // en-dash
	0x2014, // em-dash
	0x2026, // elipsis

	0x037e, // Greek question mark (looks like semicolon)
	0x0387, // Greek semicolon, ano teleia
	0x0964, // Devanagari Danda (fullstop)

	0x0589, // Armenian period
	0x055d, // Armenian comma
	0x055c, // Armenian exclamation
	0x055e, // Armenian question
	0x055b, // Armenian emphasis mark

	0x060c, // Arabic ,
	0x061b, // Arabic ;
	0x061f, // Arabic ?
	0x06d4, // Arabic .

	0x0df4, // Singhalese Kunddaliya
	0x0f0d, // Tibet Shad
	0x0f0e,

	0x1362, // Ethiopic period
	0x1363,
	0x1364,
	0x1365,
	0x1366,
	0x1367,
	0x1368,
	0x10fb, // Georgian paragraph

	0x3001, // ideograph comma
	0x3002, // ideograph period

	0xff01, // fullwidth exclamation
	0xff0c, // fullwidth comma
	0xff0e, // fullwidth period
	0xff1a, // fullwidth colon
	0xff1b, // fullwidth semicolon
	0xff1f, // fullwidth question mark

	0
};

// indexed by entry num. in punct_chars
static const unsigned int punct_attributes[] = {
	CLAUSE_COMMA,
	CLAUSE_PERIOD,
	CLAUSE_QUESTION,
	CLAUSE_EXCLAMATION,
	CLAUSE_COLON,
	CLAUSE_SEMICOLON,

	CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER,  // inverted exclamation
	CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER,  // inverted question
	CLAUSE_SEMICOLON,  // en-dash
	CLAUSE_SEMICOLON,  // em-dash
	CLAUSE_SEMICOLON | CLAUSE_SPEAK_PUNCTUATION_NAME | CLAUSE_OPTIONAL_SPACE_AFTER,  // elipsis

	CLAUSE_QUESTION,  // Greek question mark
	CLAUSE_SEMICOLON,  // Greek semicolon
	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,  // Devanagari Danda (fullstop)

	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,  // Armenian period
	CLAUSE_COMMA,  // Armenian comma
	CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD,  // Armenian exclamation
	CLAUSE_QUESTION | CLAUSE_PUNCTUATION_IN_WORD,  // Armenian question
	CLAUSE_PERIOD | CLAUSE_PUNCTUATION_IN_WORD,  // Armenian emphasis mark

	CLAUSE_COMMA,  // Arabic ,
	CLAUSE_SEMICOLON,  // Arabic ;
	CLAUSE_QUESTION,  // Arabic question mark
	CLAUSE_PERIOD,  // Arabic full stop

	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,  // Singhalese period
	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,  // Tibet period
	CLAUSE_PARAGRAPH,

	CLAUSE_PERIOD,  // Ethiopic period
	CLAUSE_COMMA,  // Ethiopic comma
	CLAUSE_SEMICOLON,  // Ethiopic semicolon
	CLAUSE_COLON,  // Ethiopic colon
	CLAUSE_COLON,  // Ethiopic preface colon
	CLAUSE_QUESTION,  // Ethiopic question mark
	CLAUSE_PARAGRAPH,  // Ethiopic paragraph
	CLAUSE_PARAGRAPH,  // Georgian paragraph

	CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER,  // ideograph comma
	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,  // ideograph period

	CLAUSE_EXCLAMATION | CLAUSE_OPTIONAL_SPACE_AFTER,  // fullwidth
	CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER,
	CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER,
	CLAUSE_COLON | CLAUSE_OPTIONAL_SPACE_AFTER,
	CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER,
	CLAUSE_QUESTION | CLAUSE_OPTIONAL_SPACE_AFTER,

	0
};

int clause_type_from_codepoint(uint32_t c)
{
	for (int ix = 0; punct_chars[ix] != 0; ++ix) {
		if (punct_chars[ix] == c)
			return punct_attributes[ix];
	}
	return CLAUSE_NONE;
}
