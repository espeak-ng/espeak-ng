/***************************************************************************
 *   Copyright (C) 2005 to 2015 by Jonathan Duddington                     *
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

#include <locale.h>

#include "speak_lib.h"
#include "espeak_ng.h"

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "translate.h"

static const char *about_string2 = "espeakedit: %s\nAuthor: Jonathan Duddington (c) 2009\n\n"
"Licensed under GNU General Public License version 3\n"
"http://espeak.sourceforge.net/";


static const char *about_string = "<font size=0><b>espeakedit </b> %s<br>Author: Jonathan Duddington (c) 2009<br>"
"<a href=\"http://espeak.sourceforge.net/\">http://espeak.sourceforge.net</a><br>"
"Licensed under <a href=\"http://espeak.sourceforge.net/license.html\">GNU General Public License version 3</a></font>";

int progress_max;
int gui_flag = 0;

char path_source[sizeof(path_home)+20];
char voice_name2[40];

class MyApp: public wxApp
{
	public:
	bool OnInit(void);
	int OnExit(void);
};

IMPLEMENT_APP(MyApp)

wxString AppName = _T("espeakedit");



int MyApp::OnExit()
{//================
	return(0);
}


static const char *help_text =
"\n\nespeakedit\n"
"\tRun with GUI\n"
"espeakedit --compile\n"
"\tCompile phoneme data in espeak-data/phsource\n"
"\tand dictionary data in espeak-data/dictsource\n";

// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{//=====================

int j;
char param[120];

if(argc > 1)
{
#if wxUSE_UNICODE == 1
	wcstombs(param, argv[1], sizeof(param));
#else
	const char *p = argv[1];
	while((param[j] = p[j]) != 0) j++;
#endif
}
else
{
	param[0] = '-';
	param[1] = 'h';
	param[2] = 0;
}

	if((strcmp(param,"--help")==0) || (strcmp(param,"-h")==0))
	{
		printf(about_string2,espeak_Info(NULL));
		printf("%s", help_text);
		exit(0);
	}

        char *env;
	if((env = getenv("ESPEAK_DATA_PATH")) == NULL)
		env = getenv("HOME");
	snprintf(path_home,sizeof(path_home),"%s/espeak-data",env);
	snprintf(path_source,sizeof(path_source),"%s/../phsource/",path_home);

	if(strcmp(param,"--compile")==0)
	{
        samplerate_native = samplerate = 22050;
	    LoadPhData(NULL);
		if(LoadVoice("", 0) == NULL)
		{
			fprintf(stderr, "Failed to load default voice\n");
			exit(1);
		}
		espeak_ng_CompilePhonemeData(22050, stderr);
		espeak_ng_CompileIntonation(stderr);
	}
    exit(0);
}
