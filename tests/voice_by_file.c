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

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <espeak-ng/espeak_ng.h>

#ifndef _WIN32
static void test_set_voice_by_file_keeps_case_and_length(void)
{
	const char *tmp = getenv("TMPDIR");
	char dir[512];
	char subdir[512];
	char voice[512];
	FILE *f;

	snprintf(dir, sizeof(dir), "%s/espeak-voice-by-file-XXXXXX", (tmp != NULL && tmp[0] == '/') ? tmp : "/tmp");
	TEST_ASSERT(mkdtemp(dir) != NULL);
	snprintf(subdir, sizeof(subdir), "%s/Mixed_Case_Voice_Directory_longer_than_sixty_bytes", dir);
	TEST_ASSERT(mkdir(subdir, 0700) == 0);
	snprintf(voice, sizeof(voice), "%s/MyVoice", subdir);
	TEST_ASSERT(strlen(voice) > 60);

	f = fopen(voice, "w");
	TEST_ASSERT(f != NULL);
	fputs("name my\nlanguage en\n", f);
	fclose(f);

	TEST_ASSERT(espeak_ng_SetVoiceByFile(voice) == ENS_OK);

	remove(voice);
	rmdir(subdir);
	rmdir(dir);
}
#endif

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
#ifndef _WIN32
	test_set_voice_by_file_keeps_case_and_length();
#endif
	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
