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

#include "StdAfx.h"

#include <stdio.h>
#include <string.h>
#include <wctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"


/* Note this module is mostly old code that needs to be rewritten to
   provide a more flexible intonation system.
*/

// bits in SYLLABLE.flags
#define SYL_RISE        1
#define SYL_EMPHASIS    2
#define SYL_END_CLAUSE   4

typedef struct {
	char stress;
	char env;
	char flags;   //bit 0=pitch rising, bit1=emnphasized, bit2=end of clause
	char nextph_type;
	short pitch1;
	short pitch2;
} SYLLABLE;

static SYLLABLE *syllable_tab;


static int tone_pitch_env;    /* used to return pitch envelope */



/* Pitch data for tone types */
/*****************************/


#define    PITCHfall   0
#define    PITCHrise   1
#define    PITCHfrise  2   // and 3 must be for the varient preceded by 'r'
#define    PITCHfrise2 4   // and 5 must be the 'r' variant
#define    PITCHrisefall   6

/*  0  fall */
unsigned char env_fall[128] = {
 0xff, 0xfd, 0xfa, 0xf8, 0xf6, 0xf4, 0xf2, 0xf0, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0,
 0xde, 0xdc, 0xda, 0xd8, 0xd6, 0xd4, 0xd2, 0xd0, 0xce, 0xcc, 0xca, 0xc8, 0xc6, 0xc4, 0xc2, 0xc0,
 0xbe, 0xbc, 0xba, 0xb8, 0xb6, 0xb4, 0xb2, 0xb0, 0xae, 0xac, 0xaa, 0xa8, 0xa6, 0xa4, 0xa2, 0xa0,
 0x9e, 0x9c, 0x9a, 0x98, 0x96, 0x94, 0x92, 0x90, 0x8e, 0x8c, 0x8a, 0x88, 0x86, 0x84, 0x82, 0x80,
 0x7e, 0x7c, 0x7a, 0x78, 0x76, 0x74, 0x72, 0x70, 0x6e, 0x6c, 0x6a, 0x68, 0x66, 0x64, 0x62, 0x60,
 0x5e, 0x5c, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48, 0x46, 0x44, 0x42, 0x40,
 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e, 0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20,
 0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x00 };

/*  1  rise */
unsigned char env_rise[128] = {
 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,
 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,
 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
 0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,
 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,
 0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
 0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfd, 0xff };

unsigned char env_frise[128] = {
 0xff, 0xf4, 0xea, 0xe0, 0xd6, 0xcc, 0xc3, 0xba, 0xb1, 0xa8, 0x9f, 0x97, 0x8f, 0x87, 0x7f, 0x78,
 0x71, 0x6a, 0x63, 0x5c, 0x56, 0x50, 0x4a, 0x44, 0x3f, 0x39, 0x34, 0x2f, 0x2b, 0x26, 0x22, 0x1e,
 0x1a, 0x17, 0x13, 0x10, 0x0d, 0x0b, 0x08, 0x06, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x13, 0x15, 0x17,
 0x1a, 0x1d, 0x1f, 0x22, 0x25, 0x28, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x39, 0x3b, 0x3d, 0x40,
 0x42, 0x45, 0x47, 0x4a, 0x4c, 0x4f, 0x51, 0x54, 0x57, 0x5a, 0x5d, 0x5f, 0x62, 0x65, 0x68, 0x6b,
 0x6e, 0x71, 0x74, 0x78, 0x7b, 0x7e, 0x81, 0x85, 0x88, 0x8b, 0x8f, 0x92, 0x96, 0x99, 0x9d, 0xa0,
 0xa4, 0xa8, 0xac, 0xaf, 0xb3, 0xb7, 0xbb, 0xbf, 0xc3, 0xc7, 0xcb, 0xcf, 0xd3, 0xd7, 0xdb, 0xe0 };

static unsigned char env_r_frise[128] = {
 0xcf, 0xcc, 0xc9, 0xc6, 0xc3, 0xc0, 0xbd, 0xb9, 0xb4, 0xb0, 0xab, 0xa7, 0xa2, 0x9c, 0x97, 0x92,
 0x8c, 0x86, 0x81, 0x7b, 0x75, 0x6f, 0x69, 0x63, 0x5d, 0x57, 0x50, 0x4a, 0x44, 0x3e, 0x38, 0x33,
 0x2d, 0x27, 0x22, 0x1c, 0x17, 0x12, 0x0d, 0x08, 0x04, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x0a, 0x0c, 0x0d, 0x0f, 0x12, 0x14, 0x16,
 0x19, 0x1b, 0x1e, 0x21, 0x24, 0x27, 0x2a, 0x2d, 0x30, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3f, 0x41,
 0x43, 0x46, 0x48, 0x4b, 0x4d, 0x50, 0x52, 0x55, 0x58, 0x5a, 0x5d, 0x60, 0x63, 0x66, 0x69, 0x6c,
 0x6f, 0x72, 0x75, 0x78, 0x7b, 0x7e, 0x81, 0x85, 0x88, 0x8b, 0x8f, 0x92, 0x96, 0x99, 0x9d, 0xa0,
 0xa4, 0xa8, 0xac, 0xaf, 0xb3, 0xb7, 0xbb, 0xbf, 0xc3, 0xc7, 0xcb, 0xcf, 0xd3, 0xd7, 0xdb, 0xe0 };

static unsigned char env_frise2[128] = {
 0xff, 0xf9, 0xf4, 0xee, 0xe9, 0xe4, 0xdf, 0xda, 0xd5, 0xd0, 0xcb, 0xc6, 0xc1, 0xbd, 0xb8, 0xb3,
 0xaf, 0xaa, 0xa6, 0xa1, 0x9d, 0x99, 0x95, 0x90, 0x8c, 0x88, 0x84, 0x80, 0x7d, 0x79, 0x75, 0x71,
 0x6e, 0x6a, 0x67, 0x63, 0x60, 0x5d, 0x59, 0x56, 0x53, 0x50, 0x4d, 0x4a, 0x47, 0x44, 0x41, 0x3e,
 0x3c, 0x39, 0x37, 0x34, 0x32, 0x2f, 0x2d, 0x2b, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x0f, 0x0e, 0x0d, 0x0c, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05,
 0x05, 0x04, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08,
 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x15, 0x17, 0x18, 0x1a, 0x1c, 0x1e, 0x20 };

static unsigned char env_r_frise2[128] = {
 0xd0, 0xce, 0xcd, 0xcc, 0xca, 0xc8, 0xc7, 0xc5, 0xc3, 0xc1, 0xc0, 0xbd, 0xbb, 0xb8, 0xb5, 0xb3,
 0xb0, 0xad, 0xaa, 0xa7, 0xa3, 0xa0, 0x9d, 0x99, 0x96, 0x92, 0x8f, 0x8b, 0x87, 0x84, 0x80, 0x7c,
 0x78, 0x74, 0x70, 0x6d, 0x69, 0x65, 0x61, 0x5d, 0x59, 0x55, 0x51, 0x4d, 0x4a, 0x46, 0x42, 0x3e,
 0x3b, 0x37, 0x34, 0x31, 0x2f, 0x2d, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a, 0x19,
 0x17, 0x15, 0x14, 0x12, 0x11, 0x0f, 0x0e, 0x0d, 0x0c, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x05,
 0x04, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08,
 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x15, 0x17, 0x18, 0x1a, 0x1c, 0x1e, 0x20 };

static unsigned char env_risefall[128] = {
 0x98, 0x99, 0x99, 0x9a, 0x9c, 0x9d, 0x9f, 0xa1, 0xa4, 0xa7, 0xa9, 0xac, 0xb0, 0xb3, 0xb6, 0xba,
 0xbe, 0xc1, 0xc5, 0xc9, 0xcd, 0xd1, 0xd4, 0xd8, 0xdc, 0xdf, 0xe3, 0xe6, 0xea, 0xed, 0xf0, 0xf2,
 0xf5, 0xf7, 0xf9, 0xfb, 0xfc, 0xfd, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfd,
 0xfb, 0xfa, 0xf8, 0xf6, 0xf3, 0xf1, 0xee, 0xec, 0xe9, 0xe6, 0xe4, 0xe0, 0xdd, 0xda, 0xd7, 0xd3,
 0xd0, 0xcc, 0xc8, 0xc4, 0xc0, 0xbc, 0xb8, 0xb4, 0xb0, 0xac, 0xa7, 0xa3, 0x9f, 0x9a, 0x96, 0x91,
 0x8d, 0x88, 0x84, 0x7f, 0x7b, 0x76, 0x72, 0x6d, 0x69, 0x65, 0x60, 0x5c, 0x58, 0x54, 0x50, 0x4c,
 0x48, 0x44, 0x40, 0x3c, 0x39, 0x35, 0x32, 0x2f, 0x2b, 0x28, 0x26, 0x23, 0x20, 0x1d, 0x1a, 0x17,
 0x15, 0x12, 0x0f, 0x0d, 0x0a, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char env_rise2[128] = {
 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x06, 0x06,
 0x07, 0x08, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
 0x16, 0x17, 0x18, 0x19, 0x1b, 0x1c, 0x1d, 0x1f, 0x20, 0x22, 0x23, 0x25, 0x26, 0x28, 0x29, 0x2b,
 0x2d, 0x2f, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x47, 0x49, 0x4b,
 0x4e, 0x50, 0x52, 0x55, 0x57, 0x5a, 0x5d, 0x5f, 0x62, 0x65, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76,
 0x79, 0x7c, 0x7f, 0x82, 0x86, 0x89, 0x8c, 0x90, 0x93, 0x96, 0x9a, 0x9d, 0xa0, 0xa3, 0xa6, 0xa9,
 0xac, 0xaf, 0xb2, 0xb5, 0xb8, 0xbb, 0xbe, 0xc1, 0xc4, 0xc7, 0xca, 0xcd, 0xd0, 0xd3, 0xd6, 0xd9,
 0xdc, 0xdf, 0xe2, 0xe4, 0xe7, 0xe9, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfb, 0xfd };

static unsigned char env_fall2[128] = {
 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfb, 0xfb, 0xfa, 0xfa, 0xf9, 0xf8, 0xf8, 0xf7, 0xf7, 0xf6, 0xf6,
 0xf5, 0xf4, 0xf4, 0xf3, 0xf3, 0xf2, 0xf2, 0xf1, 0xf0, 0xf0, 0xef, 0xee, 0xee, 0xed, 0xec, 0xeb,
 0xea, 0xea, 0xe9, 0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0, 0xde, 0xdd, 0xdc, 0xdb,
 0xd9, 0xd8, 0xd6, 0xd5, 0xd3, 0xd2, 0xd0, 0xce, 0xcc, 0xcb, 0xc9, 0xc7, 0xc5, 0xc3, 0xc0, 0xbe,
 0xbc, 0xb9, 0xb7, 0xb5, 0xb2, 0xaf, 0xad, 0xaa, 0xa7, 0xa4, 0xa1, 0x9e, 0x9a, 0x97, 0x94, 0x90,
 0x8d, 0x89, 0x85, 0x81, 0x7d, 0x79, 0x75, 0x71, 0x6d, 0x68, 0x64, 0x61, 0x5e, 0x5b, 0x57, 0x54,
 0x51, 0x4d, 0x4a, 0x46, 0x43, 0x40, 0x3c, 0x39, 0x35, 0x32, 0x2e, 0x2a, 0x27, 0x23, 0x1f, 0x1c,
 0x18, 0x14, 0x11, 0x0d, 0x0b, 0x09, 0x07, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00 };

static unsigned char env_fallrise3[128] = {
 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfd, 0xfc, 0xfa, 0xf8, 0xf6, 0xf4, 0xf1, 0xee, 0xeb,
 0xe8, 0xe5, 0xe1, 0xde, 0xda, 0xd6, 0xd2, 0xcd, 0xc9, 0xc4, 0xbf, 0xba, 0xb6, 0xb0, 0xab, 0xa6,
 0xa1, 0x9c, 0x96, 0x91, 0x8b, 0x86, 0x80, 0x7b, 0x75, 0x6f, 0x6a, 0x64, 0x5f, 0x59, 0x54, 0x4f,
 0x49, 0x44, 0x3f, 0x3a, 0x35, 0x30, 0x2b, 0x26, 0x22, 0x1d, 0x19, 0x15, 0x11, 0x0d, 0x0a, 0x07,
 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x05,
 0x07, 0x09, 0x0b, 0x0d, 0x10, 0x12, 0x15, 0x18, 0x1b, 0x1e, 0x22, 0x25, 0x29, 0x2d, 0x31, 0x35,
 0x3a, 0x3e, 0x43, 0x48, 0x4c, 0x51, 0x57, 0x5b, 0x5e, 0x62, 0x65, 0x68, 0x6b, 0x6e, 0x71, 0x74,
 0x76, 0x78, 0x7b, 0x7c, 0x7e, 0x80, 0x81, 0x82, 0x83, 0x83, 0x84, 0x84, 0x83, 0x83, 0x82, 0x81 };

static unsigned char env_fallrise4[128] = {
 0x72, 0x72, 0x71, 0x71, 0x70, 0x6f, 0x6d, 0x6c, 0x6a, 0x68, 0x66, 0x64, 0x61, 0x5f, 0x5c, 0x5a,
 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x45, 0x42, 0x3f, 0x3b, 0x38, 0x35, 0x32, 0x2f, 0x2c, 0x29,
 0x26, 0x23, 0x20, 0x1d, 0x1b, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06,
 0x07, 0x07, 0x08, 0x09, 0x0a, 0x0c, 0x0d, 0x0f, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1b, 0x1d, 0x20,
 0x23, 0x26, 0x29, 0x2c, 0x2f, 0x33, 0x37, 0x3b, 0x3f, 0x43, 0x47, 0x4c, 0x51, 0x56, 0x5b, 0x60,
 0x65, 0x6a, 0x6f, 0x74, 0x79, 0x7f, 0x84, 0x89, 0x8f, 0x95, 0x9b, 0xa1, 0xa7, 0xad, 0xb3, 0xba,
 0xc0, 0xc7, 0xce, 0xd5, 0xdc, 0xe3, 0xea, 0xf1, 0xf5, 0xf7, 0xfa, 0xfc, 0xfd, 0xfe, 0xff, 0xff };

static unsigned char env_risefallrise[128] = {
 0x7f, 0x7f, 0x7f, 0x80, 0x81, 0x83, 0x84, 0x87, 0x89, 0x8c, 0x8f, 0x92, 0x96, 0x99, 0x9d, 0xa1,
 0xa5, 0xaa, 0xae, 0xb2, 0xb7, 0xbb, 0xc0, 0xc5, 0xc9, 0xcd, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6,
 0xea, 0xed, 0xf0, 0xf3, 0xf5, 0xf8, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xfe, 0xfd, 0xfc, 0xfb, 0xf9,
 0xf7, 0xf4, 0xf0, 0xec, 0xe7, 0xe2, 0xdc, 0xd5, 0xce, 0xc6, 0xbd, 0xb4, 0xa9, 0x9e, 0x92, 0x88,
 0x82, 0x7d, 0x77, 0x72, 0x6c, 0x66, 0x60, 0x5a, 0x54, 0x4e, 0x49, 0x42, 0x3c, 0x37, 0x32, 0x2d,
 0x28, 0x24, 0x1f, 0x1b, 0x18, 0x14, 0x11, 0x0e, 0x0c, 0x09, 0x07, 0x06, 0x05, 0x04, 0x04, 0x04,
 0x04, 0x05, 0x06, 0x08, 0x0a, 0x0d, 0x10, 0x14, 0x18, 0x1d, 0x23, 0x29, 0x2f, 0x37, 0x3e, 0x47,
 0x50, 0x5a, 0x64, 0x70, 0x7c, 0x83, 0x85, 0x88, 0x8a, 0x8c, 0x8e, 0x8f, 0x91, 0x92, 0x93, 0x93 };




unsigned char *envelope_data[18] = {
	env_fall,
	env_rise,
	env_frise,  env_r_frise,
	env_frise2, env_r_frise2,
	env_risefall, env_risefall,

	env_fallrise3, env_fallrise3,
	env_fallrise4, env_fallrise4,
	env_fall2, env_fall2,
	env_rise2, env_rise2,
	env_risefallrise, env_risefallrise
 };


/* all pitches given in Hz above pitch_base */

// pitch change during the main part of the clause
static int drops_0[8] = {0x400,0x400,0x700,0x700,0x700,0xa00,0x1800,0x0e00};
//static int drops_1[8] = {0x400,0x400,0x600,0x600,0xc00,0xc00,0x0e00,0x0e00};
//static int drops_2[8] = {0x400,0x400,0x600,0x600,-0x800,0xc00,0x0e00,0x0e00};

static short oflow[] = {0, 20, 12, 4, 0};
static short oflow_emf[] = {5, 24, 15, 10, 5};
static short oflow_less[] = {3, 19, 12, 7, 2};
// static short oflow_test2[] = {20, 0, 20, 0, 20};
// static short back_emf[] = {35, 32, 0};


#define N_TONE_HEAD_TABLE  13
#define N_TONE_NUCLEUS_TABLE  13


typedef struct {
	unsigned char pre_start;
	unsigned char pre_end;

	unsigned char body_start;
	unsigned char body_end;

	int  *body_drops;
	unsigned char body_max_steps;
	char body_lower_u;

	char n_overflow;
	short *overflow;
} TONE_HEAD;


typedef struct {
	unsigned char pitch_env0;     /* pitch envelope, tonic syllable at end */
	unsigned char tonic_max0;
	unsigned char tonic_min0;

	unsigned char pitch_env1;     /*     followed by unstressed */
	unsigned char tonic_max1;
	unsigned char tonic_min1;

	short *backwards;

	unsigned char tail_start;
	unsigned char tail_end;
	unsigned char flags;
} TONE_NUCLEUS;

#define T_EMPH  1

static TONE_HEAD tone_head_table[N_TONE_HEAD_TABLE] = {
   {20, 25,   34, 22,  drops_0, 3, 3,   5, oflow},      // 0 statement
   {20, 25,   34, 20,  drops_0, 3, 3,   5, oflow},      // 1 comma
   {20, 25,   34, 20,  drops_0, 3, 3,   5, oflow},      // 2 question
   {20, 25,   36, 22,  drops_0, 3, 4,   5, oflow_emf},  // 3 exclamation
   {20, 25,   34, 22,  drops_0, 3, 3,   5, oflow},      // 4 statement, emphatic
   {20, 25,   32, 24,  drops_0, 4, 3,   5, oflow_less}, // 5 statement, less intonation
   {20, 25,   32, 24,  drops_0, 4, 3,   5, oflow_less}, // 6 comma, less intonation
   {20, 25,   32, 24,  drops_0, 4, 3,   5, oflow_less}, // 7 comma, less intonation, less rise
   {20, 25,   34, 22,  drops_0, 3, 3,   5, oflow},      // 8 pitch raises at end of sentence
   {20, 25,   34, 20,  drops_0, 3, 3,   5, oflow},      // 9 comma
   {20, 25,   34, 22,  drops_0, 3, 3,   5, oflow},      // 10  question
   {15, 18,   18, 14,  drops_0, 3, 3,   5, oflow_less},   // 11 test
   {20, 25,   24, 22,  drops_0, 3, 3,   5, oflow_less},   // 12 test
};

static TONE_NUCLEUS tone_nucleus_table[N_TONE_NUCLEUS_TABLE] = {
   {PITCHfall,   30, 5,  PITCHfall,   32, 9, NULL, 12, 7, 0},      // 0 statement
   {PITCHfrise,  35, 8,  PITCHfrise2, 35,10, NULL, 15, 23, 0},     // 1 comma
   {PITCHfrise,  39,10,  PITCHfrise2, 36,10, NULL, 15, 28, 0},     // 2 question
//   {PITCHfall,   41, 4,  PITCHfall,   41,27, NULL, 16, 4, T_EMPH},  // 3 exclamation
   {PITCHfall,   41, 4,  PITCHfall,   41,35, NULL, 35, 4, T_EMPH},  // 3 exclamation
   {PITCHfall,   38, 2,  PITCHfall,   42,30, NULL, 15, 5, 0},      // 4 statement, emphatic
   {PITCHfall,   28, 5,  PITCHfall,   28, 9, NULL, 12, 7, 0},      // 5 statement, less intonation
   {PITCHfrise,  30, 8,  PITCHfrise2, 30,10, NULL, 13, 20, 0},      // 6 comma, less intonation
   {PITCHfrise2, 28, 7,  PITCHfall,   29,14, NULL, 14, 8, 0},      // 7 comma, less intonation, less rise
   {PITCHrise,   30,20,  PITCHfall,   19,14, NULL, 20, 26, 0},     // 8 pitch raises at end of sentence
   {PITCHfrise,  35,11,  PITCHfrise2, 32,10, NULL, 19, 24, 0},     // 9 comma
   {PITCHfrise,  39,15,  PITCHfall,   28,14, NULL, 20, 36, 0},     // 10  question
   {PITCHfall,   28, 6,  PITCHfall,   28,10, NULL, 12, 6, 0},      // 11 test
   {PITCHfall,   35, 9,  PITCHfall,   35,12, NULL, 16, 10, 0},     // 12 test
};
  


/* index by 0=. 1=, 2=?, 3=! 4=none, 5=emphasized */
unsigned char punctuation_to_tone[INTONATION_TYPES][PUNCT_INTONATIONS] = {
	{0,1,2,3,0,4},
	{0,1,2,3,0,4},
	{5,6,2,3,0,4},
	{5,7,1,3,0,4},
	{8,9,10,3,0,0},
	{8,8,10,3,0,0},
	{11,11,11,11,0,0},  // 6 test
	{12,12,12,12,0,0}
};



/* indexed by stress */
static int min_drop[] =  {0x300,0x300,0x400,0x400,0x900,0x900,0x900,0xb00};




#define SECONDARY  3
#define PRIMARY    4
#define PRIMARY_STRESSED 6
#define PRIMARY_LAST 7


static int  number_pre;
static int  number_body;
static int  number_tail;
static int  last_primary;
static int  tone_posn;
static int  tone_posn2;
static int  no_tonic;


static void count_pitch_vowels(int start, int end, int clause_end)
/****************************************************************/
{
	int  ix;
	int  stress;
	int  max_stress = 0;
	int  max_stress_posn = 0;  // last syllable ot the highest stress
	int  max_stress_posn2 = 0;  // penuntimate syllable of the highest stress

	number_pre = -1;    /* number of vowels before 1st primary stress */
	number_body = 0;
	number_tail = 0;   /* number between tonic syllable and next primary */
	last_primary = -1;
	
	for(ix=start; ix<end; ix++)
	{
		stress = syllable_tab[ix].stress;   /* marked stress level */

		if(stress >= max_stress)
		{
			if(stress > max_stress)
			{
				max_stress_posn2 = ix;
			}
			else
			{
				max_stress_posn2 = max_stress_posn;
			}
			max_stress_posn = ix;
			max_stress = stress;
		}
		if(stress >= PRIMARY)
		{
			if(number_pre < 0)
				number_pre = ix - start;

			last_primary = ix;
		}

	}

	if(number_pre < 0)
		number_pre = end;

	number_tail = end - max_stress_posn - 1;
	tone_posn = max_stress_posn;
	tone_posn2 = max_stress_posn2;

	if(no_tonic)
	{
		tone_posn = tone_posn2 = end;  // next position after the end of the truncated clause
	}
	else
	if(last_primary >= 0)
	{
		if(end == clause_end)
		{
			syllable_tab[last_primary].stress = PRIMARY_LAST;
		}
	}
	else
	{
		// no primary stress. Use the highest stress
		syllable_tab[tone_posn].stress = PRIMARY_LAST;
	}
}   /* end of count_pitch_vowels */




static int count_increments(int ix, int end_ix, int min_stress)
/*************************************************************/
/* Count number of primary stresses up to tonic syllable or body_reset */
{
	int  count = 0;
	int  stress;

	while(ix < end_ix)
	{
		stress = syllable_tab[ix++].stress;
		if(stress >= PRIMARY_LAST)
			break;

		if(stress >= min_stress)
			count++;
	}
	return(count);
}  /* end of count_increments */



static void set_pitch(SYLLABLE *syl, int base, int drop)
/******************************************************/
// Set the pitch of a vowel in syllable_tab.  Base & drop are Hz * 256
{
	int  pitch1, pitch2;
	int  flags = 0;

	/* adjust experimentally */
	int  pitch_range2 = 148;
	int  pitch_base2 = 72;

	if(base < 0)  base = 0;

	pitch2 = ((base * pitch_range2 ) >> 15) + pitch_base2;

	if(drop < 0)
	{
		flags = SYL_RISE;
		drop = -drop;
	}

	pitch1 = pitch2 + ((drop * pitch_range2) >> 15);

	if(pitch1 > 511) pitch1 = 511;
	if(pitch2 > 511) pitch2 = 511;

	syl->pitch1 = pitch1;
	syl->pitch2 = pitch2;
	syl->flags |= flags;
}   /* end of set_pitch */



static int calc_pitch_segment(int ix, int end_ix, TONE_HEAD *th, TONE_NUCLEUS *tn, int min_stress, int continuing)
/**********************************************************************************************/
/* Calculate pitches until next RESET or tonic syllable, or end.
	Increment pitch if stress is >= min_stress.
	Used for tonic segment */
{
	int  stress;
	int  pitch=0;
	int  increment=0;
	int  n_primary=0;
	int  n_steps=0;
	int  initial;
	int  overflow=0;
	int  n_overflow;
	int *drops;
	short *overflow_tab;
	SYLLABLE *syl;

	static short continue_tab[5] = {-13, 16, 10, 4, 0};

	drops = th->body_drops;

	if(continuing)
	{
		initial =0;
		overflow = 0;
		n_overflow = 5;
		overflow_tab = continue_tab;
		increment = (th->body_end - th->body_start) << 8;
		increment = increment / (th->body_max_steps -1);
	}
	else
	{
		n_overflow = th->n_overflow;
		overflow_tab = th->overflow;
		initial = 1;
	}

	while(ix < end_ix)
	{
		syl = &syllable_tab[ix];
		stress = syl->stress;

//		if(stress == PRIMARY_MARKED)
//			initial = 1;    // reset the intonation pattern

		if(initial || (stress >= min_stress))
		{
			// a primary stress

			if((initial) || (stress == 5))
			{
				initial = 0;
				overflow = 0;
				n_steps = n_primary = count_increments(ix,end_ix,min_stress);

				if(n_steps > th->body_max_steps)
					n_steps = th->body_max_steps;

				if(n_steps > 1)
				{
					increment = (th->body_end - th->body_start) << 8;
					increment = increment / (n_steps -1);
				}
				else
					increment = 0;

				pitch = th->body_start << 8;
			}
			else
			{
				if(n_steps > 0)
					pitch += increment;
				else
				{
					pitch = (th->body_end << 8) - (increment * overflow_tab[overflow++])/16;
					if(overflow >= n_overflow)
					{
						overflow = 0;
						overflow_tab = th->overflow;
					}
				}
			}

			n_steps--;

			n_primary--;
			if((tn->backwards) && (n_primary < 2))
			{
					pitch = tn->backwards[n_primary] << 8;	
			}
		}

		if(stress >= PRIMARY)
		{
			syl->stress = PRIMARY_STRESSED;
			set_pitch(syl,pitch,drops[stress]);
		}
		else
		if(stress >= SECONDARY)
		{
			set_pitch(syl,pitch,drops[stress]);
		}
		else
		{
			/* unstressed, drop pitch if preceded by PRIMARY */
			if((syllable_tab[ix-1].stress & 0x3f) >= SECONDARY)
				set_pitch(syl,pitch - (th->body_lower_u << 8), drops[stress]);
			else
				set_pitch(syl,pitch,drops[stress]);
		}

		ix++;
	}
	return(ix);
}   /* end of calc_pitch_segment */





static int calc_pitch_segment2(int ix, int end_ix, int start_p, int end_p, int min_stress)
/****************************************************************************************/
/* Linear pitch rise/fall, change pitch at min_stress or stronger
	Used for pre-head and tail */
{
	int  stress;
	int  pitch;
	int  increment;
	int  n_increments;
	int  drop;
	SYLLABLE *syl;

	if(ix >= end_ix)
		return(ix);
		
	n_increments = count_increments(ix,end_ix,min_stress);
	increment = (end_p - start_p) << 8;
	
	if(n_increments > 1)
	{
		increment = increment / n_increments;
	}

	
	pitch = start_p << 8;
	while(ix < end_ix)
	{
		syl = &syllable_tab[ix];
		stress = syl->stress;

		if(increment > 0)
		{
			set_pitch(syl,pitch,-increment);
			pitch += increment;
		}
		else
		{
			drop = -increment;
			if(drop < min_drop[stress])
				drop = min_drop[stress];

			pitch += increment;

			if(drop > 0x900)
				drop = 0x900;
			set_pitch(syl, pitch, drop);
		}
			
		ix++;
	}
	return(ix);
}   /* end of calc_pitch_segment2 */






static int calc_pitches(int start, int end,  int head_tone, int nucleus_tone)
//===========================================================================
// Calculate pitch values for the vowels in this tone group
{
	int  ix;
	TONE_HEAD *th;
	TONE_NUCLEUS *tn;
	int  drop;
	int continuing = 0;

	if(start > 0)
		continuing = 1;

	th = &tone_head_table[head_tone];
	tn = &tone_nucleus_table[nucleus_tone];
	ix = start;

	/* vowels before the first primary stress */
	/******************************************/

	if(number_pre > 0)
	{
		ix = calc_pitch_segment2(ix, ix+number_pre, th->pre_start, th->pre_end, 0);
	}

	/* body of tonic segment */
	/*************************/

	if(option_tone_flags & OPTION_EMPHASIZE_PENULTIMATE)
	{
		tone_posn = tone_posn2;  // put tone on the penultimate stressed word 
	}
	ix = calc_pitch_segment(ix,tone_posn, th, tn, PRIMARY, continuing);
		
	if(no_tonic)
		return(0);

	/* tonic syllable */
	/******************/
	
	if(tn->flags & T_EMPH)
	{
		syllable_tab[ix].flags |= SYL_EMPHASIS;
	}

	if(number_tail == 0)
	{
		tone_pitch_env = tn->pitch_env0;
		drop = tn->tonic_max0 - tn->tonic_min0;
		set_pitch(&syllable_tab[ix++],tn->tonic_min0 << 8,drop << 8);
	}
	else
	{
		tone_pitch_env = tn->pitch_env1;
		drop = tn->tonic_max1 - tn->tonic_min1;
		set_pitch(&syllable_tab[ix++],tn->tonic_min1 << 8,drop << 8);
	}

	syllable_tab[tone_posn].env = tone_pitch_env;
	if(syllable_tab[tone_posn].stress == PRIMARY)
		syllable_tab[tone_posn].stress = PRIMARY_STRESSED;

	/* tail, after the tonic syllable */
	/**********************************/
	
	calc_pitch_segment2(ix, end, tn->tail_start, tn->tail_end, 0);

	return(tone_pitch_env);
}   /* end of calc_pitches */






static void CalcPitches_Tone(Translator *tr, int clause_tone)
{//==========================================================
//  clause_tone: 0=. 1=, 2=?, 3=! 4=none
	PHONEME_LIST *p;
	int  ix;
	int  count_stressed=0;
	int  final_stressed=0;

	int  tone_ph;
	int pause;
	int tone_promoted;
	PHONEME_TAB *tph;
	PHONEME_TAB *prev_tph;   // forget across word boundary
	PHONEME_TAB *prevw_tph;  // remember across word boundary
	PHONEME_TAB *prev2_tph;  // 2 tones previous
	PHONEME_LIST *prev_p;

	int  pitch_adjust = 0;     // pitch gradient through the clause - inital value
	int  pitch_decrement = 0;   //   decrease by this for each stressed syllable
	int  pitch_low = 0;         //   until it drops to this
	int  pitch_high = 0;       //   then reset to this

	p = &phoneme_list[0];

	// count number of stressed syllables
	p = &phoneme_list[0];
	for(ix=0; ix<n_phoneme_list; ix++, p++)
	{
		if((p->type == phVOWEL) && (p->stresslevel >= 4))
		{
			if(count_stressed == 0)
				final_stressed = ix;

			if(p->stresslevel >= 4)
			{
				final_stressed = ix;
				count_stressed++;
			}
		}
	}

	phoneme_list[final_stressed].stresslevel = 7;

	// language specific, changes to tones
	if(tr->translator_name == L('v','i'))
	{
		// LANG=vi
		p = &phoneme_list[final_stressed];
		if(p->tone_ph == 0)
			p->tone_ph = PhonemeCode('7');   // change default tone (tone 1) to falling tone at end of clause
	}


	pause = 1;
	tone_promoted = 0;

	prev_p = p = &phoneme_list[0];
	prev_tph = prevw_tph = phoneme_tab[phonPAUSE];

	// perform tone sandhi
	for(ix=0; ix<n_phoneme_list; ix++, p++)
	{
		if((p->type == phPAUSE) && (p->ph->std_length > 50))
		{
			pause = 1;  // there is a pause since the previous vowel
			prevw_tph = phoneme_tab[phonPAUSE];  // forget previous tone
		}

		if(p->newword)
		{
			prev_tph = phoneme_tab[phonPAUSE];  // forget across word boundaries
		}

		if(p->synthflags & SFLAG_SYLLABLE)
		{
			tone_ph = p->tone_ph;
			tph = phoneme_tab[tone_ph];

			// Mandarin
			if(tr->translator_name == L('z','h'))
			{
				if(tone_ph == 0)
				{
					if(pause || tone_promoted)
					{
						tone_ph = PhonemeCode2('5','5');  // no previous vowel, use tone 1
						tone_promoted = 1;
					}
					else
					{
						tone_ph = PhonemeCode2('1','1');  // default tone 5
					}

					p->tone_ph = tone_ph;
					tph = phoneme_tab[tone_ph];

				}
				else
				{
					tone_promoted = 0;
				}

				if(ix == final_stressed)
				{
					if((tph->mnemonic == 0x3535 ) || (tph->mnemonic == 0x3135))
					{
						// change sentence final tone 1 or 4 to stress 6, not 7
						phoneme_list[final_stressed].stresslevel = 6;
					}
				}

				if(prevw_tph->mnemonic == 0x343132)  // [214]
				{
					if(tph->mnemonic == 0x343132)   // [214]
						prev_p->tone_ph = PhonemeCode2('3','5');
					else
						prev_p->tone_ph = PhonemeCode2('2','1'); 
				}
				if((prev_tph->mnemonic == 0x3135)  && (tph->mnemonic == 0x3135))  //  [51] + [51]
				{
					prev_p->tone_ph = PhonemeCode2('5','3');
				}

				if(tph->mnemonic == 0x3131)  // [11] Tone 5
				{
					// tone 5, change its level depending on the previous tone (across word boundaries)
					if(prevw_tph->mnemonic == 0x3535)
						p->tone_ph = PhonemeCode2('2','2');
					if(prevw_tph->mnemonic == 0x3533)
						p->tone_ph = PhonemeCode2('3','3');
					if(prevw_tph->mnemonic == 0x343132)
						p->tone_ph = PhonemeCode2('4','4');

					// tone 5 is unstressed (shorter)
					p->stresslevel = 1;   // diminished stress
				}
			}

			prev_p = p;
			prev2_tph = prevw_tph;
			prevw_tph = prev_tph = tph;
			pause = 0;
		}
	}

	// convert tone numbers to pitch
	p = &phoneme_list[0];
	for(ix=0; ix<n_phoneme_list; ix++, p++)
	{
		if(p->synthflags & SFLAG_SYLLABLE)
		{
			tone_ph = p->tone_ph;

			if(p->stresslevel != 1)  // TEST, consider all syllables as stressed
			{
				if(ix == final_stressed)
				{
					// the last stressed syllable
					pitch_adjust = pitch_low;
				}
				else
				{
					pitch_adjust -= pitch_decrement;
					if(pitch_adjust <= pitch_low)
						pitch_adjust = pitch_high;
				}
			}

			if(tone_ph ==0)
			{
				tone_ph = phonDEFAULTTONE;  // no tone specified, use default tone 1
				p->tone_ph = tone_ph;
			}
			p->pitch1 = pitch_adjust + phoneme_tab[tone_ph]->start_type;
			p->pitch2 = pitch_adjust + phoneme_tab[tone_ph]->end_type;
		}
	}


}  // end of Translator::CalcPitches_Tone



void CalcPitches(Translator *tr, int clause_type)
{//==============================================
//  clause_type: 0=. 1=, 2=?, 3=! 4=none
	PHONEME_LIST *p;
	SYLLABLE *syl;
	int  ix;
	int  x;
	int  st_ix;
	int n_st;
	int  option;
	int  group_tone;
	int  group_tone_emph;
	int  group_tone_comma;
	int ph_start=0;
	int st_start;
	int st_clause_end;
	int count;
	int n_primary;
	int count_primary;
	PHONEME_TAB *ph;
	int ph_end=n_phoneme_list;

	SYLLABLE syllable_tab2[N_PHONEME_LIST];

	syllable_tab = syllable_tab2;   // don't use permanent storage. it's only needed during the call of CalcPitches()
	n_st = 0;
	n_primary = 0;
	for(ix=0; ix<(n_phoneme_list-1); ix++)
	{
		p = &phoneme_list[ix];
		if(p->synthflags & SFLAG_SYLLABLE)
		{
			syllable_tab[n_st].flags = 0;
			syllable_tab[n_st].env = PITCHfall;
			syllable_tab[n_st].nextph_type = phoneme_list[ix+1].type;
			syllable_tab[n_st++].stress = p->stresslevel;

			if(p->stresslevel >= 4)
				n_primary++;
		}
		else
		if((p->ph->code == phonPAUSE_CLAUSE) && (n_st > 0))
		{
			syllable_tab[n_st-1].flags |= SYL_END_CLAUSE;
		}
	}
	syllable_tab[n_st].stress = 0;   // extra 0 entry at the end

	if(n_st == 0)
		return;  // nothing to do



	if(tr->langopts.tone_language == 1)
	{
		CalcPitches_Tone(tr,clause_type);
		return;
	}


	option = tr->langopts.intonation_group;
	if(option >= INTONATION_TYPES)
		option = 0;

	group_tone = tr->punct_to_tone[option][clause_type]; 
	group_tone_emph = tr->punct_to_tone[option][5];   // emphatic form of statement
	group_tone_comma = tr->punct_to_tone[option][1];   // emphatic form of statement

	if(clause_type == 4)
		no_tonic = 1;       /* incomplete clause, used for abbreviations such as Mr. Dr. Mrs. */
	else
		no_tonic = 0;

	st_start = 0;
	count_primary=0;
	for(st_ix=0; st_ix<n_st; st_ix++)
	{
		syl = &syllable_tab[st_ix];

		if(syl->stress >= 4)
			count_primary++;

		if(syl->stress == 6)
		{
			// reduce the stress of the previous stressed syllable (review only the previous few syllables)
			for(ix=st_ix-1; ix>=st_start && ix>=(st_ix-3); ix--)
			{
				if(syllable_tab[ix].stress == 6)
					break;
				if(syllable_tab[ix].stress == 4)
				{
					syllable_tab[ix].stress = 3;
					break;
				}
			}

			// are the next primary syllables also emphasized ?
			for(ix=st_ix+1; ix<n_st; ix++)
			{
				if(syllable_tab[ix].stress == 4)
					break;
				if(syllable_tab[ix].stress == 6)
				{
					// emphasize this syllable, but don't end the current tone group
					syllable_tab[st_ix].flags = SYL_EMPHASIS;
					syl->stress = 5;
					break;
				}
			}
		}

		if(syl->stress == 6)
		{
			// an emphasized syllable, end the tone group after the next primary stress
			syllable_tab[st_ix].flags = SYL_EMPHASIS;

			count = 0;
			if((n_primary - count_primary) > 1)
				count =1;

			for(ix=st_ix+1; ix<n_st; ix++)
			{
				if(syllable_tab[ix].stress > 4)
					break;
				if(syllable_tab[ix].stress == 4)
				{
					count++;
					if(count > 1)
						break;
				}
			}

			count_pitch_vowels(st_start, ix, n_st);
			if((ix < n_st) || (clause_type == 0))
				calc_pitches(st_start, ix, group_tone_emph, group_tone_emph);   // split into > 1 tone groups, use emphatic tone
			else
				calc_pitches(st_start, ix, group_tone, group_tone);

			st_start = ix;
		}
		if((st_start < st_ix) && (syl->flags & SYL_END_CLAUSE))
		{
			// end of clause after this syllable, indicated by a phonPAUSE_CLAUSE phoneme
			st_clause_end = st_ix+1;
			count_pitch_vowels(st_start, st_clause_end, st_clause_end);
			calc_pitches(st_start, st_clause_end, group_tone_comma, group_tone_comma);
			st_start = st_clause_end;
		}
	}

	if(st_start < st_ix)
	{
		count_pitch_vowels(st_start, st_ix, n_st);
		calc_pitches(st_start, st_ix, group_tone, group_tone);
	}

	
	// unpack pitch data
	st_ix=0;
	for(ix=ph_start; ix < ph_end; ix++)
	{
		p = &phoneme_list[ix];
		p->stresslevel = syllable_tab[st_ix].stress;
		
		if(p->synthflags & SFLAG_SYLLABLE)
		{
			syl = &syllable_tab[st_ix];

			x = syl->pitch1 - 72;
			if(x < 0) x = 0;
			p->pitch1 = x;

			x = syl->pitch2 - 72;
			if(x < 0) x = 0;
			p->pitch2 = x;

			p->env = PITCHfall;
			if(syl->flags & SYL_RISE)
			{
				p->env = PITCHrise;
			}
			else
			if(p->stresslevel > 5)
				p->env = syl->env;

			if(p->pitch1 > p->pitch2)
			{
				// swap so that pitch2 is the higher
				x = p->pitch1;
				p->pitch1 = p->pitch2;
				p->pitch2 = x;
			}

if(p->tone_ph)
{
	ph = phoneme_tab[p->tone_ph];
	x = (p->pitch1 + p->pitch2)/2;
	p->pitch2 = x + ph->end_type;
	p->pitch1 = x + ph->start_type;
}

			if(syl->flags & SYL_EMPHASIS)
			{
				p->stresslevel |= 8;      // emphasized
			}
	
			st_ix++;
		}
	}

}  // end of Translator::CalcPitches

 
