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

#include "frame.h"
#include "speechWaveGenerator.h"
#include "speechPlayer.h"

typedef struct {
	int sampleRate;
	FrameManager* frameManager;
	SpeechWaveGenerator* waveGenerator;
} speechPlayer_handleInfo_t;

speechPlayer_handle_t speechPlayer_initialize(int sampleRate) {
	speechPlayer_handleInfo_t* playerHandleInfo=new speechPlayer_handleInfo_t;
	playerHandleInfo->sampleRate=sampleRate;
	playerHandleInfo->frameManager=FrameManager::create();
	playerHandleInfo->waveGenerator=SpeechWaveGenerator::create(sampleRate);
	playerHandleInfo->waveGenerator->setFrameManager(playerHandleInfo->frameManager);
	return (speechPlayer_handle_t)playerHandleInfo;
}

void speechPlayer_queueFrame(speechPlayer_handle_t playerHandle, speechPlayer_frame_t* framePtr, unsigned int minFrameDuration, unsigned int fadeDuration, int userIndex, bool purgeQueue) { 
	speechPlayer_handleInfo_t* playerHandleInfo=(speechPlayer_handleInfo_t*)playerHandle;
	if (fadeDuration < 1) fadeDuration = 1;
	playerHandleInfo->frameManager->queueFrame(framePtr,minFrameDuration,fadeDuration,userIndex,purgeQueue);
}

int speechPlayer_synthesize(speechPlayer_handle_t playerHandle, unsigned int sampleCount, sample* sampleBuf) {
	return ((speechPlayer_handleInfo_t*)playerHandle)->waveGenerator->generate(sampleCount,sampleBuf);
}

int speechPlayer_getLastIndex(speechPlayer_handle_t playerHandle) {
	speechPlayer_handleInfo_t* playerHandleInfo=(speechPlayer_handleInfo_t*)playerHandle;
	return playerHandleInfo->frameManager->getLastIndex();
}

void speechPlayer_terminate(speechPlayer_handle_t playerHandle) {
	speechPlayer_handleInfo_t* playerHandleInfo=(speechPlayer_handleInfo_t*)playerHandle;
	delete playerHandleInfo->waveGenerator;
	delete playerHandleInfo->frameManager;
	delete playerHandleInfo;
}
  