// shooting.h

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

#ifndef SHOOTING_H
#define SHOOTING_H

#include "../caUtil/caUtil.h"

namespace shooting
{
	enum
	{
		TEXTURE_CROSSHAIRS,
		SHOOTING_TEXTURE_COUNT
	};

	extern unsigned int textureId[];

	extern int iEnemyKills;

	void loadTextures();
	void unloadTextures();
	void handlePlayerWeapon(float fElapsedTime, bool bAllowShooting = true);
	void renderPlayerWeapon();
	void reset();
};

#endif
