// camera.h

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

#ifndef CA_CAMERA_H
#define CA_CAMERA_H

#include "vectorMath.h"

class CCamera
{
public:
	CCamera();

	void reset();
	void setPosition(const Vector4 &position) { pos=position; };
	void updateView(float fyaw, float fpitch, float fforward, float fstrafe, float fup=0, bool animate=false, float fElapsedTime=0);
	void setMoveDistance(float fdist, bool col) { m_col = col; m_fdist = fdist; };
	
	Vector4 getPosition() const		{ return pos; };
	Vector4 getForwardLook() const	{ return forward; };
	Vector4 getRightLook() const	{ return right; };
	Vector4 getUpLook() const		{ return up; };

	Vector4 getForwardMove() const	{ return forward_move; };
	Vector4 getRightMove() const	{ return right_move; };
	Vector4 getUpMove() const		{ return up_move; };
	float getYaw() const			{ return yaw; };
	float getPitch() const			{ return pitch; };
	float getRoll() const			{ return roll; };
	float getStraferoll() const		{ return straferoll; };
	float getWippUpDown() const		{ return wipp_up_down; };

	bool getPlaySound() const { return play_sound; };

	void setYaw( float fYaw )		{ yaw = fYaw; };
	void setPitch( float fPitch )	{ pitch = fPitch; };

private:
	float yaw;
	float pitch;
	float roll;
	float straferoll;
	float wipp_up_down;
	bool b_roll_sign;
	float move_time;
	bool soundok;
	bool play_sound;
	float mov_forward;
	float mov_strafe;
	Vector4 right;
	Vector4 right_move;
	Vector4 up;
	Vector4 up_move;
	Vector4 forward;
	Vector4 forward_move;
	Vector4 pos;
	void updateOrientation(bool animate=false);
	void rotate(float fyaw, float fpitch, bool animate);
	void move(float fforward, float fstrafe, float fup);
	void updateKippWipp(float fElapsedTime);
	void updateStraferoll(float fElapsedTime);
	float m_fdist;
	bool m_col;
};

#endif
