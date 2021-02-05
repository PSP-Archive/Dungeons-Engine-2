// collision_detection.cpp

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

#include "collision_detection.h"

#include <vector>
using namespace std;

OAnimatedMD3 player;

void collideWithEnemiesAndPlayers(moveinfo_t *moverMoveInfo, const Vector4 &moverStartEye, Vector4 *pMoverEndEye)
{
	float fMinDist = 1.0f;

	for (unsigned int iLoop=0; iLoop<mapAreas[iActMapArea].enemies.size()+1; iLoop++)
	{
		OAnimatedMD3 *enemy;
			
		if (iLoop==0)
			enemy = &player;
		else
			enemy = &mapAreas[iActMapArea].enemies[iLoop-1];
		
		if (enemy->isAlive())
		{
			// make same eyeheight for both collision objects
			// purpose: don't climb onto small enemies or so...

			float fCollisionEyeHeight = moverMoveInfo->getEyeHeight();
			
			if (moverMoveInfo != &enemy->moveInfo)
			{
				Vector4 enemyPosOnCollisionEyeHeight = enemy->getStandingPosition() + Vector4(0, fCollisionEyeHeight, 0);
				
				// Vector from collision source to collision target (when hitting, push target into this direction...)

				Vector4 collVec = ( -moverStartEye + enemyPosOnCollisionEyeHeight );

				bool bCollided = makeSphereCollision(moverStartEye, pMoverEndEye, enemyPosOnCollisionEyeHeight, fMinDist);
				
				if (bCollided)
				{
					collVec.normalize();

					enemy->moveInfo.vXZVelocityWS = enemy->moveInfo.vXZVelocityWS + (collVec * 1);

					moverMoveInfo->vXZVelocityWS = moverMoveInfo->vXZVelocityWS + (-collVec * 1);
				}
			}
		}
	}
}

#define MAX_STEP_SIZE 0.4f

bool collideWithWorld(AABB_t *moverAABBWorld, int iMoveAxis, Vector4 movement, Vector4 *afterMovePosEye, moveinfo_t *moveInfo, int *piConnectorCollision, bool *pbLevelExitCollision)
{
	int iColTiles = mapAreas[iActMapArea].collisionBlocks.size();
	
	AABB_t afterMoveAABBWorld = *moverAABBWorld;

	afterMoveAABBWorld.min = afterMoveAABBWorld.min + movement;
	afterMoveAABBWorld.max = afterMoveAABBWorld.max + movement;

	for (int i=0; i<iColTiles; i++)
	{
		AABB_t *bb = &mapAreas[iActMapArea].collisionBlocks[i].collisionBB;

		if (afterMoveAABBWorld.max.x < bb->min.x || afterMoveAABBWorld.min.x > bb->max.x || 
			afterMoveAABBWorld.max.y < bb->min.y || afterMoveAABBWorld.min.y > bb->max.y || 
			afterMoveAABBWorld.max.z < bb->min.z || afterMoveAABBWorld.min.z > bb->max.z)
			continue; // not inside AABB

		if (((afterMoveAABBWorld.min.y + MAX_STEP_SIZE) > bb->max.y) && (movement.y >= 0))
		{
			// move kann zugelassen werden, muss aber nach oben pushen
			float fPushHeightEye = bb->max.y + (-moveInfo->localAABBEye.min.y) + 0.1f;
			
			if (moveInfo->fPushHeightEye < fPushHeightEye || !moveInfo->bPushing)
			{
				moveInfo->bPushing			= true;
				moveInfo->fPushHeightEye	= fPushHeightEye;				
			}

			continue;
		}

		if (!mapAreas[iActMapArea].collisionBlocks[i].bIsConnector)
		{
			// connectors sollen movment nicht bremsen

			if ( iMoveAxis == 2) // fYVel = 0, wenn echte Kollision in y Richtung existiert
			{
				moveInfo->fYVel = 0;
				moveInfo->bOnGround = true;
			}

			if ( iMoveAxis == 1) moveInfo->vXZVelocityWS.x = 0;
			if ( iMoveAxis == 3) moveInfo->vXZVelocityWS.z = 0;
		}
		
		if (mapAreas[iActMapArea].collisionBlocks[i].bIsConnector)
			*piConnectorCollision = i;
		
		if (mapAreas[iActMapArea].collisionBlocks[i].bIsLevelExit)
			*pbLevelExitCollision = true;

		return true; // collision detected
	}

	// keine Kollision in y Richtung: kein weiteres Pushen mehr
	if (moveInfo->bPushing && iMoveAxis == 2)
		moveInfo->bPushing = false;

	// keine Kollision, also moverAABBWorld und afterMovePosEye anpassen
	*moverAABBWorld		= afterMoveAABBWorld;
	*afterMovePosEye	= *afterMovePosEye + movement;

	return false;
}

void moveEntity(moveinfo_t *moveInfo, float fElapsedTime, float fForward, float fStrafe, float fYaw, float fPitch, bool bJump, int *piNewArea, bool *pbLevelExitCollision, bool bAllowMapAreaExit, bool bAllowStepSound)
{
	// move von moveInfo ausführen ohne collision detection
	// collideWithEnemies (Achtung, wenn selbst enemy!)
	// collideWithWorld
	// cameraposition nach move setzen
	// ggf stepsound abspielen

	Vector4 vFrameStartPosEye = moveInfo->cam.getPosition();

	moveInfo->move(fElapsedTime, fForward, fStrafe, fYaw, fPitch, bJump);

	// collide with enemies and players
		Vector4 endPosEye = moveInfo->cam.getPosition();

		collideWithEnemiesAndPlayers(moveInfo, vFrameStartPosEye, &endPosEye);

		moveInfo->cam.setPosition(endPosEye);
	//

	// diese Bewegung würde ohne collision detection durchgeführt werden:
	Vector4 moveVec = moveInfo->cam.getPosition() - vFrameStartPosEye;

// collision detection:
	
	AABB_t moverAABBWorld;
	
	// moverAABB in Weltkoordinaten
	moverAABBWorld.min = moveInfo->localAABBEye.min + vFrameStartPosEye;
	moverAABBWorld.max = moveInfo->localAABBEye.max + vFrameStartPosEye;

	// Movement getrennt nach Achsen
	Vector4 afterMovePosEye = vFrameStartPosEye;
	
	int	iConnectorCollision = -1;
	*piNewArea				= -1;
	*pbLevelExitCollision	= false;

	if (moveVec.x > SMALL_VAL || moveVec.x < -SMALL_VAL)
		collideWithWorld(&moverAABBWorld, 1, Vector4(moveVec.x, 0, 0), &afterMovePosEye, moveInfo, &iConnectorCollision, pbLevelExitCollision);
	if (moveVec.z > SMALL_VAL || moveVec.z < -SMALL_VAL)
		collideWithWorld(&moverAABBWorld, 3, Vector4(0, 0, moveVec.z), &afterMovePosEye, moveInfo, &iConnectorCollision, pbLevelExitCollision);
	if (moveVec.y > SMALL_VAL || moveVec.y < -SMALL_VAL)
		collideWithWorld(&moverAABBWorld, 2, Vector4(0, moveVec.y, 0), &afterMovePosEye, moveInfo, &iConnectorCollision, pbLevelExitCollision);

	if (bAllowMapAreaExit && iConnectorCollision != -1)
	{
		*piNewArea			= mapAreas[iActMapArea].collisionBlocks[iConnectorCollision].iTargetArea;
		afterMovePosEye.x	= mapAreas[iActMapArea].collisionBlocks[iConnectorCollision].vTargetPosition.x;
		afterMovePosEye.z	= mapAreas[iActMapArea].collisionBlocks[iConnectorCollision].vTargetPosition.z;
	}
	else
	{
		*piNewArea = -1;
	}
	
	moveInfo->cam.setPosition(afterMovePosEye);

	if (bAllowStepSound)
	{
		if ((moveInfo->bOnGround && !moveInfo->bOnGroundBeforeMove && moveInfo->bLanding) || (moveInfo->cam.getPlaySound() && moveInfo->bOnGround))
		{
			caAudio::playSound(caAudio::SOUND_STEP);
		}
	}
}
