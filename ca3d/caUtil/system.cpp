// system.cpp

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

#include "paths.h"
#include "system.h"
#include "calog.h"
#include "rendering.h"
#include "textures.h"

bool g_bGameRunning;

typedef struct
{
	int ctrlButton;
	int ctrlAction;
} ctrlMap_t;

void checkKeyReleases(float fElapsedTime);

void (*customShutdownFct)(int code);

unsigned int systemTextureId[SYSTEM_TEXTURE_COUNT];

void loadSystemTextures()
{
	char szTexture[256];

	sprintf(szTexture, "%s/%s", SYSTEM_PATH, "gun_fire.bmp");
	systemTextureId[SYSTEM_TEXTURE_WEAPONFLASH] = ::loadTexture(szTexture, false, 4444, false, 0, 0, mapTexture_t::ALIGN_LOCAL);

	sprintf(szTexture, "%s/%s", SYSTEM_PATH, "particle.bmp");
	systemTextureId[SYSTEM_TEXTURE_PARTICLE] = ::loadTexture(szTexture, false, 4444, false, 0, 0, mapTexture_t::ALIGN_LOCAL);
}

void unloadSystemTextures()
{
	::unloadTexture(&systemTextureId[SYSTEM_TEXTURE_PARTICLE]);
	::unloadTexture(&systemTextureId[SYSTEM_TEXTURE_WEAPONFLASH]);
}

void Shutdown(int code)
{
	if (customShutdownFct) customShutdownFct(code);

	unloadSystemTextures();

	exitSystem(0);
}

#if __APPLE_CC__ | _WIN32

time_t getCurrentTime()
{
	return time(NULL);
}

int screen_width, screen_height;

#ifndef _NO_SDL
ctrlMap_t ctrlMap[] =
{
	{ SDLK_q,			ACTION_JUMP},
	{ SDLK_e,			ACTION_SHOOT},

	{ SDLK_w,			ACTION_WALK_FORWARD },
	{ SDLK_s,			ACTION_WALK_BACKWARD },
	{ SDLK_a,			ACTION_STRAFE_LEFT },
	{ SDLK_d,			ACTION_STRAFE_RIGHT },

	{ SDLK_UP,			ACTION_ROTATE_UP },
	{ SDLK_DOWN,		ACTION_ROTATE_DOWN },
	{ SDLK_LEFT,		ACTION_ROTATE_LEFT },
	{ SDLK_RIGHT,		ACTION_ROTATE_RIGHT },

	{ SDLK_RETURN,		NO_ACTION}
};
#endif

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void readInput(float fElapsedTime, bool vsync)
{
#ifndef _NO_SDL
	reshape(screen_width, screen_height); // sonst probleme bei fenster min/max !?

	Uint8 *keys;
	keys = SDL_GetKeyState(NULL);

	for (int i=0; i<ANZ_HARDWARE_BUTTONS; i++)
	{
		if (keys[ctrlMap[i].ctrlButton] == SDL_PRESSED)
		{
			if (ctrlMap[i].ctrlAction != NO_ACTION)
			{
				actionKeyPressed[ctrlMap[i].ctrlAction] = 127;
			}
			hardwareKeyPressed[i] = 127;
		}
		else
		{
			if (ctrlMap[i].ctrlAction != NO_ACTION)
			{
				actionKeyPressed[ctrlMap[i].ctrlAction] = 0;
			}
			hardwareKeyPressed[i] = 0;
		}

	}

	if ( keys[SDLK_ESCAPE] == SDL_PRESSED )
	{
		Shutdown(0);
	}

	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_QUIT:
				Shutdown(0);
				break;
			case SDL_VIDEORESIZE:
				reshape(event.resize.w, event.resize.h);
				break;
		}
	}

	checkKeyReleases(fElapsedTime);
#endif
}

#ifdef _WIN32
	#include "win32/HiResTimer.h"
	CHiResTimer timer;
#endif

void setVideoMode(int bpp, bool bFS)
{
#ifndef _NO_SDL
	int Videoflags;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (bFS)
	{
		Videoflags = SDL_OPENGL | SDL_FULLSCREEN;
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		Videoflags = SDL_OPENGL /* | SDL_RESIZABLE */;
		SDL_ShowCursor(SDL_ENABLE);
	}

	if( SDL_SetVideoMode(screen_width, screen_height, bpp, Videoflags) == 0 )
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		exitSystem(1);
	}
	reshape(screen_width, screen_height);
#endif
}

void setupPCWindow(bool bFS, char *szWindowText)
{
#ifndef _NO_SDL
	int bpp = 0;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exitSystem(1);
	}
	const SDL_VideoInfo* info = NULL;
	info = SDL_GetVideoInfo( );
	if (!info)
	{
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		exitSystem(1);
	}
	bpp = info->vfmt->BitsPerPixel;
	SDL_WM_SetCaption(szWindowText, "");
	setVideoMode(bpp, bFS);
#endif
}

void initSystem(int iScreenWidth, int iScreenHeight, bool bFullscreen, char *szWindowText)
{
	screen_width	= iScreenWidth;
	screen_height	= iScreenHeight;

	setupPCWindow(bFullscreen, szWindowText);
	
	#ifdef _WIN32
		if (GLEW_OK != glewInit()) writeLog("Problems with glewInit");
		timer.Init();
	#endif

	initGraphicsUnit();

	loadSystemTextures();

	g_bGameRunning = true;
}

void exitSystem(int code)
{
#ifndef _NO_SDL
	SDL_Quit();
#endif
	exit(code);
}

float getElapsedTime()
{
	#ifdef _WIN32
		float fElapsedTime = timer.GetElapsedSeconds();
	#else
	
#ifndef _NO_SDL	
		static int iInitTime = SDL_GetTicks();
		int iSDLMS = SDL_GetTicks();
		float fElapsedTime = ((iSDLMS-iInitTime)/1000.0f);
		iInitTime = iSDLMS;
#else
		float fElapsedTime = 0.05f;
#endif

	#endif
	
	if (fElapsedTime > 0.05f) fElapsedTime = 0.05f;
	return fElapsedTime;
}

#elif _PSP

/*
ctrlMap_t ctrlMapAnalog[] =
{
	{ 0, NO_ACTION },
	{ 1, NO_ACTION },
	{ 2, NO_ACTION },
	{ 3, NO_ACTION }
};
*/

ctrlMap_t ctrlMapAnalog[] =
{
	{ 0, ACTION_ROTATE_UP },
	{ 1, ACTION_ROTATE_DOWN },
	{ 2, ACTION_ROTATE_LEFT },
	{ 3, ACTION_ROTATE_RIGHT }
};

clock_t frameticks, currentticks, startticks;

PSP_MODULE_INFO("CA_MOD_ASDFGHJKL", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

/*
ctrlMap_t ctrlMap[] =
{
	{ PSP_CTRL_LTRIGGER,	NO_ACTION},
	{ PSP_CTRL_RTRIGGER,	ACTION_CHANGE_WEAPON},

	{ PSP_CTRL_UP,		    ACTION_SHOOT },
	{ PSP_CTRL_DOWN,		NO_ACTION },
	{ PSP_CTRL_LEFT,		ACTION_STRAFE_LEFT },
	{ PSP_CTRL_RIGHT,		ACTION_STRAFE_RIGHT },

	{ PSP_CTRL_TRIANGLE,	ACTION_WALK_FORWARD },
	{ PSP_CTRL_CROSS,		ACTION_WALK_BACKWARD },
	{ PSP_CTRL_SQUARE,		ACTION_ROTATE_LEFT },
	{ PSP_CTRL_CIRCLE,		ACTION_ROTATE_RIGHT },

	{ PSP_CTRL_START,		NO_ACTION},
};
*/

ctrlMap_t ctrlMap[] =
{
	{ PSP_CTRL_LTRIGGER,	ACTION_JUMP},
	{ PSP_CTRL_RTRIGGER,	ACTION_SHOOT},

	{ PSP_CTRL_UP,		    ACTION_ROTATE_UP },
	{ PSP_CTRL_DOWN,		ACTION_ROTATE_DOWN },
	{ PSP_CTRL_LEFT,		ACTION_ROTATE_LEFT },
	{ PSP_CTRL_RIGHT,		ACTION_ROTATE_RIGHT },

	{ PSP_CTRL_TRIANGLE,	ACTION_WALK_FORWARD },
	{ PSP_CTRL_CROSS,		ACTION_WALK_BACKWARD },
	{ PSP_CTRL_SQUARE,		ACTION_STRAFE_LEFT },
	{ PSP_CTRL_CIRCLE,		ACTION_STRAFE_RIGHT },

	{ PSP_CTRL_START,		NO_ACTION},
};

SceCtrlData pad;

void readInput(float fElapsedTime, bool vsync)
{
	if (vsync)
		sceCtrlReadBufferPositive(&pad, 1);
	else
		sceCtrlPeekBufferPositive(&pad, 1);

	if (ctrlMapAnalog[0].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[0].ctrlAction] = 0;
	if (ctrlMapAnalog[1].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[1].ctrlAction] = 0;
	if (ctrlMapAnalog[2].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[2].ctrlAction] = 0;
	if (ctrlMapAnalog[3].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[3].ctrlAction] = 0;

	for (int i=0; i<ANZ_HARDWARE_BUTTONS; i++)
	{

		if (pad.Buttons & ctrlMap[i].ctrlButton)
		{
			if (ctrlMap[i].ctrlAction != NO_ACTION) 
				actionKeyPressed[ctrlMap[i].ctrlAction] = 127;
			hardwareKeyPressed[i] = 127;
		}
		else
		{
			if (ctrlMap[i].ctrlAction != NO_ACTION)
				actionKeyPressed[ctrlMap[i].ctrlAction] = 0;
			hardwareKeyPressed[i] = 0;
		}

	}

	int yVal = pad.Ly - 127, xVal = pad.Lx - 127;
		
	if		(yVal >=  125) yVal =  127;
	else if (yVal <= -125) yVal = -127;
	if		(xVal >=  125) xVal =  127;
	else if (xVal <= -125) xVal = -127;

	if		(yVal > analogYDeadzone)
	{
		if (ctrlMapAnalog[1].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[1].ctrlAction] =  (yVal*yVal*yVal/(127*127));
	}
	if (yVal < -analogYDeadzone)
	{
		if (ctrlMapAnalog[0].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[0].ctrlAction] = -(yVal*yVal*yVal/(127*127));
	}
	if (xVal > analogXDeadzone)
	{
		if (ctrlMapAnalog[3].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[3].ctrlAction] =  (xVal*xVal*xVal/(127*127));
	}
	if		(xVal < -analogXDeadzone)
	{
		if (ctrlMapAnalog[2].ctrlAction != NO_ACTION) actionKeyPressed[ctrlMapAnalog[2].ctrlAction] = -(xVal*xVal*xVal/(127*127));
	}

	checkKeyReleases(fElapsedTime);
}

int exit_callback(int arg1, int arg2, void *common)
{
	g_bGameRunning = false;
//	sceKernelExitGame();
	return 0;
}

int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

void setupControl()
{
	analogXDeadzone = analogYDeadzone = 14;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

time_t getCurrentTime()
{
	return sceKernelLibcTime(NULL);
}

void initTimezone() { 
   int tzOffset = 0; 
   int dst = 0; 
   sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_TIMEZONE, &tzOffset); 
   sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_DAYLIGHTSAVINGS, &dst); 

   int tzOffsetAbs = tzOffset < 0 ? -tzOffset : tzOffset; 
   int hours = tzOffsetAbs / 60; 

   int minutes = tzOffsetAbs - hours * 60; 
   char *tz; 
   if (dst == 1) {
      tz = (char*)malloc(14*sizeof(char)); 
      sprintf(tz, "GMT%s%02i:%02i DST", tzOffset < 0 ? "+" : "-", hours, minutes); 
   } else { 
      tz = (char*)malloc(10*sizeof(char)); 
      sprintf(tz, "GMT%s%02i:%02i", tzOffset < 0 ? "+" : "-", hours, minutes); 
   } 
    
   setenv("TZ", tz, 1); 
   tzset(); 
} 

void initSystem(int iScreenWidth, int iScreenHeight, bool bFullscreen, char *szWindowText)
{
	initTimezone();
	
    scePowerSetClockFrequency(333, 333, 166);

	setupCallbacks();
	setupControl();
	startticks = clock();

	initGraphicsUnit();

	loadSystemTextures();

	g_bGameRunning = true;
}

void exitSystem(int code)
{
	sceKernelExitGame();
}

float getElapsedTime()
{
	currentticks = clock();
	frameticks = (currentticks - startticks);
	float fElapsedTime = float(frameticks/(float)CLOCKS_PER_SEC);
	startticks = currentticks;
	if (fElapsedTime > 0.05f) fElapsedTime = 0.05f;
	return fElapsedTime;
}

#endif

int getFramesPerSec(float fElapsedTime)
{
	static int fps = 0;
	static int frames = 0;
	static float time = 0;

	frames++;
	time += fElapsedTime;

	if (time >= 1)
	{
		fps		= frames;
		time	= 0.0f;
		frames	= 0;
	}
	return fps;
}

float autoReleaseTime = 0.25f;

char ctrlActionNames[ANZ_ACTIONS][30] =
{ 
	"NO_ACTION",
	"WALK_FORWARD",
	"WALK_BACKWARD",
	"ROTATE_UP",
	"ROTATE_DOWN",
	"ROTATE_LEFT",
	"ROTATE_RIGHT",
	"STRAFE_LEFT",
	"STRAFE_RIGHT",
	"SHOOT",
	"CHANGE_WEAPON",
	"JUMP"
};

int analogXDeadzone, analogYDeadzone;

int actionKeyPressed[ANZ_ACTIONS];

int		hardwareKeyPressed[ANZ_HARDWARE_BUTTONS];
int		hardwareKeyReleased[ANZ_HARDWARE_BUTTONS];
float	hardwareKeyTime[ANZ_HARDWARE_BUTTONS];

void checkKeyReleases(float fElapsedTime)
{
	for (int i=0; i<ANZ_HARDWARE_BUTTONS; i++)
	{

		if (hardwareKeyTime[i] > 0)
			hardwareKeyTime[i] -= fElapsedTime;
		else if (hardwareKeyTime[i] < 0)
			hardwareKeyTime[i] = 0;

		if (!hardwareKeyPressed[i] || (hardwareKeyTime[i] == 0 && i != BUTTON_START)) hardwareKeyReleased[i]=1;
	}
}

void invalidateKeyReleases()
{
	for (int i=0; i<ANZ_HARDWARE_BUTTONS; i++)
	{
		hardwareKeyReleased[i] = 0;
		hardwareKeyTime[i]     = autoReleaseTime;
	}
}

bool hardwareKeyOncePressed(hardwareKey key)
{
	bool bOK = (hardwareKeyReleased[key] && hardwareKeyPressed[key]);
	if (bOK) invalidateKeyReleases();
	return bOK;
}
