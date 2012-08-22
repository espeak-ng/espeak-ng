/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
 *   jsd@clara.co.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"




Translator_German::Translator_German() : Translator()
{//===================================
// Initialise options for this language
	const char *p;

	// set locale to ensure 8 bit character set, for isalpha(), tolower() etc
	prev_locale[0] = 0;
	if((p = setlocale(LC_CTYPE,"german")) != NULL)
		strncpy(prev_locale,p,sizeof(prev_locale));  // keep copy of previous locale

	const int stress_lengths2[8] = {170,140, 220,220, 220,240, 248,250};
	option_stress_rule = 0;
	option_vowel_pause = 1;
	option_words = 2;

	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
}

