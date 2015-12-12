/***************************************************************************
 *   Copyright (C) 2005 to 2013 by Jonathan Duddington                     *
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
 *   along with this program; if not, see:                                 *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/


#include "wx/wx.h"
#include "wx/fileconf.h"
#include "wx/filename.h"
#include <sys/stat.h>
#include "speech.h"

#ifdef PLATFORM_WINDOWS
#include "wx/msw/registry.h"
#endif

wxString path_espeakdata;
wxString path_spectload;
wxString path_spectload2;
wxString path_pitches;
wxString path_phfile;
wxString path_phsource;
wxString path_dictsource;
wxString path_modifiervoice;
wxString path_dir1;

char path_source[sizeof(path_home)+20];
char path_dsource[sizeof(path_home)+20];
char voice_name2[40];

void ConfigSetPaths()
{//==================
	// set c_string paths from wxStrings
	strncpy0(path_source,path_phsource.mb_str(wxConvLocal),sizeof(path_source)-1);
	strcat(path_source,"/");

	strncpy0(path_dsource,path_dictsource.mb_str(wxConvLocal),sizeof(path_source)-1);
	strcat(path_dsource,"/");
}


void ConfigInit(bool use_defaults)
{//==============
	wxString string;
	wxString basedir;
	const char *path_base;

#ifdef PLATFORM_WINDOWS
	int found = 0;
	char buf[200];
	wxRegKey *pRegKey = new wxRegKey(_T("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Speech\\Voices\\Tokens\\eSpeak"));

	if((path_base = getenv("ESPEAK_DATA_PATH")) != NULL)
	{
		sprintf(path_home,"%s\\espeak-data",path_base);
		if(GetFileLength(path_home) == -2)
			found = 1;   // an espeak-data directory exists
	}

	if(found == 0)
	{
		if(pRegKey->Exists() )
		{
			wxString RegVal;
			pRegKey->QueryValue(_T("Path"),RegVal);
			strncpy0(buf,RegVal.mb_str(wxConvLocal),sizeof(buf));
			path_base = buf;
		}
		else
		{
			path_base = "C:\\Program Files\\eSpeak";
		}
		sprintf(path_home,"%s\\espeak-data",path_base);
	}
#else
        char *env;
	if((env = getenv("ESPEAK_DATA_PATH")) == NULL)
		env = getenv("HOME");
	snprintf(path_home,sizeof(path_home),"%s/espeak-data",env);
	path_base = path_home;
#endif
	mkdir(path_home,S_IRWXU);    // create if it doesn't already exist

	wxFileConfig *pConfig = new wxFileConfig(_T("espeakedit"));
	wxFileConfig::Set(pConfig);

	basedir = wxString(path_base,wxConvLocal);  // this is only used to set defaults for other paths if they are not in the config file
	if (use_defaults)
	{
		path_spectload = basedir + _T("/../phsource");
		path_spectload2 = basedir + _T("/../phsource");
		path_pitches = basedir + _T("/pitch");
		path_phsource = basedir + _T("/../phsource");
		path_phfile = path_phsource + _T("/phonemes");
		path_dictsource = basedir + _T("/../dictsource");
		path_modifiervoice = basedir;
		path_dir1 = basedir;
	}
	else
	{
	pConfig->Read(_T("/espeakdata"),&path_espeakdata,wxEmptyString);
	if(path_espeakdata != wxEmptyString)
	{
		strcpy(path_home, path_espeakdata.mb_str(wxConvLocal));
	}

	pConfig->Read(_T("/spectload"),&path_spectload,basedir+_T("/phsource"));
	pConfig->Read(_T("/spectload2"),&path_spectload2,basedir+_T("/phsource"));
	pConfig->Read(_T("/pitchpath"),&path_pitches,basedir+_T("/pitch"));
	pConfig->Read(_T("/voicename"),&string,wxEmptyString);
	pConfig->Read(_T("/phsource"),&path_phsource,basedir+_T("/phsource"));
	pConfig->Read(_T("/phfile"),&path_phfile,path_phsource+_T("/phonemes"));
	pConfig->Read(_T("/dictsource"),&path_dictsource,basedir+_T("/dictsource"));
	pConfig->Read(_T("/modifiervoice"),&path_modifiervoice,basedir);
	pConfig->Read(_T("/dir1"),&path_dir1,basedir);
	}
	ConfigSetPaths();
}  // end of ConfigInit
