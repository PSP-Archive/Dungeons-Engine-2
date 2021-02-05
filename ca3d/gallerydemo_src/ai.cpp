// ai.cpp

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

#include "ai.h"

#include "../de2mapformat/de2mapformat.h"
#include "collision_detection.h"

#define PROJECTILE_MAX_RANGE 50.0f

bool findClosestWallhit(targetInfo_t *pTargetInfo)
{
	int iMapArea = iActMapArea;
	bool bHit = false;
	float rayLength = 10000.0f, t, u, v;

	pTargetInfo->fClosestWall = 999999.0f;

	for (unsigned int i=0; i<mapAreas[iMapArea].visibleQuads.size(); i++)
	{
		Quad_t *q = &mapAreas[iMapArea].visibleQuads[i];

		Vector4 vec[4];
		
		assignVector(q->verts[0], &vec[0]);
		assignVector(q->verts[1], &vec[1]);
		assignVector(q->verts[2], &vec[2]);
		assignVector(q->verts[3], &vec[3]);

		if ( rayTriangleIntersectDir(pTargetInfo->vShooterEyepos, pTargetInfo->vShooterDir, vec[0], vec[1], vec[2], &t, &u, &v) )
		{
			if (t <= rayLength)
			{
				bHit = true;

				if (t < pTargetInfo->fClosestWall)
				{
					pTargetInfo->fClosestWall = t;
				}
			}
		}

		if ( rayTriangleIntersectDir(pTargetInfo->vShooterEyepos, pTargetInfo->vShooterDir, vec[2], vec[3], vec[0], &t, &u, &v) )
		{
			if (t <= rayLength)
			{
				bHit = true;

				if (t < pTargetInfo->fClosestWall)
				{
					pTargetInfo->fClosestWall = t;
				}
			}
		}
	}

	return bHit;
}

bool findClosestEnemyhit(targetInfo_t *pTargetInfo)
{
	int iMapArea = iActMapArea;
	bool bHit = false;
	float rayLength = 10000.0f, tHit;

	pTargetInfo->fClosestEnemy = 999999.0f;

	for (unsigned int iEnemy=0; iEnemy<mapAreas[iMapArea].enemies.size(); iEnemy++)
	{
		OAnimatedMD3 *pEnemy = &mapAreas[iMapArea].enemies[iEnemy];

		if (!pEnemy->isAlive()) continue;

		if (pEnemy->checkHit(pTargetInfo->vShooterEyepos, pTargetInfo->vShooterDir, rayLength, &tHit))
		{
			bHit = true;

			if (tHit < pTargetInfo->fClosestEnemy)
			{
				pTargetInfo->iClosestEnemyNr	= iEnemy;
				pTargetInfo->fClosestEnemy		= tHit;
			}
		}
	}

	return bHit;
}

bool findEnemyInTarget(targetInfo_t *targetInfo)
{
	targetInfo->vShooterEyepos	= player.moveInfo.cam.getPosition();
	targetInfo->vShooterDir		= player.moveInfo.cam.getForwardLook();

	findClosestWallhit(targetInfo);
	bool bHitEnemy = findClosestEnemyhit(targetInfo);

	if (bHitEnemy && (targetInfo->fClosestEnemy < targetInfo->fClosestWall) && targetInfo->fClosestEnemy < PROJECTILE_MAX_RANGE)
	{
		return true;
	}
	else
	{
		targetInfo->iClosestEnemyNr = -1;
		return false;
	}
}

namespace ca_ai
{
	bool bGodmode;

	float fAllEnemiesEvadingRadius = 0.8f;

	float fReactionTime = 0.2f;	// in diesen Zeitabständen sehen die Gegner ihre Umgebung

	float fEnemyCosFovH = 0.2f; // 0.2 ~ cos(78), also FOV von etwa 156 Grad

	float fTargetReachedDistance = 0.3f;

	float fTimeToStopGlobalAlarm = 4.0f; // player not seen for this time -> stop global alarm

	bool bGlobalAlarm = false;

	float fStopGlobalAlarmTime;

	float fHandleTime;

	void startGlobalAlarm()
	{
		bGlobalAlarm = true;

		fStopGlobalAlarmTime = 0.0f;
	}

	void checkStopGlobalAlarm(float fElapsedTime)
	{
		fStopGlobalAlarmTime += (fElapsedTime);
		
		if (fStopGlobalAlarmTime >= fTimeToStopGlobalAlarm)
		{
			fStopGlobalAlarmTime = 0.0f;

			bGlobalAlarm = false;
		}
	}

	bool checkPlayerInSight(OAnimatedMD3 *enemy)
	{
		Vector4 vEyeposEnemy = enemy->moveInfo.cam.getPosition();
		
		Vector4 vEnemyToPlayer = (-vEyeposEnemy + player.moveInfo.cam.getPosition());
		
		Vector4 vEnemyForward = enemy->moveInfo.cam.getForwardLook();

		// 1) Player inside enemie's fov?

		float dot = vEnemyToPlayer * vEnemyForward;
		
		if (dot > fEnemyCosFovH)
		{
			// 2) Fog blocking the sightvector?

			float fDistEnemyToPlayer = vecLength(vEnemyToPlayer);

			if (!mapAreas[iActMapArea].mapAreaInfo.iFog || (mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance > fDistEnemyToPlayer))
			{
				// 3) Player too far away?
				
				if (fDistEnemyToPlayer < PROJECTILE_MAX_RANGE)
				{
					// 4) Walls blocking the sightvector?

					targetInfo_t targetInfoEnemy;

					targetInfoEnemy.vShooterEyepos	= vEyeposEnemy;
					targetInfoEnemy.vShooterDir		= vEnemyToPlayer;
					targetInfoEnemy.vShooterDir.normalize();

					findClosestWallhit(&targetInfoEnemy);

					if (targetInfoEnemy.fClosestWall > fDistEnemyToPlayer)
					{
						// passed every check -> enemy sees player!

						return true;
					}
				}
			}
		}

		return false;
	}

	void decideToEvade(targetInfo_t *pTargetInfoPlayer)
	{
		// evading in 2D circle

		OAnimatedMD3 *enemy = &mapAreas[iActMapArea].enemies[pTargetInfoPlayer->iClosestEnemyNr];

		Sphere3D enemySphere;

		enemySphere.center		= enemy->getBeginStandingPosition();
		enemySphere.center.y	= 0.0f;
		enemySphere.radius		= fAllEnemiesEvadingRadius;

		Vector4 playerShootVecXZ = pTargetInfoPlayer->vShooterDir;
		playerShootVecXZ.y = 0.0f;
		
		Vector4 playerShootPosXZ = pTargetInfoPlayer->vShooterEyepos;
		playerShootPosXZ.y = 0.0f;

		Vector4 v2; // orthogonal zum shootvec
		v2.x = -playerShootVecXZ.z;
		v2.y = 0.0f;
		v2.z =  playerShootVecXZ.x;

        Vector4 pHit1, pHit2;
		float tHit1, tHit2;

		int iHits;

		Ray3D rayOrtho(enemySphere.center, v2);
		rayOrtho.dir_norm.normalize();

		iHits = raySphereIntersectNew(&rayOrtho, &enemySphere, &tHit1, &tHit2, &pHit1, &pHit2);

		Vector4 evadingTargetXZ = enemy->getStandingPosition();

		evadingTargetXZ.y = 0;

		if (iHits == 1)
		{
			evadingTargetXZ = pHit1;
		}
		else if (iHits == 2)
		{
			Ray3D rayShootXZ(playerShootPosXZ, playerShootVecXZ);
			rayShootXZ.dir_norm.normalize();
			Vector4 ip;

			rayrayIntersect2D(&rayShootXZ, &rayOrtho, &ip);

			float dist1 = vecDistanceSquared(ip, pHit1);
			float dist2 = vecDistanceSquared(ip, pHit2);

			if (dist1 > dist2)
				evadingTargetXZ = pHit1;
			else
				evadingTargetXZ = pHit2;
		}
		
		behaveParams_t params;
		
		params.pTargetXZ = &evadingTargetXZ;
		
		enemy->startState(BEHAVE_EVADING, &params);
	}

	void makeNewDecisions()
	{
		targetInfo_t targetInfoPlayer;
	
		findEnemyInTarget(&targetInfoPlayer);

		for (unsigned int i=0; i<mapAreas[iActMapArea].enemies.size(); i++)
		{
			OAnimatedMD3 *enemy = &mapAreas[iActMapArea].enemies[i];

			enemy->bPlayerInSight = checkPlayerInSight(enemy);

			if (enemy->isAlive())
			{
				if (enemy->bPlayerInSight)
				{
					// Alarm, Feuern, Panik, usw!
					
					if (!bGlobalAlarm)
					{
						caAudio::playSound(caAudio::SOUND_ENEMY_WARNS);
					}
					
					startGlobalAlarm();
				}

				if (bGlobalAlarm || enemy->bPlayerInSight)
				{
					enemy->startState(BEHAVE_ALARMED);
				}
				else
				{
					enemy->behaveIdle();
				}

				if (i==targetInfoPlayer.iClosestEnemyNr && enemy->bPlayerInSight)
				{
					// decide to evade, get target position
					decideToEvade(&targetInfoPlayer);
				}
			}
		}
	}
	
	void makePitch(OAnimatedMD3 *enemy, float *pfPitch, bool bTrackPlayer)
	{
		Vector4 enemyLookAt		= enemy->moveInfo.cam.getForwardLook();
		Vector4 enemyToPlayer	= (-enemy->moveInfo.cam.getPosition() + player.moveInfo.cam.getPosition());

		if (!bTrackPlayer)
			enemyToPlayer = enemy->moveInfo.cam.getForwardMove(); // straight, without pitch

		enemyToPlayer.normalize();

		float fYDiff = (enemyToPlayer.y - enemyLookAt.y);

		float fCurPitch = enemy->moveInfo.cam.getPitch();
		
float fMaxEnemyPitch = 10.0f;

		if (fabs(fYDiff) > 0.01)
		{
			if (fYDiff > 0)
			{
				if (fCurPitch < fMaxEnemyPitch)
					*pfPitch =  1;
			}
			else
			{
				if (fCurPitch > -fMaxEnemyPitch)
					*pfPitch = -1;
			}
		}
	}

	void executeEnemyIsAlarmed(OAnimatedMD3 *enemy, float fElapsedTime, float *pfYaw, float *pfPitch)
	{
		// turn evtl. more to player
		
		Vector4 enemyLookAt		= enemy->moveInfo.cam.getForwardLook();
		Vector4 enemyToPlayer	= (-enemy->moveInfo.cam.getPosition() + player.moveInfo.cam.getPosition());
		
		// for findYawAngleBetweenVectors to work, the vectors must have same y value and must be normalized!
		enemyLookAt.y = 0.0f;
		enemyLookAt.normalize();
		enemyToPlayer.y = 0.0f;
		enemyToPlayer.normalize();

		float fAngleBetween = findYawAngleBetweenVectors(enemyLookAt, enemyToPlayer);

		if (fabs(fAngleBetween) > 3)
			enemy->startState(BEHAVE_TURNING);
		else
			enemy->stopState(BEHAVE_TURNING);
		
		if (fabs(fAngleBetween) > 1)
		{
			if (fAngleBetween > 0)
				*pfYaw =  1;
			else
				*pfYaw = -1;
		}
		else
		{
			if (enemy->bPlayerInSight)
				enemy->tryShootWeapon();
		}
	}

	void executeEnemyIsEvading(OAnimatedMD3 *enemy, float fElapsedTime, float *pfForward, float *pfStrafe)
	{
		// move to evading target
		
		Vector4 standingPositionXZ = enemy->getStandingPosition();

		standingPositionXZ.y = 0.0f;

		Vector4 moveDirection = (-standingPositionXZ + enemy->movingTargetXZ);
		
		if (vecLength(moveDirection) > fTargetReachedDistance)
		{
			moveDirection.normalize();

			Vector4 localMoveDirection = moveDirection.getRotatedY(-enemy->getYaw()); // moveDirection in local coordinates

			*pfForward	= -localMoveDirection.z;
			*pfStrafe	=  localMoveDirection.x;
		}
		else
		{
			enemy->stopState(BEHAVE_EVADING);
		}
	}

	void init()
	{
		fHandleTime = 0.0f;
		
		bGlobalAlarm = false;

		fStopGlobalAlarmTime = 0.0f;
	}

	void handle(float fElapsedTime)
	{
		fHandleTime += fElapsedTime;

		// 1) make new decisions, when it's time

		if (fHandleTime >= fReactionTime)
		{
			fHandleTime = 0.0f;

			makeNewDecisions();
		}
		
		// 2) execute decisions

		for (unsigned int i=0; i<mapAreas[iActMapArea].enemies.size(); i++)
		{
			OAnimatedMD3 *enemy = &mapAreas[iActMapArea].enemies[i];

			float fForward=0, fStrafe=0, fYaw=0, fPitch=0;
			int iNewArea;
			bool bLevelExitCollision, bJump = false;

			if (enemy->behaveState & BEHAVE_ALARMED)
				executeEnemyIsAlarmed(enemy, fElapsedTime, &fYaw, &fPitch);

			if (enemy->behaveState & BEHAVE_EVADING)
				executeEnemyIsEvading(enemy, fElapsedTime, &fForward, &fStrafe);

			makePitch(enemy, &fPitch, enemy->behaveState & BEHAVE_ALARMED);

			moveEntity(&enemy->moveInfo, fElapsedTime, fForward, fStrafe, fYaw, fPitch, bJump, &iNewArea, &bLevelExitCollision, false, false);

			enemy->adjustLegsYRotation(fElapsedTime);

			if (enemy->bFireNow)
			{
				ca_projectiles::startNewProjectile(SHOOTER_ENEMY, i, enemy->findProjectileStartPos(), enemy->moveInfo.cam.getForwardLook());

				enemy->bFireNow = false;
			}
		}
		
		// 3) check if global alarm ends

		checkStopGlobalAlarm(fElapsedTime);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ca_projectiles
{
	#define MAX_PROJECTILES			200
	#define PROJECTILE_SPEED		  5.0f

	CProjectile projectiles[MAX_PROJECTILES];
	
	int iLastActivatedProjectile;

	int iActiveProjectiles;
	
	void init()
	{
		for (int i=0; i<MAX_PROJECTILES; i++)
			projectiles[i].bActive = false;

		iLastActivatedProjectile = -1;

		iActiveProjectiles = 0;
	}

	int findFreeProjectileSlot()
	{
		int iSearchPos = iLastActivatedProjectile;

		for (int i=0; i<MAX_PROJECTILES; i++)
		{
			iSearchPos++;

			if (iSearchPos >= MAX_PROJECTILES)
				iSearchPos = 0;

			if (!projectiles[iSearchPos].bActive)
				return iSearchPos;
		}

		return -1;
	}

	void startNewProjectile(shooter_e shooterType, int shooterNumber, Vector4 vStart, Vector4 vDirNorm)
	{
		int iFreeSlot = findFreeProjectileSlot();

		if (iFreeSlot != -1)
		{
			CProjectile *proj = &projectiles[iFreeSlot];

			proj->bActive			= true;
			proj->shooterType		= shooterType;
			proj->shooterNumber		= shooterNumber;
			proj->vPos				= vStart;
			proj->vDirNorm			= vDirNorm;

			targetInfo_t targetInfo;

			targetInfo.vShooterEyepos	= vStart;
			targetInfo.vShooterDir		= vDirNorm;

			findClosestWallhit(&targetInfo);

			proj->fWallhitDistance	= targetInfo.fClosestWall;
			proj->fMovedDistance	= 0.0f;

			iLastActivatedProjectile = iFreeSlot;

			iActiveProjectiles++;
			
			caAudio::playSound(caAudio::SOUND_ENEMY_SHOOT0);
		}
	}

	void advance(float fElapsedTime)
	{
		if (!iActiveProjectiles) return;

		float fFrameMovedDistance = (fElapsedTime * PROJECTILE_SPEED);

		for (int i=0; i<MAX_PROJECTILES; i++)
		{
			if (projectiles[i].bActive)
			{
				CProjectile *proj = &projectiles[i];
				
				// test collision
				bool bCollision;
				float tHit;
				
				Vector4 playerPos = player.getStandingPosition();

				float fDistanceToPlayer2 = vecDistanceSquared(playerPos, proj->vPos);
				
				if ( fDistanceToPlayer2 > ((3 + fFrameMovedDistance) * (3 + fFrameMovedDistance)) ) // no test needed, if far away...
					bCollision = false;
				else
					bCollision = player.checkHit(proj->vPos, proj->vDirNorm, fFrameMovedDistance, &tHit); // tests ray against all 6 aabb sides
	
				if (bCollision)
				{
					if (!ca_ai::bGodmode) player.removeHitpoints(1);
					proj->bActive = false;
					iActiveProjectiles--;
				}
				else
				{
					proj->fMovedDistance += fFrameMovedDistance;

					if (proj->fMovedDistance >= proj->fWallhitDistance || proj->fMovedDistance >= PROJECTILE_MAX_RANGE)
					{
						proj->bActive = false;
						iActiveProjectiles--;
					}
					else
					{
						proj->vPos = proj->vPos + (proj->vDirNorm * fFrameMovedDistance);
					}
				}
			}
		}
	}

	void render(const CCamera &cam)
	{		
		if (!iActiveProjectiles) return;

		Vector4 right = cam.getRightLook(), up = cam.getUpLook();

		float fSize = 0.5f;

		render3DSpriteBegin(true, systemTextureId[SYSTEM_TEXTURE_PARTICLE]);

		setBlendFunctionFixedColor(1.0f);
		
		for (int i=0; i<MAX_PROJECTILES; i++)
		{
			if (projectiles[i].bActive)
				render3DSprite(right, up, projectiles[i].vPos, fSize, 0xffffffff);
		}
		
		render3DSpriteEnd();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
