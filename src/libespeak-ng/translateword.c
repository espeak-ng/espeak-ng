/*
 * Copyright (C) 2005 to 2014 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2017 Reece H. Dunn
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

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "translate.h"
#include "dictionary.h"           // for TranslateRules, LookupDictList, Cha...
#include "numbers.h"              // for SetSpellingStress, TranslateLetter
#include "phoneme.h"              // for phonSWITCH, PHONEME_TAB, phonPAUSE_...
#include "phonemelist.h"          // for MakePhonemeList
#include "readclause.h"           // for towlower2, Eof, ReadClause, is_str_...
#include "synthdata.h"            // for SelectPhonemeTable, LookupPhonemeTable
#include "synthesize.h"           // for PHONEME_LIST2, N_PHONEME_LIST, PHON...
#include "ucd/ucd.h"              // for ucd_toupper
#include "voice.h"                // for voice, voice_t
#include "speech.h"               // for MAKE_MEM_UNDEFINED

char *SpeakIndividualLetters(Translator *tr, char *word, char *phonemes, int spell_word, ALPHABET *current_alphabet, char word_phonemes[])
{
	int posn = 0;
	int capitals = 0;
	bool non_initial = false;

	if (spell_word > 2)
		capitals = 2; // speak 'capital'
	if (spell_word > 1)
		capitals |= 4; // speak character code for unknown letters

	while ((*word != ' ') && (*word != 0)) {
		word += TranslateLetter(tr, word, phonemes, capitals | non_initial, current_alphabet);
		posn++;
		non_initial = true;
		if (phonemes[0] == phonSWITCH) {
			// change to another language in order to translate this word
			strcpy(word_phonemes, phonemes);
			return NULL;
		}
	}
	SetSpellingStress(tr, phonemes, spell_word, posn);
	return word;
}
