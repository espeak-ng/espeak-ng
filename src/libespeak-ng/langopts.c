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

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "langopts.h"
#include "mnemonics.h"                // for MNEM_TAB
#include "translate.h"                // for Translator
#include "soundicon.h"                // for soundicon_tab, n_soundicon_tab
#include "speech.h"                    // for path_home, PATHSEP
#include "synthdata.h"                    // for n_tunes, tunes
#include "voice.h"                    // for ReadNumbers, Read8Numbers, ...

static int CheckTranslator(Translator *tr, const MNEM_TAB *keyword_tab, int key);
static int LookupTune(const char *name);

void LoadLanguageOptions(Translator *tr, int key, char *keyValue ) {
if (CheckTranslator(tr,langopts_tab, key) != 0) {
				return;
			}

        int ix;
        int n;

		switch (key) {
		case V_DICTMIN: {


			if (sscanf(keyValue, "%d", &n) == 1)
				tr->dict_min_size = n;

			break;
			}

			case V_DICTRULES: { // conditional dictionary rules and list entries
				ReadNumbers(keyValue, &tr->dict_condition, 32, langopts_tab, key);
				break;
			}
		case V_INTONATION: {
			sscanf(keyValue, "%d", &option_tone_flags);
			if ((option_tone_flags & 0xff) != 0) {
				tr->langopts.intonation_group = option_tone_flags & 0xff;
			}
			break;
		}
		case V_NUMBERS: {
			// expect a list of numbers
			while (*keyValue != 0) {
				const char *token;
				while (isspace((unsigned char)*keyValue)) keyValue++;
				token = keyValue;
				if ((n = atoi(keyValue)) > 0) {
					keyValue++;
					if (n < 32) {
							tr->langopts.numbers |= (1u << n);
					} else {
						if (n < 64)
							tr->langopts.numbers2 |= (1u << (n-32));
						else
							fprintf(stderr, "numbers: Bad option number %d\n", n);					}
				}
				while (isalnum((unsigned char)*keyValue)) keyValue++;
				if (keyValue == token && *keyValue != 0)
					keyValue++;
			}
			ProcessLanguageOptions(&(tr->langopts));

			break;
		}
		case V_LOWERCASE_SENTENCE: {
			tr->langopts.lowercase_sentence = true;
			break;
			}
		case V_SPELLINGSTRESS: {
			tr->langopts.spelling_stress = true;
			break;
		}
		case V_STRESSADD: { // stressAdd
                        int stress_add_set = 0;
                        int stress_add[8];

                        stress_add_set = Read8Numbers(keyValue, stress_add);

                        for (ix = 0; ix < stress_add_set; ix++) {
                            tr->stress_lengths[ix] +=stress_add[ix];
                        }

                        break;
                    }
        case V_STRESSAMP: {

                int stress_amps_set = 0;
                int stress_amps[8];

                stress_amps_set = Read8Numbers(keyValue, stress_amps);

                for (ix = 0; ix < stress_amps_set; ix++) {
                    tr->stress_amps[ix] = stress_amps[ix];
                }

                break;
            }
		case V_STRESSLENGTH: {
        			//printf("parsing: %s", keyValue);
        			int stress_lengths_set = 0;
        			int stress_lengths[8];
        			stress_lengths_set = Read8Numbers(keyValue, stress_lengths);

        			for (ix = 0; ix < stress_lengths_set; ix++) {
        				tr->stress_lengths[ix] =stress_lengths[ix];
        			}
        			break;
        		}
        case V_STRESSOPT: {
            ReadNumbers(keyValue, &tr->langopts.stress_flags, 32, langopts_tab, key);
            break;
        }
		case V_STRESSRULE: {
			sscanf(keyValue, "%d %d %d", &tr->langopts.stress_rule,
				   &tr->langopts.unstressed_wd1,
				   &tr->langopts.unstressed_wd2);

			break;
		}
            case V_TUNES: {
				char names[6][40] = { {0}, {0}, {0}, {0}, {0}, {0} };
                n = sscanf(keyValue, "%39s %39s %39s %39s %39s %39s", names[0], names[1], names[2], names[3], names[4], names[5]);
                tr->langopts.intonation_group = 0;

                int value;
                for (ix = 0; ix < n; ix++) {
                    if (strcmp(names[ix], "NULL") == 0)
                        continue;


                    if ((value = LookupTune(names[ix])) < 0)
                        fprintf(stderr, "Unknown tune '%s'\n", names[ix]);
                    else
                        tr->langopts.tunes[ix] = value;
                }
			break;
			}
			case V_WORDGAP: {
				sscanf(keyValue, "%d %d", &tr->langopts.word_gap, &tr->langopts.vowel_pause);
				break;
			}

		default: {
			if ((key & 0xff00) == 0x100) {
				sscanf(keyValue, "%d", &tr->langopts.param[key &0xff]);
			}
		break;
		}
	}
}

void LoadConfig(void) {
	// Load configuration file, if one exists
	char buf[N_PATH_BUF];
	FILE *f;
	int ix;
	char c1;
	char string[200];

	snprintf(buf, sizeof(buf), "%s%c%s", path_home, PATHSEP, "config");
	if ((f = fopen(buf, "r")) == NULL)
		return;

	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (buf[0] == '/')  continue;

		if (memcmp(buf, "tone", 4) == 0)
			ReadTonePoints(&buf[5], tone_points);
		else if (memcmp(buf, "soundicon", 9) == 0) {
			ix = sscanf(&buf[10], "_%c %199s", &c1, string);
			if ((ix == 2) && (n_soundicon_tab < N_SOUNDICON_TAB)) {
				// add sound file information to soundicon array
				// the file will be loaded to memory by LoadSoundFile2()
				soundicon_tab[n_soundicon_tab].name = c1;
				soundicon_tab[n_soundicon_tab].filename = strdup(string);
				soundicon_tab[n_soundicon_tab++].length = 0;
			}
		}
	}
	fclose(f);
}


static int LookupTune(const char *name) {
	int ix;

	for (ix = 0; ix < n_tunes; ix++) {
		if (strcmp(name, tunes[ix].name) == 0)
			return ix;
	}
	return -1;
}

int CheckTranslator(Translator *tr, const MNEM_TAB *key_tab, int key)
{
	// Return 0 if translator is set.
	// Return 1 and print an error message for specified key if not
	// used for parsing language options
	if (tr)
		return 0;

	fprintf(stderr, "Cannot set %s: language not set, or is invalid.\n", LookupMnemName(key_tab, key));
	return 1;
}
