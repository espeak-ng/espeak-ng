/***************************************************************************
 *   Copyright (C) 2007 by Jonathan Duddington                             *
 *   jonsd@users.sourceforge.net                                           *
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
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"




int Translator::ChangePhonemes(PHONEME_LIST2 *phlist, int n_ph, int index, PHONEME_TAB *ph, CHANGEPH *ch)
{//======================================================================================================
// Called for each phoneme in the phoneme list, to allow a language to make changes
// flags: bit 0=1 last phoneme in a word
//        bit 1=1 this is the highest stressed vowel in the current word
//        bit 2=1 after the highest stressed vowel in the current word
//        bit 3=1 the phonemes were specified explicitly, or found from an entry in the xx_list dictionary
// ph     The current phoneme
	return(0);
}


int Translator::SubstitutePhonemes(PHONEME_LIST2 *plist_out)
{//=========================================================
// Copy the phonemes list and perform any substitutions that are required for the
// current voice
	int ix;
	int k;
	int replace_flags;
	int n_plist_out = 0;
	int word_end;
	int max_stress = -1;
	int switched_language = 0;
	int max_stress_posn=0;
	int n_syllables = 0;
	int syllable = 0;
	int syllable_stressed = 0;
	PHONEME_LIST2 *plist2;
	PHONEME_LIST2 *pl;
	PHONEME_TAB *next=NULL;

	for(ix=0; (ix < n_ph_list2) && (n_plist_out < N_PHONEME_LIST); ix++)
	{
		plist2 = &ph_list2[ix];

		if(plist2->phcode == phonSWITCH)
			switched_language ^= 1;

		// don't do any substitution if the language has been temporarily changed
		if(switched_language == 0)
		{
			if(ix < (n_ph_list2 -1))
				next = phoneme_tab[ph_list2[ix+1].phcode];
	
			word_end = 0;
			if((plist2+1)->sourceix || ((next != 0) && (next->type == phPAUSE)))
				word_end = 1;        // this phoneme is the end of a word
	
			if(langopts.phoneme_change != 0)
			{
				// this language does changes to phonemes after translation

				if(plist2->sourceix)
				{
					// start of a word, find the stressed vowel
					syllable = 0;
					syllable_stressed = 0;
					n_syllables = 0;

					max_stress = -1;
					max_stress_posn = ix;
					for(k=ix; k < n_ph_list2; k++)
					{
						if(((pl = &ph_list2[k])->sourceix != 0) && (k > ix))
							break;
		
						pl->stress &= 0xf;
		
						if(phoneme_tab[pl->phcode]->type == phVOWEL)
						{
							n_syllables++;

							if(pl->stress  > max_stress)
							{
								syllable_stressed = n_syllables;
								max_stress = pl->stress;
								max_stress_posn = k;
							}
						}
					}
				}

				if(phoneme_tab[plist2->phcode]->type == phVOWEL)
				{
					syllable++;
				}
	
				// make any language specific changes
				int flags;
				CHANGEPH ch;
				flags = 0;
				if(ix == max_stress_posn)
					flags |= 2;
				if(ix > max_stress_posn)
					flags |= 4;
				if(ph_list2[ix].synthflags & SFLAG_DICTIONARY)
					flags |= 8;
				ch.flags = flags | word_end;

				ch.stress = plist2->stress;
				ch.stress_highest = max_stress;
				ch.n_vowels = n_syllables;
				ch.vowel_this = syllable;
				ch.vowel_stressed = syllable_stressed;

				ChangePhonemes(ph_list2, n_ph_list2, ix, phoneme_tab[ph_list2[ix].phcode], &ch);
			}
	
			// check whether a Voice has specified that we should replace this phoneme
			for(k=0; k<n_replace_phonemes; k++)
			{
				if(plist2->phcode == replace_phonemes[k].old_ph)
				{
					replace_flags = replace_phonemes[k].type;
	
					if((replace_flags & 1) && (word_end == 0))
						continue;     // this replacement only occurs at the end of a word
	
					if((replace_flags & 2) && ((plist2->stress & 0x7) > 3))
						continue;     // this replacement doesn't occur in stressed syllables
	
					// substitute the replacement phoneme
					plist2->phcode = replace_phonemes[k].new_ph;
					break;
				}
			}
	
			if(plist2->phcode == 0)
			{
				continue;   // phoneme has been replaced by NULL, so don't copy it
			}
		}

		// copy phoneme into the output list
		memcpy(&plist_out[n_plist_out++],plist2,sizeof(PHONEME_LIST2));
	}
	return(n_plist_out);
}  //  end of SubstitutePhonemes



void Translator::MakePhonemeList(int post_pause, int start_sentence)
{//============================================================================================

	int  ix=0;
	int  j;
	int  insert_ph = 0;
	int  insert_synthflags = 0;
	PHONEME_LIST *phlist;
	PHONEME_TAB *ph;
	PHONEME_TAB *prev, *next, *next2;
	int unstress_count = 0;
	int word_has_stress = 0;
	int switched_language = 0;
	int max_stress;
	int voicing;
	int regression;
	int end_sourceix;
	int alternative;
	PHONEME_LIST2 ph_list3[N_PHONEME_LIST];

	static PHONEME_LIST2 ph_list2_null = {0,0,0,0,0};
	PHONEME_LIST2 *plist2 = &ph_list2_null;

	phlist = phoneme_list;
	end_sourceix = ph_list2[n_ph_list2-1].sourceix;

	// is the last word of the clause unstressed ?
	max_stress = 0;
	for(j=n_ph_list2-3; j>=0; j--)
	{
		// start with the last phoneme (before the terminating pauses) and move forwards
		if((ph_list2[j].stress & 0x7f) > max_stress)
			max_stress = ph_list2[j].stress & 0x7f;
		if(ph_list2[j].sourceix != 0)
			break;
	}
	if(max_stress < 4)
	{
		// the last word is unstressed, look for a previous word that can be stressed
		while(--j >= 0)
		{
			if(ph_list2[j].stress & 0x80)  // dictionary flags indicated that this stress can be promoted
			{
				ph_list2[j].stress = 4;   // promote to stressed
				break;
			}
			if((ph_list2[j].stress & 0x7f) >= 4)
			{
				// found a stressed syllable, so stop looking
				break;
			}
		}
	}

	if((regression = langopts.param[LOPT_REGRESSIVE_VOICING]) != 0)
	{
		// set consonant clusters to all voiced or all unvoiced
		// Regressive
		int type;
		voicing = 0;

		for(j=n_ph_list2-1; j>=0; j--)
		{
			ph = phoneme_tab[ph_list2[j].phcode];
			if(ph == NULL)
				continue;

			if(ph->code == phonSWITCH)
				switched_language ^= 1;
			if(switched_language)
				continue;

			type = ph->type;

			if(regression & 0x2)
			{
				// LANG=Russian, [v] amd [v;] don't cause regression, or [R^]
				if((ph->mnemonic == 'v') || (ph->mnemonic == ((';'<<8)+'v')) || ((ph->mnemonic & 0xff)== 'R'))
					type = phLIQUID;
			}

			if((type==phSTOP) || type==(phFRICATIVE))
			{
				if(voicing==0)
				{
					voicing = 1;
				}
				else
				if((voicing==2) && ((ph->phflags & phALTERNATIVE)==phSWITCHVOICING))
				{
					ph_list2[j].phcode = ph->alternative_ph;  // change to voiced equivalent
				}
			}
			else
			if((type==phVSTOP) || type==(phVFRICATIVE))
			{
				if(voicing==0)
				{
					voicing = 2;
				}
				else
				if((voicing==1) && ((ph->phflags & phALTERNATIVE)==phSWITCHVOICING))
				{
					ph_list2[j].phcode = ph->alternative_ph;  // change to unvoiced equivalent
				}
			}
			else
			{
				if(regression & 0x8)
				{
					// LANG=Polish, propagate through liquids and nasals
					if((type == phPAUSE) || (type == phVOWEL))
						voicing = 0;
				}
				else
				{
					voicing = 0;
				}
			}
			if((regression & 0x4) && (ph_list2[j].sourceix))
			{
				// stop propagation at a word boundary
				voicing = 0;
			}
		}
	}

	n_ph_list2 = SubstitutePhonemes(ph_list3) - 2;

	// transfer all the phonemes of the clause into phoneme_list
	ph = phoneme_tab[phonPAUSE];
	switched_language = 0;

	for(j=0; insert_ph || ((j<n_ph_list2) && (ix < N_PHONEME_LIST-3)); j++)
	{
		prev = ph;

		plist2 = &ph_list3[j];

		if(insert_ph != 0)
		{
			// we have a (linking) phoneme which we need to insert here
			j--;
			next = phoneme_tab[plist2->phcode];      // this phoneme, i.e. after the insert
			plist2 = &ph_list2_null;
			ph = phoneme_tab[insert_ph];
			plist2->synthflags = insert_synthflags;
			insert_ph = 0;
			insert_synthflags = 0;
		}
		else
		{
			// otherwise get the next phoneme from the list
			ph = phoneme_tab[plist2->phcode];

			if(plist2->phcode == phonSWITCH)
			{
				// change phoneme table
				SelectPhonemeTable(plist2->tone_number);
				switched_language ^= SFLAG_SWITCHED_LANG;
			}
			next = phoneme_tab[(plist2+1)->phcode];      // the phoneme after this one
		}

		if(plist2->sourceix)
			word_has_stress = 0;   // start of a word

		if(ph == NULL) continue;

		if(ph->type == phVOWEL)
		{
			// check for consecutive unstressed syllables
			if(plist2->stress == 0)
			{
				// an unstressed vowel
				unstress_count++;
				if((unstress_count > 1) && ((unstress_count & 1)==0))
				{
					// in a sequence of unstressed syllables, reduce alternate syllables to 'diminished'
               // stress.  But not for the last phoneme of a stressed word
					if((langopts.stress_flags & 0x2) || (word_has_stress && ((plist2+1)->sourceix!=0)))
					{
						// An unstressed final vowel of a stressed word
						unstress_count=1;    // try again for next syllable
					}
					else
					{
						plist2->stress = 1;    // change stress to 'diminished'
					}
				}
			}
			else
			{
				unstress_count = 0;
				if(plist2->stress > 3)
					word_has_stress = 1;   // word has a primary or a secondary stress
			}
		}

		if(ph->alternative_ph > 0)
		{
			alternative = 0;

			switch(ph->phflags & phALTERNATIVE)
			{
			// This phoneme changes if vowel follows, or doesn't follow, depending on its phNOTFOLLOWS flag
			case phBEFORENOTVOWEL:
				if(next->type != phVOWEL)
					alternative = ph->alternative_ph;
				break;

			case phBEFOREVOWELPAUSE:
				if((next->type == phVOWEL) || (next->type == phPAUSE))
					alternative = ph->alternative_ph;
				break;

			case phBEFOREVOWEL:
				if(next->type == phVOWEL)
					alternative = ph->alternative_ph;
				break;
			}

			if(alternative == 1)
				continue;    // NULL phoneme, discard

			if(alternative > 1)
			{
				ph = phoneme_tab[alternative];
				if(ph->type == phVOWEL)
					plist2->synthflags |= SFLAG_SYLLABLE;
				else
					plist2->synthflags &= ~SFLAG_SYLLABLE;
			}
		}

#ifdef deleted
// experimenting with [t] glottaling
if((ph->mnemonic == 't') && ((prev->type == phVOWEL) || (prev->mnemonic == 'n')))
{
	if(((plist2+1)->sourceix != 0) || ((plist2-1)->stress == 4) && (next->type == phVOWEL))
	{
		ph = phoneme_tab[phonGLOTTALSTOP];
	}
}
#endif

		if((ph->reduce_to != 0) && (ph->type != phVOWEL) && !(plist2->synthflags & SFLAG_DICTIONARY))
		{
			// reduction for vowels has already been done in SetWordStress
			int reduce_level;

			if(next->type == phVOWEL)
			{
				reduce_level = (ph->phflags >> 28) & 7;
				if((&plist2[1])->stress < reduce_level)
				{
					// look at the stress of the following vowel
					ph = phoneme_tab[ph->reduce_to];
				}
			}
		}

		if((plist2+1)->synthflags & SFLAG_LENGTHEN)
		{
			static char types_double[] = {phFRICATIVE,phVFRICATIVE,phNASAL,phLIQUID,0};
			if(strchr(types_double,next->type))
			{
				// lengthen this consonant by doubling it
				insert_ph = next->code;
				(plist2+1)->synthflags ^= SFLAG_LENGTHEN;
			}
		}

		if((plist2+1)->sourceix != 0)
		{
			int x;

			if(langopts.word_gap & 1)
			{
				insert_ph = phonPAUSE_VSHORT;
			}
			if(langopts.vowel_pause && (ph->type != phPAUSE) && (next->type == phVOWEL))
			{
				if(langopts.vowel_pause & 0x04)
				{
					// break before a word which starts with a vowel
					insert_ph = phonPAUSE_VSHORT;
				}

				if((ph->type == phVOWEL) && ((x = langopts.vowel_pause & 0x03) != 0))
				{
					// adjacent vowels over a word boundary
					if(x == 2)
						insert_ph = phonPAUSE_SHORT;
					else
						insert_ph = phonPAUSE_VSHORT;
				}

				if(((plist2+1)->stress >= 4) && (langopts.vowel_pause & 0x08))
				{
					// pause before a words which starts with a stressed vowel
					insert_ph = phonPAUSE_SHORT;
				}
			}
		}

		next2 = phoneme_tab[(plist2+2)->phcode];

#ifdef deleted
		if((ph->type != phVOWEL)  && (ph->type != phPAUSE)   //&& ((ph->phflags & phVOICED)==0)
			&& (next->type == phLIQUID) && (next->mnemonic != ';') && (next2->type != phVOWEL))
		{
			// semi-vowel surrounded by consonants.  precede by a short schwa
			insert_ph = phonSCHWA_SHORT;
			insert_synthflags = SFLAG_SYLLABLE;
		}

		if((ph->type == phLIQUID) && (prev->type != phVOWEL) && (next->type != phVOWEL))
		{
			// semi-vowel surrounded by consonants.  precede by a short schwa
			insert_ph = phonSCHWA_SHORT;
			insert_synthflags = SFLAG_SYLLABLE;
		}
#endif

		if((insert_ph == 0) && (ph->link_out != 0) && (((plist2+1)->synthflags & SFLAG_EMBEDDED)==0))
		{
			if(ph->phflags & phAPPENDPH)
			{
				// always append the specified phoneme, unless it already is the next phoneme
				if((ph->link_out != (plist2+1)->phcode) && (next->type == phVOWEL))
//				if(ph->link_out != (plist2+1)->phcode)
				{
					insert_ph = ph->link_out;
				}
			}
			else
			if(((langopts.word_gap & 2)==0) || ((plist2+1)->sourceix == 0))
			{
				// This phoneme can be linked to a following vowel by inserting a linking phoneme
				if(next->type == phVOWEL)
					insert_ph = ph->link_out;
				else
				if(next->code == phonPAUSE_SHORT)
				{
					// Pause followed by Vowel, replace the Short Pause with the linking phoneme,
					if(next2->type == phVOWEL)
						(plist2+1)->phcode = ph->link_out;  // replace pause by linking phoneme
				}
			}
		}

		if(ph->phflags & phVOICED)
		{
			// check that a voiced consonant is preceded or followed by a vowel or liquid
			// and if not, add a short schwa

			// not yet implemented
		}

		phlist[ix].ph = ph;
		phlist[ix].type = ph->type;
		phlist[ix].env = PITCHfall;          // default, can be changed in the "intonation" module
		phlist[ix].synthflags = plist2->synthflags | switched_language;
		phlist[ix].tone = plist2->stress & 0xf;
		phlist[ix].tone_ph = plist2->tone_number;
		phlist[ix].sourceix = 0;

		if(plist2->sourceix != 0)
		{
			phlist[ix].sourceix = plist2->sourceix;
			phlist[ix].newword = 1;     // this phoneme is the start of a word

			if(start_sentence)
			{
				phlist[ix].newword = 5;  // start of sentence + start of word
				start_sentence = 0;
			}
		}
		else
		{
			phlist[ix].newword = 0;
		}

		phlist[ix].length = ph->std_length;

		if(ph->type==phVOWEL || ph->type==phLIQUID || ph->type==phNASAL || ph->type==phVSTOP || ph->type==phVFRICATIVE)
		{
			phlist[ix].length = 128;  // length_mod
			phlist[ix].env = PITCHfall;
		}

		phlist[ix].prepause = 0;
		phlist[ix].amp = 20;          // default, will be changed later
		phlist[ix].pitch1 = 0x400;
		phlist[ix].pitch2 = 0x400;
		ix++;
	}
	phlist[ix].newword = 2;     // end of clause

   phlist[ix].type = phPAUSE;  // terminate with 2 Pause phonemes
	phlist[ix].length = post_pause;  // length of the pause, depends on the punctuation
	phlist[ix].sourceix = end_sourceix;
	phlist[ix].synthflags = 0;

   phlist[ix++].ph = phoneme_tab[phonPAUSE];
   phlist[ix].type = phPAUSE;
	phlist[ix].length = 0;
	phlist[ix].sourceix=0;
	phlist[ix].synthflags = 0;
   phlist[ix++].ph = phoneme_tab[phonPAUSE_VSHORT];

	n_phoneme_list = ix;
}  // end of MakePhonemeList


