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


static float GetFrameLength(SpectSeq &spect, int frame)
{//===============================================================
	int  ix;
	float  adjust=0;

	if(frame >= spect.numframes-1) return(0);
	
	for(ix=frame+1; ix<spect.numframes-1; ix++)
	{
		if(spect.frames[ix]->keyframe) break;  // reached next keyframe
		adjust += spect.frames[ix]->length_adjust;
	}
	return ((spect.frames[ix]->time - spect.frames[frame]->time) * 1000.0 + adjust);
}



int SpectSeq::Load(wxInputStream & stream)
{//=======================================
	int n;
	int ix;
	unsigned int id1, id2;
	int set_max_y=0;
	float time_offset;

	wxDataInputStream s(stream);

	id1 = s.Read32();
	id2 = s.Read32();

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
		fprintf(stderr, "Unsupported spectral file format.\n");
		return(1);
	}

	name = s.ReadString();
	n = s.Read16();
	amplitude = s.Read16();
	max_y = s.Read16();
	s.Read16();

	if(n==0) return(0);

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

		if(frame->Load(stream, file_format) != 0)
		{
			delete frame;
			break;
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

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->keyframe)
			frames[ix]->length_adjust = frames[ix]->length - GetFrameLength(*this,ix);
	}
	return(0);
}  // end of SpectSeq::Load
