/***************************************************************************
 *   Copyright (C) 2006 to 2007 by Jonathan Duddington                     *
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



// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wx.h"
#include "wx/mdi.h"


#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"

#include "wx/textctrl.h"
#include "wx/checkbox.h"

FormantDlg *formantdlg=NULL;
ByteGraph *pitchgraph=NULL;



BEGIN_EVENT_TABLE(FormantDlg, wxPanel)
	EVT_BUTTON(T_ZOOMOUT,FormantDlg::OnCommand)
	EVT_BUTTON(T_ZOOMIN,FormantDlg::OnCommand)
	EVT_SPINCTRL(-1,FormantDlg::OnSpin)
//	EVT_SPINCTRL(T_TIMESEQ,FormantDlg::OnSpin)
//	EVT_SPINCTRL(T_AMPFRAME,FormantDlg::OnSpin)
//	EVT_SPINCTRL(T_TIMEFRAME,FormantDlg::OnSpin)
END_EVENT_TABLE()


void FormantDlg::OnCommand(wxCommandEvent& event)
{//=============================================
	int id;

	if(currentcanvas == NULL)
		return;

	switch(id = event.GetId())
	{
	case T_ZOOMIN:
	case T_ZOOMOUT:
		currentcanvas->OnZoom(id);
		break;

	case T_AMPLITUDE:
		currentcanvas->RefreshDialogValues(0);
		break;

	case T_TIMEFRAME:
	case T_AMPFRAME:
		currentcanvas->RefreshDialogValues(1);
		break;
	}
	currentcanvas->SetFocus();
}

void FormantDlg::OnSpin(wxSpinEvent& event)
{//=============================================
	int id;

	if(currentcanvas == NULL)
		return;

	switch(id = event.GetId())
	{
	case T_AMPFRAME:
		currentcanvas->RefreshDialogValues(1);
		break;

	default:
		currentcanvas->RefreshDialogValues(0);
		break;
	}
	currentcanvas->SetFocus();
}

FormantDlg::FormantDlg(wxWindow *parent) : wxPanel(parent,-1,wxDefaultPosition,wxSize(400,1000))
{//=============================================================================================
	int  ix;
	int  y;
	int  xplace;
	int  id;
	wxString string;

	int height;
	int width;
	int x;
	int y2 = 420;
	int y3 = 520;

	wxClientDisplayRect(&x,&y,&width, &height);
#ifdef PLATFORM_WINDOWS
	if(height <= 768)
	{
		y2 = 410;
		y3 = 508;
	}
#else
	if(height <= 800)
	{
		y2 = 378;
		y3 = 456;
	}
#endif
	y = 24;
	xplace = 28;
	id = 201;
	new wxStaticText(this,-1,_T("Formants"),wxPoint(4,5));
	new wxStaticText(this,-1,_T("  Ht     Width                (Klatt)"),wxPoint(78,5),wxSize(300,20));
	new wxStaticText(this,-1,_T("Bw      Ap      Bp"),wxPoint(188,29),wxSize(300,20));

	for(ix=0; ix < 8; ix++)
	{
		string.Printf(_T("%d"),ix);
		t_labpk[ix] = new wxStaticText(this,id++,string,
			wxPoint(xplace-22,y+1+24*ix));

		if(ix < 7)
		{
			t_pkfreq[ix] = new wxTextCtrl(this,id++,_T(""),
				wxPoint(xplace,y+24*ix),wxSize(44,20),
				wxTE_CENTRE);
		}

		t_pkheight[ix] = new wxTextCtrl(this,id++,_T(""),
			wxPoint(xplace+46,y+24*ix),wxSize(36,20),
			wxTE_CENTRE);

		if(ix < 6)
		{
			t_pkwidth[ix] = new wxTextCtrl(this,id++,_T(""),
				wxPoint(xplace+84,y+24*ix),wxSize(60,20),
				wxTE_CENTRE);
		}

		if((ix == 0) || (ix > 6)) continue;

		if(ix < 4)
		{
			t_klt_bw[ix] = new wxTextCtrl(this,id++,_T(""),
				wxPoint(xplace+150,y+24*ix),wxSize(40,20),
				wxTE_CENTRE);
		}
		t_klt_ap[ix] = new wxTextCtrl(this,id++,_T(""),
			wxPoint(xplace+192,y+24*ix),wxSize(36,20),
			wxTE_CENTRE);
		t_klt_bp[ix] = new wxTextCtrl(this,id++,_T(""),
			wxPoint(xplace+230,y+24*ix),wxSize(40,20),
			wxTE_CENTRE);
	}

	y=224;
	t_timeframe = new wxSpinCtrl(this,T_TIMEFRAME,_T(""), wxPoint(6,y+0), wxSize(52,24), wxTE_CENTRE,0,500);
	t_orig_frame = new wxStaticText(this,-1,_T("mS"),wxPoint(61,y+8));
	t_ampframe = new wxSpinCtrl(this,T_AMPFRAME,_T(""), wxPoint(104,y+0), wxSize(52,24), wxTE_CENTRE,0,500);
	t_lab[3] = new wxStaticText(this,-1,_T("% amp - Frame"),wxPoint(159,y+8));

	y += 40;
	s_klatt[KLATT_AV] = new wxSpinCtrl(this,T_AV,_T(""), wxPoint(6,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_AV] = new wxStaticText(this,-1,_T("AV"),wxPoint(61,y+4));

	s_klatt[KLATT_FNZ] = new wxSpinCtrl(this,T_FNZ,_T(""), wxPoint(104,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_FNZ] = new wxStaticText(this,-1,_T("FNZ"),wxPoint(159,y+4));


	y += 28;
	s_klatt[KLATT_Tilt] = new wxSpinCtrl(this,T_TILT,_T(""), wxPoint(6,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Tilt] = new wxStaticText(this,-1,_T("Tilt"),wxPoint(61,y+4));

	s_klatt[KLATT_Aspr] = new wxSpinCtrl(this,T_ASPR,_T(""), wxPoint(104,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Aspr] = new wxStaticText(this,-1,_T("Aspr"),wxPoint(159,y+4));

	s_klatt[KLATT_Skew] = new wxSpinCtrl(this,T_SKEW,_T(""), wxPoint(202,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Skew] = new wxStaticText(this,-1,_T("Skew"),wxPoint(257,y+4));

	y += 28;
	s_klatt[KLATT_AVp] = new wxSpinCtrl(this,T_AVP,_T(""), wxPoint(6,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_AVp] = new wxStaticText(this,-1,_T("AVp"),wxPoint(61,y+4));

	s_klatt[KLATT_Fric] = new wxSpinCtrl(this,T_FRIC,_T(""), wxPoint(104,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Fric] = new wxStaticText(this,-1,_T("Fric"),wxPoint(159,y+4));

	s_klatt[KLATT_FricBP] = new wxSpinCtrl(this,T_FRICBP,_T(""), wxPoint(202,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_FricBP] = new wxStaticText(this,-1,_T("FricBP"),wxPoint(257,y+4));

	y += 28;
	s_klatt[KLATT_Kopen] = new wxSpinCtrl(this,T_KOPEN,_T(""), wxPoint(6,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Kopen] = new wxStaticText(this,-1,_T("kopen"),wxPoint(61,y+4));

	s_klatt[KLATT_Turb] = new wxSpinCtrl(this,T_TURB,_T(""), wxPoint(104,y), wxSize(52,24), wxTE_CENTRE,0,500);
	t_klatt[KLATT_Turb] = new wxStaticText(this,-1,_T("Turb"),wxPoint(159,y+4));



	t_zoomout = new wxButton(this,T_ZOOMOUT,_T("Zoom-"),wxPoint(16,y2));
	t_zoomin = new wxButton(this,T_ZOOMIN,_T("Zoom+"),wxPoint(106,y2));

	y = y2 + 46;
	t_amplitude = new wxSpinCtrl(this,T_AMPLITUDE,_T(""),
		wxPoint(6,y),wxSize(52,24),wxTE_CENTRE,0,y+130);
	t_lab[2] = new wxStaticText(this,-1,_T("% amp - Sequence"),wxPoint(61,y+4));
//	t_timeseq = new wxSpinCtrl(this,T_TIMESEQ,_T(""),
//		wxPoint(6,400),wxSize(52,24),wxTE_CENTRE,0,500);
	t_orig_seq = new wxStaticText(this,-1,_T("mS"),wxPoint(61,y+30));

	t_pitch = new wxStaticText(this,-1,_T(""),wxPoint(4,y3),wxSize(192,24));

	pitchgraph = new ByteGraph(this,wxPoint(0,y3+18),wxSize(200,140));
	pitchgraph->SetData(128,env_fall);
	pitchgraph->ShowSpectrum(1);
	pitchgraph->Show();
}



void FormantDlg::GetValues(SpectSeq *spectseq, int frame)
{//======================================================
	int ix;
	wxString value;
	long num;
	SpectFrame *sf;

	if(spectseq->frames == NULL)
		return;
	sf = spectseq->frames[frame];

	for(ix=0; ix < 8; ix++)
	{
		if(ix < 7)
		{
			num = 0;
			value = t_pkfreq[ix]->GetValue();
			value.ToLong(&num);
			sf->peaks[ix].pkfreq = num;
		}

		num = 0;
		value = t_pkheight[ix]->GetValue();
		value.ToLong(&num);
		sf->peaks[ix].pkheight = num << 6;

		if(ix < 6)
		{
			num = 0;
			value = t_pkwidth[ix]->GetValue();
			value.ToLong(&num);
			sf->peaks[ix].pkwidth = sf->peaks[ix].pkright = num*2;

			if((ix < 3) && ((value = value.AfterFirst('/')) != wxEmptyString))
			{
				num = 0;
				value.ToLong(&num);
				sf->peaks[ix].pkright = num*2;
			}
		}
	}

	for(ix=1; ix < 7; ix++)
	{
		if(ix < 4)
		{
			num = 0;
			value = t_klt_bw[ix]->GetValue();
			value.ToLong(&num);
			sf->peaks[ix].klt_bw = num;
		}

		num = 0;
		value = t_klt_ap[ix]->GetValue();
		value.ToLong(&num);
		sf->peaks[ix].klt_ap = num;

		num = 0;
		value = t_klt_bp[ix]->GetValue();
		value.ToLong(&num);
		sf->peaks[ix].klt_bp = num;
	}
}  // end of FormantDlg::GetValues



void FormantDlg::ShowFrame(SpectSeq *spectseq, int frame, int pk, int field)
{//==============================================================
	int  ix;
	SpectFrame *sf;
	wxString value;
	int original_mS;
	peak_t *peak;

	if(spectseq->frames == NULL)
		return;
	sf = spectseq->frames[frame];

	if(field == 0xff)
	{
		// indicate the selected peak
//		t_select_peak[pk]->SetValue(TRUE);
	}

	for(ix=0; ix < 8; ix++)
	{
		if(field != 0xff && pk!=ix)
			continue;

		if(field == 0xff)
		{
			if(pk==ix)
				value.Printf(_T("%d="),ix);
			else
				value.Printf(_T("%d"),ix);
			t_labpk[ix]->SetLabel(value);
		}

		peak = &(sf->peaks[ix]);

		if((field & 1) && (ix < 7))
		{
			value.Printf(_T("%4d"),sf->peaks[ix].pkfreq);
			t_pkfreq[ix]->SetValue(value);
		}
		if(field & 2)
		{
			value.Printf(_T("%3d"),sf->peaks[ix].pkheight >> 6);
			t_pkheight[ix]->SetValue(value);
		}
		if((field & 4) && (ix < 6))
		{
			if(sf->peaks[ix].pkwidth == sf->peaks[ix].pkright)
				value.Printf(_T("%3d"),sf->peaks[ix].pkwidth/2);
			else
				value.Printf(_T("%3d/%3d"),sf->peaks[ix].pkwidth/2, sf->peaks[ix].pkright/2);
			t_pkwidth[ix]->SetValue(value);
		}

		if((ix > 0) && (ix < 7))
		{
			if((field & 8) && (ix < 4))
			{
				value.Printf(_T("%3d"),peak->klt_bw);
				t_klt_bw[ix]->SetValue(value);
			}

			if(field & 16)
			{
				value.Printf(_T("%3d"),peak->klt_ap);
				t_klt_ap[ix]->SetValue(value);
			}

			if(field & 32)
			{
				value.Printf(_T("%3d"),peak->klt_bp);
				t_klt_bp[ix]->SetValue(value);
			} 
		}
	}

	// find the time until the next keyframe
	SetSpinCtrl(t_timeframe,int(spectseq->GetFrameLength(frame,1,&original_mS)+0.5)); // round to nearest integer
	value.Printf(_T("%d mS"),original_mS);
	t_orig_frame->SetLabel(value);
	value.Printf(_T("%3d"),sf->amp_adjust);
	t_ampframe->SetValue(value);

	for(ix=0; ix<N_KLATTP; ix++)
	{
		SetSpinCtrl(s_klatt[ix], sf->klatt_param[ix]);
	}

}  //  end of FormantDlg::ShowFrame


BEGIN_EVENT_TABLE(ByteGraph,wxScrolledWindow)
	EVT_LEFT_DOWN(ByteGraph::OnMouse)
END_EVENT_TABLE()

static wxBrush BRUSH_FORMANT(wxColour(255,100,50),wxSOLID);
static wxPen PEN_KEYFRAME(wxColour(255,220,100),2,wxSOLID);
static wxPen PEN_KEYFORMANT(wxColour(0,0,0),2,wxSOLID);


ByteGraph::ByteGraph(wxWindow *parent, const wxPoint& pos, const wxSize &size):
    wxScrolledWindow(parent,-1,pos,size,wxSUNKEN_BORDER)
{//============================================================
	npoints = 0;
	show_spectrum = 0;
}  //  end of ByteGraph::ByteGraph


void ByteGraph::SetData(int nx, unsigned char *data)
{//=================================================
	npoints = nx;
	graph = data;
   Refresh();
} // end of ByteGraph::SetData


void ByteGraph::ShowSpectrum(int yes)
{//==================================
	show_spectrum = yes;
}


void ByteGraph::DrawSpectrum(wxDC& dc)
{//===================================
	SpectSeq *seq;
	SpectFrame *sf;
	int  ix;
	int  x, y;
	int  pk;
	double length;
	int numframes;
	int width, height;
	double xscale, yscale;

	spectrum_scale = 0;
	if(currentcanvas==NULL) return;
	if((seq = currentcanvas->spectseq) == NULL) return;
	numframes = seq->numframes;

	if(numframes == 0) return;
	
	GetClientSize(&width,&height);

	if(((length = seq->frames[numframes-1]->time)==0) && (numframes>1))
		length = seq->frames[numframes-2]->time;
		
	length = length - seq->frames[0]->time;

	yscale = height/3000.0;
	spectrum_scale = xscale = width/length;

	dc.SetBrush(BRUSH_FORMANT);
	dc.SetPen(*wxTRANSPARENT_PEN);
	for(ix=0; ix<numframes; ix++)
	{
		sf = seq->frames[ix];
		x = int(sf->time*xscale);

		if(sf->keyframe)
		{
			dc.SetPen(PEN_KEYFRAME);
			dc.DrawLine(x,0,x,height);
		}

		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawCircle(x,int(height-(sf->formants[1].freq-200)*yscale),3);
		dc.DrawCircle(x,int(height-(sf->formants[2].freq-200)*yscale),3);
		dc.DrawCircle(x,int(height-(sf->formants[3].freq-200)*yscale),3);

		if(sf->keyframe)
		{
			dc.SetPen(PEN_KEYFORMANT);
			for(pk=1; pk<6; pk++)
			{
				y = height-int((sf->peaks[pk].pkfreq-200)*yscale);
				dc.DrawLine(x-3,y,x+3,y);
			}
		}
	}
}


void ByteGraph::OnDraw(wxDC &dc)
{//============================
	int  x, y, x2, y2, ix;
	int  width, height;
	double xscale, yscale;

	if(show_spectrum) DrawSpectrum(dc);
	
	if(npoints==0) return;

	GetClientSize(&width,&height);

	xscale = double(width) / npoints;
	yscale = double(height) / 256.0;

	if(npoints > 8)
	{
		// draw pitch profile
		dc.SetPen(*wxMEDIUM_GREY_PEN);
		x = 0;
		y = height - int(graph[0]*yscale);
		for(ix=1; ix<npoints; ix++)
		{
			dc.DrawLine(x,y,x2=int(ix*xscale),y2=height - int(graph[ix]*yscale));
			x = x2;
			y = y2;
		}
	}
}  // end of ByteGraph::OnDraw


void ByteGraph::OnMouse(wxMouseEvent& event)
{//========================================
	int  ix;
	SpectSeq *seq;
	double time;
	double t0, t1;
	double t2=0;

	if(currentcanvas==NULL) return;
	if((seq = currentcanvas->spectseq) == NULL) return;

	wxClientDC dc(this);
	PrepareDC(dc);

	wxPoint pt(event.GetLogicalPosition(dc));
	t0 = t2 = seq->frames[0]->time;
	time = (double(pt.x) / spectrum_scale) + t0;
	
	for(ix=1; ix<seq->numframes; ix++)
	{
		t1=t2;
		t2 = seq->frames[ix]->time;
		if(time <= t1 + (t2-t1)/2)
		{
			currentcanvas->SelectFrame(ix-1);
			break;
		}
	}
}  // end of ByteGraph::OnMouse

