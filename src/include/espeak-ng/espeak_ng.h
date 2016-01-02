/* eSpeak NG API.
 *
 * Copyright (C) 2015-2016 Reece H. Dunn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ESPEAK_NG_H
#define ESPEAK_NG_H

#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __WIN32__
#define ESPEAK_NG_API __declspec(dllexport)
#else
#define ESPEAK_NG_API
#endif

typedef enum {
	ENS_GROUP_MASK               = 0x70000000,
	ENS_GROUP_ERRNO              = 0x00000000, /* Values 0-255 map to errno error codes. */
	ENS_GROUP_ESPEAK_NG          = 0x10000000, /* eSpeak NG error codes. */

	ENS_OK                       = 0,
	ENS_COMPILE_ERROR            = 0x100001FF,
	ENS_VERSION_MISMATCH         = 0x100002FF,
	ENS_FIFO_BUFFER_FULL         = 0x100003FF,
} espeak_ng_STATUS;

typedef enum {
	ENOUTPUT_MODE_SYNCHRONOUS = 0x0001,
	ENOUTPUT_MODE_SPEAK_AUDIO = 0x0002,
} espeak_ng_OUTPUT_MODE;

ESPEAK_NG_API void
espeak_ng_InitializePath(const char *path);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_Initialize(void);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_InitializeOutput(espeak_ng_OUTPUT_MODE output_mode,
                           int buffer_length,
                           const char *device);

ESPEAK_NG_API int
espeak_ng_GetSampleRate(void);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_SpeakCharacter(wchar_t character);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_CompileDictionary(const char *dsource,
                            const char *dict_name,
                            FILE *log,
                            int flags);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_CompileMbrolaVoice(const char *path,
                             FILE *log);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_CompilePhonemeData(long rate,
                             FILE *log);

ESPEAK_NG_API espeak_ng_STATUS
espeak_ng_CompileIntonation(FILE *log);

#ifdef __cplusplus
}
#endif

#endif
