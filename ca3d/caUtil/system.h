// system.h

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

#ifndef CA_SYSTEM_H
#define CA_SYSTEM_H

#include "plattform_includes.h"

enum
{
	SYSTEM_TEXTURE_WEAPONFLASH,
	SYSTEM_TEXTURE_PARTICLE,
	SYSTEM_TEXTURE_COUNT
};

extern unsigned int systemTextureId[];

void loadSystemTextures();

void initSystem(int iScreenWidth, int iScreenHeight, bool bFullscreen, char *szWindowText);
void exitSystem(int code);
float getElapsedTime();
void readInput(float fElapsedTime, bool vsync = false);

int getFramesPerSec(float fElapsedTime);

time_t getCurrentTime();

extern bool g_bGameRunning;
void Shutdown(int code);

extern void (*customShutdownFct)(int code);

extern int hardwareKeyPressed[];
extern int actionKeyPressed[];

extern int analogXDeadzone, analogYDeadzone;

enum hardwareKey
{
	LEFT_TRIGGER,
	RIGHT_TRIGGER,
	DIGITAL_PAD_UP,
	DIGITAL_PAD_DOWN,
	DIGITAL_PAD_LEFT,
	DIGITAL_PAD_RIGHT,
	BUTTON_TRIANGLE,
	BUTTON_CROSS,
	BUTTON_SQUARE,
	BUTTON_CIRCLE,
	BUTTON_START,
	ANZ_HARDWARE_BUTTONS
};

bool hardwareKeyOncePressed(hardwareKey key);





// TODO: Actions + Names müssen vom Prog. gesetzt werden, nicht von der Util-Klasse

enum ctrlAction
{
	NO_ACTION,
	ACTION_WALK_FORWARD,
	ACTION_WALK_BACKWARD,
	ACTION_ROTATE_UP,
	ACTION_ROTATE_DOWN,
	ACTION_ROTATE_LEFT,
	ACTION_ROTATE_RIGHT,
	ACTION_STRAFE_LEFT,
	ACTION_STRAFE_RIGHT,
	ACTION_SHOOT,
	ACTION_CHANGE_WEAPON,
	ACTION_JUMP,
	ANZ_ACTIONS
};

extern char ctrlActionNames[][30];





#endif
