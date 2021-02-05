// collision_detection.h

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

#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "../de2mapformat/de2mapformat.h"

extern OAnimatedMD3 player;

void moveEntity(moveinfo_t *moveInfo, float fElapsedTime, float fForward, float fStrafe, float fYaw, float fPitch, bool bJump, int *piNewArea, bool *pbLevelExitCollision, bool bAllowMapAreaExit, bool bAllowStepSound);

#endif
