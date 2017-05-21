/*
 * Copyright (C) 2017 Reece H. Dunn
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

void
test_espeak_terminate_without_initialize()
{
	printf("testing espeak_Terminate without espeak_Initialize\n");

	assert(event_list == NULL);
	assert(translator == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
}

void
test_espeak_initialize()
{
	printf("testing espeak_Initialize\n");

	assert(event_list == NULL);
	assert(translator == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
}

void
test_espeak_synth()
{
	printf("testing espeak_Synth\n");

	assert(event_list == NULL);
	assert(translator == NULL);

	assert(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	assert(event_list != NULL);
	assert(translator == NULL);

	const char *test = "One two three.";
	assert(espeak_Synth(test, strlen(test)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL) == EE_OK);
	assert(translator != NULL);

	assert(espeak_Synchronize() == EE_OK);
	assert(translator != NULL);

	assert(espeak_Terminate() == EE_OK);
	assert(event_list == NULL);
	assert(translator == NULL);
}

int
main(int argc, char **argv)
{
	test_espeak_terminate_without_initialize();
	test_espeak_initialize();

	test_espeak_synth();
	test_espeak_synth(); // Check that this does not crash when run a second time.

	return EXIT_SUCCESS;
}
