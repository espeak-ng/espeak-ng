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

#include <stdlib.h>

#include "common.h"
#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/encoding.h>

static void test_string_to_word_high_byte(void)
{
	TEST_ASSERT(StringToWord("\xff\xff\xff\xff") == 0xffffffffu);
}

static void test_utf8_sequence_ending_at_explicit_length(void)
{
	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "\xC3\xA9", 2, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xE9);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	TEST_ASSERT(text_decoder_decode_string(decoder, "\xE2\x82\xAC", 3, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x20AC);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	TEST_ASSERT(text_decoder_decode_string(decoder, "\xF0\x9F\x98\x80", 4, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x1F600);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

int main(void)
{
	test_string_to_word_high_byte();
	test_utf8_sequence_ending_at_explicit_length();
	return 0;
}
