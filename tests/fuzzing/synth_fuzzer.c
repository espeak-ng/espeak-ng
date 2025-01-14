/*
 * Copyright (C) 2018 Sascha Brawer
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <time.h>

#include <espeak-ng/espeak_ng.h>
#define BOLDRED(x) "\x1b[31m\x1b[1m" x "\x1b[0m"

static int initialized = 0;

static int
espeak_callback(short *data, int samples, espeak_EVENT *events)
{
	(void)data; 
	(void)samples; 
	(void)events; 

	return 0;
}

/* See http://llvm.org/docs/LibFuzzer.html */
extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	int buflength = size+1;
	if (!initialized)
	{
		int options = espeakINITIALIZE_DONT_EXIT;
		espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, buflength, PATH_ESPEAK_DATA, options);
		espeak_ng_SetRandSeed(1);
		espeak_SetSynthCallback(espeak_callback);
		const char *lang = getenv("FUZZ_VOICE");
		if (lang == NULL)
		{
			fprintf(stderr, "\n" BOLDRED("[Please set up FUZZ_VOICE env var before starting fuzzer]") "\n\n");
			exit(1);
		}
		if (espeak_SetVoiceByName(lang) != EE_OK)
		{
			fprintf(stderr, "\n" BOLDRED("[Please supply a valid voice in FUZZ_VOICE]") "\n\n");
			exit(1);
		}
		initialized = 1;
		fprintf(stderr, "VOICE FUZZED = %s\n", lang);
	}
	char *mutable_data = strndup((char *)data, size);
	if (!mutable_data)
	{
		perror("malloc");
		exit(1);
	}
	unsigned int position = 0, position_type = POS_CHARACTER, end_position = 0 , synth_flags = espeakCHARS_AUTO;
	espeak_Synth(mutable_data, buflength, position, position_type, end_position,
				 synth_flags, NULL, NULL);
	free(mutable_data);

	return 0;
}
