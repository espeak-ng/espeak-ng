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
static PHONEME_TAB ph_switch;

static void setup_phonemes(void)
{
	memset(&ph_a, 0, sizeof(ph_a));
	memset(&ph_switch, 0, sizeof(ph_switch));
	ph_a.mnemonic = 'a';
	ph_a.code = 1;
	ph_a.type = phVOWEL;
	ph_switch.mnemonic = '_' | ('^' << 8) | ('_' << 16);
	ph_switch.code = phonSWITCH;
	phoneme_tab[1] = &ph_a;
	phoneme_tab[phonSWITCH] = &ph_switch;
	n_phoneme_tab = phonSWITCH + 1;
}

static void test_fits(void)
{
	char out[8];
	EncodePhonemes("aaa", out, sizeof(out), NULL);
	TEST_ASSERT(!strcmp(out, "\x01\x01\x01"));
}

// [[...]] input can hold far more phonemes than word_phonemes[N_WORD_PHONEMES]
static void test_long_word_truncates(void)
{
	struct {
		char out[16];
		char guard[16];
	} buf;
	char in[301];

	memset(in, 'a', sizeof(in) - 1);
	in[sizeof(in) - 1] = 0;
	memset(&buf, 'X', sizeof(buf));
	EncodePhonemes(in, buf.out, sizeof(buf.out), NULL);
	TEST_ASSERT(strlen(buf.out) == sizeof(buf.out) - 1);
	for (size_t i = 0; i < sizeof(buf.guard); i++)
		TEST_ASSERT(buf.guard[i] == 'X');
}

static void test_long_language_name_truncates(void)
{
	struct {
		char out[8];
		char guard[16];
	} buf;

	memset(&buf, 'X', sizeof(buf));
	EncodePhonemes("_^_abcdefghijklmnop", buf.out, sizeof(buf.out), NULL);
	TEST_ASSERT(buf.out[0] == phonSWITCH);
	TEST_ASSERT(strlen(buf.out) == sizeof(buf.out) - 1);
	for (size_t i = 0; i < sizeof(buf.guard); i++)
		TEST_ASSERT(buf.guard[i] == 'X');

	memset(&buf, 'X', sizeof(buf));
	EncodePhonemes("_^_abcdef a", buf.out, sizeof(buf.out), NULL);
	TEST_ASSERT(!strcmp(buf.out, "\x15" "abcdef"));
	for (size_t i = 0; i < sizeof(buf.guard); i++)
		TEST_ASSERT(buf.guard[i] == 'X');
}

static void test_tiny_buffers(void)
{
	char out[3];

	memset(out, 'X', sizeof(out));
	EncodePhonemes("aa", out, 1, NULL);
	TEST_ASSERT(out[0] == 0 && out[1] == 'X');

	memset(out, 'X', sizeof(out));
	EncodePhonemes("aa", out, 0, NULL);
	TEST_ASSERT(out[0] == 'X');
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	setup_phonemes();
	test_fits();
	test_long_word_truncates();
	test_long_language_name_truncates();
	test_tiny_buffers();
	return EXIT_SUCCESS;
}
