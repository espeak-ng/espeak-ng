/* An implementation of the eSpeak API using the espeak-ng API.
 *
 * Copyright (C) 2016 Reece H. Dunn
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
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "speak_lib.h"
#include "espeak_ng.h"

#include <stdlib.h>

extern int option_phoneme_events; // from translate.h

#pragma GCC visibility push(default)

ESPEAK_API int espeak_Initialize(espeak_AUDIO_OUTPUT output_type, int buf_length, const char *path, int options)
{
	espeak_ng_InitializePath(path);
	espeak_ng_STATUS result = espeak_ng_Initialize();
	if (result != ENS_OK) {
		if (result == ENS_VERSION_MISMATCH)
			fprintf(stderr, "Wrong version of espeak-data\n");
		else {
			fprintf(stderr, "Failed to load espeak-data\n");
			if ((options & espeakINITIALIZE_DONT_EXIT) == 0)
				exit(1);
		}
	}

	switch (output_type)
	{
	case AUDIO_OUTPUT_PLAYBACK:
		espeak_ng_InitializeOutput(ENOUTPUT_MODE_SPEAK_AUDIO, buf_length, NULL);
		break;
	case AUDIO_OUTPUT_RETRIEVAL:
		espeak_ng_InitializeOutput(0, buf_length, NULL);
		break;
	case AUDIO_OUTPUT_SYNCHRONOUS:
		espeak_ng_InitializeOutput(ENOUTPUT_MODE_SYNCHRONOUS, buf_length, NULL);
		break;
	case AUDIO_OUTPUT_SYNCH_PLAYBACK:
		espeak_ng_InitializeOutput(ENOUTPUT_MODE_SYNCHRONOUS | ENOUTPUT_MODE_SPEAK_AUDIO, buf_length, NULL);
		break;
	}

	option_phoneme_events = (options & (espeakINITIALIZE_PHONEME_EVENTS | espeakINITIALIZE_PHONEME_IPA));

	return espeak_ng_GetSampleRate();
}

#pragma GCC visibility pop
