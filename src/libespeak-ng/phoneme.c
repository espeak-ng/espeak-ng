/*
 * Copyright (C) 2017 Reece H. Dunn
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
#include <string.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include "phoneme.h"
#include "error.h"

#define FEATURE(a, b, c) ((a << 16) | (b << 8) | (c))

// See docs/phonemes.md for the list of supported features.
enum feature_t {
	// invalid phoneme feature name
	inv, // Not in docs/phonemes.md. This is used to signal an unknown feature name.
	// manner of articulation
	nas = FEATURE('n', 'a', 's'),
	stp = FEATURE('s', 't', 'p'),
	afr = FEATURE('a', 'f', 'r'),
	frc = FEATURE('f', 'r', 'c'),
	flp = FEATURE('f', 'l', 'p'),
	trl = FEATURE('t', 'r', 'l'),
	apr = FEATURE('a', 'p', 'r'),
	clk = FEATURE('c', 'l', 'k'),
	ejc = FEATURE('e', 'j', 'c'),
	imp = FEATURE('i', 'm', 'p'),
	vwl = FEATURE('v', 'w', 'l'),
	lat = FEATURE('l', 'a', 't'),
	sib = FEATURE('s', 'i', 'b'),
	// place of articulation
	blb = FEATURE('b', 'l', 'b'),
	lbd = FEATURE('l', 'b', 'd'),
	bld = FEATURE('b', 'l', 'd'),
	dnt = FEATURE('d', 'n', 't'),
	alv = FEATURE('a', 'l', 'v'),
	pla = FEATURE('p', 'l', 'a'),
	rfx = FEATURE('r', 'f', 'x'),
	alp = FEATURE('a', 'l', 'p'),
	pal = FEATURE('p', 'a', 'l'),
	vel = FEATURE('v', 'e', 'l'),
	lbv = FEATURE('l', 'b', 'v'),
	uvl = FEATURE('u', 'v', 'l'),
	phr = FEATURE('p', 'h', 'r'),
	glt = FEATURE('g', 'l', 't'),
	// voice
	vcd = FEATURE('v', 'c', 'd'),
	vls = FEATURE('v', 'l', 's'),
};

uint32_t lookup_feature(const char *feature) {
	if (strlen(feature) != 3)
		return inv;
	return FEATURE(feature[0], feature[1], feature[2]);
}

espeak_ng_STATUS
phoneme_add_feature(PHONEME_TAB *phoneme,
                    const char *feature,
                    espeak_ng_ERROR_CONTEXT *context)
{
	if (!phoneme || !feature) return EINVAL;

	switch (lookup_feature(feature))
	{
	// manner of articulation
	case nas:
		phoneme->type = phNASAL;
		break;
	case stp:
	case afr: // FIXME: eSpeak treats 'afr' as 'stp'.
		phoneme->type = phSTOP;
		break;
	case frc:
	case apr: // FIXME: eSpeak is using this for [h], with 'liquid' used for [l] and [r].
		phoneme->type = phFRICATIVE;
		break;
	case flp: // FIXME: Why is eSpeak using a vstop (vcd + stp) for this?
		phoneme->type = phVSTOP;
		break;
	case trl: // FIXME: 'trill' should be the type; 'liquid' should be a flag (phoneme files specify both).
		phoneme->phflags |= phTRILL;
		break;
	case clk:
	case ejc:
	case imp:
	case lat:
		// Not supported by eSpeak.
		break;
	case vwl:
		phoneme->type = phVOWEL;
		break;
	case sib:
		phoneme->phflags |= phSIBILANT;
		break;
	// place of articulation
	case blb:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 1 << 16;
		break;
	case lbd:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 2 << 16;
		break;
	case dnt:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 3 << 16;
		break;
	case alv:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 4 << 16;
		break;
	case rfx:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 5 << 16;
		break;
	case pla:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 6 << 16;
		break;
	case pal:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 7 << 16;
		break;
	case vel:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 8 << 16;
		break;
	case lbv:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 9 << 16;
		break;
	case uvl:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 10 << 16;
		break;
	case phr:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 11 << 16;
		break;
	case glt:
		phoneme->phflags &= ~phARTICULATION;
		phoneme->phflags |= 12 << 16;
		break;
	case bld:
		// FIXME: Not supported by eSpeak. Used in German p͡f.
		break;
	case alp:
		// FIXME: Not supported by eSpeak. Used in Chinese/Japanese ɕ and ʑ.
		break;
	// voice
	case vcd:
		phoneme->phflags |= phVOICED;
		break;
	case vls:
		phoneme->phflags |= phVOICELESS;
		break;
	// invalid phoneme feature
	default:
		return create_name_error_context(context, ENS_UNKNOWN_PHONEME_FEATURE, feature);
	}
	return ENS_OK;
}
