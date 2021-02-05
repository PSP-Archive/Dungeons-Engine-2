// camera.cpp

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

#include "camera.h"

CCamera::CCamera()
{
	reset();
}

void CCamera::reset()
{
	Vector4 right_start(1,0,0), up_start(0,1,0), forward_start(0,0,-1), pos_start(0,0,0);

	yaw				= 0;
	pitch			= 0;
	roll			= 0;
	straferoll		= 0;
	m_fdist			= 0;
	move_time		= 0;
	wipp_up_down	= 0;
	soundok			= true;
	play_sound		= false;

	right	= right_move	= right_start;
	up		= up_move		= up_start;
	forward	= forward_move	= forward_start;
	pos						= pos_start;
}

void CCamera::updateOrientation(bool animate)
{
	// aktuelle Orientierung neu berechnen
	Vector4 right_start(1,0,0), up_start(0,1,0), forward_start(0,0,-1);

	// right_move vektor
	right_move = right_start.getRotatedY(yaw);

	// forward vektor (für Blickrichtung, mit Berücksichtigung auf/ab)
	forward = forward_start.getRotatedX(pitch);
	forward = forward.getRotatedY(yaw);

	// forward_move vektor (für Bewegung, ohne Berücksichtigung auf/ab)
	forward_move = forward_start.getRotatedY(yaw);
	
	// Links/rechts kippen, auf/ab wippen
	if (animate)
	{
		right      = right_move.getRotatedAxis(roll+straferoll, forward_move);
		forward.y += wipp_up_down;
	}
	else
	{
		right      = right_move;
	}

	// up vektor
	up = crossProduct(right,forward);
}

void CCamera::updateKippWipp(float fElapsedTime)
{
	if ((mov_forward == 0 && mov_strafe == 0) /*|| (m_fdist == 0)*/ )
	{
		move_time	= 0;
		roll		= 0;
		soundok		= true;

		float k = 0.05f;

		if (wipp_up_down>0) wipp_up_down = MAX(0,wipp_up_down-fElapsedTime*k);
		if (wipp_up_down<0) wipp_up_down = MIN(0,wipp_up_down+fElapsedTime*k);
	}
	else
	{
		Vector4 up_start(0,1,0);

		if (move_time == 0) (b_roll_sign = mov_strafe > 0 ? true : false);

 		move_time += fElapsedTime * 8;
		roll = sinfCa(move_time) * 0.4f;

		if (!b_roll_sign) roll = -roll;

		float d = sinfCa(move_time) * 0.008f;

		if (d > 0) d = -d;

		wipp_up_down = up_start.y * d;

		if (d < -0.004 && soundok)
		{
			soundok		= false;
			play_sound	= true;
		}
		if (d > -0.004) soundok = true;
	}
}

void CCamera::move(float fforward, float fstrafe, float fup)
{
	pos = pos + forward_move*fforward;
	pos = pos + right_move*fstrafe;
	pos = pos + up_move*fup;

	mov_forward = fforward;
	mov_strafe  = fstrafe;
}

void CCamera::rotate(float fyaw, float fpitch, bool animate)
{
	float fGrenze = 90.0f; // 85.0f;

	yaw   += fyaw;
	pitch += fpitch;

	if (pitch >  fGrenze) pitch =  fGrenze;
	if (pitch < -fGrenze) pitch = -fGrenze;

	updateOrientation(animate);
}

void CCamera::updateStraferoll(float fElapsedTime)
{
	float fMax = 0.5f;
	float fChange = fElapsedTime * 20.0f;
	float fEpsi = 0.0001f;
	
	if ( m_col || (mov_strafe < fEpsi && mov_strafe > -fEpsi) || (mov_forward < fEpsi && mov_forward > -fEpsi) )
	{
		if (straferoll > 0)
		{
			straferoll -= fChange * 0.5f;
			if (straferoll < 0)
				straferoll = 0;
		}
		if (straferoll < 0)
		{
			straferoll += fChange * 0.5f;
			if (straferoll > 0)
				straferoll = 0;
		}
	}
	else
	{
		if (mov_strafe > 0)
		{
			straferoll += fChange;
			if (straferoll > fMax)
				straferoll = fMax;
		}
		else if (mov_strafe < 0)
		{
			straferoll -= fChange;
			if (straferoll < -fMax)
				straferoll = -fMax;
		}
	}
}

void CCamera::updateView(float fyaw, float fpitch, float fforward, float fstrafe, float fup, bool animate, float fElapsedTime)
{
	rotate(fyaw, fpitch, animate);
	move(fforward, fstrafe, fup);
	play_sound = false;
	updateStraferoll(fElapsedTime);
	updateKippWipp(fElapsedTime);
}
