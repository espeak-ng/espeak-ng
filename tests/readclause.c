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
#include "test_assert.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/encoding.h>

#include "readclause.h"
#include "speech.h"
#include "phoneme.h"
#include "voice.h"
#include "synthesize.h"
#include "translate.h"

// Arguments to ReadClause. Declared here to avoid duplicating them across the
// different test functions.
static char source[N_TR_SOURCE+40]; // extra space for embedded command & voice change info at end
static short charix[N_TR_SOURCE+4];
static int charix_top = 0;
static int tone2;
static char voice_change_name[40];

static espeak_ng_STATUS
set_text(const char *text, const char *voicename)
{
	espeak_ng_STATUS status = espeak_ng_SetVoiceByName(voicename);
	if (status != ENS_OK)
		return status;

	if (p_decoder == NULL)
		p_decoder = create_text_decoder();

	count_characters = 0;
	return text_decoder_decode_string(p_decoder, text, -1, ESPEAKNG_ENCODING_UTF_8);
}

static void
test_latin()
{
	printf("testing Latin (Latn)\n");

	TEST_ASSERT(clause_type_from_codepoint('?') == CLAUSE_QUESTION);
	TEST_ASSERT(clause_type_from_codepoint('!') == CLAUSE_EXCLAMATION);
	TEST_ASSERT(clause_type_from_codepoint(',') == CLAUSE_COMMA);
	TEST_ASSERT(clause_type_from_codepoint(':') == CLAUSE_COLON);
	TEST_ASSERT(clause_type_from_codepoint(';') == CLAUSE_SEMICOLON);

	TEST_ASSERT(clause_type_from_codepoint(0x00A1) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0x00Bf) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));

	TEST_ASSERT(clause_type_from_codepoint(0x2013) == CLAUSE_SEMICOLON);
	TEST_ASSERT(clause_type_from_codepoint(0x2014) == CLAUSE_SEMICOLON);
	TEST_ASSERT(clause_type_from_codepoint(0x2026) == (CLAUSE_SEMICOLON | CLAUSE_SPEAK_PUNCTUATION_NAME | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_latin_sentence()
{
	printf("testing Latin (Latn) ... sentence\n");

	TEST_ASSERT(clause_type_from_codepoint('a') == CLAUSE_NONE);
	TEST_ASSERT(clause_type_from_codepoint('.') == CLAUSE_PERIOD);

	short retix[] = {
		0, 2, 3, 4, 5, 6, // Jane
		0, 8, 9, 10, 11, 12, 13, 14, 15, // finished
		0, 17, 18, // #1
		0, 20, 21, // in
		0, 23, 24, 25, // the
		0, 27, 28, 29, 30 }; // race

	TEST_ASSERT(set_text("Janet finished #1 in the race.", "en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == (CLAUSE_PERIOD | CLAUSE_DOT_AFTER_LAST_WORD));
	TEST_ASSERT(!strcmp(source, "Janet finished #1 in the race "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source, " "));
	TEST_ASSERT(charix_top == 0);
}

static void
test_greek()
{
	printf("testing Greek (Grek)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x037E) == CLAUSE_QUESTION);
	TEST_ASSERT(clause_type_from_codepoint(0x0387) == CLAUSE_SEMICOLON);
}

static void
test_armenian()
{
	printf("testing Armenian (Armn)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x055B) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	TEST_ASSERT(clause_type_from_codepoint(0x055C) == (CLAUSE_EXCLAMATION | CLAUSE_PUNCTUATION_IN_WORD));
	TEST_ASSERT(clause_type_from_codepoint(0x055D) == CLAUSE_COMMA);
	TEST_ASSERT(clause_type_from_codepoint(0x055E) == (CLAUSE_QUESTION | CLAUSE_PUNCTUATION_IN_WORD));
	TEST_ASSERT(clause_type_from_codepoint(0x0589) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_arabic()
{
	printf("testing Arabic (Arab)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x060C) == CLAUSE_COMMA);
	TEST_ASSERT(clause_type_from_codepoint(0x061B) == CLAUSE_SEMICOLON);
	TEST_ASSERT(clause_type_from_codepoint(0x061F) == CLAUSE_QUESTION);
	TEST_ASSERT(clause_type_from_codepoint(0x06D4) == CLAUSE_PERIOD);
}

static void
test_devanagari()
{
	printf("testing Devanagari (Deva)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x0964) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_tibetan()
{
	printf("testing Tibetan (Tibt)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x0F0D) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0x0F0E) == CLAUSE_PARAGRAPH);
}

static void
test_sinhala()
{
	printf("testing Sinhala (Sinh)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x0DF4) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_georgian()
{
	printf("testing Georgian (Geor)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x10FB) == CLAUSE_PARAGRAPH);
}

static void
test_ethiopic()
{
	printf("testing Ethiopic (Ethi)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x1362) == CLAUSE_PERIOD);
	TEST_ASSERT(clause_type_from_codepoint(0x1363) == CLAUSE_COMMA);
	TEST_ASSERT(clause_type_from_codepoint(0x1364) == CLAUSE_SEMICOLON);
	TEST_ASSERT(clause_type_from_codepoint(0x1365) == CLAUSE_COLON);
	TEST_ASSERT(clause_type_from_codepoint(0x1366) == CLAUSE_COLON);
	TEST_ASSERT(clause_type_from_codepoint(0x1367) == CLAUSE_QUESTION);
	TEST_ASSERT(clause_type_from_codepoint(0x1368) == CLAUSE_PARAGRAPH);
}

static void
test_ideographic()
{
	printf("testing Ideographic (Hani)\n");

	TEST_ASSERT(clause_type_from_codepoint(0x3001) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0x3002) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_fullwidth()
{
	printf("testing Full Width\n");

	TEST_ASSERT(clause_type_from_codepoint(0xFF01) == (CLAUSE_EXCLAMATION | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0xFF0C) == (CLAUSE_COMMA | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0xFF0E) == (CLAUSE_PERIOD | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0xFF1A) == (CLAUSE_COLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0xFF1B) == (CLAUSE_SEMICOLON | CLAUSE_OPTIONAL_SPACE_AFTER));
	TEST_ASSERT(clause_type_from_codepoint(0xFF1F) == (CLAUSE_QUESTION | CLAUSE_OPTIONAL_SPACE_AFTER));
}

static void
test_uts51_emoji_character()
{
	printf("testing Emoji ... UTS-51 ED-3. emoji character\n");

	short retix[] = {
		0, -1, -1,
		2, -1, -1,
		3, -1, -1,
		4, -1, -1, -1,
		5, -1, -1, -1,
		6 };

	TEST_ASSERT(set_text(
		"\xE2\x86\x94"      // [2194]  left right arrow
		"\xE2\x86\x95"      // [2195]  up down arrow
		"\xE2\x9B\x94"      // [26D5]  no entry
		"\xF0\x9F\x90\x8B"  // [1F40B] whale
		"\xF0\x9F\x90\xAC", // [1F42C] dolphin
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"\xE2\x86\x94"     // [2194]  left right arrow
		"\xE2\x86\x95"     // [2195]  up down arrow
		"\xE2\x9B\x94"     // [26D5]  no entry
		"\xF0\x9F\x90\x8B" // [1F40B] whale
		"\xF0\x9F\x90\xAC" // [1F42C] dolphin
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_text_presentation_sequence()
{
	printf("testing Emoji ... UTS-51 ED-8a. text presentation sequence\n");

	short retix[] = {
		0, 2, -1, -1,
		3, 4, -1, -1,
		5, -1, -1, 6, -1, -1,
		7, -1, -1, -1, 8, -1, -1,
		9 };

	TEST_ASSERT(set_text(
		"#\xEF\xB8\x8E"                 // [0023 FE0E]  number sign (text style)
		"4\xEF\xB8\x8E"                 // [0034 FE0E]  digit four (text style)
		"\xE2\x80\xBC\xEF\xB8\x8E"      // [203C FE0E]  double exclamation mark (text style)
		"\xF0\x9F\x97\x92\xEF\xB8\x8E", // [1F5D2 FE0E] spiral note pad (text style)
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"#\xEF\xB8\x8E"                // [0023 FE0E]  number sign (text style)
		"4\xEF\xB8\x8E"                // [0034 FE0E]  digit four (text style)
		"\xE2\x80\xBC\xEF\xB8\x8E"     // [203C FE0E]  double exclamation mark (text style)
		"\xF0\x9F\x97\x92\xEF\xB8\x8E" // [1F5D2 FE0E] spiral note pad (text style)
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_presentation_sequence()
{
	printf("testing Emoji ... UTS-51 ED-9a. emoji presentation sequence\n");

	short retix[] = {
		0, 2, -1, -1,
		3, 4, -1, -1,
		5, -1, -1, 6, -1, -1,
		7, -1, -1, -1, 8, -1, -1,
		9 };

	TEST_ASSERT(set_text(
		"#\xEF\xB8\x8F"                 // [0023 FE0F]  number sign (emoji style)
		"4\xEF\xB8\x8F"                 // [0034 FE0F]  digit four (emoji style)
		"\xE2\x80\xBC\xEF\xB8\x8F"      // [203C FE0F]  double exclamation mark (emoji style)
		"\xF0\x9F\x97\x92\xEF\xB8\x8F", // [1F5D2 FE0F] spiral note pad (emoji style)
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"#\xEF\xB8\x8F"                // [0023 FE0F]  number sign (emoji style)
		"4\xEF\xB8\x8F"                // [0034 FE0F]  digit four (emoji style)
		"\xE2\x80\xBC\xEF\xB8\x8F"     // [203C FE0F]  double exclamation mark (emoji style)
		"\xF0\x9F\x97\x92\xEF\xB8\x8F" // [1F5D2 FE0F] spiral note pad (emoji style)
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_modifier_sequence()
{
	printf("testing Emoji ... UTS-51 ED-13. emoji modifier sequence\n");

	short retix[] = {
		0, -1, -1, 2, -1, -1, -1,
		3, -1, -1, -1, 4, -1, -1, -1,
		5, -1, -1, -1, 6, -1, -1, -1,
		7 };

	TEST_ASSERT(set_text(
		"\xE2\x98\x9D\xF0\x9F\x8F\xBB"      // [261D 1F3FB]  index pointing up; light skin tone
		"\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBD"  // [1F5D2 1F3FD] bride with veil; medium skin tone
		"\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBF", // [1F4AA 1F3FF] flexed biceps; dark skin tone
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"\xE2\x98\x9D\xF0\x9F\x8F\xBB"     // [261D 1F3FB]  index pointing up; light skin tone
		"\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBD" // [1F5D2 1F3FD] bride with veil; medium skin tone
		"\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBF" // [1F4AA 1F3FF] flexed biceps; dark skin tone
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_flag_sequence()
{
	printf("testing Emoji ... UTS-51 ED-14. emoji flag sequence\n");

	short retix[] = {
		0, -1, -1, -1, 2, -1, -1, -1,
		3, -1, -1, -1, 4, -1, -1, -1,
		5, -1, -1, -1, 6, -1, -1, -1,
		7, -1, -1, -1, 8, -1, -1, -1,
		9 };

	TEST_ASSERT(set_text(
		"\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7"  // [1F1E6 1F1F7] AR (argentina)
		"\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC"  // [1F1E7 1F1EC] BG (bulgaria)
		"\xF0\x9F\x87\xAC\xF0\x9F\x87\xA8"  // [1F1EC 1F1E8] GC -- unknown country flag
		"\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1", // [1F1EC 1F1F1] GL (greenland)
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7" // [1F1E6 1F1F7] AR (argentina)
		"\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC" // [1F1E7 1F1EC] BG (bulgaria)
		"\xF0\x9F\x87\xAC\xF0\x9F\x87\xA8" // [1F1EC 1F1E8] GC -- unknown country flag
		"\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1" // [1F1EC 1F1F1] GL (greenland)
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_tag_sequence_emoji_character()
{
	printf("testing Emoji ... UTS-51 ED-14a. emoji tag sequence (emoji character)\n");

	short retix[] = {
		0, -1, -1, -1, // emoji character
		2, -1, -1, -1, 3, -1, -1, -1, 4, -1, -1, -1, 5, -1, -1, -1, 6, -1, -1, -1, // tag spec
		7, -1, -1, -1, // tag term
		8, -1, -1, -1, // emoji character
		9, -1, -1, -1, 10, -1, -1, -1, 11, -1, -1, -1, 12, -1, -1, -1, 13, -1, -1, -1, // tag spec
		14, -1, -1, -1, // tag term
		15, -1, -1, -1, // emoji character
		16, -1, -1, -1, 17, -1, -1, -1, 18, -1, -1, -1, 19, -1, -1, -1, // tag spec
		20, -1, -1, -1, // tag term
		21 };

	TEST_ASSERT(set_text(
		// tag_base = emoji_character (RGI sequence)
		"\xF0\x9F\x8F\xB4"  // [1F3F4] flag
		"\xF3\xA0\x81\xA7"  // [E0067] tag : g
		"\xF3\xA0\x81\xA2"  // [E0062] tag : b
		"\xF3\xA0\x81\xA5"  // [E0065] tag : e
		"\xF3\xA0\x81\xAE"  // [E006E] tag : n
		"\xF3\xA0\x81\xA7"  // [E006E] tag : g
		"\xF3\xA0\x81\xBF"  // [E007F] tag : (cancel)
		// tag_base = emoji_character (RGI sequence)
		"\xF0\x9F\x8F\xB4"  // [1F3F4] flag
		"\xF3\xA0\x81\xA7"  // [E0067] tag : g
		"\xF3\xA0\x81\xA2"  // [E0062] tag : b
		"\xF3\xA0\x81\xB3"  // [E0065] tag : s
		"\xF3\xA0\x81\xA3"  // [E006E] tag : c
		"\xF3\xA0\x81\xB4"  // [E006E] tag : t
		"\xF3\xA0\x81\xBF"  // [E007F] tag : (cancel)
		// tag_base = emoji_character (non-RGI sequence)
		"\xF0\x9F\x8F\xB4"  // [1F3F4] flag
		"\xF3\xA0\x81\xB5"  // [E0067] tag : u
		"\xF3\xA0\x81\xB3"  // [E0062] tag : s
		"\xF3\xA0\x81\xA3"  // [E0065] tag : c
		"\xF3\xA0\x81\xA1"  // [E006E] tag : a
		"\xF3\xA0\x81\xBF", // [E007F] tag : (cancel)
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		// tag_base = emoji_character (RGI sequence)
		"\xF0\x9F\x8F\xB4" // [1F3F4] flag
		"\xF3\xA0\x81\xA7" // [E0067] tag : g
		"\xF3\xA0\x81\xA2" // [E0062] tag : b
		"\xF3\xA0\x81\xA5" // [E0065] tag : e
		"\xF3\xA0\x81\xAE" // [E006E] tag : n
		"\xF3\xA0\x81\xA7" // [E006E] tag : g
		"\xF3\xA0\x81\xBF" // [E007F] tag : (cancel)
		// tag_base = emoji_character (RGI sequence)
		"\xF0\x9F\x8F\xB4" // [1F3F4] flag
		"\xF3\xA0\x81\xA7" // [E0067] tag : g
		"\xF3\xA0\x81\xA2" // [E0062] tag : b
		"\xF3\xA0\x81\xB3" // [E0065] tag : s
		"\xF3\xA0\x81\xA3" // [E006E] tag : c
		"\xF3\xA0\x81\xB4" // [E006E] tag : t
		"\xF3\xA0\x81\xBF" // [E007F] tag : (cancel)
		// tag_base = emoji_character (non-RGI sequence)
		"\xF0\x9F\x8F\xB4" // [1F3F4] flag
		"\xF3\xA0\x81\xB5" // [E0067] tag : u
		"\xF3\xA0\x81\xB3" // [E0062] tag : s
		"\xF3\xA0\x81\xA3" // [E0065] tag : c
		"\xF3\xA0\x81\xA1" // [E006E] tag : a
		"\xF3\xA0\x81\xBF" // [E007F] tag : (cancel)
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_combining_sequence()
{
	printf("testing Emoji ... UTS-51 ED-14b. emoji combining sequence\n");

	short retix[] = {
		0, -1, -1, 2, -1, -1,            // emoji character
		3, -1, -1, 4, -1, -1, 5, -1, -1, // text presentation sequence
		6, -1, -1, 7, -1, -1, 8, -1, -1, // emoji presentation sequence
		9 };

	TEST_ASSERT(set_text(
		"\xE2\x86\x95\xE2\x83\x9E"              // [2195 20DE]      up down arrow; Me (enclosing square)
		"\xE2\x86\x95\xEF\xB8\x8E\xE2\x83\x9E"  // [2195 FE0E 20DE] up down arrow; Me (enclosing square)
		"\xE2\x86\x95\xEF\xB8\x8F\xE2\x83\x9E", // [2195 FE0F 20DE] up down arrow; Me (enclosing square)
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"\xE2\x86\x95\xE2\x83\x9E"             // [2195 20DE]      up down arrow; Me (enclosing square)
		"\xE2\x86\x95\xEF\xB8\x8E\xE2\x83\x9E" // [2195 FE0E 20DE] up down arrow; Me (enclosing square)
		"\xE2\x86\x95\xEF\xB8\x8F\xE2\x83\x9E" // [2195 FE0F 20DE] up down arrow; Me (enclosing square)
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

static void
test_uts51_emoji_keycap_sequence()
{
	printf("testing Emoji ... UTS-51 ED-14c. emoji keycap sequence\n");

	short retix[] = {
		0, 2, -1, -1, 3, -1, -1,
		4, 5, -1, -1, 6, -1, -1,
		7, 8, -1, -1, 9, -1, -1,
		10 };

	TEST_ASSERT(set_text(
		"5\xEF\xB8\x8E\xE2\x83\xA3"  // [0035 FE0E 20E3] keycap 5
		"#\xEF\xB8\x8E\xE2\x83\xA3"  // [0023 FE0E 20E3] keycap #
		"*\xEF\xB8\x8E\xE2\x83\xA3", // [002A FE0E 20E3] keycap *
		"en") == ENS_OK);

	charix_top = 0;
	TEST_ASSERT(ReadClause(translator, source, charix, &charix_top, N_TR_SOURCE, &tone2, voice_change_name) == CLAUSE_EOF);
	TEST_ASSERT(!strcmp(source,
		"5\xEF\xB8\x8E\xE2\x83\xA3" // [0035 FE0E 20E3] keycap 5
		"#\xEF\xB8\x8E\xE2\x83\xA3" // [0023 FE0E 20E3] keycap #
		"*\xEF\xB8\x8E\xE2\x83\xA3" // [002A FE0E 20E3] keycap *
		" "));
	TEST_ASSERT(charix_top == (sizeof(retix)/sizeof(retix[0])) - 1);
	TEST_ASSERT(!memcmp(charix, retix, sizeof(retix)));
	TEST_ASSERT(tone2 == 0);
	TEST_ASSERT(voice_change_name[0] == 0);
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, espeakINITIALIZE_DONT_EXIT) == 22050);

	test_latin();
	test_latin_sentence();

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

	test_uts51_emoji_character();
	test_uts51_text_presentation_sequence();
	test_uts51_emoji_presentation_sequence();
	test_uts51_emoji_modifier_sequence();
	test_uts51_emoji_flag_sequence();
	test_uts51_emoji_tag_sequence_emoji_character();
	test_uts51_emoji_combining_sequence();
	test_uts51_emoji_keycap_sequence();

	TEST_ASSERT(espeak_Terminate() == EE_OK);

	return EXIT_SUCCESS;
}

// References:
//    [UTS-51]     Unicode Emoji (http://www.unicode.org/reports/tr51/tr51-12.html) 5.0-12. 2017-05-18
