// shooting.cpp

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

#include "shooting.h"

#include "../de2mapformat/de2mapformat.h"
#include "collision_detection.h"
#include "ai.h"

namespace shooting
{
	int iEnemyKills;

	struct weapon_t
	{
		char szWeaponname[30];
		float fRecoilZGoal;
		float fRecoilZDelta;
		float fRecoilDirection;
		bool bShooting;
		float fWaitForShootNoAmmo;
		void Init() {bShooting=false; fRecoilZGoal=1.0f; fRecoilZDelta=0.0f; fRecoilDirection=1.0f;fWaitForShootNoAmmo=0.0f;};
		weapon_t() {strcpy(szWeaponname, ""); Init();};
	};

	weapon_t playerWeapon;

	unsigned int textureId[SHOOTING_TEXTURE_COUNT];

	void reset()
	{
		iEnemyKills = 0;
		playerWeapon.Init();
	}

	void loadTextures()
	{
		char szTexture[256];

		sprintf(szTexture, "%s/%s", SYSTEM_PATH, "ch16.bmp");

		textureId[TEXTURE_CROSSHAIRS] = ::loadTexture(szTexture, false, 4444, false, 0, 0, mapTexture_t::ALIGN_LOCAL);
	}

	void unloadTextures()
	{
		::unloadTexture(&textureId[TEXTURE_CROSSHAIRS]);
	}

	void renderCrosshairs()
	{
		switchAlphaTest(true);

		begin2DRendering();
			render2DSprite(480/2-8,16,272/2-8,16, 0xffffffff, true, textureId[TEXTURE_CROSSHAIRS]);
		end2DRendering();

		switchAlphaTest(false);
	}

	void renderPlayerWeapon()
	{
		if (startPoint.startWeaponModelIndex == -1) return;

		renderCrosshairs();

		weapon_t *weapon = &playerWeapon;

		Vector4	vT = Vector4(2.0f,-2.5f,-3.5f);
		Vector4	vS = Vector4(0.2f,0.2f,0.2f);

		vT = vT + (5*activeModelPack.weaponModelParams[startPoint.startWeaponModelIndex].vFirstPersonOffset);

		vT.z += weapon->fRecoilZDelta*1.5f;

		float fXRot = weapon->fRecoilZDelta*5.0f;
		float roll, straferoll, wippUpDown;

		roll		= player.moveInfo.cam.getRoll();
		straferoll	= player.moveInfo.cam.getStraferoll();
		wippUpDown	= player.moveInfo.cam.getWippUpDown();
	
		Vector4	TransWipp = Vector4(0.0f, wippUpDown, 0.0f);
		float rollges = roll+straferoll;

		if (weapon->bShooting && weapon->fRecoilDirection == 1.0f)
		{
			begin2DRendering();
				switchBlendTest(true);
					setBlendFunctionFixedColor(weapon->fRecoilZDelta);
					render2DSprite(480/2-62, 256, 272/2-76, 256, 0xffffffff, true, systemTextureId[SYSTEM_TEXTURE_WEAPONFLASH]);
				switchBlendTest(false);
			end2DRendering();
		}

		clearDepthBuffer();

		setViewMatrixIdentity();
			matrixStackPush();
				matrixStackTranslate(TransWipp);
				matrixStackRotateZ(rollges);
				matrixStackTranslate(vT);
				matrixStackRotateX(fXRot);
				matrixStackRotateY(90.0f);
				matrixStackRotateX(-90.0f);
				matrixStackScale(vS);
				weaponModels[startPoint.startWeaponModelIndex]->render(0, false, false, false);
			matrixStackPop();
		resetViewMatrix();
	};

	void advanceWeaponAnimation(float fElapsedTime)
	{
		weapon_t *weapon = &playerWeapon;

		float fRecoilSpeedA = 15.0f;
		float fRecoilSpeedB =  5.0f;

		if (weapon->bShooting)
		{
			if (weapon->fRecoilDirection == 1.0f)
			{
				weapon->fRecoilZDelta += (fElapsedTime*weapon->fRecoilDirection)*fRecoilSpeedA;
			}
			else
			{
				weapon->fRecoilZDelta += (fElapsedTime*weapon->fRecoilDirection)*fRecoilSpeedB;
			}

			if ((weapon->fRecoilZDelta >= weapon->fRecoilZGoal) && (weapon->fRecoilDirection == 1))
			{
				weapon->fRecoilZDelta		= weapon->fRecoilZGoal;
				weapon->fRecoilDirection	= -1.0f;
			}
			else if ((weapon->fRecoilZDelta <= 0.0f) && (weapon->fRecoilDirection == -1))
			{
				weapon->fRecoilZDelta		= 0.0f;
				weapon->fRecoilDirection	= 1.0f;
				weapon->bShooting			= false;
			}			
		}
	}

	float getRandomVal(float min, float max)
	{
		float fRange = (max-min);
		float val;
		int iRange = (int)(fRange * 100);
		int r = rand() % iRange;

		val = (r*0.01f) + min;

		return val;
	}

	int getRandomIntVal(int min, int max)
	{
		int iRange = (max-min+1);
		int val;
		int r = rand() % iRange;

		val = r + min;

		return val;
	}

	void createHitParticles(Vector4 hitPoint, int iCount)
	{
		for (int i=0; i<iCount; i++)
		{
			Vector4 force;

			force.x = getRandomVal(-1,1);
			force.y = getRandomVal( 0,2);
			force.z = getRandomVal(-1,1);

			particles::createNewParticle(hitPoint+(force*0.2f), force);
		}
	}

	void pushEnemy(OAnimatedMD3 *pEnemy, targetInfo_t *pTargetInfo, float fMaxForce, bool bLimitVelocity)
	{
		float fPushForce = fMaxForce - pTargetInfo->fClosestEnemy * 0.25f;

		if (fPushForce < 0) fPushForce = 0;

		Vector4 vShootPush = pTargetInfo->vShooterDir * fPushForce;
		
		pEnemy->moveInfo.bLimitXZVelocityWS = bLimitVelocity;
		pEnemy->moveInfo.vXZVelocityWS		= pEnemy->moveInfo.vXZVelocityWS + vShootPush;
		pEnemy->moveInfo.fYVel				+= vShootPush.y;
	}

	void hittestEnemies()
	{
		int iMapArea = iActMapArea;

		targetInfo_t targetInfo;

		if (findEnemyInTarget(&targetInfo))
		{
			OAnimatedMD3 *pEnemy = &mapAreas[iMapArea].enemies[targetInfo.iClosestEnemyNr];

			pEnemy->removeHitpoints(1);

			if ( !pEnemy->isAlive() )
			{
				iEnemyKills++;

				caAudio::playSound(caAudio::SOUND_ENEMY_SPLAT);
				
				int iDeath = getRandomIntVal(1,3);
				
				pEnemy->iDeathAnim = iDeath;
				
				switch (iDeath)
				{
					case 1:
						pEnemy->setTorsoAnimation("BOTH_DEATH1");
						pEnemy->setLegsAnimation("BOTH_DEATH1");
						break;
					case 2:
						pEnemy->setTorsoAnimation("BOTH_DEATH2");
						pEnemy->setLegsAnimation("BOTH_DEATH2");
						break;
					default:
						pEnemy->setTorsoAnimation("BOTH_DEATH3");
						pEnemy->setLegsAnimation("BOTH_DEATH3");
						break;
				}

				pEnemy->behaveIdle();

				pushEnemy(pEnemy, &targetInfo, 10.0f, false);

				createHitParticles(targetInfo.vShooterEyepos + (targetInfo.vShooterDir * targetInfo.fClosestEnemy), 50);
			}
			else
			{
				pushEnemy(pEnemy, &targetInfo, 5.0f, true);

				createHitParticles(targetInfo.vShooterEyepos + (targetInfo.vShooterDir * targetInfo.fClosestEnemy), 25);
			}

		}
	}

	void shootWeapon(float fElapsedTime)
	{
		weapon_t *weapon = &playerWeapon;

		if (!weapon->bShooting)
		{
			weapon->bShooting = true;
			hittestEnemies();
			caAudio::playSound(caAudio::SOUND_SHOOT0);
			ca_ai::startGlobalAlarm();
		}
	}

	void handlePlayerWeapon(float fElapsedTime, bool bAllowShooting)
	{
		weapon_t *weapon = &playerWeapon;

		if (weapon->fWaitForShootNoAmmo >= 0.0f)
		{
			weapon->fWaitForShootNoAmmo -= fElapsedTime;
		}

		advanceWeaponAnimation(fElapsedTime);

		if (actionKeyPressed[ACTION_SHOOT] && bAllowShooting && startPoint.startWeaponModelIndex != -1)
		{
			shootWeapon(fElapsedTime);
		}
	}

};
