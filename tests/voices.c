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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

void
test_selection_by_name()
{
	printf("testing selection of voice by name\n");

	assert(espeak_SetVoiceByName("de") == EE_OK);

	assert(espeak_SetVoiceByName("und") == EE_NOT_FOUND);

	assert(espeak_ng_SetVoiceByName("de") == ENS_OK);

	assert(espeak_ng_SetVoiceByName("und") == ENS_VOICE_NOT_FOUND);
}

int
main(int argc, char **argv)
{
	assert(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, espeakINITIALIZE_DONT_EXIT) == 22050);

	test_selection_by_name();

	assert(espeak_Terminate() == EE_OK);

	return EXIT_SUCCESS;
}
