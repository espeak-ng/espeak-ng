/*
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

#include "soundicon.h"
#include "synthesize.h"

#define HEADER_SIZE 44

// Write a WAV-like file whose header matches the current sample rate, so the
// loader reads it directly. 'claimed' is the data length the header declares.
static void write_wav(const char *fname, size_t file_size, uint32_t claimed)
{
	unsigned char data[HEADER_SIZE + 16];
	FILE *f;

	TEST_ASSERT(file_size <= sizeof(data));
	memset(data, 0, sizeof(data));
	data[20] = 1;
	data[22] = 1;
	data[40] = claimed & 0xff;
	data[41] = (claimed >> 8) & 0xff;
	data[42] = (claimed >> 16) & 0xff;
	data[43] = (claimed >> 24) & 0xff;
	f = fopen(fname, "wb");
	TEST_ASSERT(f != NULL);
	TEST_ASSERT(fwrite(data, 1, file_size, f) == file_size);
	fclose(f);
}

static void scratch_path(char *out, size_t n, const char *name)
{
	const char *dir = getenv("TMPDIR");

	snprintf(out, n, "%s/soundicon_bounds_%s.wav", (dir != NULL && dir[0] == '/') ? dir : "/tmp", name);
}

static void test_truncated_header_rejected(void)
{
	char fname[512];

	scratch_path(fname, sizeof(fname), "short");
	write_wav(fname, 32, 0);
	TEST_ASSERT(LoadSoundFile2(fname) == -1);
	remove(fname);
}

static void test_claimed_length_clamped(void)
{
	char fname[512];
	int index;

	scratch_path(fname, sizeof(fname), "long");
	write_wav(fname, HEADER_SIZE + 10, 0x7fffffff);
	index = LoadSoundFile2(fname);
	TEST_ASSERT(index >= 0);
	TEST_ASSERT(soundicon_tab[index].length == 5);
	remove(fname);
}

static void test_table_full(void)
{
	char fname[512];
	char name[16];
	int ix;

	for (ix = 0; ix < N_SOUNDICON_TAB + 4; ix++) {
		snprintf(name, sizeof(name), "%d", ix);
		scratch_path(fname, sizeof(fname), name);
		write_wav(fname, HEADER_SIZE + 4, 4);
		LoadSoundFile2(fname);
		remove(fname);
	}
	TEST_ASSERT(n_soundicon_tab == N_SOUNDICON_TAB);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

#ifndef _WIN32
	samplerate = 0; // matches the zeroed sample rate fields in write_wav()
	test_truncated_header_rejected();
	test_claimed_length_clamped();
	test_table_full();
#endif
	return 0;
}
