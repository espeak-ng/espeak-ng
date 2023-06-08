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

#include <cstring>
#include <queue>
#include "utils.h"
#include "frame.h"

using namespace std;

struct frameRequest_t {
	unsigned int minNumSamples;
	unsigned int numFadeSamples;
	bool NULLFrame;
	speechPlayer_frame_t frame;
	double voicePitchInc; 
	int userIndex;
};

class FrameManagerImpl: public FrameManager {
	private:
	queue<frameRequest_t*> frameRequestQueue;
	frameRequest_t* oldFrameRequest;
	frameRequest_t* newFrameRequest;
	speechPlayer_frame_t curFrame;
	bool curFrameIsNULL;
	unsigned int sampleCounter;
	int lastUserIndex;

	void updateCurrentFrame() {
		sampleCounter++;
		if(newFrameRequest) {
			if(sampleCounter>(newFrameRequest->numFadeSamples)) {
				delete oldFrameRequest;
				oldFrameRequest=newFrameRequest;
				newFrameRequest=NULL;
			} else {
				double curFadeRatio=(double)sampleCounter/(newFrameRequest->numFadeSamples);
				for(int i=0;i<speechPlayer_frame_numParams;++i) {
					((speechPlayer_frameParam_t*)&curFrame)[i]=calculateValueAtFadePosition(((speechPlayer_frameParam_t*)&(oldFrameRequest->frame))[i],((speechPlayer_frameParam_t*)&(newFrameRequest->frame))[i],curFadeRatio);
				}
			}
		} else if(sampleCounter>(oldFrameRequest->minNumSamples)) {
			if(!frameRequestQueue.empty()) {
				curFrameIsNULL=false;
				newFrameRequest=frameRequestQueue.front();
				frameRequestQueue.pop();
				if(newFrameRequest->NULLFrame) {
					memcpy(&(newFrameRequest->frame),&(oldFrameRequest->frame),sizeof(speechPlayer_frame_t));
					newFrameRequest->frame.preFormantGain=0;
					newFrameRequest->frame.voicePitch=curFrame.voicePitch;
					newFrameRequest->voicePitchInc=0;
				} else if(oldFrameRequest->NULLFrame) {
					memcpy(&(oldFrameRequest->frame),&(newFrameRequest->frame),sizeof(speechPlayer_frame_t));
					oldFrameRequest->frame.preFormantGain=0;
				}
				if(newFrameRequest) {
					if(newFrameRequest->userIndex!=-1) lastUserIndex=newFrameRequest->userIndex;
					sampleCounter=0;
					newFrameRequest->frame.voicePitch+=(newFrameRequest->voicePitchInc*newFrameRequest->numFadeSamples);
				}
			} else {
				curFrameIsNULL=true;
			}
		} else {
			curFrame.voicePitch+=oldFrameRequest->voicePitchInc;
			oldFrameRequest->frame.voicePitch=curFrame.voicePitch;
		}
	}


	public:

	FrameManagerImpl(): newFrameRequest(NULL), curFrame(), curFrameIsNULL(true), sampleCounter(0), lastUserIndex(-1)  {
		oldFrameRequest=new frameRequest_t();
		oldFrameRequest->NULLFrame=true;
	}

	void queueFrame(speechPlayer_frame_t* frame, unsigned int minNumSamples, unsigned int numFadeSamples, int userIndex, bool purgeQueue) {
		frameRequest_t* frameRequest=new frameRequest_t;
		frameRequest->minNumSamples=minNumSamples; //max(minNumSamples,1);
		frameRequest->numFadeSamples=numFadeSamples; //max(numFadeSamples,1);
		if(frame) {
			frameRequest->NULLFrame=false;
			memcpy(&(frameRequest->frame),frame,sizeof(speechPlayer_frame_t));
			frameRequest->voicePitchInc=(frame->endVoicePitch-frame->voicePitch)/frameRequest->minNumSamples;
		} else {
			frameRequest->NULLFrame=true;
		}
		frameRequest->userIndex=userIndex;
		if(purgeQueue) {
			for(;!frameRequestQueue.empty();frameRequestQueue.pop()) delete frameRequestQueue.front();
			sampleCounter=oldFrameRequest->minNumSamples;
			if(newFrameRequest) {
				oldFrameRequest->NULLFrame=newFrameRequest->NULLFrame;
				memcpy(&(oldFrameRequest->frame),&curFrame,sizeof(speechPlayer_frame_t));
				delete newFrameRequest;
				newFrameRequest=NULL;
			}
		}
		frameRequestQueue.push(frameRequest);
	}

	const int getLastIndex() {
		return lastUserIndex;
	}

	const speechPlayer_frame_t* const getCurrentFrame() {
		updateCurrentFrame();
		return curFrameIsNULL?NULL:&curFrame;
	}

	~FrameManagerImpl() {
		if(oldFrameRequest) delete oldFrameRequest;
		if(newFrameRequest) delete newFrameRequest;
	}

};

FrameManager* FrameManager::create() { return new FrameManagerImpl(); }
