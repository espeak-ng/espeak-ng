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

#ifndef SPEECHPLAYER_H
#define SPEECHPLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "frame.h"
#include "sample.h"

typedef void* speechPlayer_handle_t;

speechPlayer_handle_t speechPlayer_initialize(int sampleRate);
void speechPlayer_queueFrame(speechPlayer_handle_t playerHandle, speechPlayer_frame_t* framePtr, unsigned int minFrameDuration, unsigned int fadeDuration, int userIndex, bool purgeQueue);
int speechPlayer_synthesize(speechPlayer_handle_t playerHandle, unsigned int sampleCount, sample* sampleBuf); 
int speechPlayer_getLastIndex(speechPlayer_handle_t playerHandle);
void speechPlayer_terminate(speechPlayer_handle_t playerHandle);

#ifdef __cplusplus
}
#endif

#endif
