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

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

// Arguments to ReadClause. Declared here to avoid duplicating them across the
// different test functions.
static char source[N_TR_SOURCE+40]; // extra space for embedded command & voice change info at end
static short charix[N_TR_SOURCE+4];
static int charix_top = 0;
static int tone2;
static char voice_change_name[40];
static int terminator;

static espeak_ng_STATUS
set_text(const char *text, const char *voicename)
{
	espeak_ng_STATUS status = espeak_ng_SetVoiceByName(voicename);
	if (status != ENS_OK)
		return status;

	if (p_decoder == NULL)
		p_decoder = create_text_decoder();

	return text_decoder_decode_string(p_decoder, text, -1, ESPEAKNG_ENCODING_UTF_8);
}

void
test_latin()
{
	printf("testing Latin (Latn)\n");

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
	printf("testing Greek (Grek)\n");

	assert(clause_type_from_codepoint(0x037E) == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint(0x0387) == CLAUSE_SEMICOLON);
}

void
test_armenian()
{
	printf("testing Armenian (Armn)\n");

	assert(clause_type_from_codepoint(0x055B) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x055C) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x055D) == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(0x055E) == (CLAUSE_QUESTION | CLAUSE_PUNCTUATION_IN_WORD));
	assert(clause_type_from_codepoint(0x0589) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_arabic()
{
	printf("testing Arabic (Arab)\n");

	assert(clause_type_from_codepoint(0x060C) == CLAUSE_COMMA);
	assert(clause_type_from_codepoint(0x061B) == CLAUSE_SEMICOLON);
	assert(clause_type_from_codepoint(0x061F) == CLAUSE_QUESTION);
	assert(clause_type_from_codepoint(0x06D4) == CLAUSE_PERIOD);
}

void
test_devanagari()
{
	printf("testing Devanagari (Deva)\n");

	assert(clause_type_from_codepoint(0x0964) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_tibetan()
{
	printf("testing Tibetan (Tibt)\n");

	assert(clause_type_from_codepoint(0x0F0D) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0x0F0E) == CLAUSE_PARAGRAPH);
}

void
test_sinhala()
{
	printf("testing Sinhala (Sinh)\n");

	assert(clause_type_from_codepoint(0x0DF4) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_georgian()
{
	printf("testing Georgian (Geor)\n");

	assert(clause_type_from_codepoint(0x10FB) == CLAUSE_PARAGRAPH);
}

void
test_ethiopic()
{
	printf("testing Ethiopic (Ethi)\n");

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
	printf("testing Ideographic (Hani)\n");

	assert(clause_type_from_codepoint(0x3001) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0x3002) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_fullwidth()
{
	printf("testing Full Width\n");

	assert(clause_type_from_codepoint(0xFF01) == (CLAUSE_EXCLAMATION | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF0C) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF0E) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1A) == (CLAUSE_COLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1B) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	assert(clause_type_from_codepoint(0xFF1F) == (CLAUSE_QUESTION | CLAUSE_OPTIONAL_SPACE_AFTER));
}

void
test_emoji_single_character_sequences()
{
	short retix[] = {
		0, -1, -1,
		2, -1, -1,
		3, -1, -1,
		4, -1, -1, -1,
		5, -1, -1, -1,
		6,
		0 };

	assert(set_text(
		"\xE2\x86\x94"      // [2194]  left right arrow
		"\xE2\x86\x95"      // [2195]  up down arrow
		"\xE2\x9B\x94"      // [26D5]  no entry
		"\xF0\x9F\x90\x8B"  // [1F40B] whale
		"\xF0\x9F\x90\xAC", // [1F42C] dolphin
		"en") == ENS_OK);

	assert(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	assert(!strcmp(source,
		"\xE2\x86\x94"     // [2194]  left right arrow
		"\xE2\x86\x95"     // [2195]  up down arrow
		"\xE2\x9B\x94"     // [26D5]  no entry
		"\xF0\x9F\x90\x8B" // [1F40B] whale
		"\xF0\x9F\x90\xAC" // [1F42C] dolphin
		" "));
	assert(charix_top == (sizeof(retix)/sizeof(retix[0])) - 2);
	assert(!memcmp(charix, retix, sizeof(retix)));
	assert(tone2 == 0);
	assert(voice_change_name[0] == 0);
}

void
test_emoji()
{
	printf("testing Emoji\n");

	test_emoji_single_character_sequences();
}

int
main(int argc, char **argv)
{
	assert(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, espeakINITIALIZE_DONT_EXIT) == 22050);

	test_latin();
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

	test_emoji();

	assert(espeak_Terminate() == EE_OK);

	return EXIT_SUCCESS;
}
