/***************************************************************************
 *   Copyright (C) 2005, 2006 by Jonathan Duddington                       *
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

#include "wx/wx.h"


#include "main.h"
#include "speech.h"
#include "voice.h"
#include "spect.h"
#include "options.h"

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/filename.h"

VoiceDlg *voicedlg = NULL;

voice_t voice_data;
USHORT voice_pcnt[N_PEAKS+1][3];

static void VoiceSetup()
{//=====================
	int pk;
	voice_t *v = &voice_data;

	for(pk=0; pk<N_PEAKS; pk++)
	{
		v->freq[pk] = int(voice_pcnt[pk][0] * 2.56001 * voice_pcnt[N_PEAKS][0] / 100.0);
		v->height[pk] = int(voice_pcnt[pk][1] * 2.56001 * voice_pcnt[N_PEAKS][1] / 100.0);
		v->width[pk] = int(voice_pcnt[pk][2] * 2.56001 * voice_pcnt[N_PEAKS][2] / 100.0);
	}
}


static void VoiceInit()
{//====================
	int  pk, j;
	voice_t *v;

	v = &voice_data;

	v->pitch_base = 71 << 12;
	v->pitch_range =  0x1000;     // default = 0x1000
	
	for(pk=0; pk<N_PEAKS+1; pk++)
	{
	for(j=0; j<3; j++)
		voice_pcnt[pk][j] = 100;
	}
	VoiceSetup();

	voice = v;
}  // end of voice_init


#define VD_DEFAULTS 401
#define VD_LOAD  402
#define VD_SAVE  403
#define VD_PITCHCHOICE 404
#define VD_PITCH1  405
#define VD_PITCH2  406
#define VD_USEPITCH 407

wxString pitchchoices[3] = {_T("From spectrum"),_T("Standard"),_T("Monotone")};

BEGIN_EVENT_TABLE(VoiceDlg, wxPanel)
	EVT_BUTTON(VD_DEFAULTS,VoiceDlg::OnCommand)
	EVT_BUTTON(VD_LOAD,VoiceDlg::OnCommand)
	EVT_BUTTON(VD_SAVE,VoiceDlg::OnCommand)
	EVT_SPINCTRL(VD_PITCH1,VoiceDlg::OnSpin)
	EVT_SPINCTRL(VD_PITCH2,VoiceDlg::OnSpin)
	EVT_CHECKBOX(VD_USEPITCH,VoiceDlg::OnCommand)
	EVT_COMBOBOX(VD_PITCHCHOICE,VoiceDlg::OnCommand)
END_EVENT_TABLE()

VoiceDlg::VoiceDlg(wxWindow *parent) : wxPanel(parent)
{//=========================================================
	int  ix, j;
	int  y;
	wxString string;
	voice_t *v;
	wxString captions[3] = {_T("Freq"),_T("Height"),_T("Width")};

	voice_p = v = &voice_data;
	VoiceInit();

	pitch1 = 80;
	pitch2 = 120;
	
	for(j=0; j<3; j++)
		new wxStaticText(this,-1,captions[j],wxPoint(34+j*56,6));

	y = 26;
	for(ix=0; ix<N_PEAKS+1; ix++)
	{
		if(ix==N_PEAKS)
		{
			string = _T("All");
			y += 8;
		}
		else
			string.Printf(_T("%2d"),ix);
		vd_labpk[ix] = new wxStaticText(this,-1,string,wxPoint(8,y+2));

		for(j=0; j<3; j++)
		{
			string.Printf(_T("%d"),voice_pcnt[ix][j]);
			vd[ix][j] = new wxSpinCtrl(this,-1,string,
				wxPoint(32+j*56,y),wxSize(52,22),wxSP_ARROW_KEYS,0,300);
		}
		y += 26;
	}
	
	for(ix=6; ix<N_PEAKS; ix++)
		vd[ix][2]->Hide();

	vd_defaults = new wxButton(this,VD_DEFAULTS,_T("Defaults"),wxPoint(32,293));

	y = 324;
	vd_pitch1 = new wxSpinCtrl(this,VD_PITCH1,_T("80"),
		wxPoint(32,y),wxSize(52,24),wxTE_CENTRE,60,500);
	vd_pitch2 = new wxSpinCtrl(this,VD_PITCH2,_T("120"),
		wxPoint(88,y),wxSize(52,24),wxTE_CENTRE,60,500);
	vd_lab[1] = new wxStaticText(this,-1,_T("Hz"),wxPoint(142,y+2));
	
	vd_usepitch = new wxCheckBox(this,VD_USEPITCH,_T("Pitch from spectrum"),
		wxPoint(4,y+24));
	
//	vd_load = new wxButton(this,VD_LOAD,_T("Load"),wxPoint(16,y+56));
//	vd_save = new wxButton(this,VD_SAVE,_T("Save"),wxPoint(106,y+56));

	y = 422;
	vd_duration = new wxTextCtrl(this,-1,_T(""),
		wxPoint(4,y),wxSize(52,24),wxTE_CENTRE);
	vd_lab[2] = new wxStaticText(this,-1,_T("Duration"),wxPoint(60,y+2));

	y = 422;
	vd_pitchchoice = new wxComboBox(this,VD_PITCHCHOICE,_T("Standard"),
		wxPoint(4,y+30),wxSize(192,24),3,pitchchoices);
	vd_pitchgraph = new ByteGraph(this,wxPoint(0,y+60),wxSize(200,100));
	vd_pitchgraph->SetData(128,env_fall);
	vd_pitchgraph->Show();

}  //  end of VoiceDlg::VoiceDlg


void VoiceDlg::OnCommand(wxCommandEvent& event)
{//============================================

// JSD  EVT_COMBOBOX causes GDB lockup if a breakpoint is set

	int  id;
	int  ix, j;

	switch(id = event.GetId())
	{
	case VD_DEFAULTS:
		for(ix=0; ix<N_PEAKS+1; ix++)
		{
			for(j=0; j<3; j++)
			{
				voice_pcnt[ix][j] = 100;
				vd[ix][j]->SetValue(_T("100"));
			}
		}
		VoiceSetup();
		break;

	case VD_LOAD:
		Load();
		break;

	case VD_SAVE:
		Save();
		break;

	case VD_USEPITCH:
	case VD_PITCHCHOICE:
		if(currentcanvas != NULL)
			SetFromSpect(currentcanvas->spectseq);
		break;
	}

	if(currentcanvas != NULL) currentcanvas->SetFocus();
}  //  end of VoiceDlg::OnCommand



void VoiceDlg::ReadParams()
{//========================
	int ix, j;

	pitchenv.pitch1 = vd_pitch1->GetValue();
	pitchenv.pitch2 = vd_pitch2->GetValue();

	voice_data.pitch_base = (pitchenv.pitch1 - 9) << 12;
	voice_data.pitch_range = (pitchenv.pitch2 - pitchenv.pitch1) * 108;
	
	for(ix=0; ix<N_PEAKS+1; ix++)
	{
		for(j=0; j<3; j++)
		{
			voice_pcnt[ix][j] = vd[ix][j]->GetValue();
		}
	}
	VoiceSetup();

}  // end of VoiceDlg::ReadParams


void VoiceDlg::WriteParams()
{//========================
	int ix;
	voice_t *v = voice;
	int val1, val2;

	val1 = (v->pitch_base >> 12) + 9;
	val2 = v->pitch_range/108 + val1;
	vd_pitch1->SetValue(val1);
	vd_pitch2->SetValue(val2);

	for(ix=0; ix<N_PEAKS; ix++)
	{
		vd[ix][0]->SetValue(int(float(v->freq[ix]) / 2.56001 + 0.5));
		vd[ix][1]->SetValue(int(float(v->height[ix]) / 2.56001 + 0.5));
		vd[ix][2]->SetValue(int(float(v->width[ix]) / 2.56001 + 0.5));
	}
	vd[ix][0]->SetValue(100);
	vd[ix][1]->SetValue(100);
	vd[ix][2]->SetValue(100);

}  // end of VoiceDlg::WriteParams



void VoiceDlg::OnSpin(wxSpinEvent& event)
{//======================================

	ReadParams();
}

void VoiceDlg::Save()
{//==================
	int pk;
	wxString filename;
	FILE *f;
	
	filename = wxFileSelector(_T("Save voice"),path_voices,_T(""),_T(""),_T("*"),wxSAVE);
	if(filename.IsEmpty())
		return;

	f = fopen(filename.mb_str(wxConvLocal),"w");
	if(f == NULL)
	{
		wxLogError(_T("Failed to open '%s'"),filename.c_str());
		return;
	}

	wxFileName fn(filename);
	path_voices = fn.GetPath();
	
	ReadParams();
	fprintf(f,"%3d %3d\n",vd_pitch1->GetValue(),vd_pitch2->GetValue());
	for(pk=0; pk<N_PEAKS+1; pk++)
	{
		fprintf(f,"%3d %3d %3d\n",voice_pcnt[pk][0],voice_pcnt[pk][1],voice_pcnt[pk][2]);
	}
	fclose(f);
}  // end of VoiceDlg::Save


void VoiceDlg::Load()
{//===============================
	wxString filename;
	int  pk, j;
	
	filename = wxFileSelector(_T("Load voice"),path_voices,_T(""),_T(""),_T("*"),wxOPEN);
	if(filename.IsEmpty()) return;

	wxFileInputStream stream(filename);

	if(stream.Ok() == FALSE)
	{
		wxLogError(_T("Failed to open '%s'"),filename.c_str());
		return;
	}

	wxFileName fn(filename);
	path_voices = fn.GetPath();
	
	wxTextInputStream s(stream);

	s >> pitch1;
	s >> pitch2;
	for(pk=0; pk<N_PEAKS+1; pk++)
	{
		for(j=0; j<3; j++)
		{
			s >> voice_pcnt[pk][j];
			SetSpinCtrl(vd[pk][j],voice_pcnt[pk][j]);
		}
	}

	SetSpinCtrl(voicedlg->vd_pitch1,pitch1);
	SetSpinCtrl(voicedlg->vd_pitch2,pitch2);
	ReadParams();
}  // end of VoiceDlg::Load


void VoiceDlg::SetFromSpect(SpectSeq* spect)
{//=========================================
	if(spect==NULL) return;
	
	if(vd_pitchchoice->GetValue() == _T("From spectrum"))
	{
		pitchenv = spect->pitchenv;
	}
	else
	if(vd_pitchchoice->GetValue() == _T("Monotone"))
	{
		memset(pitchenv.env,127,128);
	}
	else
	if(vd_pitchchoice->GetValue() == _T("Standard"))
	{
		memcpy(pitchenv.env,env_fall,128);
	}
	vd_pitchgraph->SetData(128,pitchenv.env);
	vd_pitchgraph->Show();

	if(vd_usepitch->GetValue())
	{
		SetSpinCtrl(vd_pitch1,spect->pitch1);
		SetSpinCtrl(vd_pitch2,spect->pitch2);
	}
	else
	{
		SetSpinCtrl(vd_pitch1,pitch1);
		SetSpinCtrl(vd_pitch2,pitch2);
	}
}  // end of VoiceDlg::SetFromSpect

