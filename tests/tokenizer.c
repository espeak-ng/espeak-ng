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
 * along with this program; if not, write see:
 *             <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/encoding.h>
#include <espeak-ng/tokenizer.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

// TODO: Find a better place for this than speech.c, so it can be implemented
// in one place without having to include all of speech.c.
int GetFileLength(const char *filename)
{
	struct stat statbuf;

	if (stat(filename, &statbuf) != 0)
		return -errno;

	if (S_ISDIR(statbuf.st_mode))
		return -EISDIR;

	return statbuf.st_size;
}

void
test_unbound_tokenizer()
{
	printf("testing unbound tokenizer\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	assert(tokenizer != NULL);

	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	assert(tokenizer_reset(tokenizer, NULL, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_tokenizer(tokenizer);
}

void
test_linux_newline_tokens()
{
	printf("testing linux newline tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\n\n", -1, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\n") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\n") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_mac_newline_tokens()
{
	printf("testing mac newline tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\r\r", -1, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\r") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\r") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_windows_newline_tokens()
{
	printf("testing windows newline tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\r\n\r\n", -1, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\r\n") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\r\n") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_unicode_newline_tokens()
{
	printf("testing unicode newline tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\x0C\x0C\xC2\x85\xC2\x85\xE2\x80\xA8\xE2\x80\xA8", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	// FORM FEED (FF) -- Used as a page (not paragraph) break.
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\x0C") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\x0C") == 0);

	// NEXT LINE (NEL) [U+0085] -- Used in EBCDIC systems as a combined CR+LF character.
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\x85") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\x85") == 0);

	// General Category: Zl -- LINE SEPARATOR [U+2028]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xE2\x80\xA8") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xE2\x80\xA8") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_paragraph_tokens()
{
	printf("testing paragraph tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\xE2\x80\xA9\xE2\x80\xA9", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	// General Category: Zp, PARAGRAPH SEPARATOR [U+2029]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PARAGRAPH);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xE2\x80\xA9") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PARAGRAPH);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xE2\x80\xA9") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_whitespace_tokens()
{
	printf("testing whitespace tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\t\t\n\x0B\x0B\n \xE3\x80\x80 \n\xC2\xA0\xC2\xA0", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	// General Category: Cc, Property: White_Space
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\t\t") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\n") == 0);

	// General Category: Cc, Property: White_Space, VERTICAL TAB (VT) -- Not treated as newline tokens.
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\x0B\x0B") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\n") == 0);

	// General Category: Zs, Property: White_Space
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " \xE3\x80\x80 ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\n") == 0);

	// General Category: Zs, Property: White_Space, Decomposition: <noBreak>, NO-BREAK SPACE [U+00A0]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\xA0\xC2\xA0") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_Latn_word_tokens()
{
	printf("testing Latin (Latn) script word tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "One one ONE OneTwo ONETwo", -1, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WORD_CAPITALIZED);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "One") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WORD_LOWERCASE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "one") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WORD_UPPERCASE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "ONE") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WORD_MIXEDCASE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "OneTwo") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WORD_MIXEDCASE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "ONETwo") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_Latn_punctuation_tokens()
{
	printf("testing Latin (Latn) script punctuation tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, ". ? .. ! ... , .... : ; \xE2\x80\xA6", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_FULL_STOP);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ".") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_QUESTION_MARK);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "?") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_FULL_STOP);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ".") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_FULL_STOP);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ".") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_EXCLAMATION_MARK);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "!") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_ELLIPSIS);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "...") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_COMMA);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ",") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_ELLIPSIS);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "...") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_FULL_STOP);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ".") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_COLON);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ":") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_SEMICOLON);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ";") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// HORIZONTAL ELLIPSIS [U+2026]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_ELLIPSIS);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xE2\x80\xA6") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_Latn_general_punctuation_tokens()
{
	printf("testing Latin (Latn) script general punctuation tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "\" () - _ \xC2\xAB\xC2\xBB", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	// General Category: Po
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\"") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Ps
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "(") == 0);

	// General Category: Pe
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), ")") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Pd
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "-") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Pc
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "_") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Pi, LEFT-POINTING DOUBLE ANGLE QUOTATION MARK [U+00AB]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\xAB") == 0);

	// General Category: Pf, RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK [U+00BB]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_PUNCTUATION);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\xBB") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
test_Latn_symbol_tokens()
{
	printf("testing Latin (Latn) script symbol tokens\n");

	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "$ ^ + \xC2\xA9", -1, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	assert(tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT) == 1);

	// General Category: Sc
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_SYMBOL);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "$") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Sk
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_SYMBOL);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "^") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: Sm
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_SYMBOL);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "+") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_WHITESPACE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), " ") == 0);

	// General Category: So, COPYRIGHT SIGN [U+00A9]
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_SYMBOL);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\xA9") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_END_OF_BUFFER);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(*tokenizer_get_token_text(tokenizer) == '\0');

	destroy_text_decoder(decoder);
	destroy_tokenizer(tokenizer);
}

void
run_tests()
{
	test_unbound_tokenizer();

	test_linux_newline_tokens();
	test_mac_newline_tokens();
	test_windows_newline_tokens();
	test_unicode_newline_tokens();
	test_paragraph_tokens();
	test_whitespace_tokens();

	test_Latn_word_tokens();
	test_Latn_punctuation_tokens();
	test_Latn_general_punctuation_tokens();
	test_Latn_symbol_tokens();

	printf("done\n");
}

void
escape_newline(const char *s)
{
	for ( ; *s; ++s) switch (*s)
	{
	case '\r': printf("\\r"); break;
	case '\n': printf("\\n"); break;
	default:   putc(*s, stdout); break;
	}
}

void
print_tokens(espeak_ng_TEXT_DECODER *decoder)
{
	espeak_ng_TOKENIZER *tokenizer = create_tokenizer();
	if (!tokenizer_reset(tokenizer, decoder, ESPEAKNG_TOKENIZER_OPTION_TEXT)) {
		destroy_tokenizer(tokenizer);
		return;
	}

	while (1) switch (tokenizer_read_next_token(tokenizer))
	{
	case ESPEAKNG_TOKEN_END_OF_BUFFER:
		destroy_tokenizer(tokenizer);
		return;
	case ESPEAKNG_TOKEN_UNKNOWN:
		printf("unknown            : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_NEWLINE:
		printf("newline            : ");
		escape_newline(tokenizer_get_token_text(tokenizer));
		putc('\n', stdout);
		break;
	case ESPEAKNG_TOKEN_PARAGRAPH:
		printf("paragraph          : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WHITESPACE:
		printf("whitespace         : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WORD_UPPERCASE:
		printf("word (upper case)  : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WORD_LOWERCASE:
		printf("word (lower case)  : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WORD_MIXEDCASE:
		printf("word (mixed case)  : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WORD_CAPITALIZED:
		printf("word (capitalized) : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_FULL_STOP:
		printf("full stop          : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_QUESTION_MARK:
		printf("question mark      : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_EXCLAMATION_MARK:
		printf("exclamation mark   : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_COMMA:
		printf("comma              : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_COLON:
		printf("colon              : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_SEMICOLON:
		printf("semicolon          : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	}
}

void
print_tokens_from_file(const char *filename, const char *encoding_name)
{
	espeak_ng_ENCODING encoding = espeak_ng_EncodingFromName(encoding_name);
	if (encoding == ESPEAKNG_ENCODING_UNKNOWN) {
		printf("Unknown encoding \"%s\".\n", encoding_name);
		return;
	}

	int length = GetFileLength(filename);
	FILE *f = (length > 0) ? fopen(filename, "rb") : NULL;
	if (!f) {
		printf("Cannot open file: %s\n", filename);
		return;
	}

	char *buffer = malloc(length);
	if (!buffer) {
		fclose(f);
		printf("Out of memory!\n");
		return;
	}

	fread(buffer, 1, length, f);
	fclose(f);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();
	if (text_decoder_decode_string(decoder, buffer, length, encoding) == ENS_OK)
		print_tokens(decoder);

	destroy_text_decoder(decoder);
}

void
usage(const char *program)
{
	printf("%s -- Run the tokenizer tests.\n", program);
	printf("%s ENCODING FILENAME -- Print the tokens for FILENAME.\n", program);
}

int
main(int argc, char **argv)
{
	switch (argc)
	{
	case 1:  run_tests(); break;
	case 3:  print_tokens_from_file(argv[2], argv[1]); break;
	default: usage(argv[0]); return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
