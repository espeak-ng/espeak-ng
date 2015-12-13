/***************************************************************************
 *   Copyright (C) 2005 to 2014 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *   Copyright (C) 2015 by Reece H. Dunn                                   *
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

#include "speech.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int value;
    char *name;
} NAMETAB;

NAMETAB *manifest = NULL;
int n_manifest;

const char *LookupManifest(unsigned int addr)
{//=============================================
    int ix;

    if(manifest != NULL)
    {
        for(ix=0; ix < n_manifest; ix++)
        {
            if(manifest[ix].value == addr)
                return(manifest[ix].name);
            if(manifest[ix].value > addr)
                break;
        }
    }
    return("");
}


void ReadPhondataManifest()
{//=========================
// Read the phondata-manifest file
    FILE *f;
    int n_lines=0;
    int ix;
    char *p;
    unsigned int value;
    char buf[sizeof(path_home)+40];
    char name[120];

	sprintf(buf,"%s%c%s",path_home,PATHSEP,"phondata-manifest");
    if((f = fopen(buf, "r")) == NULL)
        return;

    while(fgets(buf, sizeof(buf), f) != NULL)
        n_lines++;

    rewind(f);

    if(manifest != NULL)
    {
        for(ix=0; ix < n_manifest; ix++)
            free(manifest[ix].name);
    }

    if((manifest = (NAMETAB *)realloc(manifest, n_lines * sizeof(NAMETAB))) == NULL)
    {
        fclose(f);
        return;
    }

    n_manifest = 0;
    while(fgets(buf, sizeof(buf), f) != NULL)
    {
        if(!isalpha(buf[0]))
           continue;

        if(sscanf(&buf[2], "%x %s", &value, name) == 2)
        {
            if((p = (char *)malloc(strlen(name)+1)) != NULL)
            {
                strcpy(p, name);
                manifest[n_manifest].value = value;
                manifest[n_manifest].name = p;
                n_manifest++;
            }
        }
    }
    fclose(f);
}
