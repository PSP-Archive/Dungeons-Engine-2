// moveinfo.cpp

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

#include "moveinfo.h"
#include "system.h"
#include <iostream>

using namespace std;

void moveinfo_t::init(const AABB_t &localAABBStanding, const orientation_t &standingOrientation)
{
	float fDistanceEyeFromTop = 0.15f; // 15 cm von Augen zu Stirnhöhe

	bOnGround				= true;
	bPushing				= false;
	fPushHeightEye			= 0.0f;
	fYVel					= 0.0f;
	vXZVelocityWS			= Vector4(0,0,0);
	bLimitXZVelocityWS		= true;

	float fHeightStanding	= localAABBStanding.max.y;

	localAABBEye			= localAABBStanding;
	localAABBEye.max.y		= fDistanceEyeFromTop;
	localAABBEye.min.y		= fDistanceEyeFromTop-fHeightStanding;

	float fEyeHeight		= getEyeHeight();

	cam.reset();
	cam.setPosition(standingOrientation.position + Vector4(0, fEyeHeight, 0));
	cam.updateView(standingOrientation.yaw, standingOrientation.pitch, 0, 0);
}

float moveinfo_t::getEyeHeight()
{
	return (-localAABBEye.min.y);
}

float moveinfo_t::makeUpMovement(const Vector4 &vFrameStartPosEye, float fElapsedTime, bool bJump)
{
	if (bJump) fYVel = 4;

	fYVel -= 9.81 * fElapsedTime;

	float fLocalUpMovement = fYVel * fElapsedTime;

	if (bPushing)
	{
		if (fYVel < 0) fYVel = 0;
		fLocalUpMovement = 4 * fElapsedTime;
	}

	// fLocalUpMovement maximal bis PushHeight
	if (bPushing && ((vFrameStartPosEye.y + fLocalUpMovement) > fPushHeightEye) )
		fLocalUpMovement = (fPushHeightEye - vFrameStartPosEye.y);

	// jumping ggf. dazuaddieren
	if (bPushing)
		fLocalUpMovement += (fYVel * fElapsedTime);

	return fLocalUpMovement;
}

void vectorMakeNewLength(Vector4 *pVec, float fLen, float fMin, float fMax)
{
	if (fLen < fMin)
		fLen = fMin;
	else if (fLen > fMax)
		fLen = fMax;

	pVec->normalize();

	*pVec = *pVec * fLen;
}

void moveinfo_t::move(float fElapsedTime, float fForward, float fStrafe, float fYaw, float fPitch, bool bJump)
{
	// fForward, fStrafe, fYaw, fPitch kann nur -1, 0, +1 sein

	Vector4 vFrameStartPosEye = cam.getPosition();

	float fFrameVeloAdd = g_fAcceleration*fElapsedTime; // adding each second 50 m/s ( a = 50m/(s*s) )
	float fRotation = g_fRotationSpeed*fElapsedTime;
	float fFrameForward, fFrameStrafe, fFrameYaw, fFramePitch;

	bOnGroundBeforeMove	= bOnGround;
	bLanding			= (fYVel < -2);

	fFrameForward	= fForward	* fFrameVeloAdd;
	fFrameStrafe	= fStrafe	* fFrameVeloAdd;
	fFrameYaw		= fYaw		* fRotation;
	fFramePitch		= fPitch	* fRotation;

	if ( fFrameForward != 0 && fFrameStrafe != 0 )
	{
		fFrameForward *= 0.7071;
		fFrameStrafe  *= 0.7071;
	}
	
	vXZVelocityWS = vXZVelocityWS + (cam.getForwardMove() * fFrameForward) + (cam.getRightMove() * fFrameStrafe);
	
	float fFrameDeaccel = g_fDecelleration*fElapsedTime;

	float lenMoveVec	= vecLength(vXZVelocityWS);
	
	if (bOnGround)
	{
		lenMoveVec -= fFrameDeaccel;
		bOnGround = false;
	}
	
	if (bLimitXZVelocityWS)
		vectorMakeNewLength(&vXZVelocityWS, lenMoveVec, 0, g_fMaxVelocity);
	else
		vectorMakeNewLength(&vXZVelocityWS, lenMoveVec, 0, 999999);

	Vector4 vOS = vXZVelocityWS;
	
	// velocity in lokalen Koordinaten, forward = -z
	vOS = vOS.getRotatedY(-cam.getYaw()) * fElapsedTime;
	
	float fLocalForwardMovement	= -vOS.z;
	float fLocalStrafeMovement	=  vOS.x;
	float fLocalUpMovement		= makeUpMovement(vFrameStartPosEye, fElapsedTime, bJump);
	
	cam.updateView( fFrameYaw, fFramePitch, fLocalForwardMovement, fLocalStrafeMovement, fLocalUpMovement, true, fElapsedTime );
}
