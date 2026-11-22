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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

static const char *voice_path = "voice_params_bounds.tmp";

static void synth_with_voice_line(const char *line)
{
	FILE *f = fopen(voice_path, "w");
	TEST_ASSERT(f != NULL);
	fprintf(f, "name t\nlanguage en\n%s\n", line);
	fclose(f);

	TEST_ASSERT(espeak_ng_SetVoiceByFile(voice_path) == ENS_OK);
	TEST_ASSERT(espeak_Synth("hello world", 12, 0, POS_CHARACTER, 0, espeakCHARS_UTF8, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);
}

static void test_voice_lines(void)
{
	static const char *lines[] = {
		"echo 5000 50", "echo -5 50", "echo 100 -5",
		"tone 8400 100", "tone 100000 100", "tone -800 100 2000 100",
		"roughness -3", "clarity -5", "pitch 5 5", "pitch 0 0",
		"pitch -100 5000000", "flutter 99999999", "flutter -4",
		"stressOpt 1,2",
	};
	for (size_t i = 0; i < sizeof(lines) / sizeof(lines[0]); i++)
		synth_with_voice_line(lines[i]);
	remove(voice_path);
}

static void test_switch_language_long_name(void)
{
	const char *text = "[[_^_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]]";

	TEST_ASSERT(espeak_ng_SetVoiceByName("en") == ENS_OK);
	TEST_ASSERT(espeak_Synth(text, strlen(text) + 1, 0, POS_CHARACTER, 0, espeakCHARS_UTF8 | espeakPHONEMES, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);
}

static void test_long_voice_path(void)
{
	char path[200];

	memset(path, 'a', sizeof(path));
	path[sizeof(path) - 1] = 0;
	TEST_ASSERT(espeak_ng_SetVoiceByFile(path) == ENS_VOICE_NOT_FOUND);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0) == 22050);
	test_voice_lines();
	test_switch_language_long_name();
	test_long_voice_path();
	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
