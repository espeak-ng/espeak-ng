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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>

#define TEXT "A long sentence with many words makes the phoneme queue fill up so that the generator has to wait for room more than once."

static int n_symbols;

static void count_symbol(char *pho_code, int pho_type)
{
	(void)pho_code;
	(void)pho_type;
	n_symbols++;
}

static int ignore_audio(short *wav, int numsamples, espeak_EVENT *events)
{
	(void)wav;
	(void)numsamples;
	(void)events;
	return 0;
}

static int count_symbols_for(const char *text)
{
	espeak_ng_OUTPUT_HOOKS hooks = { count_symbol, NULL, NULL, NULL };

	n_symbols = 0;
	espeak_ng_SetOutputHooks(&hooks);
	TEST_ASSERT(espeak_Synth(text, strlen(text) + 1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);
	espeak_ng_SetOutputHooks(NULL);
	return n_symbols;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	espeak_SetSynthCallback(ignore_audio);
	TEST_ASSERT(espeak_SetVoiceByName("en") == EE_OK);

	// One symbol per entry of the clause's phoneme list. Before the hook moved
	// below the queue-space check, each wait re-announced the same phoneme and
	// this clause reported 149 symbols.
	TEST_ASSERT(count_symbols_for(TEXT) == 80);

	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
