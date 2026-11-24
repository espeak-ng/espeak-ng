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

// Regression test for OSS-Fuzz issue 536581693: PeaksToHarmspect() indexed the
// peak shape table past its end when a formant peak was very narrow, and divided
// by zero when it was narrower still. Run under AddressSanitizer or valgrind
// to see the out-of-bounds read.

#include "config.h"
#include "test_assert.h"

#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include "wavegen.h"

#define HTAB_SIZE 1024

static void harmspect_with_peak_width(int width)
{
	wavegen_peaks_t peaks[N_PEAKS];
	int htab[HTAB_SIZE];
	int pitch = 100 << 16;  // 100 Hz

	memset(peaks, 0, sizeof(peaks));
	memset(htab, 0, sizeof(htab));
	// The first harmonic of the peak lies just above freq - left, so the
	// shape index (fp - f) / (left >> 8) is at its largest.
	peaks[1].left = width;
	peaks[1].right = width;
	peaks[1].height = 1 << 15;
	peaks[1].freq = width + 10 * pitch + (pitch - 1);

	TEST_ASSERT(PeaksToHarmspect(peaks, pitch, htab, 0) >= 0);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0) == 22050);
	TEST_ASSERT(espeak_ng_SetVoiceByName("en") == ENS_OK);
	// set up the wavegen voice
	TEST_ASSERT(espeak_Synth("a", 2, 0, POS_CHARACTER, 0, espeakCHARS_UTF8, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);

	harmspect_with_peak_width(2760);   // shape index 275, past the 257 entry table
	harmspect_with_peak_width(65535);  // shape index 257
	harmspect_with_peak_width(100);    // left >> 8 == 0

	espeak_Terminate();
	return 0;
}
