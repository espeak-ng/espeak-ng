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


#include "wx/wx.h"

#include "speak_lib.h"
#include "speech.h"
#include "main.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "options.h"
#include "wx/txtstrm.h"
#include "wx/brush.h"
#include "wx/datstrm.h"


extern int PeaksToHarmspect(wavegen_peaks_t *peaks, int pitch, int *htab, int control);

extern unsigned char pk_shape1[];
extern int pk_select;
extern char voice_name[];

wxPen BLUE_PEN(wxColour(0,0,255),2,wxSOLID);
wxBrush BRUSH_SELECTED_PEAK(wxColour(255,180,180),wxSOLID);
wxBrush BRUSH_MARKER[N_MARKERS] = {
	wxBrush(wxColour(200,0,255),wxSOLID),
	wxBrush(wxColour(255,0,0),wxSOLID),
	wxBrush(wxColour(255,200,0),wxSOLID),
	wxBrush(wxColour(0,255,0),wxSOLID),
	wxBrush(wxColour(0,255,255),wxSOLID),
	wxBrush(wxColour(200,0,255),wxSOLID),
	wxBrush(wxColour(200,0,255),wxSOLID),
	wxBrush(wxColour(255,0,200),wxSOLID) };
	
#define DRAWPEAKWIDTH 2000
#define PEAKSHAPEW 256

#include <math.h>


static int default_freq[N_PEAKS] =
	{200,500,1200,3000,3500,4000,6900,7800,9000};
static int default_width[N_PEAKS] =
	{750,500,550,550,600,700,700,700,700};


float SpectTilt(int value, int freq)
{//=================================
	float x;
	float y;

	if((currentcanvas == NULL) || (currentcanvas->spectseq->bass_reduction == 0))
		return(float(value));

	y = value*value*2;

	if(freq < 600)
	{
		return(sqrt(y/2.5));
	}
	else
	if(freq < 1050)
	{
		x =  1.0 + ((1050.0-freq)* 1.5)/450.0;
		return(sqrt(y/x));
	}
	else
	{
		return(sqrt(y));	
	}
}


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
   }

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
	if(nx==0)
	{
		nx = int(8000/dx);
		spect_data = new USHORT[nx];
		for(ix=0; ix<nx; ix++)
			spect_data[ix] = 1;
		max_y = 1;
	}
   spect = spect_data;
	free(p);
	return(0);
}  //  end of ImportSPC2



int SpectFrame::Load(wxInputStream& stream)
{//========================================
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

	for(ix=0; ix<N_PEAKS; ix++)
	{
		formants[ix].freq = s.Read16();
		formants[ix].bandw = s.Read16();
		peaks[ix].pkfreq = s.Read16();
		if((peaks[ix].pkheight = s.Read16()) > 0)
			keyframe = 1;
		peaks[ix].pkwidth = s.Read16();
		peaks[ix].pkright = s.Read16();
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


int SpectFrame::Save(wxOutputStream& stream)
{//=========================================
	int ix;

   wxDataOutputStream s(stream);

	s.WriteDouble(time);
	s.WriteDouble(pitch);
	s.WriteDouble(length);
	s.WriteDouble(dx);
	s.Write16(nx);
	s.Write16(markers);
	s.Write16(amp_adjust);

	for(ix=0; ix<N_PEAKS; ix++)
	{
		s.Write16(formants[ix].freq);
		s.Write16(formants[ix].bandw);

		s.Write16(peaks[ix].pkfreq);
		s.Write16(keyframe ? peaks[ix].pkheight : 0);
		s.Write16(peaks[ix].pkwidth);
		s.Write16(peaks[ix].pkright);
	}

	for(ix=0; ix<nx; ix++)
	{
		s.Write16(spect[ix]);
	}

	return(0);
}  //  end of SpectFrame::Save


void SpectFrame::ZeroPeaks()
{//=========================
	int pk;

	for(pk=0; pk<N_PEAKS; pk++)
		peaks[pk].pkheight = 0;
}



void SpectFrame::CopyPeaks(SpectFrame *sf)
{//=======================================
	memcpy(peaks,sf->peaks,sizeof(peaks));
	keyframe = sf->keyframe;
}



void SpectFrame::ToggleMarker(int n)
{//=================================
	markers ^= 1<<n;
}


void SpectFrame::ApplyVoiceMods()
{//==============================
	// apply the modifications to the formants which are defined in the current voice
	int pk;
	char voice_name1[40];

	strcpy(voice_name1, voice_name2);  // remember current voice name

	if(LoadVoice(path_modifiervoice.mb_str(wxConvLocal),0x13) == NULL)
	{
		wxLogError(_T("Can't read voice: ")+path_modifiervoice);
		OnOptions2(MENU_PATH4);
		return;
	}

	wxLogStatus(_T("Convert using voice: ")+path_modifiervoice);

	for(pk=0; pk<N_PEAKS; pk++)
	{
		peaks[pk].pkfreq = (peaks[pk].pkfreq * voice->freq2[pk])/256;
		peaks[pk].pkheight = (peaks[pk].pkheight * voice->height2[pk])/256;
		peaks[pk].pkwidth = (peaks[pk].pkwidth * voice->width2[pk])/256;
		peaks[pk].pkright = (peaks[pk].pkright * voice->width2[pk])/256;
	}
	LoadVoice(voice_name1,1);
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
//	DrawPeaks(NULL,0,0,amp);
	return(rms);
}



void SpectFrame::DrawPeaks(wxDC *dc, int offy, int frame_width, int seq_amplitude, double scale_x)
{//==============================================================================================
 // dc==NULL means don't draw, just calculate RMS

	int peak;
	peak_t *pk;
	int  x1,x2,x3,width,ix;
	int  y1, y2;
	double yy;
	int max_ix;
	int buf[DRAWPEAKWIDTH*2];

	max_ix = int(9000 * scale_x);

	memset(buf,0,sizeof(buf));
	for(peak=0; peak<N_PEAKS; peak++)
	{
	   pk = &peaks[peak];

		if((pk->pkfreq == 0) || (pk->pkheight==0)) continue;

		x1 = (int)(pk->pkfreq*scale_x);
		x2 = (int)((pk->pkfreq + pk->pkright)*scale_x);
		x3 = (int)((pk->pkfreq - pk->pkwidth)*scale_x);

		if(x3 >= DRAWPEAKWIDTH)
			continue;   // whole peak is off the scale

		if((width = x2-x1) <= 0) continue;
		for(ix=0; ix<width; ix++)
		{
			buf[x1+ix] += pk->pkheight * pk_shape1[(ix*PEAKSHAPEW)/width];
		}

		if((width = x1-x3) <= 0) continue;
		for(ix=1; ix<width; ix++)
		{
			if(x3+ix >= 0)
			{
				buf[x3+ix] += pk->pkheight * pk_shape1[((width-ix)*PEAKSHAPEW)/width];
			}
		}
	}

	rms = buf[0]>>12;
	rms = rms*rms*23;
	rms = rms*rms;

	if(dc != NULL) dc->SetPen(*wxGREEN_PEN);
	x1 = 0;
	y1 = offy - ((buf[0] * FRAME_HEIGHT) >> 21);
	for(ix=1; ix<max_ix; ix++)
	{
		yy = buf[ix]>>12;
		yy = yy*yy*23;
		rms += (yy*yy);

		x2 = ix;
		y2 = offy - ((buf[ix] * FRAME_HEIGHT) >> 21);
		if(dc != NULL) dc->DrawLine(x1,y1,x2,y2);
		x1 = x2;
		y1 = y2;
	}
	rms = sqrt(rms)/200000.0;
	// apply adjustment from spectseq amplitude
	rms = rms * seq_amplitude * amp_adjust / 10000.0;

rms = GetRms(seq_amplitude);
}  // end of SpectFrame::DrawPeaks



void SpectFrame::Draw(wxDC& dc, int offy, int frame_width, double scalex, double scaley)
{//=====================================================================================
	int  pt;
	int  peak;
   peak_t *pk;
	int  ix;
	double x0, x1;
	int  y0, y1;
	int  x, x2, x3;
	double xinc;
	double yf;
	int font_height;
	wxString text;

	if(currentcanvas == NULL)
		return;

	dc.SetFont(*wxSWISS_FONT);

	xinc = dx * scalex;

	x0 = xinc;
	x1 = nx * xinc;

	if(selected)  // this frame is selected
	{
		// highlight selected peak by drawing a red triangle
		pk = &peaks[pk_select];

		x2 = int(pk->pkright * scalex * 0.44);
		x3 = int(pk->pkwidth * scalex * 0.44);
		x = int((pk->pkfreq) * scalex);
		y1 = (pk->pkheight * FRAME_HEIGHT) >> 14;
		if(y1 < 5) y1 = 5;

		wxPoint triangle[3];
		dc.SetBrush(BRUSH_SELECTED_PEAK);
		dc.SetPen(*wxTRANSPARENT_PEN);
		triangle[0] = wxPoint(0,-y1);
		triangle[1] = wxPoint(x2,0);
		triangle[2] = wxPoint(-x3,0);
		dc.DrawPolygon(3,triangle,x,offy);
	}

	// draw the measured formants
	dc.SetPen(BLUE_PEN);
	for(peak=1; peak<=5; peak++)
	{
		if(formants[peak].freq != 0)
		{
		   // set height from linear interpolation of the adjacent
			// points in the spectrum
			pt = (int)(formants[peak].freq / dx);
			y0 = spect[pt-1];
			y1 = spect[pt];
			yf = (y1-y0) * (formants[peak].freq - pt*dx)/dx;

			y1 = offy - (int)((y0+yf) * scaley);
			x1 = formants[peak].freq * scalex;
			dc.DrawLine((int)x1,offy,(int)x1,y1);

		}
	}

	// draw the spectrum outline
	if(keyframe)
		dc.SetPen(*wxBLACK_PEN);
	else
		dc.SetPen(*wxMEDIUM_GREY_PEN);

   if(spect != NULL)
   {
		y0 = offy - (int)(spect[0] * scaley);
		for(pt=1; pt<nx; pt++)
	   {
		   x1 = x0 + xinc;
		   y1 = offy - (int)(SpectTilt(spect[pt],int(pt*dx)) * scaley);
		   dc.DrawLine((int)x0,y0,(int)x1,y1);
		   x0 = x1;
		   y0 = y1;
      }
	}

	if(currentcanvas->zoom < 2)
		dc.SetFont(FONT_SMALL);
	else
		dc.SetFont(FONT_MEDIUM);


	// Markers
	x = frame_width - 120 - 32;
	for(ix=0; ix<N_MARKERS; ix++)
	{
		if(markers & 1<<ix)
		{
			dc.SetBrush(BRUSH_MARKER[ix]);
			y0 = offy-FRAME_HEIGHT+22;
			dc.DrawRectangle(x,y0,22,22);
			if(currentcanvas->zoom > 2)
			{
				text.Printf(_T("%d"),ix);
				dc.DrawText(text,x+2,y0);
			}
			x -= 26;
		}
	}

	DrawPeaks(&dc,offy,frame_width,currentcanvas->spectseq->amplitude,scalex);

	font_height = int(15 / currentcanvas->zoomy);

	text.Printf(_T("%3dmS  %.1fHz"),int(time*1000),pitch);
	dc.DrawText(text,frame_width-130,offy-FRAME_HEIGHT+20+font_height);
	if(keyframe || rms > 0)
	{
		text.Printf(_T("%3d"),(int)rms);
		dc.DrawText(text,frame_width-130,offy-FRAME_HEIGHT+20+font_height*2);
	}
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(0,offy,frame_width,offy);  // base line

}  // end of SpectFrame::Draw

