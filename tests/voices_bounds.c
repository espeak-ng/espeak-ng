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

#include <espeak-ng/speak_lib.h>

static void test_long_variant_name(void)
{
	char name[200];

	memset(name, 'a', sizeof(name));
	memcpy(name, "en+", 3);
	name[sizeof(name) - 1] = 0;
	TEST_ASSERT(espeak_SetVoiceByName(name) != EE_INTERNAL_ERROR);
}

static void test_long_language_list(void)
{
	espeak_VOICE selector;
	char languages[200];

	memset(languages, 'e', sizeof(languages));
	languages[sizeof(languages) - 1] = 0;
	memset(&selector, 0, sizeof(selector));
	selector.languages = languages;
	TEST_ASSERT(espeak_ListVoices(&selector) != NULL);
}

static void test_short_name_does_not_match_longer_identifier_suffix(void)
{
	TEST_ASSERT(espeak_SetVoiceByName("a-voice-name-longer-than-any-identifier") == EE_NOT_FOUND);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	test_long_variant_name();
	test_long_language_list();
	test_short_name_does_not_match_longer_identifier_suffix();
	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
