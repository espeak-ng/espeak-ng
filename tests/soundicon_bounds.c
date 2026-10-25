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

static void test_truncated_header_rejected(void)
{
	write_wav("/tmp/soundicon_bounds_short.wav", 32, 0);
	TEST_ASSERT(LoadSoundFile2("/tmp/soundicon_bounds_short.wav") == -1);
	remove("/tmp/soundicon_bounds_short.wav");
}

static void test_claimed_length_clamped(void)
{
	int index;

	write_wav("/tmp/soundicon_bounds_long.wav", HEADER_SIZE + 10, 0x7fffffff);
	index = LoadSoundFile2("/tmp/soundicon_bounds_long.wav");
	TEST_ASSERT(index >= 0);
	TEST_ASSERT(soundicon_tab[index].length == 5);
	remove("/tmp/soundicon_bounds_long.wav");
}

static void test_table_full(void)
{
	char fname[64];
	int ix;

	for (ix = 0; ix < N_SOUNDICON_TAB + 4; ix++) {
		snprintf(fname, sizeof(fname), "/tmp/soundicon_bounds_%d.wav", ix);
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
