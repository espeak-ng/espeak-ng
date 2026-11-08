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

static int ignore_samples(short *wav, int numsamples, espeak_EVENT *events)
{
	(void)wav;
	(void)numsamples;
	(void)events;
	return 0;
}

static void synth(const char *text, unsigned int flags)
{
	TEST_ASSERT(espeak_Synth(text, strlen(text) + 1, 0, POS_CHARACTER, 0, flags, NULL, NULL) == EE_OK);
	TEST_ASSERT(espeak_Synchronize() == EE_OK);
}

static void test_before_voice_selected(void)
{
	const char *text = "hello";
	const void *ptr = text;

	TEST_ASSERT(espeak_SetParameter(espeakINTONATION, 5, 0) == EE_OK);
	TEST_ASSERT(espeak_TextToPhonemes(&ptr, espeakCHARS_UTF8, 0) != NULL);
}

static void test_parameter_index_range(void)
{
	espeak_SetParameter((espeak_PARAMETER)1000, 5, 0);
	espeak_SetParameter((espeak_PARAMETER)-1, 5, 0);
	espeak_Synchronize();
	TEST_ASSERT(espeak_GetParameter((espeak_PARAMETER)1000, 1) == -1);
	TEST_ASSERT(espeak_GetParameter((espeak_PARAMETER)-1, 0) == -1);
}

static void test_ssml_edge_cases(void)
{
	synth("<mark name>hi", espeakSSML | espeakCHARS_UTF8);
	synth("<emphasis level=\"bogus\">hi</emphasis>", espeakSSML | espeakCHARS_UTF8);
	synth("<speak><voice xml:lang=\"fr\">a<voice xml:lang=\"de\">b</voice>c d</voice></speak>", espeakSSML | espeakCHARS_UTF8);
	synth("x &abc\xC5\x95 y", espeakSSML | espeakCHARS_UTF8);
}

static void test_punctuation_list_overflow(void)
{
	char text[900];
	size_t n = 790;

	memset(text, 'a', n);
	memcpy(text + n, "\001B", 2);
	memset(text + n + 2, 'x', 59);
	text[n + 2 + 59] = 0;
	synth(text, espeakCHARS_UTF8 | espeakPHONEMES);
}

static void test_long_decimal_fraction(void)
{
	TEST_ASSERT(espeak_SetVoiceByName("it") == EE_OK);
	synth("1,12345678901234567890", espeakCHARS_UTF8);
	TEST_ASSERT(espeak_SetVoiceByName("be") == EE_OK);
	synth("\xD0\xB0\xD0\xB5\xD1\x91", espeakCHARS_UTF8);
	TEST_ASSERT(espeak_SetVoiceByName("xex") == EE_OK);
	synth("a'b", espeakCHARS_UTF8);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	TEST_ASSERT(espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0) == 22050);
	espeak_SetSynthCallback(ignore_samples);
	test_before_voice_selected();
	test_parameter_index_range();
	TEST_ASSERT(espeak_SetVoiceByName("en") == EE_OK);
	test_ssml_edge_cases();
	test_punctuation_list_overflow();
	test_long_decimal_fraction();
	TEST_ASSERT(espeak_Terminate() == EE_OK);
	return 0;
}
