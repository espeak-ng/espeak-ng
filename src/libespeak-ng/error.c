/* Error handling APIs.
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

#include <errno.h>
#include <stdio.h>
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#include <string.h>
#include <malloc.h>

#include "espeak_ng.h"

#include "error.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"

espeak_ng_STATUS create_file_error_context(espeak_ng_ERROR_CONTEXT *context, espeak_ng_STATUS status, const char *filename)
{
	if (context) {
		if (*context) {
			free((*context)->filename);
		} else {
			*context = malloc(sizeof(espeak_ng_ERROR_CONTEXT_));
		}
		(*context)->filename = strdup(filename);
	}
	return status;
}

#pragma GCC visibility push(default)

ESPEAK_NG_API void espeak_ng_ClearErrorContext(espeak_ng_ERROR_CONTEXT *context)
{
	if (context && *context) {
		free((*context)->filename);
		free(*context);
		*context = NULL;
	}
}

ESPEAK_NG_API void espeak_ng_GetStatusCodeMessage(espeak_ng_STATUS status, char *buffer, size_t length)
{
	switch (status)
	{
	case ENS_COMPILE_ERROR:
		strncpy0(buffer, "Compile error.", length);
		break;
	case ENS_VERSION_MISMATCH:
#ifdef PLATFORM_WINDOWS
		sprintf(buffer, "Wrong version of espeak-data (expected 0x%x) at '%s'.", version_phdata, path_home);
#else
		snprintf(buffer, length, "Wrong version of espeak-data (expected 0x%x) at '%s'.", version_phdata, path_home);
#endif
		break;
	case ENS_FIFO_BUFFER_FULL:
		strncpy0(buffer, "The FIFO buffer is full.", length);
		break;
	case ENS_NOT_INITIALIZED:
		strncpy0(buffer, "The espeak-ng library has not been initialized.", length);
		break;
	case ENS_AUDIO_ERROR:
		strncpy0(buffer, "Cannot initialize the audio device.", length);
		break;
	case ENS_VOICE_NOT_FOUND:
		strncpy0(buffer, "The specified espeak-ng voice does not exist.", length);
		break;
	case ENS_MBROLA_NOT_FOUND:
		strncpy0(buffer, "Could not load the mbrola.dll file.", length);
		break;
	case ENS_MBROLA_VOICE_NOT_FOUND:
		strncpy0(buffer, "Could not load the specified mbrola voice file.", length);
		break;
	case ENS_EVENT_BUFFER_FULL:
		strncpy0(buffer, "The event buffer is full.", length);
		break;
	case ENS_NOT_SUPPORTED:
		strncpy0(buffer, "The requested functionality has not been built into espeak-ng.", length);
		break;
	case ENS_UNSUPPORTED_PHON_FORMAT:
		strncpy0(buffer, "The phoneme file is not in a supported format.", length);
		break;
	case ENS_NO_SPECT_FRAMES:
		strncpy0(buffer, "The spectral file does not contain any frame data.", length);
		break;
	default:
		if ((status & ENS_GROUP_MASK) == ENS_GROUP_ERRNO)
			strerror_r(status, buffer, length);
		else {
#ifdef PLATFORM_WINDOWS
			sprintf(buffer, "Unspecified error 0x%x.", status);
#else
			snprintf(buffer, length, "Unspecified error 0x%x.", status);
#endif
		}
		break;
	}
}

ESPEAK_NG_API void espeak_ng_PrintStatusCodeMessage(espeak_ng_STATUS status, FILE *out)
{
	char error[512];
	espeak_ng_GetStatusCodeMessage(status, error, sizeof(error));
	fprintf(out, "%s\n", error);
}

#pragma GCC visibility pop
