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

#ifndef SPEECHPLAYERWAVEGENERATOR_H
#define SPEECHPLAYERWAVEGENERATOR_H

#include <list>
#include "sample.h"
#include "speechPlayer.h"

class WaveGenerator {
	public:
	virtual unsigned int generate(const unsigned int bufSize, sample* buffer)=0;
};

#endif
