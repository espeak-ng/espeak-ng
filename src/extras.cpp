/***************************************************************************
 *   Copyright (C) 2006 by Jonathan Duddington                             *
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

#include <math.h>
#include "wx/wx.h"
#include <wx/dcmemory.h>
#include <wx/dc.h>
#include <wx/bitmap.h>
#include <wx/dirdlg.h>
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/sound.h"

#include "sys/stat.h"

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
extern char path_source[];
extern char voice_name[];

extern char *spects_data;
extern int n_phoneme_tables;
extern PHONEME_TAB_LIST phoneme_tab_list[N_PHONEME_TABS];



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
 * gimp_hsl_to_rgb_int:
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
{
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

#define WIDTH  1200
#define HEIGHT 800

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
	for(ix=200; ix<=1000; ix+=100)
	{
		y = VowelY(ix);
		dc.DrawLine(0,y,WIDTH,y);
		dc.DrawText(wxString::Format(_T("%d"),ix),1,y);
	}
	for(ix=800; ix<=2400; ix+=200)
	{
		x = VowelX(ix);
		dc.DrawLine(x,0,x,HEIGHT);
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


static void FindPhonemesUsed(void)
{//===============================
	int hash;
	char *p;
	char *next;
	unsigned char c;
	int count = 0;

	// look through all the phoneme strings in the **_rules data
	// and mark these phoneme codes as used.
	p = translator->data_dictrules;
	while(*p != 0)
	{
		if(*p == RULE_GROUP_END)
			p++;
		if(*p == RULE_GROUP_START)
			p += (strlen(p)+1);

		while((((c = *p) != RULE_PHONEMES)) && (c != 0)) p++;
		count++;
		if(c == RULE_PHONEMES)
		{
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
	char dirname[100];
	char fname[120];

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

//******************************************************************************************************



FILE *f_wavtest = NULL;
FILE *f_events = NULL;

int OpenWaveFile3(const char *path, int rate)
/******************************************/
{
	int *p;

	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0,0,0,0 };


	if(path == NULL)
		return(2);

	// set the sample rate in the header
	p = (int *)(&wave_hdr[24]);
	p[0] = rate;
	p[1] = rate * 2;

	f_wavtest = fopen(path,"wb");

	if(f_wavtest != NULL)
	{
		fwrite(wave_hdr,1,sizeof(wave_hdr),f_wavtest);
		return(0);
	}
	return(1);
}   //  end of OpenWaveFile




void CloseWaveFile3(int rate)
/******************/
{
   unsigned int pos;
   static int value;


   fflush(f_wavtest);
   pos = ftell(f_wavtest);

   value = pos - 8;
   fseek(f_wavtest,4,SEEK_SET);
   fwrite(&value,4,1,f_wavtest);

	value = rate;
	fseek(f_wavtest,24,SEEK_SET);
	fwrite(&value,4,1,f_wavtest);

	value = rate*2;
	fseek(f_wavtest,28,SEEK_SET);
	fwrite(&value,4,1,f_wavtest);

   value = pos - 44;
   fseek(f_wavtest,40,SEEK_SET);
   fwrite(&value,4,1,f_wavtest);

   fclose(f_wavtest);
   f_wavtest = NULL;

} // end of CloseWaveFile2


int TestUriCallback(int type, const char *uri, const char *base)
{//=============================================================

	if(strcmp(uri,"hello")==0)
		return(1);
	return(0);
}



int TestSynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{//====================================================================
	int type;

if(f_wavtest == NULL) return(0);

	if(wav == NULL)
	{
		CloseWaveFile3(samplerate);
		fclose(f_events);
		return(0);
	}

	fwrite(wav,numsamples*2,1,f_wavtest);

	while((type = events->type) != 0)
	{
		fprintf(f_events,"%4d %4d (%2d):  %d  ",events->audio_position,events->text_position,events->length,type);
		if((type==3) || (type==4))
			fprintf(f_events,"'%s'\n",events->id.name);
		else
			fprintf(f_events,"%d\n",events->id.number);

		events++;
	}
	return(0);
}

//******************************************************************************************************


#ifdef deleted
static int RuLex_sorter(char **a, char **b)
{//=======================================
	char *pa, *pb;
	int xa, xb;
	int ix;

	pa = *a;
	pb = *b;

	xa = strlen(pa)-1;
	xb = strlen(pb)-1;

	while((xa >= 0) && (xb >= 0))
	{
		if((ix = (pa[xa] - pb[xb])) != 0)
			return(ix);

		xa--;
		xb--;
	}
	return(pa - pb);
}   /* end of strcmp2 */
#endif


static const unsigned short KOI8_R[0x60] = {
   0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556, // a0
   0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d, 0x255e, // a8
   0x255f, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565, // b0
   0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b, 0x256c, 0x00a9, // b8
   0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, // c0
   0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, // c8
   0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, // d0
   0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a, // d8
   0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413, // e0
   0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, // e8
   0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412, // f0
   0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a, // f8
};


#define N_CHARS  34
int *p_unicode;
int unicode[80];


void Lexicon_Ru()
{//==============
	// compare stress markings in Russian RuLex file with lookup in ru_rules
	int ix;
	char *p;
	int  c;
	FILE *f_in;
	FILE *f_out;
	FILE *f_log;
	FILE *f_roots;
	PHONEME_TAB *ph;
	int vcount;
	int ru_stress;
	int max_stress;
	int max_stress_posn;
	int n_words=0;
	int n_wrong=0;
	int wlength;
	int input_length;

	int sfx;
	char *suffix;
	int wlen;
	int len;
	int check_root;
	WORD_TAB winfo;

	char word[80];
	char word2[80];
	int counts[20][20][10];

	char phonemes[N_WORD_PHONEMES];
	char buf[100];

	// KOI8-R codes for Russian vowels
	static char vowels[] = {0xa3,0xc0,0xc1,0xc5,0xc9,0xcf,0xd1,0xd5,0xd9,0xdc,0};

	typedef struct {
		char *suffix;
		int  syllables;
	} SUFFIX;

	static SUFFIX suffixes[] = {
{NULL,0},
	{"ичу",2},
	{"ского",2},
	{"ская",2},
	{"ски",1},
	{"ские",2},
	{"ский",1},
	{"ским",1},
	{"ское",2},
	{"ской",1},
	{"ском",1},
	{"скую",2},

	{"а",1},
	{"е",1},
	{"и",1},

	{NULL,0}};


	memset(counts,0,sizeof(counts));

	if(gui_flag)
	{
		wxString fname = wxFileSelector(_T("Read lexicon.dict"),path_dictsource,
			_T(""),_T(""),_T("*"),wxOPEN);
		if(fname.IsEmpty())
			return;
		strcpy(buf,fname.mb_str(wxConvLocal));
	}
	else
	{
		strcpy(buf,"lexicon.dict");
	}

	if((f_in = fopen(buf,"r")) == NULL)
	{
		if(gui_flag)
			wxLogError(_T("Can't read file: ") + wxString(buf,wxConvLocal));
		else
			fprintf(stderr,"Can't read file: %s\n",buf);
		return;
	}
	input_length = GetFileLength(buf);

	sprintf(buf,"%s%c%s",path_dsource,PATHSEP,"ru_listx_1");
	if((f_out = fopen(buf,"w")) == NULL)
	{
		wxLogError(_T("Can't write to: ")+wxString(buf,wxConvLocal));
		fclose(f_in);
		return;
	}

	sprintf(buf,"%s%c%s",path_dsource,PATHSEP,"ru_log");
	f_log = fopen(buf,"w");
	sprintf(buf,"%s%c%s",path_dsource,PATHSEP,"ru_roots_1");
	f_roots = fopen(buf,"w");

	LoadVoice("ru",0);


	if(gui_flag)
		progress = new wxProgressDialog(_T("Lexicon"),_T(""),input_length);
	else
		fprintf(stderr,"Processing lexicon.dict\n");

	for(;;)
	{
		if(((n_words & 0x3ff) == 0) && gui_flag)
		{
			progress->Update(ftell(f_in));
		}

		if(fgets(buf,sizeof(buf),f_in) == NULL)
			break;

		if(isspace2(buf[0]))
			continue;

		// convert word from KOI8-R to UTF8
		p = buf;
		ix = 0;
		wlength = 0;
p_unicode = unicode;
		while(!isspace2(c = (*p++ & 0xff)))
		{
			if(c >= 0xa0)
			{
				c = KOI8_R[c-0xa0];
*p_unicode++ = c;
			}


			wlength++;
			ix += utf8_out(c,&word[ix]);
		}
		word[ix] = 0;
*p_unicode=0;

		sprintf(word2," %s ",word);

		// find the marked stress position
		vcount = 0;
		ru_stress = 0;
		while(*p == ' ') p++;

		while((c = (*p++ & 0xff)) != '\n')
		{
			if(c == '+')
			{
				ru_stress = vcount;
				break;
			}
			if(strchr(vowels,c) != NULL)
			{
				vcount++;
			}
		}

		// translate
		memset(&winfo,0,sizeof(winfo));
		translator->TranslateWord(&word2[1],0,&winfo);
		DecodePhonemes(translator->word_phonemes,phonemes);

		// find the stress position in the translation
		max_stress = 0;
		max_stress_posn = -1;
		vcount = 0;
		check_root = 0;

		ph = phoneme_tab[phonPAUSE];
		for(p=translator->word_phonemes; *p != 0; p++)
		{
			ph = phoneme_tab[(unsigned int)*p];
			if(ph == NULL)
				continue;

			if(ph->type == phVOWEL)
				vcount++;
			if(ph->type == phSTRESS)
			{
				if(ph->std_length > max_stress)
				{
					max_stress = ph->std_length;
					max_stress_posn = vcount+1;
				}
			}
		}

		n_words++;
		if(ru_stress > vcount)
		{
			if(f_log != NULL)
			{
				fprintf(f_log,"%s\t $%d\t // %s\n",word,ru_stress,phonemes);
			}
		}
		else
		{
			counts[vcount][ru_stress][ph->type]++;

			if((vcount > 1) && (ru_stress != max_stress_posn))
			{
				n_wrong++;
				if((ru_stress==0) || (ru_stress > 7))
					fprintf(f_out,"// ");   // we only have $1 to $7 to indicate stress position
				else
					check_root = 1;

#define X_COMPACT

				fprintf(f_out,"%s",word);
#ifdef X_COMPACT
				if(wlength < 8) fputc('\t',f_out);
				if(wlength < 16) fputc('\t',f_out);
				fprintf(f_out," $%d\n",ru_stress);
#else
				while(wlength++ < 20)
					fputc(' ',f_out);
				fprintf(f_out," $%d  //%d  %s\n",ru_stress,max_stress_posn,phonemes);
#endif
//CharStats();
			}
		}

		if(check_root)
		{
			// does this word match any suffixes ?
			wlen = strlen(word);
			for(sfx=0;(suffix = suffixes[sfx].suffix) != NULL; sfx++)
			{
				len = strlen(suffix);
				if(len >= (wlen-2))
					continue;
	
				if(ru_stress > (vcount - suffixes[sfx].syllables))
					continue;
				
				if(strcmp(suffix,&word[wlen-len])==0)
				{
					strcpy(word2,word);
					word2[wlen-len] = 0;
//					fprintf(f_roots,"%s\t $%d\t\\ %s\n",word2,ru_stress,suffix);
					fprintf(f_roots,"%s\t $%d\n",word2,ru_stress);
				}
			}
		}
	}

	fclose(f_in);
	fclose(f_out);
	fclose(f_roots);

	sprintf(buf,"Lexicon: Total %d  OK %d  wrong %d",n_words,n_words-n_wrong,n_wrong);
	if(gui_flag)
	{
		delete progress;
		wxLogStatus(wxString(buf,wxConvLocal));
	}
	else
	{
		fprintf(stderr,"%s\n",buf);
	}

	if(f_log != NULL)
	{

#ifdef deleted
		// list tables of frequency of stress position for words of different syllable lengths
		int j,k;
		for(ix=0; ix<12; ix++)
		{
			fprintf(f_log,"%2d syl: ",ix);
			for(k=0; k<10; k++)
			{
				fprintf(f_log,"  %2d :",k);
				for(j=0; j<10; j++)
				{
					fprintf(f_log,"%6d ",counts[ix][j][k]);
				}
				fprintf(f_log,"\n");
			}
			fprintf(f_log,"\n\n");
		}
#endif
		fclose(f_log);
	}

}  // end of Lexicon_Ru



void CompareLexicon(int id)
{//========================
	switch(id)
	{
	case MENU_LEXICON_RU:
		Lexicon_Ru();
		break;
	}
}  // end of CompareLexicon


//******************************************************************************************************
extern int HashDictionary(const char *string);
static int n_words;

struct wcount {
	struct wcount *link;
	int count;
	char *word;
};



static int wfreq_sorter(wcount **p1, wcount **p2)
{//==============================================
	int x;
	wcount *a, *b;
	a = *p1;
	b = *p2;
	if((x = b->count - a->count) != 0)
		return(x);
	return(strcmp(a->word,b->word));
}


static void wfreq_add(const char *word, wcount **hashtab)
{//======================================================
	wcount *p;
	wcount **p2;
	int len;
	int hash;

	hash = HashDictionary(word);
	p2 = &hashtab[hash];
	p = *p2;

	while(p != NULL)
	{
		if(strcmp(p->word,word)==0)
		{
			p->count++;
			return;
		}
		p2 = &p->link;
		p = *p2;
	}

	// word not found, add it to the list
	len = strlen(word) + 1;
	if((p = (wcount *)malloc(sizeof(wcount)+len)) == NULL)
		return;

	p->count = 1;
	p->link = NULL;
	p->word = (char *)p + sizeof(wcount);
	strcpy(p->word,word);
	*p2 = p;
	n_words++;
}


void CountWordFreq(wxString path, wcount **hashtab)
{//================================================
	// Count the occurances of words in this file
	FILE *f_in;
	unsigned char c;
	int wc;
	unsigned int ix, j, k;
	int n_chars;
	char buf[80];
	char wbuf[80];

	if((f_in = fopen(path.mb_str(wxConvLocal),"rb")) == NULL)
		return;

	while(!feof(f_in))
	{
		while((c = fgetc(f_in)) < 'A')
		{
			// skip leading spaces, numbers, etc
			if(feof(f_in)) break;
		}
	
		// read utf8 bytes until a space, number or punctuation
		ix = 0;
		while(!feof(f_in) && (c >= 'A') && (ix < sizeof(buf)-1))
		{
			buf[ix++] = c;
			c = fgetc(f_in);
		}
		buf[ix++] = 0;
		buf[ix] = 0;
	
		// the buf may contain non-alphabetic characters
		j = 0;
		n_chars = 0;
		for(k=0; k<ix; )
		{
			k += utf8_in(&wc,&buf[k],0);
			wc = towlower(wc);       // convert to lower case
			if(iswalpha(wc))
			{
				j += utf8_out(wc,&wbuf[j]);
				n_chars++;
			}
			else
			{
				wbuf[j] = 0;
				if(n_chars > 2)
				{
					wfreq_add(wbuf,hashtab);
				}
				j = 0;
				n_chars = 0;
			}
		}
	}
	fclose(f_in);
	
}   // end of CountWordFreq


void MakeWordFreqList()
{//====================
// Read text files from a specified directory and make a list of the most frequently occuring words.
	struct wcount *whashtab[N_HASH_DICT];
	wcount **w_list;
	int ix;
	int j;
	int hash;
	wcount *p;
	FILE *f_out;
	char buf[200];
	char buf2[200];

	wxString dir = wxDirSelector(_T("Directory of text files"),path_speaktext);
	if(dir.IsEmpty()) return;

	memset(whashtab,0,sizeof(whashtab));

	wxString path = wxFindFirstFile(dir+_T("/*"),wxFILE);

	while (!path.empty())
	{
		if(path.AfterLast(PATHSEP) != _T("!wordcounts"))
		{
			CountWordFreq(path,whashtab);
			path = wxFindNextFile();
		}
	}

	// put all the words into a list and then sort it
	w_list = (wcount **)malloc(sizeof(wcount *) * n_words);

	ix = 0;
	for(hash=0; hash < N_HASH_DICT; hash++)
	{
		p = whashtab[hash];
		while((p != NULL) && (ix < n_words))
		{
			w_list[ix++] = p;
			p = p->link;
		}
	}

	qsort((void *)w_list,ix,sizeof(wcount *),(int(*)(const void *,const void *))wfreq_sorter);

	// write out the sorted list
	strcpy(buf,dir.mb_str(wxConvLocal));
	sprintf(buf2,"%s/!wordcounts",buf);
	if((f_out = fopen(buf2,"w")) == NULL)
		return;

	for(j=0; j<ix; j++)
	{
		p = w_list[j];
		fprintf(f_out,"%5d  %s\n",p->count,p->word);
		free(p);
	}
	fclose(f_out);
	
}  // end of Make WorkFreqList



//******************************************************************************************************

void ConvertToUtf8()
{//=================
// Convert a file from 8bit to UTF8, according to the current voice
	unsigned int c;
	int ix;
	FILE *f_in;
	FILE *f_out;
	char buf[80];

	wxString fname = wxFileSelector(_T("Convert file to UTF8"),wxString(path_home,wxConvLocal),
		_T(""),_T(""),_T("*"),wxOPEN);
	if(fname.IsEmpty())
		return;
	strcpy(buf,fname.mb_str(wxConvLocal));
	f_in = fopen(buf,"r");
	if(f_in == NULL)
	{
		wxLogError(_T("Can't read file: ")+fname);
		return;
	}

	strcat(buf,"_1");
	f_out = fopen(buf,"w");
	if(f_out == NULL)
	{
		wxLogError(_T("Can't create file: ")+wxString(buf,wxConvLocal));
		fclose(f_in);
		return;
	}

	while(!feof(f_in))
	{
		c = fgetc(f_in);
		if(c >= 0xa0)
			c = translator->charset_a0[c-0xa0];

		ix = utf8_out(c,buf);
		fwrite(buf,ix,1,f_out);
	}
	fclose(f_in);
	fclose(f_out);


}  // end of ConvertToItf8




char* text[]=
{
  "<voice xml:lang=\"en\" variant=\"1\">eSpeak.</voice>",
  " <voice xml:lang=\"en\" gender=\"male\" variant=\"1\"><prosody rate=\"1.0\"> normally would, </prosody></voice>"
};



void TestTest(int control)
{//=======================
	FILE *f;
	unsigned int c;
	unsigned int ix=0;
	char textbuf[2000];
	espeak_VOICE voice;

//FindPhonemesUsed();
//return;

if(control==2)
{
	return;
}
	memset(&voice,0,sizeof(voice));

	f = fopen("/home/jsd1/speechdata/text/test","r");
	if(f==NULL)
		return;
	
	while(!feof(f) && (ix < sizeof(textbuf)-2))
	{
		c = fgetc(f);
		if(!feof(f))
			textbuf[ix++] = c;
	}
	textbuf[ix] = 0;
	fclose(f);

	OpenWaveFile3("/home/jsd1/speechdata/text/test.wav",samplerate);
	f_events = fopen("/home/jsd1/speechdata/text/events","w");

	espeak_Initialize(AUDIO_OUTPUT_PLAYBACK,100,NULL);
//	espeak_SetSynthCallback(TestSynthCallback);
//	espeak_SetUriCallback(TestUriCallback);

	voice.languages = "fr";
	voice.gender = 0;
	voice.age = 0;
	voice.variant = 0;
	espeak_SetVoiceByName("en");
//	espeak_SetVoiceByProperties(&voice);
	espeak_SetParameter(espeakLINELENGTH,11,0);
	espeak_Synth(textbuf,ix+1,0,POS_CHARACTER,0,espeakCHARS_8BIT+espeakSSML,NULL,NULL);

//	for(voice.variant = 1; voice.variant < 9; voice.variant++)
//	{
//		espeak_SetVoiceByProperties(&voice);
//		espeak_Synth(textbuf,ix+1,0,POS_CHARACTER,0,espeakCHARS_8BIT+espeakSSML,NULL,NULL);
//	}
}


