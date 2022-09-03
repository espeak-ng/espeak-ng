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
#include "translateword.h"

static int NonAsciiNumber(int letter);


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


static const char *hex_letters[] = {"'e:j",	"b'i:",	"s'i:",	"d'i:",	"'i:",	"'ef"};
static const char *modifiers[] = { NULL, "_sub", "_sup", NULL };
// unicode ranges for non-ascii digits 0-9 (these must be in ascending order)
static const int number_ranges[] = {
	0x660, 0x6f0, // arabic
	0x966, 0x9e6, 0xa66, 0xae6, 0xb66, 0xbe6, 0xc66, 0xce6, 0xd66, // indic
	0xe50, 0xed0, 0xf20, 0x1040, 0x1090,
	0
};


int TranslateLetter(Translator *tr, char *word, char *phonemes, int control, ALPHABET *current_alphabet)
{
	// get pronunciation for an isolated letter
	// return number of bytes used by the letter
	// control bit 0:  a non-initial letter in a word
	//         bit 1:  say 'capital'
	//         bit 2:  say character code for unknown letters

	int n_bytes;
	int letter;
	int len;
	int ix;
	int c;
	char *p2;
	char *pbuf;
	const char *modifier;
	ALPHABET *alphabet;
	int al_offset;
	int al_flags;
	int language;
	int number;
	int phontab_1;
	int speak_letter_number;
	char capital[30];
	char ph_buf[80];
	char ph_buf2[80];
	char ph_alphabet[80];
	char hexbuf[12];
	static const char pause_string[] = { phonPAUSE, 0 };

	ph_buf[0] = 0;
	ph_alphabet[0] = 0;
	capital[0] = 0;
	phontab_1 = translator->phoneme_tab_ix;

	n_bytes = utf8_in(&letter, word);

	if ((letter & 0xfff00) == 0x0e000)
		letter &= 0xff; // uncode private usage area

	if (control & 2) {
		// include CAPITAL information
		if (iswupper(letter))
			Lookup(tr, "_cap", capital);
	}
	letter = towlower2(letter, tr);
	LookupLetter(tr, letter, word[n_bytes], ph_buf, control & 1);

	if (ph_buf[0] == 0) {
		// is this a subscript or superscript letter ?
		if ((c = IsSuperscript(letter)) != 0) {
			letter = c & 0x3fff;
			if ((control & 4 ) && ((modifier = modifiers[c >> 14]) != NULL)) {
				// don't say "superscript" during normal text reading
				Lookup(tr, modifier, capital);
				if (capital[0] == 0) {
					capital[2] = SetTranslator3(ESPEAKNG_DEFAULT_VOICE); // overwrites previous contents of translator3
					Lookup(translator3, modifier, &capital[3]);
					if (capital[3] != 0) {
						capital[0] = phonPAUSE;
						capital[1] = phonSWITCH;
						len = strlen(&capital[3]);
						capital[len+3] = phonSWITCH;
						capital[len+4] = phontab_1;
						capital[len+5] = 0;
					}
				}
			}
		}
		LookupLetter(tr, letter, word[n_bytes], ph_buf, control & 1);
	}

	if (ph_buf[0] == phonSWITCH) {
		strcpy(phonemes, ph_buf);
		return 0;
	}


	if ((ph_buf[0] == 0) && ((number = NonAsciiNumber(letter)) > 0)) {
		// convert a non-ascii number to 0-9
		LookupLetter(tr, number, 0, ph_buf, control & 1);
	}

	al_offset = 0;
	al_flags = 0;
	if ((alphabet = AlphabetFromChar(letter)) != NULL) {
		al_offset = alphabet->offset;
		al_flags = alphabet->flags;
	}

	if (alphabet != current_alphabet) {
		// speak the name of the alphabet
		current_alphabet = alphabet;
		if ((alphabet != NULL) && !(al_flags & AL_DONT_NAME) && (al_offset != translator->letter_bits_offset)) {
			if ((al_flags & AL_DONT_NAME) || (al_offset == translator->langopts.alt_alphabet) || (al_offset == translator->langopts.our_alphabet)) {
				// don't say the alphabet name
			} else {
				ph_buf2[0] = 0;
				if (Lookup(translator, alphabet->name, ph_alphabet) == 0) { // the original language for the current voice
					// Can't find the local name for this alphabet, use the English name
					ph_alphabet[2] = SetTranslator3(ESPEAKNG_DEFAULT_VOICE); // overwrites previous contents of translator3
					Lookup(translator3, alphabet->name, ph_buf2);
				} else if (translator != tr) {
					phontab_1 = tr->phoneme_tab_ix;
					strcpy(ph_buf2, ph_alphabet);
					ph_alphabet[2] = translator->phoneme_tab_ix;
				}

				if (ph_buf2[0] != 0) {
					// we used a different language for the alphabet name (now in ph_buf2)
					ph_alphabet[0] = phonPAUSE;
					ph_alphabet[1] = phonSWITCH;
					strcpy(&ph_alphabet[3], ph_buf2);
					len = strlen(ph_buf2) + 3;
					ph_alphabet[len] = phonSWITCH;
					ph_alphabet[len+1] = phontab_1;
					ph_alphabet[len+2] = 0;
				}
			}
		}
	}

	// caution: SetWordStress() etc don't expect phonSWITCH + phoneme table number

	if (ph_buf[0] == 0) {
		if ((al_offset != 0) && (al_offset == translator->langopts.alt_alphabet))
			language = translator->langopts.alt_alphabet_lang;
		else if ((alphabet != NULL) && (alphabet->language != 0) && !(al_flags & AL_NOT_LETTERS))
			language = alphabet->language;
		else
			language = L('e', 'n');

		if ((language != tr->translator_name) || (language == L('k', 'o'))) {
			char *p3;
			int initial, code;
			char hangul_buf[12];

			// speak in the language for this alphabet (or English)
			ph_buf[2] = SetTranslator3(WordToString2(language));

			if (translator3 != NULL) {
				if (((code = letter - 0xac00) >= 0) && (letter <= 0xd7af)) {
					// Special case for Korean letters.
					// break a syllable hangul into 2 or 3 individual jamo

					hangul_buf[0] = ' ';
					p3 = &hangul_buf[1];
					if ((initial = (code/28)/21) != 11) {
						p3 += utf8_out(initial + 0x1100, p3);
					}
					utf8_out(((code/28) % 21) + 0x1161, p3); // medial
					utf8_out((code % 28) + 0x11a7, &p3[3]); // final
					p3[6] = ' ';
					p3[7] = 0;
					ph_buf[3] = 0;
					TranslateRules(translator3, &hangul_buf[1], &ph_buf[3], sizeof(ph_buf)-3, NULL, 0, NULL);
					SetWordStress(translator3, &ph_buf[3], NULL, -1, 0);
				} else
					LookupLetter(translator3, letter, word[n_bytes], &ph_buf[3], control & 1);

				if (ph_buf[3] == phonSWITCH) {
					// another level of language change
					ph_buf[2] = SetTranslator3(&ph_buf[4]);
					LookupLetter(translator3, letter, word[n_bytes], &ph_buf[3], control & 1);
				}

				SelectPhonemeTable(voice->phoneme_tab_ix); // revert to original phoneme table

				if (ph_buf[3] != 0) {
					ph_buf[0] = phonPAUSE;
					ph_buf[1] = phonSWITCH;
					len = strlen(&ph_buf[3]) + 3;
					ph_buf[len] = phonSWITCH; // switch back
					ph_buf[len+1] = tr->phoneme_tab_ix;
					ph_buf[len+2] = 0;
				}
			}
		}
	}

	if (ph_buf[0] == 0) {
		// character name not found

		if (ph_buf[0] == 0) {
			speak_letter_number = 1;
			if (!(al_flags & AL_NO_SYMBOL)) {
				if (iswalpha(letter))
					Lookup(translator, "_?A", ph_buf);

				if ((ph_buf[0] == 0) && !iswspace(letter))
					Lookup(translator, "_??", ph_buf);

				if (ph_buf[0] == 0)
					EncodePhonemes("l'et@", ph_buf, NULL);
			}

			if (!(control & 4) && (al_flags & AL_NOT_CODE)) {
				// don't speak the character code number, unless we want full details of this character
				speak_letter_number = 0;
			}

			if (speak_letter_number) {
				if (al_offset == 0x2800) {
					// braille dots symbol, list the numbered dots
					p2 = hexbuf;
					for (ix = 0; ix < 8; ix++) {
						if (letter & (1 << ix))
							*p2++ = '1'+ix;
					}
					*p2 = 0;
				} else {
					// speak the hexadecimal number of the character code
					sprintf(hexbuf, "%x", letter);
				}

				pbuf = ph_buf;
				for (p2 = hexbuf; *p2 != 0; p2++) {
					pbuf += strlen(pbuf);
					*pbuf++ = phonPAUSE_VSHORT;
					LookupLetter(translator, *p2, 0, pbuf, 1);
					if (((pbuf[0] == 0) || (pbuf[0] == phonSWITCH)) && (*p2 >= 'a')) {
						// This language has no translation for 'a' to 'f', speak English names using base phonemes
						EncodePhonemes(hex_letters[*p2 - 'a'], pbuf, NULL);
					}
				}
				strcat(pbuf, pause_string);
			}
		}
	}

	len = strlen(phonemes);

	if (tr->langopts.accents & 2)  // 'capital' before or after the word ?
		sprintf(ph_buf2, "%c%s%s%s", 0xff, ph_alphabet, ph_buf, capital);
	else
		sprintf(ph_buf2, "%c%s%s%s", 0xff, ph_alphabet, capital, ph_buf); // the 0xff marker will be removed or replaced in SetSpellingStress()
	if ((len + strlen(ph_buf2)) < N_WORD_PHONEMES)
		strcpy(&phonemes[len], ph_buf2);
	return n_bytes;
}


static int NonAsciiNumber(int letter)
{
	// Change non-ascii digit into ascii digit '0' to '9', (or -1 if not)
	const int *p;
	int base;

	for (p = number_ranges; (base = *p) != 0; p++) {
		if (letter < base)
			break; // not found
		if (letter < (base+10))
			return letter-base+'0';
	}
	return -1;
}
