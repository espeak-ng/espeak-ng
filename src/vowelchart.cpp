/***************************************************************************
 *   Copyright (C) 2005 to 2013 by Jonathan Duddington                     *
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
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "translate.h"
#include "options.h"

/* Read a file of vowel symbols and f1,f2 formants, and produce a vowel diagram
*/
extern wxString path_phsource;

extern char *phondata_ptr;
extern USHORT *phoneme_index;
extern int n_phoneme_tables;


// size of the vowelchart png
#define WIDTH  1580
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
	return(WIDTH - int((log2a(f2) - 9.40)*WIDTH/1.9));
//	return(WIDTH - int((log2a(f2) - 9.49)*WIDTH/1.8));
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
	if(x > (WIDTH-12)) x = (WIDTH-12);

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
	int colour;
	int count=0;
	wxFileName filename;
	char name[40];
	char buf[200];
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
		ix = sscanf(buf,"%s %d %d %d %d %d %d",name,&colour,&f1,&f2,&f3,&g1,&g2);
		if(ix >= 4)
		{
			if(colour == 1)
			{
				dc->SetPen(*wxMEDIUM_GREY_PEN);
				dc->SetTextForeground(wxColour(100,100,128));
			}
			else
			{
				dc->SetPen(*wxBLACK_PEN);
				dc->SetTextForeground(*wxBLACK);
			}

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
	for(ix=700; ix<=2400; ix+=100)
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

	dc.SetTextForeground(*wxBLACK);

	if(control != 1)
		wxLogStatus(_T("Plotted %d vowels"),count);
}



void FindPhonemesUsed(void)
{//========================
	int hash;
	char *p;
	unsigned int *pw;
	char *next;
	unsigned char c;
	int count = 0;
	int ignore;
	char phonetic[N_WORD_PHONEMES];

	// look through all the phoneme strings in the **_rules data
	// and mark these phoneme codes as used.
	p = translator->data_dictrules;
	while(*p != 0)
	{
		if(*p == RULE_CONDITION)
			p+=2;
		if(*p == RULE_LINENUM)
			p+=3;
		if(*p == RULE_GROUP_END)
		{
			p++;
			if(*p == 0) break;
		}
		if(*p == RULE_GROUP_START)
		{
			if(p[1] == RULE_REPLACEMENTS)
			{
				p++;
				pw = (unsigned int *)(((long)p+4) & ~3);  // advance to next word boundary
				while(pw[0] != 0)
				{
					pw += 2;   // find the end of the replacement list, each entry is 2 words.
				}
				p = (char *)(pw+1);
				continue;
			}

			if(p[1] == RULE_LETTERGP2)
			{
				while(*p != RULE_GROUP_END) p++;
				continue;
			}

			p += (strlen(p)+1);
		}

		while((c = *p) != 0)
		{
			if(c == RULE_CONDITION)
				p++;   // next byte is the condition number, which may be 3 (= RULE_PHONEMES)
			if(c == RULE_PHONEMES)
				break;
			p++;
		}


		count++;
		if(c == RULE_PHONEMES)
		{
			ignore = 0;
			p++;
			while((c = *p) != 0)
			{
				if(c == phonSWITCH)
					ignore = 1;

				if(ignore == 0)
					phoneme_tab_flags[c] |= 2;
				p++;
			}
		}
		p++;
	}

	// NOTE, we should recognise langopts.textmode and ignore the *_list file (lang=zh)

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
				strcpy(phonetic,p);
				p += strlen(phonetic) +1;

				// examine flags
				ignore = 0;
				while(p < next)
				{
					if(*p == BITNUM_FLAG_TEXTMODE)
					{
						ignore = 1;
						break;
					}
					p++;
				}

				if(ignore == 0)
				{
					p = phonetic;
					while((c = *p) != 0)
					{
						if(c == phonSWITCH)
							break;

						phoneme_tab_flags[c] |= 2;
						p++;
					}
				}
			}
			p = next;
		}
	}
}   // end of FindPhonemesUsed




#define N_VOWELFMT_ADDR  20
int n_vowelfmt_addr;
int vowelfmt_addr[N_VOWELFMT_ADDR];    // FMT() statements found in a phoneme definition

static void FindVowelFmt(int prog_start, int length)
{//=================================================
	USHORT *prog;
	USHORT instn;
	int prog_end;

	prog_end = prog_start + length;
	n_vowelfmt_addr = 0;

	for(prog = &phoneme_index[prog_start]; prog < &phoneme_index[prog_end]; prog += NumInstnWords(prog))
	{
		instn = *prog;
		
		if((instn >> 12) == 11)
		{
			// FMT instruction
			if(n_vowelfmt_addr < N_VOWELFMT_ADDR)
			{
				vowelfmt_addr[n_vowelfmt_addr++] = ((instn & 0xf) << 18) + (prog[1] << 2);
			}
		}
	}
}  // end of FindVowelFmt


static int prog_log_sorter(PHONEME_PROG_LOG *p1, PHONEME_PROG_LOG *p2)
{//===================================================================
	return(p1->addr - p2->addr);
}

void MakeVowelLists(void)
{//======================
// For each phoneme table, make a list of its vowels and their
// formant frequencies (f1,f2,f3) for use by VowelChart()

	int table;
	int ix;
	int phcode;
	PHONEME_TAB *ph;
	FILE *f;
	FILE *f_prog_log;
	SPECT_SEQ *seq;
	SPECT_SEQK *seqk;
	frame_t *frame;
	int n_prog_log;
	int vowelfmt_ix;
	int colour;
	int voice_found;
	PHONEME_PROG_LOG *prog_log_table;
	PHONEME_PROG_LOG *found_prog;
	PHONEME_PROG_LOG this_prog;
	char dirname[sizeof(path_source)+20];
	char fname[sizeof(dirname)+40];
	char save_voice_name[80];

	strcpy(save_voice_name,voice_name2);

	sprintf(fname,"%s%s",path_source,"compile_prog_log");
	if((f_prog_log = fopen(fname,"rb")) == NULL)
	{
		wxLogError(_T("Can't read 'compile_prog_log;"));
		return;
	}
	ix = GetFileLength(fname);
	prog_log_table = (PHONEME_PROG_LOG *)malloc(ix);
	if(prog_log_table == NULL)
		return;
	ix = fread(prog_log_table, 1, ix, f_prog_log);
	fclose(f_prog_log);
	n_prog_log = ix / sizeof(PHONEME_PROG_LOG);

	progress = new wxProgressDialog(_T("Vowel charts"),_T(""),n_phoneme_tables);

	sprintf(dirname,"%s%s",path_source,"vowelcharts");
	mkdir(dirname,S_IRWXU | S_IRGRP | S_IROTH);

	sprintf(fname,"%s/vowel_log",dirname);

	for(table=0; table<n_phoneme_tables; table++)
	{
		sprintf(fname,"%s/%s",dirname,phoneme_tab_list[table].name);
		if((f = fopen(fname,"w"))==NULL) continue;

		progress->Update(table);

		// select the phoneme table by name
//		if(SetVoiceByName(phoneme_tab_list[table].name) != 0) continue;
		if(SelectPhonemeTableName(phoneme_tab_list[table].name) < 0) continue;

		voice_found = 0;
		if((LoadVoice(phoneme_tab_list[table].name, 0) != NULL) && (translator->data_dictrules != NULL))
		{
			voice_found = 1;
			FindPhonemesUsed();
		}

		// phoneme table is terminated by a phoneme with no name (=0)
		for(phcode=1; phcode < n_phoneme_tab; phcode++)
		{
			ph = phoneme_tab[phcode];

			if((ph==NULL) || (ph->type != phVOWEL) || (ph->program == 0))
				continue;

			if(voice_found && (phoneme_tab_flags[phcode] & 3) == 0)
			{
				continue;   // inherited, and not used 
			}

			// find the size of this program
			this_prog.addr = ph->program;
			found_prog = (PHONEME_PROG_LOG *)bsearch((void *)&this_prog, (void *)prog_log_table, n_prog_log, sizeof(PHONEME_PROG_LOG), (int(*)(const void *,const void *))prog_log_sorter);

			FindVowelFmt(ph->program, found_prog->length);

			for(vowelfmt_ix=0; vowelfmt_ix < n_vowelfmt_addr; vowelfmt_ix++)
			{
				ix = vowelfmt_addr[vowelfmt_ix];

				seq = (SPECT_SEQ *)(&phondata_ptr[ix]);
				seqk = (SPECT_SEQK *)seq;
	
				if(seq->frame[0].frflags & FRFLAG_KLATT)
					frame = &seqk->frame[1];
				else
					frame = (frame_t *)&seq->frame[1];

				if((n_vowelfmt_addr - vowelfmt_ix) == 1)
					colour = 0;
				else
					colour = 1;

				fprintf(f,"%s\t %d %3d %4d %4d",WordToString(ph->mnemonic), colour,
						frame->ffreq[1],frame->ffreq[2],frame->ffreq[3]);
	
				if(seq->frame[0].frflags & FRFLAG_KLATT)
					frame = &seqk->frame[seqk->n_frames-1];
				else
					frame = (frame_t *)&seq->frame[seq->n_frames-1];
				fprintf(f,"   %3d %4d %4d\n",frame->ffreq[1],frame->ffreq[2],frame->ffreq[3]);
			}
		}
		fclose(f);

		VowelChart(1,fname);  // draw the vowel chart
	}
	free(prog_log_table);
	LoadVoice(voice_name2,0);  // reset the original phoneme table
	delete progress;
	LoadVoiceVariant(save_voice_name,0);
}


#define N_ENVELOPES  30
extern int n_envelopes;
extern char envelope_paths[N_ENVELOPES][80];
extern unsigned char envelope_dat[N_ENVELOPES][ENV_LEN];

#define HT_ENV 140
#define WD_ENV 128*2

void DrawEnvelopes()
{//================
	int ix_env;
	int y_base;
	int x;
	FILE *f_txt=NULL;
	unsigned char *env;
	char name[200];

	wxBitmap bitmap(WD_ENV,HT_ENV*n_envelopes);

	// Create a memory DC
	wxMemoryDC dc;
	dc.SelectObject(bitmap);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetFont(*wxSWISS_FONT);
	dc.Clear();

	sprintf(name,"%s%s",path_source,"envelopes.txt");
//	f_txt = fopen(name,"w");

	for(ix_env=0; ix_env<n_envelopes; ix_env++)
	{
		y_base = HT_ENV * ix_env;
		dc.SetPen(*wxLIGHT_GREY_PEN);
		dc.DrawLine(0,y_base+0,256,y_base+0);
		dc.DrawLine(0,y_base+64,256,y_base+64);
		dc.DrawLine(0,y_base+128,256,y_base+128);
		dc.DrawLine(128,y_base+0,128,y_base+128);

		dc.SetPen(*wxBLACK_PEN);
		strncpy0(name,envelope_paths[ix_env],sizeof(name));
		dc.DrawText(wxString(name,wxConvLocal),1,y_base);

		env = envelope_dat[ix_env];
		y_base = y_base+128;
		for(x=0; x<127; x++)
		{
			dc.DrawLine(x*2, y_base-env[x]/2, (x+1)*2, y_base-env[x+1]/2);
		}

		if(f_txt != NULL)
		{
			fprintf(f_txt,"%s\n",name);
			for(x=0; x<128; x++)
			{
				fprintf(f_txt," 0x%.2x,",env[x]);
				if((x & 0xf) == 0xf)
					fputc('\n',f_txt);
			}
			fputc('\n',f_txt);
		}
	}

	bitmap.SaveFile(path_phsource+_T("/envelopes.png"),wxBITMAP_TYPE_PNG);
	if(f_txt != NULL)
		fclose(f_txt);
}

