/*
This file is a part of the NV Speech Player project. 
URL: https://bitbucket.org/nvaccess/speechplayer
Copyright 2014 NV Access Limited.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License, as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
This license can be found at:
http://www.gnu.org/licenses/gpl.html
*/

#ifndef SPEECHPLAYER_UTILS_H
#define SPEECHPLAYER_UTILS_H

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static inline int MAX(int a, int b) { return((a) > (b) ? a : b); }
static inline int MIN(int a, int b) { return((a) < (b) ? a : b); }

static inline int ISNAN (double x) {
	if (x != x)
	return 1;
	else
	return 0;
}

inline double calculateValueAtFadePosition(double oldVal, double newVal, double curFadeRatio) {
	if(ISNAN(newVal)) return oldVal;
	return oldVal+((newVal-oldVal)*curFadeRatio);
}

#endif
