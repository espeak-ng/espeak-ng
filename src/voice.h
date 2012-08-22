/***************************************************************************
 *   Copyright (C) 2005 by Jonathan Duddington                             *
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


typedef struct {
	char name[40];

	int phoneme_tab_ix;  // phoneme table number
	int pitch_base;    // Hz<<12
	int pitch_range;   // standard = 0x1000
	
	int speedf1;
	int speedf2;
	int speedf3;

	int flutter;
	int roughness;
	int echo_delay;
	int echo_amp;
	int n_harmonic_peaks;  // highest formant which is formed from adding harmonics
	int peak_shape;        // alternative shape for formant peaks (0=standard 1=squarer)

	// parameters used by Wavegen
	int freq[N_PEAKS];    // 100% = 256
	int height[N_PEAKS];  // 100% = 256
	int width[N_PEAKS];   // 100% = 256

	// copies without temporary adjustments from embedded commands
	int freq2[N_PEAKS];    // 100% = 256
	int height2[N_PEAKS];  // 100% = 256
	int width2[N_PEAKS];   // 100% = 256

	// This table provides the opportunity for tone control.
	// Adjustment of harmonic amplitudes, steps of 8Hz
	// value of 128 means no change
	#define N_TONE_ADJUST  1000
	unsigned char tone_adjust[N_TONE_ADJUST];   //  8Hz steps * 1000 = 8kHz

} voice_t;

// percentages shown to user, ix=N_PEAKS means ALL peaks
extern USHORT voice_pcnt[N_PEAKS+1][3];


extern voice_t *voice;
extern int tone_points[10];

voice_t *LoadVoice(char *voice_name, int control);
voice_t *LoadVoiceVariant(const char *voice_name, int variant);
void WavegenSetVoice(voice_t *v);
void ReadTonePoints(char *string, int *tone_pts);

