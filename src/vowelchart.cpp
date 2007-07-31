/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/


#include <math.h>
#include "wx/wx.h"
#include <wx/dcmemory.h>
#include <wx/dc.h>
#include <wx/bitmap.h>
#include <wx/dirdlg.h>
#include "wx/filename.h"
#include "wx/wfstream.h"

#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "voice.h"
#include "spect.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"
#include "options.h"

/* Read a file of vowel symbols and f1,f2 formants, and produce a vowel diagram
*/
extern wxString path_phsource;
extern char voice_name[];

extern char *spects_data;
extern int n_phoneme_tables;
extern PHONEME_TAB_LIST phoneme_tab_list[N_PHONEME_TABS];


// size of the vowelchart png
#define WIDTH  1200
#define HEIGHT 800


#define ROUND(x) ((int) ((x) + 0.5))

static int HslValue (double n1, double n2, double hue)
{//===================================================
	double value;
	
	if (hue > 255)
		hue -= 255;
	else if (hue < 0)
		hue += 255;
	
	if (hue < 42.5)
		value = n1 + (n2 - n1) * (hue / 42.5);
	else if (hue < 127.5)
		value = n2;
	else if (hue < 170)
		value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
	else
		value = n1;
	
	return ROUND (value * 255.0);
}

/**
 * @hue: Hue channel, returns Red channel
 * @saturation: Saturation channel, returns Green channel
 * @lightness: Lightness channel, returns Blue channel
 *
 * The arguments are pointers to int, with the values pointed to in the
 * following ranges:  H [0, 360], L [0, 255], S [0, 255].
 *
 * The function changes the arguments to point to the RGB value
 * corresponding, with the returned values all in the range [0, 255].
 **/
void HslToRgb (int *hue, int *saturation, int *lightness)
{//======================================================
	double h, s, l;

	h = (*hue * 256)/360;
	s = *saturation;
	l = *lightness;

	if (s == 0)
	{
		/*  achromatic case  */
		*hue        = (int)l;
		*lightness  = (int)l;
		*saturation = (int)l;
	}
	else
	{
		double m1, m2;

		if (l < 128)
			m2 = (l * (255 + s)) / 65025.0;
		else
			m2 = (l + s - (l * s) / 255.0) / 255.0;

		m1 = (l / 127.5) - m2;

		/*  chromatic case  */
		*hue        = HslValue (m1, m2, h + 85);
		*saturation = HslValue (m1, m2, h);
		*lightness  = HslValue (m1, m2, h - 85);
	}
}


static int vowel_posn[N_PHONEME_TAB];
static int vowel_posn_ix;


static double log2a(double x)
{//========================
// log2(x) = log(x) / log(2)
	return(log(x) / 0.693147);
}

static int VowelX(int f2)
{//======================
	return(WIDTH - int((log2a(f2) - 9.49)*WIDTH/1.8));
//	return(1024 - int((log2a(f2) - 9.50)*1020/1.8));
}

static int VowelY(int f1)
{//======================
	return(int((log2a(f1) - 7.85)*HEIGHT/2.15));
}

static int VowelZ(int f3)
{//======================
	int z;
// range 2000-3000Hz, log2= 10.96 to 11.55
	z = int((log2a(f3) - 11.05)*256/0.50);
	if(z < 0) z = 0;
	if(z > 255) z = 255;
	return(z);
}


static void DrawVowel(wxDC *dc, wxString name, int f1, int f2, int f3, int g1, int g2)
{//==================================================================================
	int ix;
	int posn;
	int collisions;
	int x,y,z,x2,y2;
	int r,g,b;
	wxBrush brush;


	y = VowelY(f1);
	x = VowelX(f2);
	z = VowelZ(f3);

	if(y < 0) y = 0;
	if(y > (HEIGHT-4)) y= (HEIGHT-4);
	if(x < 0) x = 0;
	if(x > (WIDTH-8)) x = (WIDTH-8);

	r = z;
	g = 255;
	b = 100 + z/2;
	HslToRgb(&r,&g,&b);
	brush.SetColour(r,g,b);
	dc->SetBrush(brush);
	dc->DrawCircle(x,y,4);

	// check for a label already at this position
	collisions = 0;
	posn = (x/8)*WIDTH + (y/8);
	for(ix=0; ix<vowel_posn_ix; ix++)
	{
		if(posn == vowel_posn[ix])
			collisions++;
	}
	vowel_posn[vowel_posn_ix++] = posn;

	dc->DrawText(name,x+4,y+(collisions*10));

	if(g2 != 0xffff)
	{
		y2 = VowelY(g1);
		x2 = VowelX(g2);	
		dc->DrawLine(x,y,x2,y2);
	}
}



static int VowelChartDir(wxDC *dc, wxBitmap *bitmap)
{//=================================================
	int ix;
	int nf;
	int count = 0;
	SpectSeq *spectseq;
	SpectFrame *frame1;
	SpectFrame *frame2=NULL;
	wxFileName filename;

	wxString dir = wxDirSelector(_T("Directory of vowel files"),path_phsource);
	if(dir.IsEmpty()) return(0);

	wxString path = wxFindFirstFile(dir+_T("/*"),wxFILE);

	while (!path.empty())
	{
		if((spectseq = new SpectSeq) == NULL) break;

		filename = wxFileName(path);
		wxFileInputStream stream(path);
		if(stream.Ok() == FALSE)
		{
			path = wxFindNextFile();
			continue;
		}
		spectseq->Load(stream);

		nf = 0;
		frame1 = NULL;

		if(spectseq->numframes > 0)
		{
			frame2 = spectseq->frames[0];
		}
		for(ix=0; ix<spectseq->numframes; ix++)
		{
			if(spectseq->frames[ix]->keyframe)
			{
				nf++;
				frame2 = spectseq->frames[ix];
				if(frame2->markers & FRFLAG_VOWEL_CENTRE)
					frame1 = frame2;
			}
		}
		if((nf >= 3) && (frame1 != NULL))
		{
			DrawVowel(dc,wxString(filename.GetName()),
				frame1->peaks[1].pkfreq, frame1->peaks[2].pkfreq, frame1->peaks[3].pkfreq, 
				frame2->peaks[1].pkfreq, frame2->peaks[2].pkfreq);

			count++;
		}
		delete spectseq;
		path = wxFindNextFile();
	}
	filename.SetPath(dir);
	filename.SetFullName(_T("vowelchart.png"));
	bitmap->SaveFile(filename.GetFullPath(),wxBITMAP_TYPE_PNG);
	return(count);
}


static int VowelChartList(wxDC *dc, wxBitmap *bitmap, char *fname)
{//===============================================================
// Plot a graph of vowel formants.
// y-axis  is decreasing f1  (closeness)
// x-axis is decreasing f2  (backness)
	FILE *f_in;
	int ix;
	int f1,f2,f3,g1,g2;
	int count=0;
	wxFileName filename;
	char name[40];
	char buf[120];
	wxString path;

	if(fname != NULL)
	{
		path = wxString(fname,wxConvLocal);
	}
	else
	{
		path = wxFileSelector(_T("Read file of vowel formants"),path_phsource,
			_T(""),_T(""),_T("*"),wxOPEN);
	}
	if(path.IsEmpty())
	{
		return(0);
	}

	filename = wxFileName(path);
	strcpy(buf,path.mb_str(wxConvLocal));
	f_in = fopen(buf,"r");
	if(f_in == NULL)
	{
		wxLogError(_T("Can't read file: %s"),buf);
		return(0);
	}


	while(fgets(buf,sizeof(buf),f_in) != NULL)
	{
		g2 = 0xffff;
		ix = sscanf(buf,"%s %d %d %d %d %d",name,&f1,&f2,&f3,&g1,&g2);
		if(ix >= 3)
		{
			DrawVowel(dc,wxString(name,wxConvLocal),
				f1,f2,f3,g1,g2);
			count++;
		}
	}

	filename.SetExt(_T("png"));
	bitmap->SaveFile(filename.GetFullPath(),wxBITMAP_TYPE_PNG);
	return(count);
}


void VowelChart(int control, char *fname)
{//======================================
// Plot a graph of vowel formants.
// y-axis  is decreasing f1  (closeness)
// x-axis is decreasing f2  (backness)
// control=1  from directory of lists
// control=2  from single list
// control=3  from directory of phoneme source data files

	int ix;
	int x,y;
	int count;
	wxFileName filename;

	wxBitmap bitmap(WIDTH,HEIGHT);

	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(bitmap);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetFont(*wxSWISS_FONT);
	dc.Clear();

	// draw grid
	dc.SetPen(*wxLIGHT_GREY_PEN);
	for(ix=200; ix<=1000; ix+=50)
	{
		y = VowelY(ix);
		dc.DrawLine(0,y,WIDTH,y);
		if((ix % 100) == 0)
			dc.DrawText(wxString::Format(_T("%d"),ix),1,y);
	}
	for(ix=800; ix<=2400; ix+=100)
	{
		x = VowelX(ix);
		dc.DrawLine(x,0,x,HEIGHT);
		if((ix % 200)==0)
			dc.DrawText(wxString::Format(_T("%d"),ix),x+1,0);
	}
	dc.SetPen(*wxBLACK_PEN);
	
	vowel_posn_ix = 0;
	if(control==3)
		count = VowelChartDir(&dc, &bitmap);
	else
		count = VowelChartList(&dc, &bitmap, fname);

	if(control != 1)
		wxLogStatus(_T("Plotted %d vowels"),count);
}



void FindPhonemesUsed(void)
{//========================
	int hash;
	char *p;
	char *start;
	char *next;
	unsigned char c;
	int count = 0;

	// look through all the phoneme strings in the **_rules data
	// and mark these phoneme codes as used.
	p = translator->data_dictrules;
	while(*p != 0)
	{
		if(*p == RULE_CONDITION)
			p+=2;
		if(*p == RULE_GROUP_END)
		{
			p++;
			if(*p == 0) break;
		}
		if(*p == RULE_GROUP_START)
		{
			if(p[1] == RULE_LETTERGP2)
			{
				while(*p != RULE_GROUP_END) p++;
				continue;
			}

			p += (strlen(p)+1);
		}

		while((((c = *p) != RULE_PHONEMES)) && (c != 0)) p++;
		count++;
		if(c == RULE_PHONEMES)
		{
			start = p;
			p++;
			while(*p != 0)
			{
				phoneme_tab_flags[*p & 0xff] |= 2;
				p++;
			}
		}
		p++;
	}

	for(hash=0; hash<N_HASH_DICT; hash++)
	{
		p = translator->dict_hashtab[hash];
		if(p == NULL)
			continue;

		while(*p != 0)
		{
			next = p + p[0];
			if((p[1] & 0x80) == 0)
			{
				p += ((p[1] & 0x3f) + 2);
				while(*p != 0)
				{
					phoneme_tab_flags[*p & 0xff] |= 2;
					p++;
				}
			}
			p = next;
		}
	}
}   // end of FindPhonemesUsed



void MakeVowelLists(void)
{//======================
// For each phoneme table, make a list of its vowels and their
// formant frequencies (f1,f2,f3) for use by VowelChart()

	int table;
	int ix;
	int phcode;
	PHONEME_TAB *ph;
	FILE *f;
	SPECT_SEQ *seq;
	frame_t *frame;
	int match_level;
	char dirname[sizeof(path_source)+20];
	char fname[sizeof(dirname)+40];

	progress = new wxProgressDialog(_T("Vowel charts"),_T(""),n_phoneme_tables);

	sprintf(dirname,"%s%s",path_source,"vowelcharts");
	mkdir(dirname,S_IRWXU | S_IRGRP | S_IROTH);

	for(table=0; table<n_phoneme_tables; table++)
	{
		sprintf(fname,"%s/%s",dirname,phoneme_tab_list[table].name);
		if((f = fopen(fname,"w"))==NULL) continue;

		progress->Update(table);

		// select the phoneme table by name
//		if(SetVoiceByName(phoneme_tab_list[table].name) != 0) continue;
		if(SelectPhonemeTableName(phoneme_tab_list[table].name) < 0) continue;
		FindPhonemesUsed();

		// phoneme table is terminated by a phoneme with no name (=0)
		for(phcode=1; phcode < n_phoneme_tab; phcode++)
		{
//if((phoneme_tab_flags[phcode] & 3) == 0)
//	continue;   // inherited, and not used 

			ph = phoneme_tab[phcode];

			if(ph->type != phVOWEL)
				continue;

			if((ix = LookupSound(ph, phoneme_tab[phonPAUSE], 1, &match_level, 0)) == 0)
				continue;

			seq = (SPECT_SEQ *)(&spects_data[ix]);
			frame = &seq->frame[1];
			fprintf(f,"%s\t %3d %4d %4d",WordToString(ph->mnemonic),
					frame->ffreq[1],frame->ffreq[2],frame->ffreq[3]);
			frame = &seq->frame[seq->n_frames-1];
			fprintf(f,"   %3d %4d %4d\n",frame->ffreq[1],frame->ffreq[2],frame->ffreq[3]);
		}
		fclose(f);

		VowelChart(1,fname);  // draw the vowel chart
	}
	LoadVoice(voice_name,0);  // reset the original phoneme table
	delete progress;
}
