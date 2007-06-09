/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
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
#include "wx/fileconf.h"
#include "sys/stat.h"
#include "speech.h"

#ifdef PLATFORM_WINDOWS
#include "wx/msw/registry.h"
#endif

#include "main.h"

#include "options.h"

extern void WavegenInit(int samplerate, int wavemult_fact);
extern void strncpy0(char *to,const char *from, int size);
extern int GetNumeric(wxTextCtrl *t);
extern void SetNumeric(wxTextCtrl *t, int value);
extern int samplerate;
extern char path_home[120];            // .speak directory in user's HOME
extern char voice_name[40];

wxString basedir;
wxString path_spectload;
wxString path_spectload2;
wxString path_pitches;
wxString path_wave;
wxString path_speech;
wxString path_voices;
wxString path_phsource;
wxString path_dictsource;
wxString path_speaktext;
wxString path_dir1;
int option_speed=160;

char path_dsource[120];

BEGIN_EVENT_TABLE(Options, wxDialog)
		EVT_BUTTON(wxID_SAVE,Options::OnCommand)
		EVT_BUTTON(wxID_CLOSE,Options::OnCommand)
END_EVENT_TABLE()


Options::Options(wxWindow *parent) : wxDialog(parent,-1,_T("Options"),wxDefaultPosition,wxDefaultSize)
{//===================================================================================================

	m_lab[0] = new wxStaticText(this,-1,_T("Sample rate"),wxPoint(72,84));
	m_samplerate = new wxTextCtrl(this,-1,_T(""),wxPoint(8,80),wxSize(60,24));
	SetNumeric(m_samplerate,samplerate);

	m_save = new wxButton(this,wxID_SAVE,_T("Save"),wxPoint(8,120));
	m_close = new wxButton(this,wxID_CLOSE,_T("Cancel"),wxPoint(98,120));
	Show();
}


Options::~Options()
{//================
	int ix;

	for(ix=0; ix < 1; ix++)
		delete m_lab[ix];
		
	delete m_samplerate;
	delete m_save;
	delete m_close;
}

void Options::OnCommand(wxCommandEvent& event)
{//===========================================
	int  id;
	int  value;
	
	switch(id = event.GetId())
	{
	case wxID_SAVE:
		value = GetNumeric(m_samplerate);
		if(value > 0) WavegenInit(value,0);
		
		Destroy();
		break;

	case wxID_CLOSE:
		Destroy();
		break;
	}
}



void ConfigSetPaths()
{//==================
	// set paths from wxStrings
	strncpy0(path_source,path_phsource.mb_str(wxConvLocal),sizeof(path_source)-1);
	strcat(path_source,"/");

	strncpy0(path_dsource,path_dictsource.mb_str(wxConvLocal),sizeof(path_source)-1);
	strcat(path_dsource,"/");
}


void ConfigInit()
{//==============
	long value;
	wxString string;
	const char *path_base;
	
#ifdef PLATFORM_WINDOWS
	char buf[200];
	wxRegKey *pRegKey = new wxRegKey(_T("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Speech\\Voices\\Tokens\\eSpeak"));

	if(pRegKey->Exists() )
	{
		wxString RegVal;
		pRegKey->QueryValue(_T("Path"),RegVal); 
		strcpy(buf,RegVal.mb_str(wxConvLocal));
		path_base = buf;
	}
	else
	{
		path_base = "C:\\Program Files\\eSpeak";
	}
	sprintf(path_home,"%s\\espeak-data",path_base);
#else
	snprintf(path_home,sizeof(path_home),"%s/espeak-data",getenv("HOME"));
	path_base = path_home;
#endif
	mkdir(path_home,S_IRWXU);    // create if it doesn't already exist

	wxFileConfig *pConfig = new wxFileConfig(_T("espeakedit"));
	wxFileConfig::Set(pConfig);

	pConfig->Read(_T("/samplerate"),&value,22050);
#ifdef PLATFORM_WINDOWS
	value = 22050;
#endif
	WavegenInit(value,0);

	pConfig->Read(_T("/basedir"),&basedir,wxString(path_base,wxConvLocal));
	pConfig->Read(_T("/spectload"),&path_spectload,basedir+_T("/phsource"));
	pConfig->Read(_T("/spectload2"),&path_spectload2,basedir+_T("/phsource"));
	pConfig->Read(_T("/pitchpath"),&path_pitches,basedir+_T("/pitch"));
	pConfig->Read(_T("/wavepath"),&path_wave,wxEmptyString);
	pConfig->Read(_T("/speechpath"),&path_speech,wxEmptyString);
	pConfig->Read(_T("/voicepath"),&path_voices,basedir+_T("/voices"));
	pConfig->Read(_T("/voicename"),&string,wxEmptyString);
	strcpy(voice_name,string.mb_str(wxConvLocal));
	pConfig->Read(_T("/phsource"),&path_phsource,basedir+_T("/phsource"));
	pConfig->Read(_T("/dictsource"),&path_dictsource,basedir+_T("/dictsource"));
	pConfig->Read(_T("/speaktext"),&path_speaktext,wxEmptyString);
	pConfig->Read(_T("/dir1"),&path_dir1,basedir);
	option_speed = pConfig->Read(_T("/speed"),160);
	ConfigSetPaths();
}  // end of ConfigInit



void ConfigSave(int exit)
{//======================
	wxFileConfig *pConfig = (wxFileConfig *)(wxConfigBase::Get());

#ifndef PLATFORM_WINDOWS
	pConfig->Write(_T("/samplerate"),samplerate);
#endif
	pConfig->Write(_T("/basedir"),basedir);
	pConfig->Write(_T("/spectload"),path_spectload);
	pConfig->Write(_T("/spectload2"),path_spectload2);
	pConfig->Write(_T("/pitchpath"),path_pitches);
	pConfig->Write(_T("/wavepath"),path_wave);
	pConfig->Write(_T("/speechpath"),path_speech);
	pConfig->Write(_T("/voicepath"),path_voices);
	pConfig->Write(_T("/voicename"),wxString(voice_name,wxConvLocal));
	pConfig->Write(_T("/phsource"),path_phsource);
	pConfig->Write(_T("/dictsource"),path_dictsource);
	pConfig->Write(_T("/speaktext"),path_speaktext);
	pConfig->Write(_T("/speed"),option_speed);
	pConfig->Write(_T("/dir1"),path_dir1);

	if(exit)
		delete wxFileConfig::Set((wxFileConfig *)NULL);
}
