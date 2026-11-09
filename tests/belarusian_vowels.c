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

#include <stdlib.h>

#include "dictionary.h"
#include "translate.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	// а е ё о у ы э ю я і
	static const int vowels[] = { 0x430, 0x435, 0x451, 0x43e, 0x443, 0x44b, 0x44d, 0x44e, 0x44f, 0x456 };
	// б в г ў
	static const int consonants[] = { 0x431, 0x432, 0x433, 0x45e };
	Translator *tr = SelectTranslator("be");

	TEST_ASSERT(tr != NULL);
	for (size_t ix = 0; ix < sizeof(vowels) / sizeof(vowels[0]); ix++)
		TEST_ASSERT(IsVowel(tr, vowels[ix]));
	for (size_t ix = 0; ix < sizeof(consonants) / sizeof(consonants[0]); ix++)
		TEST_ASSERT(!IsVowel(tr, consonants[ix]));

	DeleteTranslator(tr);
	return 0;
}
