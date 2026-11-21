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

// A voice file without a language line (such as the variant files in
// espeak-ng-data/voices/!v) used as the main voice must still get a valid
// phoneme table and be able to speak (espeak-ng/espeak-ng#2469).

#include "config.h"
#include "test_assert.h"

#include <stdio.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>

static int synth_callback(short *wav, int numsamples, espeak_EVENT *events)
{
	(void)wav;
	(void)numsamples;
	(void)events;
	return 0;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	espeak_SetSynthCallback(synth_callback);
	// "alex" is a variant file: no language line, so no phoneme table name
	TEST_ASSERT(espeak_SetVoiceByName("alex") == EE_OK);
	TEST_ASSERT(espeak_Synth("example", 8, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);
	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
