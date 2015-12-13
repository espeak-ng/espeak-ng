/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *   Copyright (C) 2013-2015 by Reece H. Dunn                              *
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


#include "wx/wx.h"

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "wx/txtstrm.h"
#include "wx/datstrm.h"


#ifdef INCLUDE_KLATT
extern "C" void SetSynth_Klatt(int length, int modn, frame_t *fr1, frame_t *fr2, voice_t *v, int control);
#endif

extern "C" void SetSynth(int length, int modn, frame_t *fr1, frame_t *fr2, voice_t *v);

extern "C" int PeaksToHarmspect(wavegen_peaks_t *peaks, int pitch, int *htab, int control);

extern unsigned char pk_shape1[];
extern int pk_select;
extern char voice_name[];

static int frame_width;
int pk_select;

#define DRAWPEAKWIDTH 2000
#define PEAKSHAPEW 256

#include <math.h>


static int default_freq[N_PEAKS] =
	{200,500,1200,3000,3500,4000,6900,7800,9000};
static int default_width[N_PEAKS] =
	{750,500,550,550,600,700,700,700,700};
static int default_klt_bw[N_PEAKS] =
	{89,90,140,260,260,260,500,500,500};


float polint(float xa[],float ya[],int n,float x)
{//==============================================
// General polinomial interpolation routine, xa[1...n] ya[1...n]
	int i,m,ns=1;
	float den,dif,dift,ho,hp,w;
	float y;  // result
	float c[9],d[9];

	dif=fabs(x-xa[1]);

	for(i=1;i<=n;i++){
		if((dift=fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	y=ya[ns--];
	for(m=1;m<n;m++) {
		for(i=1;i<=n-m;i++) {
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if((den=ho-hp) == 0.0)
			{
//				fprintf(stderr,"Error in routine 'polint'");
				return(ya[2]);  // two input xa are identical
			}
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		y += ((2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	return(y);
}  // end of polint


static void PeaksZero(peak_t *sp, peak_t *zero)
{//=====================================
	int pk;

	memcpy(zero,sp,sizeof(peak_t)*N_PEAKS);

	for(pk=0; pk<N_PEAKS; pk++)
		zero[pk].pkheight = 0;

}  // end of PeaksZero

SpectFrame::SpectFrame(SpectFrame *copy)
{//=====================================

	int  ix;

	selected = 0;
	keyframe = 0;
	spect = NULL;
	markers = 0;
	pitch = 0;
	nx = 0;
	time = 0;
	length = 0;
	amp_adjust = 100;
	length_adjust = 0;

	for(ix=0; ix<N_PEAKS; ix++)
   {
		formants[ix].freq = 0;
		peaks[ix].pkfreq = default_freq[ix];
		peaks[ix].pkheight = 0;
		peaks[ix].pkwidth = default_width[ix];
		peaks[ix].pkright = default_width[ix];
		peaks[ix].klt_bw = default_klt_bw[ix];
		peaks[ix].klt_ap = 0;
		peaks[ix].klt_bp = default_klt_bw[ix];
   }

	memset(klatt_param, 0, sizeof(klatt_param));
	klatt_param[KLATT_AV] = 59;
	klatt_param[KLATT_Kopen] = 40;

   if(copy != NULL)
   {
		*this = *copy;
		spect = new USHORT[nx];
		memcpy(spect,copy->spect,sizeof(USHORT)*nx);
	}
}


SpectFrame::~SpectFrame()
{//=======================

	if(spect != NULL)
		delete spect;
}

int SpectFrame::Import(wxInputStream& stream1)
{//==========================================
// Import Pratt analysis data
	int ix;
	double x;
	unsigned short *spect_data;

	wxTextInputStream stream(stream1);

	stream >> time;
	stream >> pitch;
	stream >> nx;
	stream >> dx;

	if(stream1.Eof())
		return(1);

	for(ix=0; ix<N_PEAKS; ix++)
	{
		peaks[ix].pkfreq = default_freq[ix];
		peaks[ix].pkheight = 0;
		peaks[ix].pkwidth = default_width[ix];
		peaks[ix].pkright = default_width[ix];
	}
	for(ix=1; ix<=5; ix++)
	{
      stream >> x;
		formants[ix].freq = (int)x;

		if(x > 0)
			peaks[ix].pkfreq = (int)x;

      stream >> x;
		formants[ix].bandw = (int)x;
	}

	spect_data = new USHORT[nx];

	if(spect_data == NULL)
	{
		wxLogError(_T("Failed to allocate memory"));
		return(1);
	}

	max_y = 0;
	for(ix=0; ix<nx; ix++)
	{
		stream >> x;
		spect_data[ix] = (int)(sqrt(x) * 16386);
		if(spect_data[ix] > max_y)
			max_y = spect_data[ix];
	}
   spect = spect_data;

	return(0);
}  //  End of SpectFrame::Import



int SpectFrame::ImportSPC2(wxInputStream& stream, float &time_acc)
{//===============================================================
	int ix;
	int size;
	unsigned short *spect_data;
	CYCLE cy;
	CYCLE *p;
	float len;
	static char peak_factor[8] = {4,5,11,20,20,25,32,32};

	stream.Read(&cy,44);
	size = SPC2_size_cycle(&cy);

	p = (CYCLE *)malloc(size);
	if(p == NULL)
	{
		return(1);
	}
	stream.SeekI(-44,wxFromCurrent);
	stream.Read(p,size);

	time = time_acc;
	len = cy.length / 15625.0;
	time_acc += len;
	pitch = float(cy.pitch) / 16.0;
	nx = cy.n_harm;
	dx = pitch;

	for(ix=0; ix<7; ix++)
	{
		peaks[ix].pkfreq = cy.peak_data[ix].freq * peak_factor[ix];
		if(peaks[ix].pkfreq == 0)
			peaks[ix].pkfreq = default_freq[ix];

		peaks[ix].pkheight = cy.peak_data[ix].height * 40;
		peaks[ix].pkwidth = cy.peak_data[ix].width_l * 12;
		peaks[ix].pkright = cy.peak_data[ix].width_r * 12;
	}
	for(ix=7; ix<=8; ix++)
	{
		peaks[ix].pkfreq = default_freq[ix];  // default
		peaks[ix].pkheight = 0;
		peaks[ix].pkwidth = peaks[ix].pkright = default_width[ix];
	}
	if(((cy.flags & 0x80)==0) && (peaks[1].pkheight > 0))
		keyframe = 1;

	if(cy.flags & 0x08)
		markers |= 4;
	if(cy.flags & 0x10)
		markers |= 2;
	if(cy.flags & 0x04)
		markers |= 8;

	if(nx>0)
	{
		spect_data = new USHORT[nx];

		if(spect_data == NULL)
		{
			wxLogError(_T("Failed to allocate memory"));
			return(1);
		}

		max_y = 0;
		for(ix=0; ix<nx; ix++)
		{
			spect_data[ix] = p->data[ix];
			if(spect_data[ix] > max_y)
				max_y = spect_data[ix];
		}
	}
	else
	{
		nx = int(8000/dx);
		spect_data = new USHORT[nx];
		if(spect_data == NULL)
		{
			wxLogError(_T("Failed to allocate memory"));
			return(1);
		}
		for(ix=0; ix<nx; ix++)
			spect_data[ix] = 1;
		max_y = 1;
	}
   spect = spect_data;
	free(p);
	return(0);
}  //  end of ImportSPC2



int SpectFrame::Load(wxInputStream& stream, int file_format_type)
{//==============================================================
	int  ix;
	int  x;
	unsigned short *spect_data;

   wxDataInputStream s(stream);

	time = s.ReadDouble();
	pitch = s.ReadDouble();
	length = s.ReadDouble();
	dx = s.ReadDouble();
	nx = s.Read16();
	markers = s.Read16();
	amp_adjust = s.Read16();

	if(file_format_type == 2)
	{
		ix = s.Read16();  // spare
		ix = s.Read16();  // spare
	}

	for(ix=0; ix<N_PEAKS; ix++)
	{
		formants[ix].freq = s.Read16();
		formants[ix].bandw = s.Read16();
		peaks[ix].pkfreq = s.Read16();
		if((peaks[ix].pkheight = s.Read16()) > 0)
			keyframe = 1;
		peaks[ix].pkwidth = s.Read16();
		peaks[ix].pkright = s.Read16();

		if(file_format_type == 2)
		{
			peaks[ix].klt_bw = s.Read16();
			peaks[ix].klt_ap = s.Read16();
			peaks[ix].klt_bp = s.Read16();
		}
	}

	if(file_format_type > 0)
	{
		for(ix=0; ix<N_KLATTP2; ix++)
		{
			klatt_param[ix] = s.Read16();
		}
	}

	spect_data = new USHORT[nx];

	if(spect_data == NULL)
	{
		wxLogError(_T("Failed to allocate memory"));
		return(1);
	}

	max_y = 0;
	for(ix=0; ix<nx; ix++)
	{
		x = spect_data[ix] = s.Read16();
		if(x > max_y) max_y = x;
	}
   spect = spect_data;

	return(0);
}  //  End of SpectFrame::Load



void SpectFrame::ZeroPeaks()
{//=========================
	int pk;

	for(pk=0; pk<N_PEAKS; pk++)
		peaks[pk].pkheight = 0;
}



void SpectFrame::CopyPeaks(SpectFrame *sf)
{//=======================================
	memcpy(peaks,sf->peaks,sizeof(peaks));
	memcpy(klatt_param, sf->klatt_param, sizeof(klatt_param));
	keyframe = sf->keyframe;
}



void SpectFrame::ToggleMarker(int n)
{//=================================
	markers ^= 1<<n;
}



double SpectFrame::GetRms(int seq_amplitude)
{//=========================================
	int h;
	float total=0;
	int maxh;
	int height;
	int htab[400];
	wavegen_peaks_t wpeaks[9];

	for(h=0; h<9; h++)
	{
		height = (peaks[h].pkheight * seq_amplitude * amp_adjust)/10000;
		wpeaks[h].height = height << 8;

		wpeaks[h].freq = peaks[h].pkfreq << 16;
		wpeaks[h].left = peaks[h].pkwidth << 16;
		wpeaks[h].right = peaks[h].pkright << 16;
	}

	maxh = PeaksToHarmspect(wpeaks,90<<16,htab,0);
	for(h=1; h<maxh; h++)
	{
		total += ((htab[h] * htab[h]) >> 10);
	}
	rms = sqrt(total) / 7.25;
	return(rms);
}



void SpectFrame::KlattDefaults()
{//============================
	// set default values for Klatt parameters
	int pk;
	int bw;
	int bw3;

	klatt_param[KLATT_AV] = 59;
	klatt_param[KLATT_AVp] = 0;
	klatt_param[KLATT_Fric] = 0;
	klatt_param[KLATT_FricBP] = 0;
	klatt_param[KLATT_Aspr] = 0;
	klatt_param[KLATT_Turb] = 0;
	klatt_param[KLATT_Skew] = 0;
	klatt_param[KLATT_Tilt] = 0;
	klatt_param[KLATT_Kopen] = 40;
	klatt_param[KLATT_FNZ] = 280;

	bw = 60;
	if(peaks[1].pkfreq < 400)
		bw = 55;
	if(peaks[1].pkfreq > 600)
		bw = 70;
	if(peaks[1].pkfreq > 650)
		bw = 80;
	if(peaks[1].pkfreq > 750)
		bw = 90;
	peaks[1].pkwidth = bw;

	bw = 90;
	bw3 = 150;
	if(peaks[2].pkfreq < 1000)
	{
		bw = 80;
		bw3 = 120;
	}
	if(peaks[2].pkfreq > 1600)
	{
		bw = 100;
		bw3 = 200;
	}
	if(peaks[2].pkfreq > 2000)
	{
		bw = 110;
		bw3 = 250;
	}
	peaks[2].pkwidth = bw;
	peaks[3].pkwidth = bw3;
	peaks[4].pkwidth = 200;
	peaks[5].pkwidth = 200;
	peaks[6].pkwidth = 500;
	peaks[0].pkfreq = 280;   // FNP
	peaks[0].pkwidth = 280; // FNZ

	peaks[7].pkfreq = 7800;
	peaks[7].pkwidth = 0;
	peaks[8].pkfreq = 9000;
	peaks[8].pkwidth = 0;

	for(pk=0; pk<=8; pk++)
	{
		peaks[pk].pkheight = peaks[pk].pkwidth << 6;
		peaks[pk].pkright = 0;
	}
}


SpectSeq::SpectSeq(int n)
{//======================
	numframes = n;
	if(n > 0)
		frames = new SpectFrame* [n];
	else
		frames = NULL;
	
	pk_select = 1;
	grid = 1;
   duration = 0;
   pitch1 = 0;
   pitch2 = 0;
	bass_reduction = 0;

	max_x = 3000;
	max_y = 1;
	file_format = 0;
}

SpectSeq::~SpectSeq()
{//==================
	int ix;
	if(frames != NULL)
	{
		for(ix=0; ix<numframes; ix++)
		{
			if(frames[ix] != NULL)
				delete frames[ix];
		}
		delete frames;
	}
}


void SpectSeq::SelectAll(int yes)
{//==============================
	// select of deselect all frames in the sequence
	int ix;
	for(ix=0; ix<numframes; ix++)
	   frames[ix]->selected = yes;
}


int SpectSeq::CountSelected()
{//==========================
	int  ix;
	int  count=0;
	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->selected)
			count++;
	}
	return(count);
}  //  end of SpectSeq::CountSelected


void SpectSeq::DeleteSelected()
{//============================
	int  ix;
	int  count=0;

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->selected)
		{
			count++;
		}
		else
		if(count > 0)
		{
			*frames[ix-count] = *frames[ix];
		}
	}
	numframes = numframes - count;
}  //  end of SpectSeq::DeleteSelected


void SpectSeq::SetFrameLengths()
{//=============================
	int  frame;

	for(frame=0; frame<numframes; frame++)
	{
		if(frames[frame]->keyframe)
			frames[frame]->length = GetFrameLength(frame,1,NULL);
		else
			frames[frame]->length = 0;
	}
}  // end of SetFrameLengths



float SpectSeq::GetFrameLength(int frame, int plus, int *original)
{//===============================================================
	int  ix;
	float  adjust=0;

	if(frame >= numframes-1) return(0);
	
	// include the adjustment for this frame ?
	if(plus) adjust = frames[frame]->length_adjust;
	
	for(ix=frame+1; ix<numframes-1; ix++)
	{
		if(frames[ix]->keyframe) break;  // reached next keyframe
		adjust += frames[ix]->length_adjust;
	}
	if(original != NULL)
		*original = int((frames[ix]->time - frames[frame]->time) * 1000.0 + 0.5);
	return ((frames[ix]->time - frames[frame]->time) * 1000.0 + adjust);
}


float SpectSeq::GetKeyedLength()
{//=============================
	int ix;
	int first;
	int last=0;
	float adjust=0;
	
	first = -1;
	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->keyframe)
		{
			last = ix;
			if(first == -1) first = ix;
		}
	}
	if(first == -1)
		return(0);   // no keyframes
		
	for(ix=first; ix<last; ix++)
		adjust += frames[ix]->length_adjust;

	return((frames[last]->time - frames[first]->time) * 1000 + adjust);
}


void SpectSeq::Load2(wxInputStream& stream, int import, int n)
{//===========================================================
	// continuation of load/import
   int  ix;
	wxString string;
	float time_offset;
	float time_acc=0;
	int  set_max_y=0;

	if(n==0) return;

	if(frames != NULL) delete frames;
	frames = new SpectFrame* [n];

	numframes = 0;
	max_x = 3000;
	if(max_y == 0)
	{
		set_max_y = 1;
		max_y = 1;
	}
	for(ix = 0; ix < n; ix++)
	{
		SpectFrame *frame = new SpectFrame;

		if(import==1)
		{
			if(frame->Import(stream) != 0)
			{
				delete frame;
				break;
			}
		}
		else
		if(import==2)
		{
			if(frame->ImportSPC2(stream,time_acc) != 0)
			{
				delete frame;
				break;
			}
		}
		else
		{
			if(frame->Load(stream, file_format) != 0)
			{
				delete frame;
				break;
			}
		}

		frames[numframes++] = frame;

		if(set_max_y && (frame->max_y > max_y))
			max_y = frame->max_y;
		if(frame->nx * frame->dx > max_x) max_x = int(frame->nx * frame->dx);
	}
max_x = 9000;   // disable auto-xscaling

	frame_width = int((FRAME_WIDTH*max_x)/MAX_DISPLAY_FREQ);
	if(frame_width > FRAME_WIDTH) frame_width = FRAME_WIDTH;


	// start times from zero
	time_offset = frames[0]->time;
	for(ix=0; ix<numframes; ix++)
		frames[ix]->time -= time_offset;

	pitch1 = pitchenv.pitch1;
	pitch2 = pitchenv.pitch2;
	duration = int(frames[numframes-1]->time * 1000);

if(max_y < 400)
	max_y = 200;
else
	max_y = 29000;  // disable auto height scaling
}  // end of SpectSeq::Load2


int SpectSeq::Import(wxInputStream& stream)
{//========================================
// Import data from Pratt analysis
	int  n = 0;


	wxTextInputStream text_stream(stream);
	name = _T("");
	text_stream >> n;
	amplitude = 100;
	max_y = 0;

	Load2(stream,1,n);
	return(0);
}  // end of SpectSeq::Import


int SPC2_size_cycle(CYCLE *cy)
/****************************/
/* Find number of bytes in cycle record */
{
	int  i;
	
	i = 44 + cy->n_harm;
	
	if(cy->flags & 1)
	{
		i += 4;     /* label */
	}
	return(i);   
}   /* end of size_cycle */




int SpectSeq::ImportSPC2(wxInputStream & stream)
{//=============================================
// load an spectrum with an old "SPC2" format
	int n_cycles = 0;
	int x;
	CYCLE cy;
	
	/* count number of cycles */
	while(!stream.Eof())
	{
		stream.TellI();
		stream.Read(&cy,44);
		stream.TellI();
		if(stream.Eof()) break;
		
		n_cycles++;
		x = SPC2_size_cycle(&cy) - 44;
		stream.SeekI(x,wxFromCurrent);
	}
	
	if(n_cycles == 0) return(0);
	
	name = _T("");
	amplitude = 100;
	max_y = 0;
	
	stream.SeekI(4);        // rewind and skip header
	Load2(stream,2,n_cycles);
	
	return(0);
}


int SpectSeq::Load(wxInputStream & stream)
{//=======================================
	int n;
	int ix;
	unsigned int id1, id2;

	wxDataInputStream s(stream);

	id1 = s.Read32();
	id2 = s.Read32();

	if(id1 == FILEID1_SPC2)
	{
		stream.SeekI(4);
		return(ImportSPC2(stream));
	}
	else
	if((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSEQ))
	{
			file_format = 0;   // eSpeak formants
	}
	else
	if((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSEK))
	{
			file_format = 1;   // formants for Klatt synthesizer
	}
	else
	if((id1 == FILEID1_SPECTSEQ) && (id2 == FILEID2_SPECTSQ2))
	{
			file_format = 2;   // formants for Klatt synthesizer
	}
	else
	{
		// Praat analysis data
		stream.SeekI(0);
		return(Import(stream));
	}

	name = s.ReadString();
	n = s.Read16();
	amplitude = s.Read16();
	max_y = s.Read16();
	s.Read16();
	Load2(stream,0,n);

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->keyframe)
			frames[ix]->length_adjust = frames[ix]->length - GetFrameLength(ix,0,NULL);
	}
	return(0);
}  // end of SpectSeq::Load


void SpectSeq::InterpolatePeak(int peak)
{//=====================================
	int  f, f1=0, f2;
	peak_t *p, *p1=NULL, *p2;
	double t1=0, t2;
	double interval;
	double ratio;
	int  first = 1;

	for(f2=0; f2 < numframes; f2++)
	{
		if(frames[f2]->keyframe)
		{
			t2 = frames[f2]->time;
			p2 = &frames[f2]->peaks[peak];

			if(first)
				first = 0;
			else
			{
				interval = t2 - t1;

				for(f=f1+1; f<f2; f++)
				{
					p = &frames[f]->peaks[peak];

					ratio = (frames[f]->time - t1)/interval;

					p->pkfreq = p1->pkfreq + int((p2->pkfreq - p1->pkfreq)*ratio);
					p->pkheight=p1->pkheight+int((p2->pkheight-p1->pkheight)*ratio);
					p->pkwidth = p1->pkwidth + int((p2->pkwidth - p1->pkwidth)*ratio);
					p->pkright =p1->pkright + int((p2->pkright - p2->pkright)*ratio);
				}
			}
			f1 = f2;
			t1 = t2;
			p1 = p2;
		}
	}
}  // end of SpectSeq::InterpolatePeak


void SpectSeq::InterpolatePeaks(int control)
{//=========================================
// 0=turn off  1=turn on
	int  f, peak;

	if(control==1)
	{
		for(peak=0; peak<N_PEAKS; peak++)
		{
			InterpolatePeak(peak);
		}
	}
	else
	{
		for(f=0; f<numframes; f++)
		{
			if(frames[f]->keyframe == 0)
				frames[f]->ZeroPeaks();
		}
	}
}  // end of SpectSeq::InterpolatePeaks


void SpectSeq::CopyDown(int frame, int direction)
{//==============================================
// Copy peaks down from next earlier/later keyframe
	int  f1;

	for(f1=frame+direction; f1>=0 && f1<numframes; f1 += direction)
	{
		if(frames[f1]->keyframe)
		{
			memcpy(frames[frame]->peaks, frames[f1]->peaks, sizeof(frames[frame]->peaks));
			memcpy(frames[frame]->klatt_param, frames[f1]->klatt_param, sizeof(frames[frame]->klatt_param));
			break;
		}
	}
}  //  end of CopyDown


void PeaksToFrame(SpectFrame *sp1, peak_t *pks, frame_t *fr)
{//=========================================================
	int  ix;
	int  x;

	fr->frflags = FRFLAG_KLATT;

	for(ix=0; ix < 8; ix++)
	{
		if(ix < 7)
		{
			fr->ffreq[ix] = pks[ix].pkfreq;
			fr->klatt_ap[ix] = pks[ix].klt_ap;
			fr->klatt_bp[ix] = pks[ix].klt_bp/2;
		}

		if(ix < 4)
			fr->bw[ix] = pks[ix].klt_bw/2;

		fr->fheight[ix] = pks[ix].pkheight >> 6;
		if(ix < 6)
		{
			if((x = (pks[ix].pkwidth >> 2)) > 255)
				x = 255;
			fr->fwidth[ix] = x;

			if(ix < 3)
			{
				if((x = (pks[ix].pkright >> 2)) > 255)
					x = 255;
				fr->fright[ix] = x;
			}
		}
	}

	for(ix=0; ix<N_KLATTP; ix++)
	{
		fr->klattp[ix] = sp1->klatt_param[ix];
	}
	fr->klattp[KLATT_FNZ] = sp1->klatt_param[KLATT_FNZ]/2;
	if(fr->fheight[1] == 0)
	{
		fr->klattp[KLATT_AV] -= 10;   // fade in/out
	}
}

static void SetSynth_mS(int length_mS, SpectFrame *sp1, SpectFrame *sp2, peak_t *pks1, peak_t *pks2, int control)
{//==============================================================================================================
	static frame_t fr1, fr2;

	PeaksToFrame(sp1,pks1,&fr1);
	PeaksToFrame(sp2,pks2,&fr2);

	if(voice->klattv[0])
	{
#ifdef INCLUDE_KLATT
		SetSynth_Klatt((length_mS * samplerate) / 1000, 0, &fr1, &fr2, voice, control);    // convert mS to samples
#endif
	}
	else
	{
		SetSynth((length_mS * samplerate) / 1000, 0, &fr1, &fr2, voice);    // convert mS to samples
	}
};


void SpectFrame::MakeHtab(int numh, int *htab, int pitch)
{//======================================================
	// interpolate the spectrum to give a harmonic table for
	// the given pitch (Hz<<12)

}  // end of SpectFrame::MakeHtab
