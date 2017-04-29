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

#include "encoding.h"
#include "tokenizer.h"
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
test_latin_common()
{
	printf("testing Latin/Common (Latn/Zyyy) script classification\n");

	assert(clause_type_from_codepoint('a') == CLAUSE_NONE);
	assert(clause_type_from_codepoint('.') == CLAUSE_PERIOD);
	assert(clause_type_from_codepoint('?') == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint('!') == CLAUSE_EXCLAMATION);
	assert(clause_type_from_codepoint(',') == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(':') == CLAUSE_COLON);
	assert(clause_type_from_codepoint(';') == CLAUSE_SEMICOLON);

	assert(clause_type_from_codepoint(0x00A1) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0x00Bf) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));

	assert(clause_type_from_codepoint(0x2013) == CLAUSE_SEMICOLON);
	assert(clause_type_from_codepoint(0x2014) == CLAUSE_SEMICOLON);
	assert(clause_type_from_codepoint(0x2026) == (CLAUSE_SEMICOLON | CLAUSE_SPEAK_PUNCTUATION_NAME | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_greek()
{
	printf("testing Greek (Grek) script classification\n");

	assert(clause_type_from_codepoint(0x037E) == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint(0x0387) == CLAUSE_SEMICOLON);
}

void
test_armenian()
{
	printf("testing Armenian (Armn) script classification\n");

	assert(clause_type_from_codepoint(0x055B) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x055C) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x055D) == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(0x055E) == (CLAUSE_QUESTION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x0589) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_arabic()
{
	printf("testing Arabic (Arab) script classification\n");

	assert(clause_type_from_codepoint(0x060C) == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(0x061B) == CLAUSE_SEMICOLON);
	assert(clause_type_from_codepoint(0x061F) == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint(0x06D4) == CLAUSE_PERIOD);
}

void
test_devanagari()
{
	printf("testing Devanagari (Deva) script classification\n");

	assert(clause_type_from_codepoint(0x0964) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_tibetan()
{
	printf("testing Tibetan (Tibt) script classification\n");

	assert(clause_type_from_codepoint(0x0F0D) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0x0F0E) == CLAUSE_PARAGRAPH);
}

void
test_sinhala()
{
	printf("testing Sinhala (Sinh) script classification\n");

	assert(clause_type_from_codepoint(0x0DF4) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_georgian()
{
	printf("testing Georgian (Geor) script classification\n");

	assert(clause_type_from_codepoint(0x10FB) == CLAUSE_PARAGRAPH);
}

void
test_ethiopic()
{
	printf("testing Ethiopic (Ethi) script classification\n");

	assert(clause_type_from_codepoint(0x1362) == CLAUSE_PERIOD);
	assert(clause_type_from_codepoint(0x1363) == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(0x1364) == CLAUSE_SEMICOLON);
	assert(clause_type_from_codepoint(0x1365) == CLAUSE_COLON);
	assert(clause_type_from_codepoint(0x1366) == CLAUSE_COLON);
	assert(clause_type_from_codepoint(0x1367) == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint(0x1368) == CLAUSE_PARAGRAPH);
}

void
test_ideographic()
{
	printf("testing Ideographic (Hani) script classification\n");

	assert(clause_type_from_codepoint(0x3001) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0x3002) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_fullwidth()
{
	printf("testing Full Width/Common (Zyyy) script classification\n");

	assert(clause_type_from_codepoint(0xFF01) == (CLAUSE_EXCLAMATION | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF0C) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF0E) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1A) == (CLAUSE_COLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1B) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1F) == (CLAUSE_QUESTION | CLAUSE_OPTIONAL_SPACE_AFTER));
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

	// U+000C : FORM FEED (FF) -- Used as a page (not paragraph) break.
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\x0C") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\x0C") == 0);

	// U+0085 : NEXT LINE (NEL) -- Used in EBCDIC systems as a combined CR+LF character.
	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\x85") == 0);

	assert(tokenizer_read_next_token(tokenizer) == ESPEAKNG_TOKEN_NEWLINE);
	assert(tokenizer_get_token_text(tokenizer) != NULL);
	assert(strcmp(tokenizer_get_token_text(tokenizer), "\xC2\x85") == 0);

	// General Category: Zl -- LINE SEPARATOR
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

	// General Category: Zp -- PARAGRAPH SEPARATOR
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

	// General Category: Zs, Property: White_Space, Decomposition: <noBreak>
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
run_tests()
{
	test_latin_common();
	test_greek();
	test_armenian();
	test_arabic();
	test_devanagari();
	test_tibetan();
	test_sinhala();
	test_georgian();
	test_ethiopic();
	test_ideographic();
	test_fullwidth();

	test_unbound_tokenizer();

	test_linux_newline_tokens();
	test_mac_newline_tokens();
	test_windows_newline_tokens();
	test_unicode_newline_tokens();
	test_paragraph_tokens();
	test_whitespace_tokens();

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
		printf("unknown    : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_NEWLINE:
		printf("newline    : ");
		escape_newline(tokenizer_get_token_text(tokenizer));
		putc('\n', stdout);
		break;
	case ESPEAKNG_TOKEN_PARAGRAPH:
		printf("paragraph  : %s\n", tokenizer_get_token_text(tokenizer));
		break;
	case ESPEAKNG_TOKEN_WHITESPACE:
		printf("whitespace : %s\n", tokenizer_get_token_text(tokenizer));
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
