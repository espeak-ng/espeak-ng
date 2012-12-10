/* Sound_JSD.c
 * A modification to praat (www.praat.org)
 * to analyse a speech sample to produce a data file "spectrum.dat" which can
 * be loaded into my  espeakedit  program to show a sequence of time-slice spectra.
 *
 * Copyright (C) 2005  Jonathan Duddington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "Sound_to_PointProcess.h"  // JSD
#include "Sound_to_Pitch.h"
#include "Pitch_to_PointProcess.h"
#include "Formant.h"
#include "Sound_to_Formant.h"

#include "Sound_and_Spectrogram.h"
#include "NUM2.h"

Spectrogram Sound_to_Espeak (Sound me, double fmin, double fmax, double maxfreq, double minFreqStep1, double maxformant, double phase)
// Added by JSD - testing
{
	long  pt;
	long  i, j;
	long  n_samples, leftSample, rightSample;
	int  rept;
	int  count;
	int  best;
	int  start, end;
	FILE *fout;
	int  ny=1;
	double cyclen;
	double f0;
	double value;
	double bandw;
	double time;
	double timenext;
	double total;
	double max;
	double average;
	Spectrogram thee = NULL;
	Spectrogram spect = NULL;
	Sound sound = NULL;
	PointProcess points = NULL;
	Formant formants = NULL;

	Pitch pitch = Sound_to_Pitch (me, 0.8 / fmin, fmin, fmax);
	if (! pitch) goto error;

	if (! (points = Sound_Pitch_to_PointProcess_cc (me, pitch))) goto error;

   formants = Sound_to_Formant_burg (me, 0.00625, 5, maxformant, 0.025, 50);


	// make temporary sound object, 0.1 sec, for doing analysis on
	sound = Sound_create(1, 0.0, 0.1, 0.1/my dx, my dx, my dx/2);

	fout = fopen("spectrum.dat","w");
	if(fout != NULL)
	{
		fprintf(fout,"%d\n",points->nt-1); // num of frames
	}

	// analyse each interval in the pitch point process
	for(pt=1; pt < points->nt; pt++)
	{
		time = points->t[pt];
		if(pt < points->nt-1)
			timenext = points->t[pt+1];
		else
			timenext = time + cyclen;  // end of pitch points, reuse last length

		leftSample = Sampled_xToLowIndex (me, time);
		rightSample = Sampled_xToLowIndex (me, timenext);
		n_samples = rightSample - leftSample;
		cyclen = timenext - time;

		// make sequential copies of the cycle
		j=1;
		for(rept=0; rept<5; rept++)
		{
		   for(i=1; i<=n_samples && j<=sound->nx; i++)
		   {
		      sound->z[1][j++] = me->z[1][i+leftSample];
		   }
		}

		if(spect != NULL) forget (spect);

		spect = Sound_to_Spectrogram(sound,cyclen,maxfreq,cyclen/16,
		minFreqStep1,kSound_to_Spectrogram_windowShape_GAUSSIAN,
		8.0,8.0);

		if(thee == NULL)
		{
			ny = spect->ny;
			thee = Spectrogram_create(1,points->nt-1,points->nt-1,1,0.5,
				0.0,maxfreq,ny+1,spect->dy,
				0.5*spect->dy);
		}

		f0 = Pitch_getValueAtTime(pitch,time,0,1);
		while(f0==NUMundefined && pt<points->nt-1)
		{
			f0 = Pitch_getValueAtTime(pitch,points->t[pt+1],0,1); // ????
			pt++;
		}

		// extract spectrum from spectrogram
		if(fout != NULL)
		{
			if(spect ==  NULL) break;

			fprintf(fout,"%f %.2f %d %.2f\n",
				time,f0,spect->ny,spect->dy);

			for(count=1; count<=5; count++)
			{
				value = 0;
				if(formants != NULL)
				{
					value =
					Formant_getValueAtTime(formants,count,time,0);
					if(value == NUMundefined)
					   value = 0.0;

					bandw =Formant_getBandwidthAtTime(formants,count,time,0);
					if(bandw == NUMundefined)
						bandw = 0.0;
				}
				fprintf(fout,"%.1f %.1f  ",value,bandw);
			}
			fputc('\n',fout);

			// find the 'best' looking slice - largest standard deviation
			max = 0.0;
			best = 24;
			start = 1;
			if(spect->dy > 0)
				start = (int)(phase / spect->dy) + 1;  // only use higher freq than 'phase'
//			end = spect->ny;
			end = (int)(maxformant/spect->dy)+1;  // and lower than 'end'

			for(count=32; count<48; count++)
			{
				total = 0.0;
				for(i=start; i<=end; i++)
					total += spect->z[i][count];

				if(spect->ny <= start) break;

				average = total / (spect->ny + 1 - start);
				total = 0.0;
				for(i=start; i<=end; i++)
				{
					value = spect->z[i][count] - average;
					total += (value * value);
				}
				if(total > max)
				{
					max = total;
					best = count;
				}
			}
//printf("%d: fstart=%d-%d max=%f best=%d\n",pt,start,end,max,best);
			for(i=1; i <= spect->ny; i++)
			{
				value = spect->z[i][best];
				if(value < 0) value = 0;

				fprintf(fout,"%f%c",sqrt(value),
					(i % 8)==0?'\n':' ');
			}
			fprintf(fout,"\n\n");
		}
		for(i=1; i <= ny; i++)
		{
			if(i <= spect->ny)
				thee->z[i][pt] = spect->z[i][best];
			else
				thee->z[i][pt] = 0;
		}
	}
	if(fout != NULL) fclose(fout);

	forget (spect);
	forget (pitch);
	forget (points);
	forget (sound);
	return thee;
error:
	forget (pitch);
	return Melder_errorp ("(Sound_to_PointProcess:) Not performed.");
}

