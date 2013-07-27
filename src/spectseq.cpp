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
#include <math.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "main.h"
#include "wx/numdlg.h"
#include "wx/txtstrm.h"
#include "wx/datstrm.h"

#define MAX_HARMONIC  400           // 400 * 50Hz = 20 kHz, more than enough


extern void SetSynth_Klatt(int length, int modn, frame_t *fr1, frame_t *fr2, voice_t *v, int control);
extern int Wavegen_Klatt(int resume);

extern void SetSynth(int length, int modn, frame_t *fr1, frame_t *fr2, voice_t *v);
extern int Wavegen();
extern void CloseWaveFile2();
extern void KlattReset(int control);
extern FILE *f_wave;

static int frame_width;

int pk_select;
wxBrush CREAM_BRUSH(wxColour(255,253,245),wxSOLID);
wxPen BORDER_PEN(wxColour(255,240,0),4,wxSOLID);
wxPen VLIGHT_GREY_PEN(wxColour(230,230,230),1,wxSOLID);


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



void MakeWaveFile(int synthesis_method)
{//====================================
	int result=1;
	int resume=0;
	unsigned char wav_outbuf[1024];

	while(result != 0)
	{
		out_ptr = out_start = wav_outbuf;
		out_end = &wav_outbuf[sizeof(wav_outbuf)];

		if(synthesis_method == 1)
			result = Wavegen_Klatt(resume);
		else
			result = Wavegen();

		if(f_wave != NULL)
			fwrite(wav_outbuf, 1, out_ptr - wav_outbuf, f_wave);
		resume=1;
	}
}  // end of MakeWaveFile




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


void SpectSeq::ClipboardCopy()
{//===========================
	int  ix;
	int  nframes;
	int  count=0;

	nframes = CountSelected();
	if(nframes == 0) return;

	if(clipboard_spect != NULL)
		delete clipboard_spect;

	if((clipboard_spect = new SpectSeq(nframes))==NULL) return;

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->selected)
		{
			if((clipboard_spect->frames[count] = new SpectFrame(frames[ix])) == NULL)
				break;

			count++;
		}
	}
}  //  end of SpectSeq::ClipboardCopy



int SpectSeq::ClipboardInsert(int insert_at)
{//=========================================
	int  ix;
	int  j;
	int  total;
	int  result=insert_at;
	float thistime=0;
	float timeinc=0;
	float timeoffset=0;
	SpectFrame **frames2;

	if(clipboard_spect == NULL) return(result);
	if(clipboard_spect->numframes == 0) return(result);
	timeoffset = clipboard_spect->frames[0]->time;

	if(CountSelected() == 0)
		insert_at = -1;

	total = numframes + clipboard_spect->numframes;
	frames2 = new SpectFrame* [total];
	if(frames2 == NULL) return(result);

	total = 0;
	for(ix=0; ix<numframes; ix++)
	{
		thistime = frames[ix]->time;
		if(ix == insert_at)
		{
			result = total;
			for(j=0; j<clipboard_spect->numframes; j++)
			{
				frames2[total] = new SpectFrame(clipboard_spect->frames[j]);
				frames2[total]->time += (thistime - timeoffset);
				timeinc = frames2[total]->time - thistime + (frames2[total]->length/1000);
				total++;
			}
		}
		frames2[total] = new SpectFrame(frames[ix]);
		frames2[total++]->time += timeinc;
	}
	if(insert_at == -1)
	{
		// insert at the end
		result = total;
		for(j=0; j<clipboard_spect->numframes; j++)
		{
			 frames2[total] = new SpectFrame(clipboard_spect->frames[j]);
			 frames2[total++]->time += (thistime - timeoffset);
		}
	}
	delete frames;
	frames = frames2;
	numframes = total;
	return(result);
}  //  end of SpectSeq::ClipboardInsert


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
			if(frame->Import(stream) != 0) break;
		}
		else
		if(import==2)
		{
			if(frame->ImportSPC2(stream,time_acc) != 0) break;
		}
		else
		{
			if(frame->Load(stream, file_format) != 0) break;
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


int SpectSeq::Save(wxOutputStream &stream, int selection)
{//======================================================
	int ix;
	int count=numframes;

	if(selection)
	{
		count = CountSelected();
	}
	
	SetFrameLengths();
	
	wxDataOutputStream s(stream);

file_format = 2;   // inclue Klatt data in new saves

	s.Write32(FILEID1_SPECTSEQ);
	if(file_format == 2)
		s.Write32(FILEID2_SPECTSQ2);
	else
	if(file_format == 1)
		s.Write32(FILEID2_SPECTSEK);
	else
		s.Write32(FILEID2_SPECTSEQ);

	s.WriteString(name);
	s.Write16(count);
	s.Write16(amplitude);
	s.Write16(selection ? max_y : 0);
	s.Write16(0);    // spare
	for(ix=0; ix<numframes; ix++)
	{
		if((selection==0) || frames[ix]->selected)
		{
			if(frames[ix]->Save(stream, file_format) != 0) return(1);
		}
	}
	return(0);
}  // end of SpectSeq::Save



void SpectSeq::ConstructVowel(void)
{//================================

// not completed


	int ix;
	int j=0;
	int frames_selected[4];

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->selected)
		{
			if(ix >= 4)
				break;
			frames_selected[j++] = ix;
		}
	}
	if(j==0 || j>= 4)
		return;
	if(frames_selected[0] == 0)
		return;

	
}  // end of ConstructVowel




void SpectSeq::Draw(wxDC& dc, int start_y, int end_y)
{//==================================================
	int fm;
	int f, f1, f2;
   int x;

	if(end_y < start_y) return;

   if((start_y -= 4) < 0) start_y = 0;

	f1 = start_y / FRAME_HEIGHT;
	f2 = end_y / FRAME_HEIGHT;

	scaley = double(FRAME_HEIGHT) / max_y;
	scalex = double(frame_width) / max_x;
//	scalex = 0.6;

	for(fm=f1; fm <= f2 && fm < numframes; fm++)
	{
		if(frames[fm]->keyframe)
		{
			dc.SetBrush(CREAM_BRUSH);
			dc.SetPen(BORDER_PEN);
		}
		else
		{
			dc.SetBrush(*wxWHITE_BRUSH);
			dc.SetPen(*wxTRANSPARENT_PEN);
		}

		if(frames[fm]->selected)
			dc.SetPen(*wxRED_PEN);

		dc.DrawRectangle(0,FRAME_HEIGHT*fm+2,frame_width,
				FRAME_HEIGHT-2);
	}

	if(grid==1)
	{
		for(f=500; f<=MAX_DISPLAY_FREQ; f+=500)
		{
			x = int(f * scalex);
			if(x > max_x) break;
			if(f==3000 || f==6000 || f==9000)
				dc.SetPen(*wxLIGHT_GREY_PEN);
			else
				dc.SetPen(VLIGHT_GREY_PEN);
			dc.DrawLine(x,start_y,x,numframes*FRAME_HEIGHT);
		}
	}

	for(fm=f1; fm <= f2 && fm < numframes; fm++)
	{
		frames[fm]->Draw(dc,FRAME_HEIGHT*(fm+1),frame_width,
			scalex,scaley);
	}

}  // end of SpectSeq::Draw


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


void SpectSeq::InterpolateAdjacent(void)
{//=====================================
	int ix;
	int f1 = -1;
	int select = -1;
	int f2 = -1;
	float ratio;
	peak_t *p = NULL;
	peak_t *p1 = NULL;
	peak_t *p2 = NULL;

	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->selected)
			select = ix;
		else
		if(frames[ix]->keyframe)
		{
			if(select >= 0)
			{
				f2 = ix;
				break;
			}
			else
				f1 = ix;
		}
	}

	if(f1 < 0)
	{
		wxLogError(_T("No previous keyframe"));
		return;
	}
	if(select < 0)
	{
		wxLogError(_T("No selected frame"));
		return;
	}
	if(f2 < 0)
	{
		wxLogError(_T("No subsequent keyframe"));
		return;
	}
	// get ratio
	ix = wxGetNumberFromUser(_T("Interpolate between adjacent frames"),_T("percent"),_T(""),50);
	ratio = (float)ix/100.0;

	for(ix=0; ix<N_PEAKS; ix++)
	{
		p = &frames[select]->peaks[ix];
		p1 = &frames[f1]->peaks[ix];
		p2 = &frames[f2]->peaks[ix];

		p->pkfreq = p1->pkfreq + int((p2->pkfreq - p1->pkfreq)*ratio);
		p->pkheight=p1->pkheight+int((p2->pkheight-p1->pkheight)*ratio);
		p->pkwidth = p1->pkwidth + int((p2->pkwidth - p1->pkwidth)*ratio);
		p->pkright =p1->pkright + int((p2->pkright - p1->pkright)*ratio);
	}
	frames[select]->keyframe = 1;
	formantdlg->ShowFrame(this,select,1,0xff);
}


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


void SpectSeq::MakePitchenv(PitchEnvelope &pitchenv, int start_frame, int end_frame)
{//=================================================================================
	double f;
	double min=8000;
	double max=0;
	double diff;
	double t_start = -1;
	double t_end=0, t_diff;
	double yy;
	int  ix;
	int  x, y;
	int  xx;
	int  nx=0;
   float *ax, *ay;

	memset(pitchenv.env,127,128);

	for(ix=start_frame; ix<=end_frame; ix++)
	{
		if((f = frames[ix]->pitch) == 0) continue;
		nx++;

		t_end = frames[ix]->time;
		if(t_start < 0) t_start = t_end;

		if(f < min) min = f;
		if(f > max) max = f;
	}
	diff = max-min;
	t_diff = t_end - t_start;

	if(nx<2 || diff<=0 || t_diff<=0)
	{
		// no pitch info, use defaults
		pitchenv.pitch1=80;
		pitchenv.pitch2=120;
		return;
	}

	pitchenv.pitch1 = int(min);
	pitchenv.pitch2 = int(max);

	ax = new float [nx+1];
	ay = new float[nx+1];

	nx = 0;
	for(ix=start_frame; ix<=end_frame; ix++)
	{
		if((f = frames[ix]->pitch) == 0) continue;

		ax[++nx] = (frames[ix]->time - t_start) * 128 / t_diff;
		ay[nx] = (frames[ix]->pitch - min) * 255 / diff;
	}

	pitchenv.env[0] = int(ay[1]);
	pitchenv.env[127] = int(ay[nx]);

	// create pitch envelope by interpolating the time/pitch
	// values from the spectrum sequence
	xx = 1;
	for(x=1; x<127; x++)
	{
		while((ax[xx] < x) && (xx < nx)) xx++;
		if(xx < 3)
			yy = polint(&ax[xx-1],&ay[xx-1],3,(float)x);
		else if(xx > nx-1)
			yy = polint(&ax[xx-2],&ay[xx-2],3,(float)x);
		else
			yy = polint(&ax[xx-2],&ay[xx-2],4,(float)x);

		y = int(yy);
		if(y < 0) y = 0;
		if(y > 255) y = 255;
		pitchenv.env[x] = y;
	}

	delete ax;
	delete ay;
}  // end of SpectSeq::MakePitchenv


void SpectSeq::ApplyAmp_adjust(SpectFrame *sp, peak_t *peaks)
{//=============================================================
	int  ix;
	int  y;

	memcpy(peaks,sp->peaks,sizeof(*peaks)*N_PEAKS);

	for(ix=0; ix<N_PEAKS; ix++)
	{
		y = peaks[ix].pkheight * sp->amp_adjust * amplitude;
		peaks[ix].pkheight = y / 10000;
	}
}  // end of ApplyAmp_adjust



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
		SetSynth_Klatt((length_mS * samplerate) / 1000, 0, &fr1, &fr2, voice, control);    // convert mS to samples
	}
	else
	{
		SetSynth((length_mS * samplerate) / 1000, 0, &fr1, &fr2, voice);    // convert mS to samples
	}
};




void SpectSeq::MakeWave(int start, int end, PitchEnvelope &pitch)
{//==============================================================
	int  ix;
	int  length;
	int  len_samples;
	int  total_length;
	float  sum_length=0;
	float  prev_length=0;
	int  first;
	char *fname_speech;
	SpectFrame *sp1 = NULL;
	SpectFrame *sp2;
	double lfactor;
	peak_t peaks0[N_PEAKS];
	peak_t peaks1[N_PEAKS];
	peak_t peaks2[N_PEAKS];
	int synthesizer_type = 0;

	if(voice->klattv[0])
	{
		synthesizer_type = 1;
		KlattReset(2);
	}

	SpeakNextClause(NULL,NULL,2);  // stop speaking file

	if(numframes==0) return;
	SetFrameLengths();

	// find overall length of sequence
	for(ix=0; ix<numframes; ix++)
	{
		if(frames[ix]->keyframe)
		{
			sum_length += prev_length;
			prev_length = frames[ix]->length;
			sp2 = frames[ix];
			if(sp1 == NULL)
				sp1 = sp2;
		}
	}
	if(sp1 == NULL)
	{
		wxLogError(_T("(No frames have peaks set"));
		return;
	}

	total_length = int(sum_length);

	if((start==end) || (total_length == 0))
	{
		sp1->MakeWaveF(0,voicedlg->pitchenv,amplitude,duration);
		return;
	}


	if((duration > 0) && (duration < 40000))
		lfactor = double(duration)/double(total_length);
	else
	{
		duration = total_length;
		lfactor = 1;
	}

	len_samples = int(((total_length * lfactor + 50) * samplerate) / 1000);
	SetPitch(len_samples,pitch.env,9,44);

	fname_speech = WavFileName();
	OpenWaveFile2(fname_speech);

	first = 0;
	if(start > 0)
		first=1;     // a selection, use fade-in

	sp2 = NULL;
	for(ix=start; ix<=end; ix++)
	{
		if(frames[ix]->keyframe)
		{
			sp1 = sp2;
			sp2 = frames[ix];
			if(sp1 != NULL)
			{
				ApplyAmp_adjust(sp1,peaks1);
				ApplyAmp_adjust(sp2,peaks2);

				if(first)
				{
					PeaksZero(peaks1,peaks0);  // fade in

					SetSynth_mS(20,sp1,sp1,peaks0,peaks1,0);
					MakeWaveFile(synthesizer_type);
					first=0;
				}

				length = int(sp1->length * lfactor);
				SetSynth_mS(length,sp1,sp2,peaks1,peaks2,0);
				MakeWaveFile(synthesizer_type);
			}
		}
	}

	PeaksZero(peaks2,peaks0);  // fade out

	SetSynth_mS(30,sp2,sp2,peaks2,peaks0,2);
	MakeWaveFile(synthesizer_type);

	CloseWaveFile2();
	PlayWavFile(fname_speech);

}  // end of SpectSeq::MakeWave


void SpectFrame::MakeHtab(int numh, int *htab, int pitch)
{//======================================================
	// interpolate the spectrum to give a harmonic table for
	// the given pitch (Hz<<12)

}  // end of SpectFrame::MakeHtab


void SpectFrame::MakeWaveF(int control, PitchEnvelope &pitche, int amplitude, int duration)
{//======================================================================================
   //  amplitude:  percentage adjustment

	int  ix;
	int  length;   // mS
	int  len_samples;
	int  y;
	peak_t peaks0[N_PEAKS];
	peak_t peaks1[N_PEAKS];
	char *fname_speech;
	int synthesizer_type = 0;

	if(voice->klattv[0])
	{
		synthesizer_type = 1;
		KlattReset(2);
	}

	SpeakNextClause(NULL,NULL,2);  // stop speaking file

	length = duration;
	if(length==0)
		length = 200;   // default length, mS

	len_samples = (length * samplerate) / 1000;
	SetPitch(len_samples + 50,pitche.env,9,44);

	fname_speech = WavFileName();
	if(OpenWaveFile2(fname_speech) != 0)
		return;


	if(control==0)
	{
		memcpy(peaks1,peaks,sizeof(peaks1));

		for(ix=0; ix<N_PEAKS; ix++)
		{
			y = peaks1[ix].pkheight * amp_adjust * amplitude;
			peaks1[ix].pkheight = y/10000;
		}
		PeaksZero(peaks1,peaks0);
	
		SetSynth_mS(20,this,this,peaks0,peaks1,0);
		MakeWaveFile(synthesizer_type);
		SetSynth_mS(length,this,this,peaks1,peaks1,0);
		MakeWaveFile(synthesizer_type);
		SetSynth_mS(30,this,this,peaks1,peaks0,2);
		MakeWaveFile(synthesizer_type);
	}
	else
	{
#ifdef SPECT_EDITOR
		int maxh;
		USHORT htab0[600];

		maxh = nx;
		if(maxh >= 600)
			maxh = 600-1;
		
		for(ix=0; ix<=maxh; ix++)
			htab0[ix] = 0;
//		SetSynthHtab(20,htab0,maxh,dx,spect,maxh,dx);
		SetSynthHtab(20,spect,maxh,dx,spect,maxh,dx);
		MakeWaveFile(0);
		SetSynthHtab(length,spect,maxh,dx,spect,maxh,dx);
		MakeWaveFile(0);
		SetSynthHtab(30,spect,maxh,dx,htab0,maxh,dx);
		MakeWaveFile(0);
#endif
	}

	CloseWaveFile2();
	PlayWavFile(fname_speech);

}  // end of SpectFrame::MakeWaveFrame


