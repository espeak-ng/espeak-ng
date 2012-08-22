/***************************************************************************
 *   Copyright (C) 2005, 2006 by Jonathan Duddington                       *
 *   jsd@clara.co.uk                                                       *
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


// this version keeps wavemult window as a constant fraction
// of the cycle length - but that spreads out the HF peaks too much

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "portaudio.h"

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"

#define N_SINTAB  2048
#include "sintab.h"

//#define LOG_WGEN

extern float polint(float xa[],float ya[],int n,float x);

#define PI2 6.283185307
#define STEPSIZE  64                // 2.9mS at 22 kHz sample rate

FILE *f_log = NULL;
int option_amplitude = 60;
int option_waveout = 0;
int option_harmonic1 = 10;

int general_amplitude = 60;
int consonant_amp = 25;   // 24

int PHASE_INC_FACTOR;
int n_peaks1=5;   // highest peak to use shape
int samplerate = 22050;

wavegen_peaks_t peaks[N_PEAKS];
int peak_harmonic[N_PEAKS];
int peak_height[N_PEAKS];

// only used when generating wave from harmonic table rather than peaks
float harm_sqrt_pitch=0;
int harm_sqrt_n = 0;
float harm_sqrt[600];
float harm_sqrt_inc[600];

#define N_ECHO_BUF 5500   // max of 250mS at 22050 Hz
int echo_head;
int echo_tail;
int echo_amp = 0;
short echo_buf[N_ECHO_BUF];

#define N_TONE_ADJUST  1250
unsigned char tone_adjust[N_TONE_ADJUST];   //  8Hz steps * 1250 = 10kHz


#define N_LOWHARM  30
int harm_inc[N_LOWHARM];    // only for these harmonics do we interpolate amplitude between steps
int *harmspect;
int hswitch=0;
int hspect[2][MAX_HARMONIC];         // 2 copies, we interpolate between then
int max_hval=0;

int nsamples=0;       // number to do
int amplitude = 32;
int amplitude_v = 0;
int cycle_modulation = 0;

unsigned char *mix_wavefile = NULL;  // wave file to be added to synthesis
int n_mix_wavefile = 0;       // length in bytes
int mix_wave_scale = 0;         // 0=2 byte samples
int mix_wavefile_ix = 0;


int pitch;          // pitch Hz*256
int pitch_ix;       // index into pitch envelope (*256)
int pitch_inc;      // increment to pitch_ix
unsigned char *pitch_env;
int pitch_base;     // Hz*256 low, before modified by envelope
int pitch_range;    // Hz*256 range of envelope

static int samplecount=0;    // number done
static int samplecount_start=0;  // count at start of this segment
int end_wave=0;      // continue to end of wave cycle
int wavephase;
int phaseinc;
int wbytes;         // number of samples in half a cycle at current pitch
int cbytes;
int hf_factor;


unsigned char *out_ptr;
unsigned char *out_end;
unsigned char outbuf[1024];

// the queue of operations passed to wavegen from sythesize
int wcmdq[N_WCMDQ][5];
int wcmdq_head=0;
int wcmdq_tail=0;

FILE *f_wave = NULL;
PortAudioStream *pa_stream=NULL;

/* default pitch envelope, a steady fall */
#define ENV_LEN  128
unsigned char Pitch_env0[ENV_LEN] = {
    255,253,251,249,247,245,243,241,239,237,235,233,231,229,227,225,
    223,221,219,217,215,213,211,209,207,205,203,201,199,197,195,193,
    191,189,187,185,183,181,179,177,175,173,171,169,167,165,163,161,
    159,157,155,153,151,149,147,145,143,141,139,137,135,133,131,129,
    127,125,123,121,119,117,115,113,111,109,107,105,103,101, 99, 97,
     95, 93, 91, 89, 87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 67, 65,
     63, 61, 59, 57, 55, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33,
     31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11,  9,  7,  5,  3,  1
};

/*
unsigned char Pitch_long[ENV_LEN] = {
	254,249,250,251,252,253,254,254, 255,255,255,255,254,254,253,252,
	251,250,249,247,244,242,238,234, 230,225,221,217,213,209,206,203,
	199,195,191,187,183,179,175,172, 168,165,162,159,156,153,150,148,
	145,143,140,138,136,134,132,130, 128,126,123,120,117,114,111,107,
	104,100,96,91, 86,82,77,73, 70,66,63,60, 58,55,53,51,
	49,47,46,45, 43,42,40,38, 36,34,31,28, 26,24,22,20,
	18,16,14,12, 11,10,9,8, 8,8,8,8, 9,8,8,8,
	8,8,7,7, 6,6,6,5, 4,4,3,3, 2,1,1,0
};
*/


// Flutter table, to add natural variations to the pitch
#define N_FLUTTER  0x1b0
static signed char Flutter_tab[N_FLUTTER] = {
     0x00, 0x1b, 0x35, 0x4b, 0x5c, 0x68, 0x6d, 0x6c,
     0x66, 0x5c, 0x4e, 0x3f, 0x30, 0x23, 0x18, 0x10,
     0x0c, 0x0b, 0x0c, 0x0f, 0x12, 0x14, 0x15, 0x12,
     0x0c, 0x03, 0xf8, 0xe9, 0xd9, 0xc9, 0xbc, 0xb1,
     0xaa, 0xa9, 0xad, 0xb6, 0xc4, 0xd6, 0xe9, 0xfd,
     0x0f, 0x1f, 0x2a, 0x31, 0x32, 0x2d, 0x24, 0x16,
     0x07, 0xf8, 0xe9, 0xdc, 0xd3, 0xcf, 0xcf, 0xd5,
     0xde, 0xeb, 0xfa, 0x08, 0x16, 0x22, 0x2b, 0x30,

     0x31, 0x2e, 0x28, 0x20, 0x18, 0x11, 0x0b, 0x08,
     0x09, 0x0d, 0x14, 0x1d, 0x28, 0x32, 0x3b, 0x40,
     0x41, 0x3d, 0x34, 0x25, 0x12, 0xfc, 0xe3, 0xca,
     0xb2, 0x9e, 0x8e, 0x85, 0x82, 0x85, 0x8f, 0x9e,
     0xb0, 0xc4, 0xd9, 0xed, 0xff, 0x0c, 0x16, 0x1c,
     0x1f, 0x1f, 0x1d, 0x1b, 0x19, 0x19, 0x1c, 0x21,
     0x29, 0x33, 0x3f, 0x4a, 0x55, 0x5c, 0x60, 0x5e,
     0x58, 0x4c, 0x3b, 0x26, 0x0f, 0xf7, 0xe0, 0xcb,

     0xba, 0xae, 0xa8, 0xa9, 0xaf, 0xba, 0xc8, 0xd9,
     0xea, 0xfa, 0x06, 0x10, 0x14, 0x15, 0x11, 0x09,
     0x00, 0xf5, 0xeb, 0xe2, 0xdc, 0xda, 0xdc, 0xe1,
     0xe9, 0xf4, 0x00, 0x0a, 0x14, 0x1a, 0x1e, 0x1d,
     0x18, 0x10, 0x06, 0xfc, 0xf1, 0xe8, 0xe2, 0xe0,
     0xe3, 0xeb, 0xf8, 0x08, 0x1b, 0x2f, 0x42, 0x52,
     0x5f, 0x66, 0x67, 0x62, 0x57, 0x46, 0x32, 0x1c,
     0x04, 0xef, 0xdb, 0xcb, 0xc0, 0xb9, 0xb7, 0xb8,

     0xbd, 0xc3, 0xca, 0xd0, 0xd4, 0xd6, 0xd5, 0xd2,
     0xcd, 0xc8, 0xc2, 0xbf, 0xbe, 0xc1, 0xc9, 0xd6,
     0xe7, 0xfc, 0x13, 0x2b, 0x43, 0x59, 0x6a, 0x76,
     0x7c, 0x7b, 0x74, 0x67, 0x55, 0x40, 0x2a, 0x14,
     0x00, 0xf1, 0xe4, 0xdd, 0xda, 0xdc, 0xe1, 0xe8,
     0xf0, 0xf7, 0xfd, 0xff, 0xff, 0xfb, 0xf4, 0xeb,
     0xe1, 0xd7, 0xce, 0xc8, 0xc6, 0xc8, 0xce, 0xd9,
     0xe6, 0xf5, 0x04, 0x13, 0x1f, 0x27, 0x2b, 0x2a,

     0x24, 0x19, 0x0b, 0xfb, 0xea, 0xdb, 0xce, 0xc6,
     0xc3, 0xc5, 0xcd, 0xda, 0xeb, 0xff, 0x12, 0x26,
     0x38, 0x45, 0x4f, 0x53, 0x52, 0x4d, 0x44, 0x39,
     0x2d, 0x21, 0x16, 0x0e, 0x09, 0x07, 0x07, 0x0a,
     0x0d, 0x11, 0x12, 0x11, 0x0c, 0x04, 0xf8, 0xe8,
     0xd5, 0xc1, 0xae, 0x9c, 0x8e, 0x85, 0x81, 0x85,
     0x8f, 0x9f, 0xb4, 0xcd, 0xe8, 0x01, 0x1a, 0x30,
     0x41, 0x4d, 0x53, 0x53, 0x50, 0x48, 0x3f, 0x35,

     0x2b, 0x24, 0x1f, 0x1c, 0x1d, 0x20, 0x25, 0x2a,
     0x2e, 0x31, 0x30, 0x2b, 0x23, 0x16, 0x07, 0xf6,
     0xe3, 0xd1, 0xc2, 0xb6, 0xaf, 0xad, 0xb1, 0xba,
     0xc8, 0xd9, 0xeb, 0xfe, 0x0e, 0x1c, 0x26, 0x2a,
     0x29, 0x23, 0x18, 0x0a, 0xfb, 0xec, 0xdd, 0xd2,
     0xca, 0xc8, 0xca, 0xd0, 0xda, 0xe7, 0xf5, 0x02,
};

// waveform shape table for HF peaks, formants 6,7,8
#define N_WAVEMULT 512
int wavemult_offset=0;
int wavemult_max=0;
unsigned char wavemult[N_WAVEMULT];

int WavegenFill(void);


void WcmdqStop()
{//=============
	wcmdq_head = 0;
	wcmdq_tail = 0;
}


int WcmdqFree()
{//============
	int i;
	i = wcmdq_head - wcmdq_tail;
	if(i <= 0) i += N_WCMDQ;
	return(i);
}

int WcmdqUsed()
{//============
   return(N_WCMDQ - WcmdqFree());
}


void WcmdqInc()
{//============
	wcmdq_tail++;
	if(wcmdq_tail >= N_WCMDQ) wcmdq_tail=0;
}

void WcmdqIncHead()
{//================
	wcmdq_head++;
	if(wcmdq_head >= N_WCMDQ) wcmdq_head=0;
}

// data points from which to make the pk_shape
#define PEAKSHAPEW 256

static float pk_shape_x[2][8] = {0,-0.6, 0.0, 0.6, 1.4, 2.5, 4.5, 5.5,
                                 0,-0.6, 0.0, 0.6, 1.4, 2.0, 4.5, 5.5};
static float pk_shape_y[2][8] = {0,  67,  81,  67,  31,  14,   0,  -6,
                                 0,  77,  81,  77,  31,   6,   0,  -6};
unsigned char pk_shape1[PEAKSHAPEW+1];
unsigned char pk_shape2[PEAKSHAPEW+1];
unsigned char *pk_shape;

void WavegenInitPkData(int which)
{//==============================
	int ix;
	int p;
	float x;
	float y[PEAKSHAPEW];
	float maxy=0;

	if(which==0)
		pk_shape = pk_shape1;
	else
		pk_shape = pk_shape2;

	p = 0;
	for(ix=0;ix<PEAKSHAPEW;ix++)
	{
		x = (4.5*ix)/PEAKSHAPEW;
		if(x >= pk_shape_x[which][p+3]) p++;
		y[ix] = polint(&pk_shape_x[which][p],&pk_shape_y[which][p],3,x);
		if(y[ix] > maxy) maxy = y[ix];
	}
	for(ix=0;ix<PEAKSHAPEW;ix++)
	{
		p = (int)(y[ix]*255/maxy);
      pk_shape[ix] = (p >= 0) ? p : 0;
	}
	pk_shape[PEAKSHAPEW]=0;

}  //  end of WavegenInitPkData




// PortAudio interface

int userdata[4];
int callback_count=0;
PaError pa_init_err=0;
PaDeviceID default_dev = 0;
const PaDeviceInfo *default_info = NULL;

int WaveCallback(void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer, PaTimestamp outTime, void *userData )
{//=======================================================================
	int ix;

	callback_count++;
	
	out_ptr = (unsigned char *)outputBuffer;
	out_end = out_ptr + framesPerBuffer*2;
	
	ix = WavegenFill();
	return(ix);
}  //  end of WaveCallBack


int WavegenOpenSound()
{//===================
	PaError err, err2;
	PaError active;

	if(option_waveout)
	{
		// writing to WAV file, not to portaudio
		return(0);
	}

	active = Pa_StreamActive(pa_stream);
	if(active == 1)
		return(0);
	if(active < 0)
	{
		err2 = Pa_OpenDefaultStream(&pa_stream,0,1,paInt16,samplerate,512,20,WaveCallback,(void *)userdata);
	}
	err = Pa_StartStream(pa_stream);
	if(err != paNoError)
	{
		return(2);
	}
		
	return(0);
}


int WavegenCloseSound()
{//====================
	PaError active;

	// check whether speaking has finished, and close the stream
	if(pa_stream != NULL)
	{
		active = Pa_StreamActive(pa_stream);
		if(active == 0)
		{
			Pa_CloseStream(pa_stream);
			pa_stream = NULL;
			return(1);
		}
	}
	return(0);
}


int WavegenInitSound()
{//===================
	PaError err;

	// PortAudio sound output library
	err = Pa_Initialize();
	pa_init_err = err;
	if(err != paNoError)
	{
		fprintf(stderr,"Failed to initialise the PortAudio sound\n");
		return(1);
	}
	return(0);
}


void WavegenInit(int rate, int wavemult_fact)
{//==========================================
	int  ix;
   double x;

	if(wavemult_fact == 0)
		wavemult_fact=60;  // default

	samplerate = rate;
	PHASE_INC_FACTOR = 0x8000000 / samplerate;   // assumes pitch is Hz*32

	samplecount = 0;
	nsamples = 0;
	wavephase = 0x7fffffff;
	max_hval = 0;

	// set up window to generate a spread of harmonics from a
	// single peak for HF peaks
	wavemult_max = (samplerate * wavemult_fact)/(256 * 50);
	if(wavemult_max > N_WAVEMULT) wavemult_max = N_WAVEMULT;
	
	wavemult_offset = wavemult_max/2;

	for(ix=0; ix<wavemult_max; ix++)
	{
		x = 127*(1.0 - cos(PI2*ix/wavemult_max));
		wavemult[ix] = (int)x;
	}

	// adjustment of harmonic amplitudes, steps of 8Hz
	// value of 128 means no change
	for(ix=0; ix<N_TONE_ADJUST; ix++)
	{
		if(ix < 100)
		{
			x = 128.0 - (100-ix) * 0;
			if(x < 0) x = 0;
		}
		else
			x = 128;
		tone_adjust[ix] = int(x);
	}
  
	WavegenInitPkData(1);
	WavegenInitPkData(0);
	pk_shape = pk_shape2;

}  // end of WavegenInit


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
//				wxLogError(_T("Error in routine 'polint'"));
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





#ifdef deleted
void log_params()
{//==============
if(f_log != NULL)
{
	fprintf(f_log,"%2d %5d: %4d %5.1f %5d %3.0f %5d %5.1f %5d %3.0f\n",amplitude,samplecount,
	peaks[1].freq>>16,(float)peaks[1].freq_inc/0x10000,peaks[1].height>>8,peaks[1].height_inc/256,
	peaks[2].freq>>16,(float)peaks[2].freq_inc/0x10000,peaks[2].height>>8,peaks[2].height_inc/256);
}
}
#endif



void WavegenSetEcho(int delay, int amp)
{//====================================
	if(delay >= N_ECHO_BUF)
		delay = N_ECHO_BUF-1;
	if(amp > 100)
		amp = 100;

	memset(echo_buf,0,sizeof(echo_buf));
	echo_tail = 0;
	echo_head = delay;
	echo_amp = amp;

	// compensate (partially) for increase in amplitude due to echo
	general_amplitude = ((option_amplitude * (512-amp))/512);
}


#ifdef SPECT_EDITOR
int HarmToHarmspect(int pitch, int *htab)
{//======================================
	// Interpolate from an original harmonic table and pitch to give a
	// new htab at a specified pitch

// Not currently used by "speak"

	int  pitch_source;
	int  pitch_max;
	int  fq;
	int  h;
	int  ix;
	float xa[4];
	float y;
	int  y1;
	
	pitch_source = int(harm_sqrt_pitch * 65536);    // pitch << 16
	pitch_max = pitch_source * harm_sqrt_n;
	
	for(fq = pitch, h=1; fq<pitch_max; fq += pitch, h++)
	{
		ix = int(fq / pitch_source + 0.5);   // nearest harmonic
		if(ix < 2) ix = 2;
		if(ix > harm_sqrt_n - 1)  ix = harm_sqrt_n - 1;

		xa[0] = pitch_source * (ix-2);
		xa[1] = pitch_source * (ix-1);
		xa[2] = pitch_source * (ix);
		xa[3] = pitch_source * (ix+1);
		y = polint(xa,&harm_sqrt[ix-2],3,float(fq));
		y1 = int(y) >> 2;
		htab[h] = (y1 * y1) >> 11;
	}
	return(h-1);
}
#endif


int PeaksToHarmspect(wavegen_peaks_t *peaks, int pitch, int *htab)
{//===============================================================
// Calculate the amplitude of each  harmonics from the formants
// Only for formants 0 to 5

   // pitch and freqs are Hz<<16

	int f;
	wavegen_peaks_t *p;
	int fp;   // centre freq of peak
	int fhi;  // high freq of peak
	int h;    // harmonic number
	int pk;
	int hmax;
	int x;
	int h1;

#ifdef SPECT_EDITOR
	if(harm_sqrt_n > 0)
		return(HarmToHarmspect(pitch,htab));
#endif

	// initialise as much of *out as we will need
	hmax = (peaks[n_peaks1].freq + peaks[n_peaks1].right)/pitch;
	for(h=0;h<=hmax;h++)
		htab[h]=0;

	h=0;
	for(pk=0; pk<=n_peaks1; pk++)
	{
		p = &peaks[pk];
		if((p->height == 0) || (fp = p->freq)==0)
			continue;

		fhi = p->freq + p->right;
		h = ((p->freq - p->left) / pitch) + 1;
		if(h <= 0) h = 1;
		
		for(f=pitch*h; f < fp; f+=pitch)
		{
			htab[h++] += pk_shape[(fp-f)/(p->left>>8)] * p->height;
		}
		for(;f < fhi; f+=pitch)
		{
			htab[h++] += pk_shape[(f-fp)/(p->right>>8)] * p->height;
		}
	}

	// find the nearest harmonic for HF peaks where we don't use shape
	for(; pk<N_PEAKS; pk++)
	{
		peak_harmonic[pk] = peaks[pk].freq / pitch;
		x = peaks[pk].height >> 14;
		peak_height[pk] = x * x * 5;
	}

	// convert from the square-rooted values
	f = 0;
	for(h=0; h<=hmax; h++, f+=pitch)
	{
		x = htab[h] >> 15;
		htab[h] = (x * x) >> 7;

		htab[h] = (htab[h] * tone_adjust[f >> 19]) >> 13;  // index tone_adjust with Hz/8
	}

	// adjust the amplitude of the first harmonic, affects tonal quality
	h1 = htab[1] * option_harmonic1;
	htab[1] = h1/8;

	// calc intermediate increments of LF harmonics
	if(samplecount != 0)
	{
		for(h=1; h<N_LOWHARM; h++)
		{
			harm_inc[h] = (htab[h] - harmspect[h]) >> 3;
		}
	}
	
	return(hmax);  // highest harmonic number
}  // end of PeaksToHarmspect



static void AdvanceParameters()
{//============================
// Called every 64 samples to increment the formant freq, height, and widths

	int x;
	int ix;
	static int Flutter_ix = 0;
	static int FlutterAmp=64;
	
	// advance the pitch
	pitch_ix += pitch_inc;
	if((ix = pitch_ix>>8) > 127) ix = 127;
	x = pitch_env[ix] * pitch_range;
	pitch = (x>>8) + pitch_base;
	
	/* add pitch flutter */
	if(Flutter_ix >= N_FLUTTER)
		Flutter_ix = 0;
	x = (signed int)Flutter_tab[Flutter_ix++] * FlutterAmp;
	pitch += x;


	if(samplecount == samplecount_start)
		return;
		
	for(ix=0; ix<n_peaks1; ix++)
	{
		peaks[ix].freq1 += peaks[ix].freq_inc;
		peaks[ix].freq = int(peaks[ix].freq1);
		peaks[ix].height1 += peaks[ix].height_inc;
		if((peaks[ix].height = int(peaks[ix].height1)) < 0)
			peaks[ix].height = 0;
		peaks[ix].left1 += peaks[ix].left_inc;
		peaks[ix].left = int(peaks[ix].left1);
		peaks[ix].right1 += peaks[ix].right_inc;
		peaks[ix].right = int(peaks[ix].right1);
	}
	for(;ix < N_PEAKS; ix++)
	{
		// formants 6,7,8 don't have a width parameter
		peaks[ix].freq1 += peaks[ix].freq_inc;
		peaks[ix].freq = int(peaks[ix].freq1);
		peaks[ix].height1 += peaks[ix].height_inc;
		if((peaks[ix].height = int(peaks[ix].height1)) < 0)
			peaks[ix].height = 0;
	}

	if(harm_sqrt_n != 0)
	{
		// We are generating from a harmonic spectrum at a given pitch, not from formant peaks
		for(ix=0; ix<harm_sqrt_n; ix++)
			harm_sqrt[ix] += harm_sqrt_inc[ix];
	}
}  //  end of AdvanceParameters



static int Wavegen()
{//=================
	unsigned short waveph;
	unsigned short theta;
	int total;
	int h;
	int z, z1, z2;
	int ov;
	static int maxh, maxh2;
	int pk;
	signed char c;
	int sample;
	static int agc = 256;
	static int h_switch_sign = 0;
	static int cycle_count = 0;
	static int amplitude2 = 0;   // adjusted for pitch

	// continue until the output buffer is full, or
	// the required number of samples have been produced

	for(;;)
	{
		if((end_wave==0) && (samplecount==nsamples))
			return(0);

		if((samplecount & 0x3f) == 0)
		{
			// every 64 samples, adjust the parameters
			if(samplecount == 0)
			{
				hswitch = 0;
				maxh2 = PeaksToHarmspect(peaks,pitch<<4,hspect[0]);
				amplitude2 = (amplitude * pitch)/(100 << 12);

            // switch sign of harmonics above about 900Hz, to reduce max peak amplitude
				h_switch_sign = 890 / (pitch >> 12);
			}
			else
				AdvanceParameters();
				
// log_params();
			// pitch is Hz<<12
			phaseinc = (pitch>>7) * PHASE_INC_FACTOR;
			wbytes = samplerate/(pitch >> 11);  // sr/(pitch*2)
			hf_factor = pitch >> 11;

			maxh = maxh2;
			harmspect = hspect[hswitch];
			hswitch ^= 1;
			maxh2 = PeaksToHarmspect(peaks,pitch<<4,hspect[hswitch]);
			
		}
		else
		if((samplecount & 0x07) == 0)
		{
			for(h=1; h<N_LOWHARM && h<maxh2; h++)
				harmspect[h] += harm_inc[h];
				
			// bring automctic gain control back towards unity
			if(agc < 256) agc++;
		}
		
		samplecount++;

		if(wavephase > 0)
		{
			wavephase += phaseinc;
			if(wavephase < 0)
			{
				// sign has changed, reached a quiet point in the waveform
				cbytes = wavemult_offset-wbytes;
				if(samplecount > nsamples)
					return(0);

				cycle_count++;

				// adjust amplitude to compensate for fewer harmonics at higher pitch
				amplitude2 = (amplitude * pitch)/(100 << 12);

				// introduce roughness into the sound by reducing the amplitude of
				// alternate cycles
				if(cycle_modulation == 2)
				{
					if(cycle_count & 1)
						amplitude2 = (amplitude2 * 14)/16;
				}
				else
				if(cycle_modulation == 3)
				{
					// used for R sounds
					if((cycle_count % 3)==0)
						amplitude2 = (amplitude2 * 8)/16;
				}	
			}
		}
		else
		{
#ifdef test2
if(cycle_count & 1)
	wavephase += 100000;
else
	wavephase -= 100000;
#endif
			wavephase += phaseinc;
		}
		waveph = (unsigned short)(wavephase >> 16);
		total = 0;

		// apply HF peaks, formants 6,7,8
		// add a single harmonic and then spread this my multiplying by a
		// window.  This is to reduce the processing power needed to add the
		// higher frequence harmonics.
		cbytes++;
		if(cbytes >=0 && cbytes<wavemult_max)
		{
			for(pk=n_peaks1+1; pk<N_PEAKS; pk++)
			{
				theta = peak_harmonic[pk] * waveph;
				total += (long)sin_tab[theta >> 5] * peak_height[pk];
			}
			
			// spread the peaks by multiplying by a window
			total = (long)(total / hf_factor) * wavemult[cbytes];
		}


		// apply main peaks, formants 0 to 5
		theta = waveph;

		for(h=1; h<=h_switch_sign; h++)
		{
			total += (int(sin_tab[theta >> 5]) * harmspect[h]);
			theta += waveph;
		}
		while(h<=maxh)
		{
			total -= (int(sin_tab[theta >> 5]) * harmspect[h]);
			theta += waveph;
			h++;
		}

		// mix with sampled wave if required
		z2 = 0;
		if(mix_wavefile_ix < n_mix_wavefile)
		{
			if(mix_wave_scale == 0)
			{
				// a 16 bit sample
				c = mix_wavefile[mix_wavefile_ix+1];
				sample = mix_wavefile[mix_wavefile_ix] + (c * 256);
				mix_wavefile_ix += 2;
			}
			else
			{
				// a 8 bit sample, scaled
				sample = (signed char)mix_wavefile[mix_wavefile_ix++] * mix_wave_scale;
			}
			z2 = (sample * amplitude_v) >> 10;
		}

		z1 = z2 + (((total>>7) * amplitude2) >> 14);
		z = (z1 * agc) >> 8;
		
		z += ((echo_buf[echo_tail++] * echo_amp) >> 8);
		if(echo_tail >= N_ECHO_BUF)
			echo_tail=0;

		// check for overflow, 16bit signed samples
		if(z >= 32768)
		{
			ov = 8388608/z1 - 1;      // 8388608 is 2^23, i.e. max value * 256
			if(ov < agc) agc = ov;    // set agc to number of 1/256ths to multiply the sample by
			z = (z1 * agc) >> 8;      // reduce sample by agc value to prevent overflow
		}
		else
		if(z <= -32768)
		{
			ov = -8388608/z1 - 1;
			if(ov < agc) agc = ov;
			z = (z1 * agc) >> 8;
		}
		*out_ptr++ = z;
		*out_ptr++ = z >> 8;

		echo_buf[echo_head++] = z;
		if(echo_head >= N_ECHO_BUF)
			echo_head = 0;

		if(out_ptr >= out_end)
			return(1);
	}
	return(0);
}  //  end of Wavegen



int PlaySilence(int length, int resume)
{//====================================
	static int n_samples;
	int value;

	nsamples = 0;
	samplecount = 0;
	
	if(resume==0)
		n_samples = length;
		
	while(n_samples-- > 0)
	{
		value = (echo_buf[echo_tail++] * echo_amp) >> 8;

		if(echo_tail >= N_ECHO_BUF)
			echo_tail = 0;

		*out_ptr++ = value;
		*out_ptr++ = value >> 8;

		echo_buf[echo_head++] = value;
		if(echo_head >= N_ECHO_BUF)
			echo_head = 0;

		if(out_ptr >= out_end)
			return(1);
	}
	return(0);
}  // end of PlaySilence



int PlayWave(int length, int resume, unsigned char *data, int scale)
{//=================================================================
	static int n_samples;
	static int ix=0;
	int value;
	signed char c;
	
	if(resume==0)
	{
		n_samples = length;
		ix = 0;
	}
		
	nsamples = 0;
	samplecount = 0;
	
	while(n_samples-- > 0)
	{
		if(scale == 0)
		{
			c = data[ix+1];
			value = data[ix] + (c * 256);
			ix+=2;
		}
		else
		{
			value = (signed char)data[ix++] * scale;
		}
		value *= (consonant_amp * general_amplitude);   // reduce strength of consonant
		value = value >> 10;

		value += ((echo_buf[echo_tail++] * echo_amp) >> 8);

		if(value > 32767)
			value = 32768;
		else
		if(value < -32768)
			value = -32768;

		if(echo_tail >= N_ECHO_BUF)
			echo_tail = 0;

		out_ptr[0] = value;
		out_ptr[1] = value >> 8;
		out_ptr+=2;
		
		echo_buf[echo_head++] = (value*3)/4;
		if(echo_head >= N_ECHO_BUF)
			echo_head = 0;

		if(out_ptr >= out_end)
			return(1);
	}
	return(0);
}



void SetAmplitude(int value)
{//=========================
	amplitude = (value * general_amplitude)/16;
	amplitude_v = amplitude * 15;           // for wave mixed with voiced sounds
}


void SetPitch(int length, unsigned char *env, int pitch1, int pitch2)
{//==================================================================
// length in samples
// base and range are in Hz<<12
	int x;
	
	if((pitch_env = env)==NULL)
		pitch_env = Pitch_env0;  // default

	pitch_ix = 0;
	if(length==0)
		pitch_inc = 0;
	else
		pitch_inc = (256 * ENV_LEN * STEPSIZE)/length;

	if(pitch1 > pitch2)
	{
		x = pitch1;   // swap values
		pitch1 = pitch2;
		pitch2 = x;
	}

	pitch_base = voice->pitch_base + (pitch1 * voice->pitch_range);
	pitch_range = voice->pitch_base + (pitch2 * voice->pitch_range) - pitch_base;
	
	// set initial pitch
	pitch = ((pitch_env[0]*pitch_range)>>8) + pitch_base;   // Hz << 12

}  // end of SetPitch



void SetPitch_Hz(int length, unsigned char *env, int pitch1, int pitch2)
{//==================================================================
	SetPitch(length, env, pitch1, pitch2);
}


void PeaksZero(peak_t *sp, peak_t *zero)
{//===========================================
	int pk;

	memcpy(zero,sp,sizeof(peak_t)*N_PEAKS);

	for(pk=0; pk<N_PEAKS; pk++)
		zero[pk].pkheight = 0;

}  // end of PeaksZero


#ifdef SPECT_EDITOR
void SetSynthHtab(int length_mS, USHORT *ht1, int nh1, float dx1, USHORT *ht2, int nh2, float dx2)
{//===============================================================================================
// Not used by "speak"
	int ix;
	int length;
	int length2;
	float harm_sqrt2[600];
	
	
	length = (length_mS * samplerate)/1000;
	
	for(ix=0; ix<nh1 && ix<600; ix++)
	{
		harm_sqrt[ix] = ht1[ix];
	}
	for(ix=0; ix<nh2 && ix<600; ix++)
	{
		harm_sqrt2[ix] = ht2[ix];
	}
	harm_sqrt_pitch = dx1;
	harm_sqrt_n = nh1;
	
	if(dx1 != dx2)
	{
		// convert the harmonic tables to the same pitch
	}
	
	for(ix=0; ix<nh1; ix++)
	{
		if(nh1 <= nh2)
			harm_sqrt_inc[ix] = (harm_sqrt2[ix] - harm_sqrt[ix]) / length * STEPSIZE;
		else
			harm_sqrt_inc[ix] = 0;
	}
	
	// round the length to a multiple of the stepsize
	length2 = (length + STEPSIZE/2) & ~0x3f;
	if(length2 == 0)
		length2 = STEPSIZE;

	// add this length to any left over from the previous synth
	samplecount_start = samplecount;
	nsamples += length2;
}
#endif



void SetSynth(int param1, peak_t *sp1, peak_t *sp2)
{//================================================
	int ix;
	double next;
	int length;
	int length2;
	int qix;
	int cmd;
	voice_t *v;

	length = param1 & 0xffff;
	harm_sqrt_n = 0;
	end_wave = 1;

	// any additional information in the param1 ?
	cycle_modulation = (param1 >> 16) & 3;

	for(qix=wcmdq_head+1;;qix++)
	{
		if(qix >= N_WCMDQ) qix = 0;
		if(qix == wcmdq_tail) break;
		
		cmd = wcmdq[qix][0];
		if(cmd==WCMD_SPECT)
		{
			end_wave = 0;  // next wave generation is from another spectrum
			break;
		}
		if((cmd==WCMD_WAVE) || (cmd==WCMD_PAUSE))
			break;   // next is not from spectrum, so continue until end of wave cycle
	}

	v = voice;

	// round the length to a multiple of the stepsize
	length2 = (length + STEPSIZE/2) & ~0x3f;
	if(length2 == 0)
		length2 = STEPSIZE;

	// add this length to any left over from the previous synth
	samplecount_start = samplecount;
	nsamples += length2;

#ifdef LOG_WGEN
if(f_log != NULL)
{
	fprintf(f_log,"%c %3.1f mS  %3d/%3d, %4d/%3d,  to  %3d/%3d, %4d/%3d\n",
		'A',(float(length)*1000.0)/samplerate,
		sp1[1].pkfreq,sp1[1].pkheight/40, sp1[2].pkfreq,sp1[2].pkheight/40,
		sp2[1].pkfreq,sp2[1].pkheight/40, sp2[2].pkfreq,sp2[2].pkheight/40);
}
#endif

	for(ix=0; ix<N_PEAKS; ix++)
	{
		peaks[ix].freq1 = (sp1[ix].pkfreq * v->freq[ix]) << 8;
		peaks[ix].freq = int(peaks[ix].freq1);
		next = (sp2[ix].pkfreq * v->freq[ix]) << 8;
		peaks[ix].freq_inc =  ((next - peaks[ix].freq1) * STEPSIZE) / length2;
			
		peaks[ix].height1 = sp1[ix].pkheight * v->height[ix];
		peaks[ix].height = int(peaks[ix].height1);
		next = sp2[ix].pkheight * v->height[ix];
		peaks[ix].height_inc =  ((next - peaks[ix].height1) * STEPSIZE) / length2;

		if(ix <= n_peaks1)
		{
			peaks[ix].left1 = (sp1[ix].pkwidth * v->width[ix]) << 8;
			peaks[ix].left = int(peaks[ix].left1);
			next = (sp2[ix].pkwidth * v->width[ix]) << 8;
			peaks[ix].left_inc =  ((next - peaks[ix].left1) * STEPSIZE) / length2;
			
			peaks[ix].right1 = (sp1[ix].pkright * v->width[ix]) << 8;
			peaks[ix].right = int(peaks[ix].right1);
			next = (sp2[ix].pkright * v->width[ix]) << 8;
			peaks[ix].right_inc = ((next - peaks[ix].right1) * STEPSIZE) / length2;
		}
	}
}  // end of SetSynth


void SetSynth_mS(int length_mS, peak_t *sp1, peak_t *sp2)
{//======================================================
	SetSynth((length_mS * samplerate) / 1000, sp1, sp2);    // convert mS to samples
}


int Wavegen2(int length, int resume, peak_t *sp1, peak_t *sp2)
{//===========================================================
	if(resume==0)
		SetSynth(length,sp1,sp2);
		
	return(Wavegen());
}



int OpenWaveFile(const char *path)
/********************************/
{
   const unsigned char wave_hdr[44] = {
      'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
      0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
      2,0,0x10,0,'d','a','t','a',  0,0,0,0 };

#ifdef LOG_WGEN
if(f_log==NULL)
   f_log=fopen("speech.log","w");
#endif

	if(path == NULL)
		return(2);

   wavephase = 0x7fffffff;
   
   f_wave = fopen(path,"wb");
   if(f_wave != NULL)
   {
      fwrite(wave_hdr,1,sizeof(wave_hdr),f_wave);
		return(0);
   }
	return(1);
}   //  end of OpenWaveFile




void CloseWaveFile()
/******************/
{
   unsigned int pos;
   static int value;

   if(f_log != NULL)
	{
		fclose(f_log);
		f_log = NULL;
	}

   if(f_wave == NULL)
      return;

   fflush(f_wave);
   pos = ftell(f_wave);

   value = pos - 8;
   fseek(f_wave,4,SEEK_SET);
   fwrite(&value,4,1,f_wave);

	value = samplerate;
	fseek(f_wave,24,SEEK_SET);
	fwrite(&value,4,1,f_wave);

	value = samplerate*2;
	fseek(f_wave,28,SEEK_SET);
	fwrite(&value,4,1,f_wave);

   value = pos - 44;
   fseek(f_wave,40,SEEK_SET);
   fwrite(&value,4,1,f_wave);

   fclose(f_wave);
   f_wave = NULL;

} // end of CloseWaveFile



void MakeWaveFile()
{//================
	int result=1;

	while(result != 0)
	{
		out_ptr = outbuf;
		out_end = &outbuf[sizeof(outbuf)];
		result = Wavegen();
		if(f_wave != NULL)
			fwrite(outbuf,1,out_ptr-outbuf,f_wave);
	}
}  // end of MakeWaveFile





int WavegenFill()
{//==============
// Pick up next wavegen commands from the queue
// return: 0  output buffer has been filled
// return: 1  input command queue is now empty

	int *q;
	int length;
	int result;
	static int resume=0;

	while(out_ptr < out_end)
	{
		if(WcmdqUsed() <= 0)
		{
			while(out_ptr < out_end)
				*out_ptr++ = 0;
			return(1);              // queue empty, close sound channel
		}

		result = 0;
		q = wcmdq[wcmdq_head];
   	length = q[1];
	
		switch(q[0])
		{
		case WCMD_PITCH:
			SetPitch(length,(unsigned char *)q[2],q[3],q[4]);
			break;
	
		case WCMD_PAUSE:
			n_mix_wavefile = 0;
			if(length==0) break;
			result = PlaySilence(length,resume);
			break;
	
		case WCMD_WAVE:
			n_mix_wavefile = 0;
			result = PlayWave(length,resume,(unsigned char*)q[2],q[3]);
			break;
	
		case WCMD_WAVE2:
			// wave file to be played at the same time as synthesis
			mix_wave_scale = q[3];
			if(mix_wave_scale == 0)
				n_mix_wavefile = length*2;
			else
				n_mix_wavefile = length;
			mix_wavefile_ix = 0;
			mix_wavefile = (unsigned char *)q[2];
			break;
	
		case WCMD_SPECT2:   // as WCMD_SPECT but stop any concurrent wave file
			n_mix_wavefile = 0;   // ... and drop through to WCMD_SPECT case
		case WCMD_SPECT:
			result = Wavegen2(length,resume,(peak_t *)q[2],(peak_t *)q[3]);
			break;
	
		case WCMD_MARKER:
			break;
	
		case WCMD_AMPLITUDE:
			SetAmplitude(q[2]);
			break;
	
		case WCMD_VOICE:
			break;
		}
	
		if(result==0)
		{
			WcmdqIncHead();
			resume=0;
		}
		else
		{
			resume=1;
		}
	}
	
	return(0);
}  // end of WavegenFill



int WavegenFile(void)
{//==================
	int finished;

	out_ptr = outbuf;
	out_end = outbuf + sizeof(outbuf);

	finished = WavegenFill();

	if(f_wave != NULL)
	{
		fwrite(outbuf,1,out_ptr-outbuf,f_wave);
	}
	return(finished);
}  // end of WavegenFile

