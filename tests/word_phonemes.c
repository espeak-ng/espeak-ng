/*
 * Regression test for espeak_TextToWordPhonemePairsWithTerminator().
 *
 * Copyright (C) 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "test_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

/* The test text exercises: number expansion ("2.5"), dotted tokens (".NET"),
 * contractions ("Don't", "we're", "doesn't"), hyphen+apostrophe
 * ("espeak-ng's"), a multi-word dictionary pronunciation ("I will"), and
 * clauses broken by , ! ; and a period before an uppercase letter. */
static const char test_text[] =
	"Version 2.5 uses .NET. Don't stop, and we're testing espeak-ng's "
	"features! I will record it; Play the record. doesn't it interesting?";

typedef struct {
	int word_position;   /* expected espeak_word_phoneme_pair.word_position */
	int word_length;     /* expected espeak_word_phoneme_pair.word_length */
	const char *word;    /* expected espeak_word_phoneme_pair.word */
	const char *phonemes;/* expected espeak_word_phoneme_pair.phonemes */
} expected_pair;

typedef struct {
	const char *clause_phonemes;  /* expected pairs->clause_phonemes */
	int n_pairs;                  /* expected pairs->size_pairs */
	const expected_pair *pairs;
} expected_clause;

/* Golden output of espeak_TextToWordPhonemePairsWithTerminator() (IPA,
 * en-us) for test_text.  If upstream dictionary/voice data legitimately
 * changes a pronunciation, regenerate this table; a change here means the
 * API output changed. */
static const expected_pair clause0_pairs[] = {
	{ 0, 7, "Version", "vˈɜːʒən" },
	{ 8, 3, "2.5", "tˈuː pɔɪnt fˈaɪv" },
	{ 12, 4, "uses", "jˈuːsᵻz" },
	{ 17, 4, ".NET", "dˈɑːt ˌɛnˌiːtˈiː" },
};
static const expected_pair clause1_pairs[] = {
	{ 23, 5, "Don't", "dˈoʊnt" },
	{ 29, 4, "stop", "stˈɑːp" },
};
static const expected_pair clause2_pairs[] = {
	{ 35, 3, "and", "ænd" },
	{ 39, 5, "we're", "wɪɹ" },
	{ 45, 7, "testing", "tˈɛstɪŋ" },
	{ 53, 11, "espeak-ng's", "ˈiːspiːkˌɛndʒˈiːz" },
	{ 65, 8, "features", "fˈiːtʃɚz" },
};
static const expected_pair clause3_pairs[] = {
	{ 75, 6, "I will", "aɪ wɪl" },
	{ 82, 6, "record", "ɹᵻkˈɔːɹd" },
	{ 89, 2, "it", "ɪt" },
};
static const expected_pair clause4_pairs[] = {
	{ 93, 4, "Play", "plˈeɪ" },
	{ 98, 3, "the", "ðə" },
	{ 102, 6, "record", "ɹˈɛkɚd" },
	{ 110, 7, "doesn't", "dˈʌzənt" },
	{ 118, 2, "it", "ɪɾ" },
	{ 121, 11, "interesting", "ˈɪntɹɛstɪŋ" },
};
static const expected_clause expected_clauses[] = {
	{ "vˈɜːʒən tˈuː pɔɪnt fˈaɪv jˈuːsᵻz dˈɑːt ˌɛnˌiːtˈiː", 4, clause0_pairs },
	{ "dˈoʊnt stˈɑːp", 2, clause1_pairs },
	{ "ænd wɪɹ tˈɛstɪŋ ˈiːspiːkˌɛndʒˈiːz fˈiːtʃɚz", 5, clause2_pairs },
	{ "aɪ wɪl ɹᵻkˈɔːɹd ɪt", 3, clause3_pairs },
	{ "plˈeɪ ðə ɹˈɛkɚd dˈʌzənt ɪɾ ˈɪntɹɛstɪŋ", 6, clause4_pairs },
};
static const int n_expected_clauses = 5;

/* Advance `n` characters (not bytes) through a UTF-8 string. */
static void
utf8_advance(const char **text, int n)
{
	while (n-- > 0) {
		(*text)++;
		while ((**text & 0xc0) == 0x80)
			(*text)++;
	}
}

/* Copy the characters [position, position+length) of text into out. */
static void
slice_text(const char *text, int position, int length, char *out)
{
	const char *p = text;
	utf8_advance(&p, position);
	{
		const char *end = p;
		size_t n;
		utf8_advance(&end, length);
		n = (size_t)(end - p);
		memcpy(out, p, n);
		out[n] = 0;
	}
}

static void
test_word_phoneme_pairs()
{
	const void *textptr = test_text;
	int n_clauses = 0;

	printf("testing espeak_TextToWordPhonemePairsWithTerminator\n");

	for (;;) {
		int terminator = -1;
		espeak_word_phoneme_pairs *pairs;
		const expected_clause *expected;
		char joined[4096] = "";
		int i;

		pairs = espeak_TextToWordPhonemePairsWithTerminator(
			&textptr, espeakCHARS_UTF8, espeakPHONEMES_IPA, &terminator);
		if (pairs == NULL)
			break;
		TEST_ASSERT(n_clauses < n_expected_clauses);
		expected = &expected_clauses[n_clauses];

		/* The flat clause string must match the golden value. */
		TEST_ASSERT(strcmp(pairs->clause_phonemes, expected->clause_phonemes) == 0);
		TEST_ASSERT(pairs->size_pairs == expected->n_pairs);

		for (i = 0; i < pairs->size_pairs; i++) {
			espeak_word_phoneme_pair *pair = &pairs->pairs[i];
			char slice[128];

			TEST_ASSERT(pair->word_position == expected->pairs[i].word_position);
			TEST_ASSERT(pair->word_length == expected->pairs[i].word_length);
			TEST_ASSERT(strcmp(pair->word, expected->pairs[i].word) == 0);
			TEST_ASSERT(strcmp(pair->phonemes, expected->pairs[i].phonemes) == 0);

			/* The word copy must equal the span sliced out of the original
			 * text, and the phonemes must have no leading/trailing space. */
			slice_text(test_text, pair->word_position, pair->word_length, slice);
			TEST_ASSERT(strcmp(slice, pair->word) == 0);
			TEST_ASSERT(pair->phonemes[0] != ' ');
			TEST_ASSERT(pair->phonemes[strlen(pair->phonemes) - 1] != ' ');

			/* Join invariant: joining all pairs' phonemes with single
			 * spaces reproduces the flat clause string. */
			if (i > 0)
				strcat(joined, " ");
			strcat(joined, pair->phonemes);
		}
		TEST_ASSERT(strcmp(joined, pairs->clause_phonemes) == 0);

		espeak_FreeWordPhonemePairs(pairs);
		n_clauses++;
		if (textptr == NULL)
			break;
	}

	TEST_ASSERT(n_clauses == n_expected_clauses);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	/* The data path comes from the ESPEAK_DATA_PATH environment variable,
	 * which CTest sets (see cmake/data.cmake). */
	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0) > 0);
	TEST_ASSERT(espeak_SetVoiceByName("en-us") == EE_OK);

	test_word_phoneme_pairs();

	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return EXIT_SUCCESS;
}
