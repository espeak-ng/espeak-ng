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
#ifndef _WIN32
#include <unistd.h>
#endif

#include <espeak-ng/speak_lib.h>

#include "langopts.h"
#include "translate.h"
#include "voice.h"

static void test_non_numeric_tokens_terminate(void)
{
	static const char *values[] = {
		"-", ",", "x", "1 - 2", "1,2,3", "!!!", "-5", "40 31 99 abc", "",
	};
	Translator *tr = SelectTranslator("en");

	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%s", values[i]);
		LoadLanguageOptions(tr, V_NUMBERS, buf);
	}
	TEST_ASSERT((tr->langopts.numbers & (1u << 31)) != 0);
	DeleteTranslator(tr);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

#ifndef _WIN32
	alarm(10);
#endif
	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	test_non_numeric_tokens_terminate();
	printf("langopts numbers: ok\n");
	return EXIT_SUCCESS;
}
