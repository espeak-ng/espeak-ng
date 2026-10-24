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
#include "speech.h"
#include "translate.h"

#define LONG_PHONEMES 120
#define GUARD_SIZE 16
#define GUARD_BYTE 0x5a

typedef struct {
	char buf[12];
	char guard[GUARD_SIZE];
} GuardedBuf;

static void guarded_init(GuardedBuf *g)
{
	memset(g->buf, GUARD_BYTE, sizeof(g->buf));
	memset(g->guard, GUARD_BYTE, sizeof(g->guard));
}

static int guard_intact(const GuardedBuf *g)
{
	size_t i;
	for (i = 0; i < GUARD_SIZE; i++)
		if ((unsigned char)g->guard[i] != GUARD_BYTE)
			return 0;
	return 1;
}

static void install_long_entry(Translator *tr, unsigned char *entry)
{
	memset(entry, 0, 2 + 1 + LONG_PHONEMES + 1 + 1);
	entry[0] = 2 + 1 + LONG_PHONEMES + 1;
	entry[1] = 1;
	entry[2] = 'a';
	memset(&entry[3], 'k', LONG_PHONEMES);
	memset(tr, 0, sizeof(*tr));
	tr->dict_hashtab[HashDictionary("a")] = (char *)entry;
}

static void test_long_entry_truncated(void)
{
	unsigned char entry[2 + 1 + LONG_PHONEMES + 1 + 1];
	Translator tr;
	GuardedBuf g;

	install_long_entry(&tr, entry);
	guarded_init(&g);
	TEST_ASSERT(Lookup(&tr, "a", g.buf, sizeof(g.buf)) != 0);
	TEST_ASSERT(strlen(g.buf) == sizeof(g.buf) - 1);
	TEST_ASSERT(!memcmp(g.buf, "kkkkkkkkkkk", sizeof(g.buf)));
	TEST_ASSERT(guard_intact(&g));
}

static void test_single_byte_destination(void)
{
	unsigned char entry[2 + 1 + LONG_PHONEMES + 1 + 1];
	Translator tr;
	GuardedBuf g;

	install_long_entry(&tr, entry);
	guarded_init(&g);
	TEST_ASSERT(Lookup(&tr, "a", g.buf, 1) != 0);
	TEST_ASSERT(g.buf[0] == 0);
	TEST_ASSERT((unsigned char)g.buf[1] == GUARD_BYTE);
	TEST_ASSERT(guard_intact(&g));
}

static void test_zero_size_destination(void)
{
	unsigned char entry[2 + 1 + LONG_PHONEMES + 1 + 1];
	Translator tr;
	GuardedBuf g;
	size_t i;

	install_long_entry(&tr, entry);
	guarded_init(&g);
	Lookup(&tr, "a", g.buf, 0);
	for (i = 0; i < sizeof(g.buf); i++)
		TEST_ASSERT((unsigned char)g.buf[i] == GUARD_BYTE);
	TEST_ASSERT(guard_intact(&g));
}

static void test_large_destination_untruncated(void)
{
	unsigned char entry[2 + 1 + LONG_PHONEMES + 1 + 1];
	Translator tr;
	char out[N_WORD_PHONEMES];

	install_long_entry(&tr, entry);
	TEST_ASSERT(Lookup(&tr, "a", out, sizeof(out)) != 0);
	TEST_ASSERT(strlen(out) == LONG_PHONEMES);
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	test_long_entry_truncated();
	test_single_byte_destination();
	test_zero_size_destination();
	test_large_destination_untruncated();
	return EXIT_SUCCESS;
}
