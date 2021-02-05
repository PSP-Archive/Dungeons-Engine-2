// caUtil.h

/*
Dungeons Engine 2

Copyright (C) 2008 Christoph Arnold "charnold" (http://www.caweb.de / charnold@gmx.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CA_UTIL_H
#define CA_UTIL_H

#include "paths.h"
#include "system.h"
#include "memory.h"
#include "camera.h"
#include "plane.h"
#include "vectorMath.h"
#include "vertex.h"
#include "calog.h"
#include "textures.h"
#include "rendering.h"
#include "font.h"
#include "md3.h"
#include "particles.h"

#if __APPLE_CC__

#ifndef _NO_SDL
	#include "SDL.h" // muss bei Mac vor main includiert werden
#endif
	
#endif

#endif

/*

minimal prog:

#include "../caUtil/caUtil.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main(int argc, char** argv)
{
	float fElapsedTime;

	writeLog("minimal");

	initSystem(480, 272, false, "minimal");

	while (g_bGameRunning)
	{
		fElapsedTime = getElapsedTime();
		if (fElapsedTime > 0.0f)
		{
			readInput(fElapsedTime);
		}
	}
	Shutdown(0);

	return 0;
}

*/
