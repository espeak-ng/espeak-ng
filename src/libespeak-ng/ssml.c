/*
 * Copyright (C) 2005 to 2015 by Jonathan Duddington
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
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>


#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>
#include <ucd/ucd.h>

#include "error.h"
#include "speech.h"
#include "phoneme.h"
#include "voice.h"
#include "synthesize.h"
#include "translate.h"
#include "ssml.h"

int attrcmp(const wchar_t *string1, const char *string2)
{
	int ix;

	if (string1 == NULL)
		return 1;

	for (ix = 0; (string1[ix] == string2[ix]) && (string1[ix] != 0); ix++)
		;
	if (((string1[ix] == '"') || (string1[ix] == '\'')) && (string2[ix] == 0))
		return 0;
	return 1;
}


int attrlookup(const wchar_t *string1, const MNEM_TAB *mtab)
{
	int ix;

	for (ix = 0; mtab[ix].mnem != NULL; ix++) {
		if (attrcmp(string1, mtab[ix].mnem) == 0)
			return mtab[ix].value;
	}
	return mtab[ix].value;
}

int attrnumber(const wchar_t *pw, int default_value, int type)
{
	int value = 0;

	if ((pw == NULL) || !IsDigit09(*pw))
		return default_value;

	while (IsDigit09(*pw))
		value = value*10 + *pw++ - '0';
	if ((type == 1) && (ucd_tolower(*pw) == 's')) {
		// time: seconds rather than ms
		value *= 1000;
	}
	return value;
}

int attrcopy_utf8(char *buf, const wchar_t *pw, int len)
{
	// Convert attribute string into utf8, write to buf, and return its utf8 length
	unsigned int c;
	int ix = 0;
	int n;
	int prev_c = 0;

	if (pw != NULL) {
		while ((ix < (len-4)) && ((c = *pw++) != 0)) {
			if ((c == '"') && (prev_c != '\\'))
				break; // " indicates end of attribute, unless preceded by backstroke
			n = utf8_out(c, &buf[ix]);
			ix += n;
			prev_c = c;
		}
	}
	buf[ix] = 0;
	return ix;
}

int GetVoiceAttributes(wchar_t *pw, int tag_type, SSML_STACK *ssml_sp, SSML_STACK *ssml_stack, int n_ssml_stack, char current_voice_id[])
{
	// Determines whether voice attribute are specified in this tag, and if so, whether this means
	// a voice change.
	// If it's a closing tag, delete the top frame of the stack and determine whether this implies
	// a voice change.
	// Returns  CLAUSE_TYPE_VOICE_CHANGE if there is a voice change

	wchar_t *lang;
	wchar_t *gender;
	wchar_t *name;
	wchar_t *age;
	wchar_t *variant;
	int value;
	const char *new_voice_id;

	static const MNEM_TAB mnem_gender[] = {
		{ "male", ENGENDER_MALE },
		{ "female", ENGENDER_FEMALE },
		{ "neutral", ENGENDER_NEUTRAL },
		{ NULL, ENGENDER_UNKNOWN }
	};

	if (tag_type & SSML_CLOSE) {
		// delete a stack frame
		if (n_ssml_stack > 1)
			n_ssml_stack--;
	} else {
		// add a stack frame if any voice details are specified
		lang = GetSsmlAttribute(pw, "xml:lang");

		if (tag_type != SSML_VOICE) {
			// only expect an xml:lang attribute
			name = NULL;
			variant = NULL;
			age = NULL;
			gender = NULL;
		} else {
			name = GetSsmlAttribute(pw, "name");
			variant = GetSsmlAttribute(pw, "variant");
			age = GetSsmlAttribute(pw, "age");
			gender = GetSsmlAttribute(pw, "gender");
		}

		if ((tag_type != SSML_VOICE) && (lang == NULL))
			return 0; // <s> or <p> without language spec, nothing to do

		ssml_sp = &ssml_stack[n_ssml_stack++];

		attrcopy_utf8(ssml_sp->language, lang, sizeof(ssml_sp->language));
		attrcopy_utf8(ssml_sp->voice_name, name, sizeof(ssml_sp->voice_name));
		if ((value = attrnumber(variant, 1, 0)) > 0)
			value--; // variant='0' and variant='1' the same
		ssml_sp->voice_variant_number = value;
		ssml_sp->voice_age = attrnumber(age, 0, 0);
		ssml_sp->voice_gender = attrlookup(gender, mnem_gender);
		ssml_sp->tag_type = tag_type;
	}

	new_voice_id = VoiceFromStack();
	if (strcmp(new_voice_id, current_voice_id) != 0) {
		// add an embedded command to change the voice
		strcpy(current_voice_id, new_voice_id);
		return CLAUSE_TYPE_VOICE_CHANGE;
	}

	return 0;
}
