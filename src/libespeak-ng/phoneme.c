/*
 * Copyright (C) 2016 Reece H. Dunn
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

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include "phoneme.h"
#include "speech.h"
#include "error.h"

enum feature_t {
	inv, // invalid phoneme feature name
};

static MNEM_TAB features[] = {
	{ NULL,  inv },
};

espeak_ng_STATUS
phoneme_add_feature(PHONEME_TAB *phoneme,
                    const char *feature,
                    espeak_ng_ERROR_CONTEXT *context)
{
	if (!phoneme || !feature) return EINVAL;

	switch (LookupMnem(features, feature))
	{
	default:
		return create_name_error_context(context, ENS_UNKNOWN_PHONEME_FEATURE, feature);
	}
	return ENS_OK;
}
