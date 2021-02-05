// plattform_includes.h

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

#ifndef CA_PLATTFORM_INCLUDES_H
#define CA_PLATTFORM_INCLUDES_H

#if _WIN32

#ifndef _NO_SDL
	#include "SDL.h"
#endif

	#include <windows.h>
	#include <gl/glew.h>
	#include <gl/gl.h>
	#include <gl/glu.h>

#ifndef _NO_OPENAL	
	#include <al/al.h>
	#include <al/alc.h>
	#include <al/alut.h>
	#include "audio/caAudio.h"
#endif

	#include <malloc.h>
	#include <ctime>

#ifndef _NO_SDL
	#pragma comment (lib, "SDL.lib")
	#pragma comment (lib, "SDLmain.lib")
#endif
	
	#pragma comment (lib, "opengl32.lib")
	#pragma comment (lib, "glew32.lib")
	#pragma comment (lib, "glu32.lib")
	#pragma comment (lib, "alut.lib")
	#pragma comment (lib, "openal32.lib")

	#define ALIGN_MEMORY

#elif __APPLE_CC__

#ifndef _NO_SDL
	#include "SDL.h"
#endif

	#include <GLUT/glut.h>
	
#ifndef _NO_OPENAL	
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
	#include "alut.h"
	#include "audio/caAudio.h"
#endif

	#include <ctime>

	#define ALIGN_MEMORY

#elif _PSP

	#include <pspgu.h>
	#include <pspgum.h>
	#include <pspkernel.h>
	#include <pspdisplay.h>
	#include <pspdebug.h>
	#include <psppower.h>
	#include <pspctrl.h>
	#include <pspaudiolib.h>
	#include <pspaudio.h>
	#include <malloc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <limits.h>
	#include <psputility_sysparam.h>
	#include <ctime>
	#include "audio/caAudio.h"

	#define ALIGN_MEMORY __attribute__((aligned(64)))

	#define BUF_WIDTH  (512)
	#define SCR_WIDTH  (480)
	#define SCR_HEIGHT (272)
	#define PIXEL_SIZE (4)
	#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
	#define ZBUF_SIZE  (BUF_WIDTH * SCR_HEIGHT * 2)

#endif

	#include <iostream>
	using namespace std;

#endif
