// moveinfo.h

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

#ifndef MOVEINFO_H
#define MOVEINFO_H

#include "vectorMath.h"
#include "camera.h"

const float	g_fAcceleration			=  50.0f;
const float	g_fDecelleration		=  15.0f;
const float	g_fMaxVelocity			=   5.0f;
const float	g_fRotationSpeed		= 140.0f;
const float	g_fLegsRotationSpeed	= 140.0f * 1.5f;

struct moveinfo_t
{
	CCamera cam;
	
	bool bOnGround;
	bool bPushing;
	float fPushHeightEye;
	float fYVel;
	
	bool bOnGroundBeforeMove;	// temp
	bool bLanding;				// temp

	bool bLimitXZVelocityWS;

	Vector4 vXZVelocityWS;		// Geschwindigkeitsvektor in m/sec in Worldorientierung

	AABB_t localAABBEye;		// AABB relative to eye position
	
	void init(const AABB_t &localAABBStanding, const orientation_t &standingOrientation);

	void move(float fElapsedTime, float fForward, float fStrafe, float fYaw, float fPitch, bool bJump);

	float getEyeHeight();

private:
	float makeUpMovement(const Vector4 &vFrameStartPos, float fElapsedTime, bool bJump);
};

#endif
