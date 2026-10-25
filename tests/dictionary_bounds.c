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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "dictionary.h"
#include "speech.h"
#include "translate.h"

// A phoneme string that fits comfortably within the destination buffer
// must round-trip unchanged, and the returned length must be its real
// (unbounded) strlen, since callers use it to skip past the source
// string in the surrounding buffer.
static void
test_copy_within_bounds(void)
{
	char dest[16];
	size_t n = CopyBoundedPhoneticString(dest, sizeof(dest), "abc");
	TEST_ASSERT(n == 3);
	TEST_ASSERT(!strcmp(dest, "abc"));
}

// A phoneme string that does not fit must be truncated to fit dest,
// not overflow it -- but the returned length must still be the source
// string's real length, so the caller can correctly skip past all of
// it in the surrounding buffer even though only a prefix was copied.
static void
test_copy_overlong_is_truncated_not_overflowed(void)
{
	char dest[8];
	const char *src = "abcdefghijklmnop"; // 16 bytes, does not fit in dest[8]
	size_t n = CopyBoundedPhoneticString(dest, sizeof(dest), src);
	TEST_ASSERT(n == strlen(src));
	TEST_ASSERT(strlen(dest) == sizeof(dest) - 1);
	TEST_ASSERT(!strcmp(dest, "abcdefg"));
}

// A source string that exactly fills dest (dest_size - 1 bytes, plus
// the NUL) must round-trip in full, not be truncated by one byte.
static void
test_copy_exact_fit(void)
{
	char dest[4];
	size_t n = CopyBoundedPhoneticString(dest, sizeof(dest), "abc");
	TEST_ASSERT(n == 3);
	TEST_ASSERT(!strcmp(dest, "abc"));
}

static int load_dict_bytes(const unsigned char *data, size_t size, Translator *tr)
{
	char dir[] = "dictionary_bounds_XXXXXX";
	char fname[N_PATH_BUF + 16];
	FILE *f;
	int result;

	TEST_ASSERT(mkdtemp(dir) != NULL);
	snprintf(path_home, sizeof(path_home), "%s", dir);
	snprintf(fname, sizeof(fname), "%s/xx_dict", dir);
	f = fopen(fname, "wb");
	TEST_ASSERT(f != NULL);
	TEST_ASSERT(fwrite(data, 1, size, f) == size);
	fclose(f);

	memset(tr, 0, sizeof(*tr));
	result = LoadDictionary(tr, "xx", 1);

	remove(fname);
	rmdir(dir);
	return result;
}

static void test_dict_rules_offset_beyond_file(void)
{
	static unsigned char data[2048];
	Translator tr;
	uint32_t hash_size = N_HASH_DICT;
	uint32_t rules_offset = 0x100000;

	memset(data, 0, sizeof(data));
	memcpy(data, &hash_size, 4);
	memcpy(data + 4, &rules_offset, 4);
	TEST_ASSERT(load_dict_bytes(data, sizeof(data), &tr) == 2);
	TEST_ASSERT(tr.data_dictlist == NULL);
}

static void test_dict_truncated_hash_chain(void)
{
	static unsigned char data[2048];
	Translator tr;
	uint32_t hash_size = N_HASH_DICT;
	uint32_t rules_offset = 2000;

	memset(data, 0xff, sizeof(data));
	memcpy(data, &hash_size, 4);
	memcpy(data + 4, &rules_offset, 4);
	TEST_ASSERT(load_dict_bytes(data, sizeof(data), &tr) == 2);
	TEST_ASSERT(tr.data_dictlist == NULL);
}

static void test_dict_unterminated_rules(void)
{
	static unsigned char data[2048];
	Translator tr;
	uint32_t hash_size = N_HASH_DICT;
	uint32_t rules_offset = 1100;

	memset(data, 0, sizeof(data));
	memcpy(data, &hash_size, 4);
	memcpy(data + 4, &rules_offset, 4);
	data[rules_offset] = 6; // RULE_GROUP_START
	memset(data + rules_offset + 1, 'a', sizeof(data) - rules_offset - 1);
	TEST_ASSERT(load_dict_bytes(data, sizeof(data), &tr) == 0);
	free(tr.data_dictlist);
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	test_copy_within_bounds();
	test_copy_overlong_is_truncated_not_overflowed();
	test_copy_exact_fit();
#ifndef _WIN32
	test_dict_rules_offset_beyond_file();
	test_dict_truncated_hash_chain();
	test_dict_unterminated_rules();
#endif
	return EXIT_SUCCESS;
}
