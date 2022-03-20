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

#ifndef SPEECHPLAYERSPEECHWAVEGENERATOR_H
#define SPEECHPLAYERSPEECHWAVEGENERATOR_H

#include "frame.h"
#include "waveGenerator.h"

class SpeechWaveGenerator: public WaveGenerator {
	public:
	static SpeechWaveGenerator* create(int sampleRate); 
	virtual void setFrameManager(FrameManager* frameManager)=0;
	virtual ~SpeechWaveGenerator() {};
};

#endif
