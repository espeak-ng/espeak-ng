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

int attr_prosody_value(int param_type, const wchar_t *pw, int *value_out)
{
	int sign = 0;
	wchar_t *tail;
	double value;

	while (iswspace(*pw)) pw++;
	if (*pw == '+') {
		pw++;
		sign = 1;
	}
	if (*pw == '-') {
		pw++;
		sign = -1;
	}
	value = (double)wcstod(pw, &tail);
	if (tail == pw) {
		// failed to find a number, return 100%
		*value_out = 100;
		return 2;
	}

	if (*tail == '%') {
		if (sign != 0)
			value = 100 + (sign * value);
		*value_out = (int)value;
		return 2; // percentage
	}

	if ((tail[0] == 's') && (tail[1] == 't')) {
		double x;
		// convert from semitones to a  frequency percentage
		x = pow((double)2.0, (double)((value*sign)/12)) * 100;
		*value_out = (int)x;
		return 2; // percentage
	}

	if (param_type == espeakRATE) {
		if (sign == 0)
			*value_out = (int)(value * 100);
		else
			*value_out = 100 + (int)(sign * value * 100);
		return 2; // percentage
	}

	*value_out = (int)value;
	return sign;   // -1, 0, or 1
}

int GetVoiceAttributes(wchar_t *pw, int tag_type, SSML_STACK *ssml_sp, SSML_STACK *ssml_stack, int n_ssml_stack, char current_voice_id[40], espeak_VOICE *base_voice, char base_voice_variant_name[40])
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

	new_voice_id = VoiceFromStack(ssml_stack, n_ssml_stack, base_voice, base_voice_variant_name);
	if (strcmp(new_voice_id, current_voice_id) != 0) {
		// add an embedded command to change the voice
		strcpy(current_voice_id, new_voice_id);
		return CLAUSE_TYPE_VOICE_CHANGE;
	}

	return 0;
}

const char *VoiceFromStack(SSML_STACK *ssml_stack, int n_ssml_stack, espeak_VOICE *base_voice, char base_voice_variant_name[40])
{
	// Use the voice properties from the SSML stack to choose a voice, and switch
	// to that voice if it's not the current voice

	int ix;
	const char *p;
	SSML_STACK *sp;
	const char *v_id;
	int voice_name_specified;
	int voice_found;
	espeak_VOICE voice_select;
	static char voice_name[40];
	char language[40];
	char buf[80];

	strcpy(voice_name, ssml_stack[0].voice_name);
	strcpy(language, ssml_stack[0].language);
	voice_select.age = ssml_stack[0].voice_age;
	voice_select.gender = ssml_stack[0].voice_gender;
	voice_select.variant = ssml_stack[0].voice_variant_number;
	voice_select.identifier = NULL;

	for (ix = 0; ix < n_ssml_stack; ix++) {
		sp = &ssml_stack[ix];
		voice_name_specified = 0;

		if ((sp->voice_name[0] != 0) && (SelectVoiceByName(NULL, sp->voice_name) != NULL)) {
			voice_name_specified = 1;
			strcpy(voice_name, sp->voice_name);
			language[0] = 0;
			voice_select.gender = ENGENDER_UNKNOWN;
			voice_select.age = 0;
			voice_select.variant = 0;
		}
		if (sp->language[0] != 0) {
			strcpy(language, sp->language);

			// is this language provided by the base voice?
			p = base_voice->languages;
			while (*p++ != 0) {
				if (strcmp(p, language) == 0) {
					// yes, change the language to the main language of the base voice
					strcpy(language, &base_voice->languages[1]);
					break;
				}
				p += (strlen(p) + 1);
			}

			if (voice_name_specified == 0)
				voice_name[0] = 0; // forget a previous voice name if a language is specified
		}
		if (sp->voice_gender != ENGENDER_UNKNOWN)
			voice_select.gender = sp->voice_gender;

		if (sp->voice_age != 0)
			voice_select.age = sp->voice_age;
		if (sp->voice_variant_number != 0)
			voice_select.variant = sp->voice_variant_number;
	}

	voice_select.name = voice_name;
	voice_select.languages = language;
	v_id = SelectVoice(&voice_select, &voice_found);
	if (v_id == NULL)
		return "default";

	if ((strchr(v_id, '+') == NULL) && ((voice_select.gender == ENGENDER_UNKNOWN) || (voice_select.gender == base_voice->gender)) && (base_voice_variant_name[0] != 0)) {
		// a voice variant has not been selected, use the original voice variant
		sprintf(buf, "%s+%s", v_id, base_voice_variant_name);
		strncpy0(voice_name, buf, sizeof(voice_name));
		return voice_name;
	}
	return v_id;
}

void ProcessParamStack(char *outbuf, int *outix, int n_param_stack, PARAM_STACK *param_stack, int *speech_parameters)
{
	// Set the speech parameters from the parameter stack
	int param;
	int ix;
	int value;
	char buf[20];
	int new_parameters[N_SPEECH_PARAM];
	static char cmd_letter[N_SPEECH_PARAM] = { 0, 'S', 'A', 'P', 'R', 0, 'C', 0, 0, 0, 0, 0, 'F' }; // embedded command letters

	for (param = 0; param < N_SPEECH_PARAM; param++)
		new_parameters[param] = -1;

	for (ix = 0; ix < n_param_stack; ix++) {
		for (param = 0; param < N_SPEECH_PARAM; param++) {
			if (param_stack[ix].parameter[param] >= 0)
				new_parameters[param] = param_stack[ix].parameter[param];
		}
	}

	for (param = 0; param < N_SPEECH_PARAM; param++) {
		if ((value = new_parameters[param]) != speech_parameters[param]) {
			buf[0] = 0;

			switch (param)
			{
			case espeakPUNCTUATION:
				option_punctuation = value-1;
				break;
			case espeakCAPITALS:
				option_capitals = value;
				break;
			case espeakRATE:
			case espeakVOLUME:
			case espeakPITCH:
			case espeakRANGE:
			case espeakEMPHASIS:
				sprintf(buf, "%c%d%c", CTRL_EMBEDDED, value, cmd_letter[param]);
				break;
			}

			speech_parameters[param] = new_parameters[param];
			strcpy(&outbuf[*outix], buf);
			*outix += strlen(buf);
		}
	}
}
PARAM_STACK *PushParamStack(int tag_type, int *n_param_stack, PARAM_STACK *param_stack)
{
	int ix;
	PARAM_STACK *sp;

	sp = &param_stack[*n_param_stack];
	if (*n_param_stack < (N_PARAM_STACK-1))
		*n_param_stack = *n_param_stack + 1;

	sp->type = tag_type;
	for (ix = 0; ix < N_SPEECH_PARAM; ix++)
		sp->parameter[ix] = -1;
	return sp;
}

void PopParamStack(int tag_type, char *outbuf, int *outix, int *n_param_stack, PARAM_STACK *param_stack, int *speech_parameters)
{
	// unwind the stack up to and including the previous tag of this type
	int ix;
	int top = 0;

	if (tag_type >= SSML_CLOSE)
		tag_type -= SSML_CLOSE;

	for (ix = 0; ix < *n_param_stack; ix++) {
		if (param_stack[ix].type == tag_type)
			top = ix;
	}
	if (top > 0)
		*n_param_stack = top;
	ProcessParamStack(outbuf, outix, *n_param_stack, param_stack, speech_parameters);
}

wchar_t *GetSsmlAttribute(wchar_t *pw, const char *name)
{
	// Gets the value string for an attribute.
	// Returns NULL if the attribute is not present

	int ix;
	static wchar_t empty[1] = { 0 };

	while (*pw != 0) {
		if (iswspace(pw[-1])) {
			ix = 0;
			while (*pw == name[ix]) {
				pw++;
				ix++;
			}
			if (name[ix] == 0) {
				// found the attribute, now get the value
				while (iswspace(*pw)) pw++;
				if (*pw == '=') pw++;
				while (iswspace(*pw)) pw++;
				if ((*pw == '"') || (*pw == '\'')) // allow single-quotes ?
					return pw+1;
				else
					return empty;
			}
		}
		pw++;
	}
	return NULL;
}

int ReplaceKeyName(char *outbuf, int index, int *outix)
{
	// Replace some key-names by single characters, so they can be pronounced in different languages
	static MNEM_TAB keynames[] = {
		{ "space ",        0xe020 },
		{ "tab ",          0xe009 },
		{ "underscore ",   0xe05f },
		{ "double-quote ", '"' },
		{ NULL,            0 }
	};

	int ix;
	int letter;
	char *p;

	p = &outbuf[index];

	if ((letter = LookupMnem(keynames, p)) != 0) {
		ix = utf8_out(letter, p);
		*outix = index + ix;
		return letter;
	}
	return 0;
}

