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

#include "wx/wfstream.h"
#include "wx/filename.h"
#include "wx/datstrm.h"
#include "wx/listimpl.cpp"
#include "wx/numdlg.h"
#include "wx/utils.h"

#include <ctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "main.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "options.h"

extern MyFrame *frame ;
#define SCROLLUNITS  20

#define N_CANVASLIST   50
static int canvaslistix=0;
static SpectDisplay *canvaslist[N_CANVASLIST];
SpectSeq *clipboard_spect=NULL;
wxMenu *menu_spectdisplay=NULL;

BEGIN_EVENT_TABLE(SpectDisplay, wxScrolledWindow)
	EVT_LEFT_DOWN(SpectDisplay::OnMouse)
	EVT_RIGHT_DOWN(SpectDisplay::OnMouse)
	EVT_KEY_DOWN(SpectDisplay::OnKey)

	EVT_MENU(-1, SpectDisplay::OnMenu)

END_EVENT_TABLE()

// Define a constructor for my canvas
SpectDisplay::SpectDisplay(wxWindow *parent, const wxPoint& pos, const wxSize& size, SpectSeq *spect)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER | wxNO_FULL_REPAINT_ON_RESIZE)
{
   wxString filename;

//	SetCursor(wxCursor(wxCURSOR_PENCIL));
	SetBackgroundColour(* wxWHITE);

	zoom = 2;
	zoomx = 0.5;
	zoomy = 0.5;

	savepath = _T("");
	spectseq = spect;
	sframe = 0;
	pk_num = 1;

	SetExtent();
	
	if(canvaslistix < N_CANVASLIST-1)
		canvaslist[canvaslistix++] = this;
//	WavegenInitPkData(0);
}  // end of SpectDisplay::SpectDisplay



SpectDisplay::~SpectDisplay()
{//==========================
	// delete this from the canvaslist
	int  ix;

	for(ix=0; ix<canvaslistix; ix++)
	{
		if(canvaslist[ix] == this)
		{
			for(ix=ix+1; ix<canvaslistix; ix++)
				canvaslist[ix-1] = canvaslist[ix];
			canvaslistix--;
			break;
		}
	}

	if(currentcanvas==this)
	{
		if(canvaslistix > 0)
			currentcanvas = canvaslist[0];
		else
			currentcanvas = NULL;
	}
}


void SpectDisplay::SetExtent()
{//===========================
	SetScrollbars(SCROLLUNITS,SCROLLUNITS,
		(int)(FRAME_WIDTH*zoomx/SCROLLUNITS)+1,
		(int)(spectseq->numframes*FRAME_HEIGHT*zoomy/SCROLLUNITS)+1);
	Refresh();
}  //  end of SpectDisplay::SetExtent


void SpectDisplay::OnDraw(wxDC& dc)
{//================================
	wxRegion region;

	dc.SetUserScale(zoomx,zoomy);

	int f1 = 0x7fffffff, f2 = -1;
	int x1,x2,y1,y2;
	int vX,vY,vW,vH;        // Dimensions of client area in pixels
	wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

	while (upd)
	{
		vX = upd.GetX();
		vY = upd.GetY();
		vW = upd.GetW();
		vH = upd.GetH();

		CalcUnscrolledPosition(vX,vY,&x1,&y1);
		CalcUnscrolledPosition(vX+vW,vY+vH,&x2,&y2);
    // Repaint this rectangle

		if(y1 < f1) f1 = int(y1/zoomy);
		if(y2 > f2) f2 = int(y2/zoomy);

		upd ++ ;
	}

	if(spectseq != NULL)
		spectseq->Draw(dc,f1,f2);
}


void SpectDisplay::OnActivate(int active)
{//======================================
	if(active)
	{
		currentcanvas = this;
		SetFocus();    // this doesn't seem to work

		if(sframe >= 0)
			formantdlg->ShowFrame(spectseq,sframe,pk_num,0xff);
		WriteDialogValues();

		if(wxGetKeyState(WXK_CONTROL))
		{
			ReadDialogValues();
			spectseq->MakeWave(0,spectseq->numframes-1,voicedlg->pitchenv);
		}
	}
	else
	{
		ReadDialogValues();
	}
}


void SetNumeric(wxTextCtrl *t, int value)
//{=====================================
{
	wxString string;

	string.Printf(_T("%d"),value);
	t->SetValue(string);
}

void SetSpinCtrl(wxSpinCtrl *t, int value)
//{=======================================
{
	wxString string;

	string.Printf(_T("%d"),value);
	t->SetValue(string);
}




int GetNumeric(wxTextCtrl *t)
//{==========================
{
	return(atoi(t->GetValue().mb_str(wxConvLocal)));
}


void SpectDisplay::WriteDialogLength()
{//===================================
	wxString string;
	
	string.Printf(_T("%d mS"),int(spectseq->GetKeyedLength()+0.5));
	formantdlg->t_orig_seq->SetLabel(string);
}



void SpectDisplay::WriteDialogValues()
//{===============================
{
	wxString string;
	
	SetSpinCtrl(formantdlg->t_amplitude,spectseq->amplitude);

	string.Printf(_T("%d - %d Hz    %d mS"),
		spectseq->pitch1,spectseq->pitch2,spectseq->duration);
	formantdlg->t_pitch->SetLabel(string);

	pitchgraph->SetData(128,spectseq->pitchenv.env);
	voicedlg->SetFromSpect(spectseq);
	
	WriteDialogLength();
}


void SpectDisplay::ReadDialogValues()
//{==================================
{
	spectseq->amplitude = formantdlg->t_amplitude->GetValue();
	spectseq->duration = GetNumeric(voicedlg->vd_duration);
	voicedlg->ReadParams();
}


void SpectDisplay::RefreshDialogValues(int type)
//{=============================================
{
	spectseq->amplitude = formantdlg->t_amplitude->GetValue();
	spectseq->frames[sframe]->amp_adjust = formantdlg->t_ampframe->GetValue();
	spectseq->frames[sframe]->length_adjust =
		formantdlg->t_timeframe->GetValue() - spectseq->GetFrameLength(sframe,0,NULL);

	if(type==0)
	{
		WriteDialogLength();
		Refresh();
	}
	else
		RefreshFrame(sframe);
}


void SpectDisplay::RefreshFrame(int frame)
{//=======================================
	int frame_x, frame_y;

	CalcScrolledPosition(0,int(frame*FRAME_HEIGHT*zoomy),
			&frame_x,&frame_y);

	RefreshRect(wxRect(0,frame_y,int(1000*zoomx),int(FRAME_HEIGHT*zoomy)));
}



int SpectDisplay::ScrollToFrame(int frame, int centre)
{//===================================================
	int scrollx, scrolly;
	int x, y;
	int scrolled = 0;

	GetClientSize(&x,&y);
	GetViewStart(&scrollx,&scrolly);
	scrolly *= SCROLLUNITS;

	if(centre)
	{
		Scroll(-1, int(((frame+0.5)*FRAME_HEIGHT*zoomy - y/2) /SCROLLUNITS));
		return(1);
	}

	if(scrolly > frame * FRAME_HEIGHT * zoomy)
	{
		Scroll(-1, int((frame * FRAME_HEIGHT * zoomy) / SCROLLUNITS));
		scrolled = 1;
	}
	else
	{
		if(((frame+1) * FRAME_HEIGHT * zoomy) > (scrolly + y))
		{
			Scroll(-1, int(((frame+1)*FRAME_HEIGHT*zoomy - y) /SCROLLUNITS)+1);
			scrolled = 1;
		}
	}
	return(scrolled);
}  // end of SpectDisplay::ScrollToFrame


void SpectDisplay::SelectFrame(int frame)
{//======================================
	sframe = frame;
	spectseq->SelectAll(0);
	spectseq->frames[frame]->selected = 1;
	formantdlg->ShowFrame(spectseq,frame,1,0xff);
	ScrollToFrame(frame,1);
	Refresh();
	SetFocus();
}  // end of SpectDisplay::SelectFrame


void SpectDisplay::OnZoom(int command)
{//===============================
   static float zoomtab[] = {0.25,0.3536,0.5,0.7071,1,1.14142};
	int f1, f2, frame;
	int x,y;
	int height,width;

	GetClientSize(&width,&height);
	CalcScrolledPosition(0,0,&x,&y);

	// centre on a visible selected frame, or if none keep the centred frame centred
	f1 = int(-y/(FRAME_HEIGHT*zoomy));
	f2 = int((-y+height)/(FRAME_HEIGHT*zoomy));
	for(frame=f1; frame<=f2 && frame<spectseq->numframes; frame++)
	{
		if(spectseq->frames[frame]->selected) break;
	}
	if(frame > f2)
		frame = int((-y+height/2)/(FRAME_HEIGHT*zoomy));  // keep centred frame

	switch(command)
	{
	case T_ZOOMIN:
		if(zoom < 5) zoom++;
		break;
	case T_ZOOMOUT:
		if(zoom > 0) zoom--;
		break;
	}

	zoomx = zoomy = zoomtab[zoom];
	SetExtent();
	ScrollToFrame(frame,1);
	Refresh();
}  // end of OnZoom



void SpectDisplay::Save(const wxString &path, int selection)
{//=========================================================
	wxString filename;
	wxFrame *parent;
	wxString msg;
	wxString fname = _T("");
	wxFileName f;

	if(selection)
		msg = _T("Save selected frames");
	else
	{
		msg = _T("Save spectrum sequence");
		fname = spectseq->name;
	}

	if(path.empty())
	{
		filename = wxFileSelector(msg,wxFileName(savepath).GetPath(),fname,_T(""),_T("*"),wxSAVE);
	}
	else
		filename = path;

	if(filename.IsEmpty())
		return;

	wxFileOutputStream stream(filename);

	if(stream.Ok() == FALSE)
	{
		wxLogError(_T("Failed to write '%s'"),filename.c_str());
		return;
	}
	spectseq->Save(stream,selection);
	savepath = filename;
	if((parent = (wxFrame *)GetParent()) != NULL)
	{
		f = wxFileName(filename);
		parent->SetTitle(f.GetName());
		spectseq->name = f.GetName();
	}
	Refresh();  // Shouldn't need this, but redraw problem

	if(path.empty())
	{
		wxFileName p(filename);
	}
}  //  end of Save


void SpectDisplay::SavePitchenv(PitchEnvelope &pitch)
{//==================================================
	// save the pitch envelope

	int  ix;
	FILE *f_env;
	wxString filename;
	char fname[200];

	filename = wxFileSelector(_T("(Save pitch envelope"),path_pitches,_T(""),_T(""),_T("*"),wxSAVE);
	if(filename.IsEmpty())
		return;

	strcpy(fname, filename.mb_str(wxConvLocal));
	f_env = fopen(fname,"w");
	if(f_env == NULL)
	{
		wxLogError(_T("Failed to write ")+filename);
		return;
	}

	for(ix=0; ix<128; ix++)
	{
		fprintf(f_env," 0x%.2x,",pitch.env[ix]);
		if((ix & 0xf) == 0xf)
			fprintf(f_env,"\n");
	}
	fclose(f_env);
}  // end of SpectDisplay::SavePitchenv


// WX_DECLARE_LIST(MyChild,ChildList);
// WX_DEFINE_LIST(ChildList);


void SpectDisplay::PlayChild(int number, PitchEnvelope pitchenv)
{//=========================================================
	SpectSeq *seq;


	if(number >= canvaslistix) return;

	if((seq = canvaslist[number]->spectseq) == NULL)
		return;

	ReadDialogValues();
	seq->MakeWave(0,seq->numframes-1,pitchenv);
}  // end of PlayChild


void SpectDisplay::SetKeyframe(SpectFrame *sf, int yes)
{//====================================================
	if(sf->keyframe == yes) return;  // already done

	sf->keyframe = yes;
	pitchgraph->Refresh();
	
	WriteDialogLength();
}



void SpectDisplay::OnMenu(wxCommandEvent& event)
{//=============================================
	int id;
	int code;
	wxKeyEvent keyevent;
	SpectFrame *sf;
	static int key[] = {0x1044,0x3044,0x104e,0x1047,0x105a,0x1051,0x3051,WXK_F1,WXK_F2,0x1049,WXK_F10};

	id = event.GetId();

	if(id >= 300)
	{
		sf = spectseq->frames[sframe];
		sf->ToggleMarker(id-300);
		Refresh();
		return;
	}

	// simulate a KeyEvent that corresponds to this menu entry
	code = key[id - 201];
	keyevent.m_keyCode = code & 0xfff;
	if(code & 0x1000)
		keyevent.m_controlDown = TRUE;
	else
		keyevent.m_controlDown = FALSE;
	if(code & 0x2000)
		keyevent.m_shiftDown = TRUE;
	else
		keyevent.m_shiftDown = FALSE;
	OnKey(keyevent);
}



void SpectDisplay::OnKey(wxKeyEvent& event)
{//====================================
	int key;
	int display=0;  // 1=change one peak 2=one frame, 3=all
	int direction=0;
	int x_inc;
	int y_inc;
	int  i;
	int field=0xff;
	int keyfr;
	int control;
	int number;
	double value;
	SpectFrame *sf;
	peak_t *pk;
//	static short pk_inc1[N_PEAKS] = {2,2,2,5,5,5,5,5,5};
	static short pk_inc1[N_PEAKS] = {4,4,4,8,8,8,8,8,8};
	static short pk_inc2[N_PEAKS] = {8,8,20,20,20,20,25,25,25};

	sf = spectseq->frames[sframe];
	pk = &sf->peaks[pk_num];
	key = event.GetKeyCode();
	control = event.ControlDown();
	if(control && isalnum(key)) key |= 0x1000;

	if(event.ShiftDown())
	{
		x_inc = pk_inc1[pk_num];
		y_inc = 0x40;
	}
	else
	{
		x_inc = pk_inc2[pk_num];
		y_inc = 0x100;
	}

	switch(key)
	{
	case WXK_NEXT:
		if(sframe < spectseq->numframes-1)
		{
			spectseq->SelectAll(0);
			sframe++;
			spectseq->frames[sframe]->selected = 1;
			display = 3;
		}
		break;

	case WXK_PRIOR:
		if(sframe > 0)
		{
			spectseq->SelectAll(0);
			sframe--;
			spectseq->frames[sframe]->selected = 1;
			display = 3;
		}
		break;


	case 'Z':
		if(pk_num > 0)
		{
			pk_num--;
			display = 2;
		}
		break;

	case 'X':
		if(pk_num < N_PEAKS-1)
		{
			pk_num++;
			display = 2;
		}
		break;

	case WXK_RIGHT:
		pk->pkfreq = pk->pkfreq - (pk->pkfreq % x_inc) + x_inc;
		if(pk->pkfreq >= MAX_DISPLAY_FREQ)
			pk->pkfreq = MAX_DISPLAY_FREQ;
		display = 1;
		direction = 1;
		field = 1;
		break;

	case WXK_LEFT:
		pk->pkfreq = pk->pkfreq - (pk->pkfreq % x_inc) - x_inc;
		if(pk->pkfreq < 50)
			pk->pkfreq = 50;
		display = 1;
		direction = -1;
		field = 1;
		break;

	case WXK_UP:
		pk->pkheight = pk->pkheight - (pk->pkheight % y_inc) + y_inc;
		if(pk->pkheight >= 0x7fff)
			pk->pkheight = 0x7fff;
		display = 1;
		field = 2;
		SetKeyframe(sf,1);
		break;

	case WXK_DOWN:
		pk->pkheight = pk->pkheight - (pk->pkheight % y_inc) - y_inc;
		if(pk->pkheight <= 0)
		{
			pk->pkheight = 0;
			keyfr = 0;  // but only if all the other peaks are zero
			for(i=0; i<N_PEAKS; i++)
			{
				if(sf->peaks[i].pkheight > 0)
					keyfr = 1;
			}
			SetKeyframe(sf,keyfr);
		}

		display = 1;
		field = 2;
		break;

	case ',':   // width--
		if(event.ControlDown())
		{
			// CTRL, rotate, make right slope steeper
			pk->pkright-= 5;
			pk->pkwidth += 5;
		}
		else
		{
			pk->pkright -= 10;
			pk->pkwidth -= 10;
		}
		field = 4;
		display = 1;
		break;

	case '.':   // width++
		if(event.ControlDown())
		{
			// CTRL: rotate, make left slope steeper
			pk->pkright += 5;
			pk->pkwidth -= 5;
		}
		else
		{
			pk->pkright += 10;
			pk->pkwidth += 10;
		}
		field = 4;
		display = 1;
		break;

	case '<':   // width--
		pk->pkright -= 2;
		pk->pkwidth -= 2;
		display = 1;
		field = 4;
		break;

	case '>':   // width++
		pk->pkright += 2;
		pk->pkwidth += 2;
		display = 1;
		field = 4;
		break;

	case '/':  // make left=right
		i = pk->pkwidth + pk->pkright;
		pk->pkwidth = pk->pkright = i/2;
		display = 1;
		field = 4;
		break;

	case 0x1041:   // CTRL-A
		spectseq->SelectAll(1);
		Refresh();
		break;

	case 0x1042:  // CTRL-B
		// toggle bass reduction
		spectseq->bass_reduction ^= 1;
		Refresh();
		break;
		
	case 0x1043:   // CTRL-C
		spectseq->ClipboardCopy();
		break;

	case 0x1044:   // CTRL-D
		// copy peaks down from previous/next keyframe
		if(event.ShiftDown())
			spectseq->CopyDown(sframe,1);
		else
			spectseq->CopyDown(sframe,-1);
		SetKeyframe(sf,1);
      display = 2;
		break;

	case 0x1047:   // CTRL-G  toggle grid
		spectseq->grid ^= 1;
		Refresh();
		break;

	case 0x1049:   // CTRL-I  interpolate between two frames
		spectseq->InterpolateAdjacent();
		display = 2;
		break;

	case 0x104d:   // CTRL-M
		sf->ToggleMarker(1);
      display = 2;
		break;

	case 0x104e:   // CTRL-N
		number = wxGetNumberFromUser(_T("Toggle Marker"),_T("Marker"),_T(""),1,0,7);
		sf->ToggleMarker(number);
      display = 2;
		break;

	case 0x104f:   // CTRL-O
		spectseq->ConstructVowel();
		break;

	case 0x1051:   // CTRL-Q
		if(event.ShiftDown())
			spectseq->InterpolatePeaks(0);  // remove interpolation
		else
			spectseq->InterpolatePeaks(1);  // show interpolation
		display = 3;
		break;

	case 0x1052:   // CTRL-R
		value = sf->GetRms(spectseq->amplitude);
		break;

	case 0x1053:   // CTRL-S
		Save(savepath);
		break;

	case 0x1056:   // CTRL-V
		if(event.ShiftDown())
		{
			// insert frames from the clipboard
			sframe = spectseq->ClipboardInsert(sframe);
			SetExtent();
			display = 3;
			pitchgraph->Refresh();
			break;
		}

		// overwrite just the peaks
		if(clipboard_spect == NULL)
			break;
		i = 0;
		while((i < clipboard_spect->numframes) && ((sframe+i) < spectseq->numframes))
		{
			spectseq->frames[sframe+i]->CopyPeaks(clipboard_spect->frames[i]);
			i++;
		}
		display = 3;
		break;

	case 0x1058:   // CTRL-X
		spectseq->ClipboardCopy();
		spectseq->DeleteSelected();
		SetExtent();
		sframe = 0;
		display = 3;
		pitchgraph->Refresh();
		break;

	case 0x105a:   // CTRK-Z
		sf->ZeroPeaks();
		SetKeyframe(sf,0);
		display = 2;
		break;

	case WXK_F10:
		sf->ApplyVoiceMods();
		display = 2;
		break;

	case WXK_F2:
		// make and play sequence
		ReadDialogValues();
		spectseq->MakeWave(0,spectseq->numframes-1,voicedlg->pitchenv);
		break;

	case WXK_F1:
		// make and play selected
		ReadDialogValues();
		sf->MakeWave(0,voicedlg->pitchenv,spectseq->amplitude,spectseq->duration);
		break;
	case WXK_F3:
		// make and play selected
		ReadDialogValues();
		sf->MakeWave(1,voicedlg->pitchenv,spectseq->amplitude,spectseq->duration);
		break;

	default:
		if(key>='0' && key<='9')
		{
			i = key-'0';
			if(event.ControlDown())
			{
				if(i==0) i=10;
				PlayChild(i-1,voicedlg->pitchenv);
			}
			else
			{
				// select peak number
				if((pk_num = i) >= N_PEAKS) pk_num = N_PEAKS-1;
			}
			display=2;
		}
		event.Skip();
		break;
	}

	if(display)
	{
		pk_select = pk_num;
		sf = spectseq->frames[sframe];
		pk = &sf->peaks[pk_num];

		if(pk->pkwidth < 50) pk->pkwidth = 50; // min. width
		if(pk->pkright < 50) pk->pkright = 50;

		// ensure minimum separation between peaks & prevent crossover
		if(direction > 0)
		{
			for(i=pk_num+1; i<N_PEAKS; i++)
			{
				if(sf->peaks[i].pkfreq < sf->peaks[i-1].pkfreq + 100)
					sf->peaks[i].pkfreq = sf->peaks[i-1].pkfreq + 100;
			}
		}
		else
		if(direction < 0)
		{
			for(i=pk_num-1; i>=0; i--)
			{
				if(sf->peaks[i].pkfreq > sf->peaks[i+1].pkfreq - 100)
					sf->peaks[i].pkfreq = sf->peaks[i+1].pkfreq - 100;
			}
		}

		if(display==3)
      {
		   formantdlg->ShowFrame(spectseq,sframe,pk_num,0xff);
			Refresh();
		}
		else
		{
			// only one frame needs to be redrawn
			formantdlg->ShowFrame(spectseq,sframe,pk_num,field);
			RefreshFrame(sframe);
		}

		if(sframe >= 0)
		{
			if(ScrollToFrame(sframe,0))
				Refresh();   // shouldn't need this, but we have redraw problems
		}

	}
}  // end of SpectDisplay::OnKey


void SpectDisplay::OnMouse(wxMouseEvent& event)
{//========================================
	int  frame;
	int  ix;

	if(event.RightDown())
	{
		PopupMenu(menu_spectdisplay);
		return;
	}

	wxClientDC dc(this);
	PrepareDC(dc);

	wxPoint pt(event.GetLogicalPosition(dc));

	if(spectseq->numframes==0) return;
	
	frame = (int)(pt.y/(FRAME_HEIGHT*zoomy));

	if(!event.ControlDown())
		spectseq->SelectAll(0);

	if(event.ShiftDown())
	{
		if(sframe >= 0)
		{
			if(frame < sframe)
				for(ix=frame; ix<=sframe; ix++)
					spectseq->frames[ix]->selected =1;
			else
				for(ix=sframe; ix<=frame && ix<spectseq->numframes; ix++)
					spectseq->frames[ix]->selected =1;
			Refresh();
		}
	}
	else
	{
		if(frame < spectseq->numframes)
			spectseq->frames[frame]->selected ^= 1;
		Refresh();
	}

	if(frame < spectseq->numframes)
	{
		if(sframe != frame)
			formantdlg->ShowFrame(spectseq,frame,pk_num,0xff);

		sframe = frame;
	}
}  // end of SpectDisplay::OnMouse


void MyFrame::OnNewWindow(wxCommandEvent& event)
{//=============================================
	SpectSeq *spectseq;
	wxString leaf;
	wxString pathload;

	if(event.GetId() == MENU_SPECTRUM)
		pathload = path_spectload;
	else
		pathload = path_spectload2;

	wxString filename = wxFileSelector(_T("Read spectrum or praat data"),pathload,
		_T(""),_T(""),_T("*"),wxOPEN);
	if(filename.IsEmpty())
	{
		return;
	}

	// create SpectSeq and import data
	spectseq = new SpectSeq;
	if(spectseq == NULL)
	{
		wxLogError(_T("Failed to create SpectSeq"));
		return;
	}

	wxFileInputStream stream(filename);
	if(stream.Ok() == FALSE)
	{
		wxLogError(_T("Failed to open '%s'"),filename.c_str());
		return;
	}
	wxFileName path = wxFileName(filename);
   leaf = path.GetName();

	setlocale(LC_NUMERIC,"C");    // read numbers in the form 1.23456
	spectseq->Load(stream);
	spectseq->name = leaf;
	spectseq->MakePitchenv(spectseq->pitchenv,0,spectseq->numframes-1);

	if(event.GetId() == MENU_SPECTRUM)
		path_spectload = path.GetPath();
	else
		path_spectload2 = path.GetPath();

	// Make another frame, containing a canvas
	MyChild *subframe = new MyChild(myframe, _T("Spectrum"),
                                      wxPoint(10, 10), wxSize(300, 300),
                                      wxDEFAULT_FRAME_STYLE | wxMAXIMIZE |
                                      wxNO_FULL_REPAINT_ON_RESIZE);

	subframe->SetTitle(leaf);

	// Give it a status line
	subframe->CreateStatusBar();

	int width, height;
	subframe->GetClientSize(&width, &height);
	SpectDisplay *canvas = new SpectDisplay(subframe, wxPoint(0, 0), wxSize(width, height), spectseq);
	canvas->savepath = filename;
   currentcanvas = canvas;

	// Associate the menu bar with the frame
	subframe->SetMenuBar(MakeMenu(1));
	subframe->canvas = canvas;
	subframe->Show(TRUE);

}

void InitSpectrumDisplay()
{//=======================
	wxMenu *menu_markers = new wxMenu;
	menu_spectdisplay = new wxMenu;

	menu_markers->Append(301,_T("1  Vowel break point"));
	menu_markers->Append(302,_T("2  Reduce length changes"));
	menu_markers->Append(303,_T("3  Break F0 to F2"));
	menu_markers->Append(304,_T("4  Break. All formants"));
	menu_markers->Append(305,_T("5  Allow greater rate of formant change"));
	menu_markers->Append(306,_T("6  Trill"));
	menu_markers->Append(307,_T("7  Defer wav mixing"));

	menu_spectdisplay->Append(201,_T("Copy Peaks Down	CTRL+D"));
	menu_spectdisplay->Append(202,_T("Copy Peaks Up	SHIFT+CTRL+D"));
	menu_spectdisplay->Append(204,_T("Grid (toggle)	CTRL+G"));
	menu_spectdisplay->Append(210,_T("Interpolate (percentage)	CTRL+I"));
	menu_spectdisplay->Append(203,_T("Marker (toggle)	CTRL+N"));
	menu_spectdisplay->Append(206,_T("Show Interpolation	CTRL+Q"));
	menu_spectdisplay->Append(207,_T("Hide Interpolation	CTRL+SHIFT+Q"));
	menu_spectdisplay->Append(205,_T("Zero Peaks	CTRL+Z"));
	menu_spectdisplay->Append(210,_T("Toggle Marker"),menu_markers);
	menu_spectdisplay->Append(211,_T("Apply the formant adjustments from a voice file"));
	menu_spectdisplay->AppendSeparator();
	menu_spectdisplay->Append(208,_T("Play Selected Frame	F1"));
	menu_spectdisplay->Append(209,_T("Play All	F2"));

}
