// ai.h

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

#ifndef CA_AI_H
#define CA_AI_H

#include "../caUtil/caUtil.h"

struct targetInfo_t
{
	Vector4			vShooterEyepos;
	Vector4			vShooterDir;		// must be normalized
	float			fClosestWall;
	float			fClosestEnemy;
	unsigned int	iClosestEnemyNr;

	targetInfo_t() {fClosestWall=999999.0f; fClosestEnemy=999999.0f; iClosestEnemyNr=-1;};
};

bool findEnemyInTarget(targetInfo_t *targetInfo);

namespace ca_ai
{
	extern bool bGodmode;

	void init();
	void handle(float fElapsedTime);
	void startGlobalAlarm();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// new maparea -> clear / init all projectiles

enum shooter_e
{
	SHOOTER_PLAYER,
	SHOOTER_ENEMY
};

class CProjectile
{
public:
	bool		bActive;
	shooter_e	shooterType;
	int			shooterNumber;
	Vector4		vPos;
	Vector4		vDirNorm;
	float		fWallhitDistance;
	float		fMovedDistance;
private:

};

namespace ca_projectiles
{
	void init();
	void startNewProjectile(shooter_e shooterType, int shooterNumber, Vector4 vStart, Vector4 vDirNorm);
	void advance(float fElapsedTime);
	void render(const CCamera &cam);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif
