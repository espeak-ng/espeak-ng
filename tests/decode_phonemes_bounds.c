/*
 * Copyright (C) 2026
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

#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "phoneme.h"
#include "speech.h"
#include "translate.h"

static PHONEME_TAB ph_a;
static PHONEME_TAB ph_long;

static void setup_phonemes(void)
{
	memset(&ph_a, 0, sizeof(ph_a));
	memset(&ph_long, 0, sizeof(ph_long));
	ph_a.mnemonic = 'a';
	ph_long.mnemonic = 'b' | ('c' << 8) | ('d' << 16) | ('e' << 24);
	phoneme_tab[1] = &ph_a;
	phoneme_tab[2] = &ph_long;
}

static void test_fits(void)
{
	char out[16];
	const char in[] = { 1, 2, 0 };
	DecodePhonemes(in, out, sizeof(out));
	TEST_ASSERT(!strcmp(out, "abcde"));
}

static void test_truncates_and_terminates(void)
{
	char out[6];
	const char in[] = { 1, 2, 1, 2, 0 };
	memset(out, 'X', sizeof(out));
	DecodePhonemes(in, out, sizeof(out));
	TEST_ASSERT(!strcmp(out, "abcde"));
}

static void test_exact_fit(void)
{
	char out[6];
	const char in[] = { 1, 2, 0 };
	DecodePhonemes(in, out, sizeof(out));
	TEST_ASSERT(!strcmp(out, "abcde"));
}

static void test_tiny_buffers(void)
{
	char out[3];
	const char in[] = { 1, 2, 0 };

	memset(out, 'X', sizeof(out));
	DecodePhonemes(in, out, 1);
	TEST_ASSERT(out[0] == 0 && out[1] == 'X');

	memset(out, 'X', sizeof(out));
	DecodePhonemes(in, out, 2);
	TEST_ASSERT(!strcmp(out, "a"));

	memset(out, 'X', sizeof(out));
	DecodePhonemes(in, out, 0);
	TEST_ASSERT(out[0] == 'X');
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	setup_phonemes();
	test_fits();
	test_truncates_and_terminates();
	test_exact_fit();
	test_tiny_buffers();
	return EXIT_SUCCESS;
}
