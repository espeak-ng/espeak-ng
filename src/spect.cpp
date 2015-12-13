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

#define DRAWPEAKWIDTH 2000
#define PEAKSHAPEW 256

#include <math.h>


static int default_freq[N_PEAKS] =
	{200,500,1200,3000,3500,4000,6900,7800,9000};
static int default_width[N_PEAKS] =
	{750,500,550,550,600,700,700,700,700};
static int default_klt_bw[N_PEAKS] =
	{89,90,140,260,260,260,500,500,500};

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
