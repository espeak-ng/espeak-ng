/*
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2018 Reece H. Dunn
 * Copyright (C) 2018 Juho Hiltunen
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

#ifndef ESPEAK_NG_PHONEMELIST_H
#define ESPEAK_NG_PHONEMELIST_H

// NOTE: this file should only be included by 'translate.c' and 'phonemelist.c'
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// in translate.h
struct Translator;

/* Moved from synthesize.h; synthesize.c does not require it.currently used in
 * intonation.c (once, for an on-stack array) phonemelist.c and translate.c
 *
 * The following was original designed as enough for one buffer-full of input
 * bytes.  Currently it is enough space for a quite long clause; long enough for
 * it to most likely be incomprehensible.
 *
 * Testing 'make check' with power-of-two values reveals that the smallest for
 * the checks to pass is 512.  The checks also pass with 65536, indicating that
 * apart from the known-bad test_phon ru check which uses a very large (and
 * nonsensical) words the current checks are not spliting the input at arbitrary
 * points.
 */
#define N_PHONEME_LIST 1000

// a clause translated into phoneme codes (first stage)
typedef struct PhonemeBase {
	unsigned short synthflags; // NOTE Put shorts on 32bit boundaries, because of RISC OS compiler bug?
	unsigned char phcode;
	unsigned char stresslevel;
	unsigned short sourceix;  // ix into the original source text string, only set at the start of a word
	unsigned char wordstress; // the highest level stress in this word
	unsigned char tone_ph;    // tone phoneme to use with this vowel
} PHONEME_LIST2; // 8 bytes iff packed

void MakePhonemeList(const struct Translator *tr, PHONEME_LIST2 *const ph_list2, uint16_t *n_ph_list2p, int post_pause, bool start_sentence);
        /* converts 'ph_list2' (from translate.c) into 'phoneme_list' */

#ifdef __cplusplus
}
#endif

#endif

