/*
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2017 Reece H. Dunn
 * Copyright (C) 2022 Juho Hiltunen
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "langopts.h"
#include "mnemonics.h"                // for MNEM_TAB
#include "translate.h"                // for Translator
#include "voice.h"                    // for CheckTranslator()

enum {
	V_NAME = 1,
	V_LANGUAGE,
	V_GENDER,
	V_PHONEMES,
	V_DICTIONARY,
	V_VARIANTS,

	V_MAINTAINER,
	V_STATUS,

	// these affect voice quality, are independent of language
	V_FORMANT,
	V_PITCH,
	V_ECHO,
	V_FLUTTER,
	V_ROUGHNESS,
	V_CLARITY,
	V_TONE,
	V_VOICING,
	V_BREATH,
	V_BREATHW,

	// these override defaults set by the translator
	V_LOWERCASE_SENTENCE,
	V_WORDGAP,
	V_INTONATION,
	V_TUNES,
	V_STRESSLENGTH,
	V_STRESSAMP,
	V_STRESSADD,
	V_DICTRULES,
	V_STRESSRULE,
	V_STRESSOPT,
	V_NUMBERS,

	V_MBROLA,
	V_KLATT,
	V_FAST,
	V_SPEED,
	V_DICTMIN,

	// these need a phoneme table to have been specified
	V_REPLACE,
	V_CONSONANTS
};

extern const MNEM_TAB langopts_tab[];

void LoadLanguageOptions(Translator *translator, int key, char *keyValue ) {
        int ix;

		switch (key) {
		case V_INTONATION: {
			sscanf(keyValue, "%d", &option_tone_flags);
			if ((option_tone_flags & 0xff) != 0) {
				if (CheckTranslator(translator, langopts_tab, key) != 0)
					break;

				translator->langopts.intonation_group = option_tone_flags & 0xff;
			}
			break;
		}
		case V_LOWERCASE_SENTENCE: {
			if (CheckTranslator(translator, langopts_tab, key) != 0)
				break;

			translator->langopts.lowercase_sentence = true;
			break;
			}
		case V_STRESSADD: { // stressAdd
                        if (CheckTranslator(translator, langopts_tab, key) != 0)
                            break;

                        int stress_add_set = 0;
                        int stress_add[8];

                        stress_add_set = Read8Numbers(keyValue, stress_add);

                        for (ix = 0; ix < stress_add_set; ix++) {
                            translator->stress_lengths[ix] += stress_add[ix];
                        }

                        break;
                    }
        case V_STRESSAMP: {
                if (CheckTranslator(translator, langopts_tab, key) != 0)
                    break;

                int stress_amps_set = 0;
                int stress_amps[8];

                stress_amps_set = Read8Numbers(keyValue, stress_amps);

                for (ix = 0; ix < stress_amps_set; ix++) {
                    translator->stress_amps[ix] = stress_amps[ix];
                }

                break;
            }
		case V_STRESSLENGTH: {
        			if (CheckTranslator(translator, langopts_tab, key) != 0)
        				break;

        			//printf("parsing: %s", keyValue);
        			int stress_lengths_set = 0;
        			int stress_lengths[8];
        			stress_lengths_set = Read8Numbers(keyValue, stress_lengths);

        			for (ix = 0; ix < stress_lengths_set; ix++) {
        				translator->stress_lengths[ix] = stress_lengths[ix];
        			}
        			break;
        		}
        case V_STRESSOPT: {
            if (CheckTranslator(translator, langopts_tab, key) != 0)
                 break;

            ReadNumbers(keyValue, &translator->langopts.stress_flags, 32, langopts_tab, key);
            break;
        }
		case V_STRESSRULE: {
			if (CheckTranslator(translator, langopts_tab, key) != 0)
				break;

			sscanf(keyValue, "%d %d %d", &translator->langopts.stress_rule,
				   &translator->langopts.unstressed_wd1,
				   &translator->langopts.unstressed_wd2);

			break;
		}

		case V_MAINTAINER:
		case V_STATUS:
			break;
	}
}
