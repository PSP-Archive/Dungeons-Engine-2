// gamestate.cpp

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

#include "gamestate.h"
#include "../de2mapformat/de2mapformat.h"
#include "../caUtil/caUtil.h"
#include "shooting.h"
#include "ai.h"
#include "highscores.h"
#include "collision_detection.h"

#include <iostream>
#include <map>
using namespace std;

namespace gamestate
{
	CFont font1;

	int iCurrentMap;

	char g_szLoadedMap[100] = {0};

	bool bVSync;

	int iPlayerStartHitPoints;

	enum gamestate_e
	{
		MENU_SCREEN,
		INGAME_MENU,
		GAME_STARTING,
		GAMEPLAY,
		GAME_ENDED
	};

	gamestate_e currentGamestate;

	float fMenuHighscoretime;
	float fStartupTime;
	float fGametime;

//	bool bShowHighscores	= false;
	bool bQuitGame			= false;
	int iMainMenuScreen		= 0; // 0=Main, 1=Info
	int iMainMenuItem		= 0; // 0=Start, 1=Info, 2=Exit

	int iTestSwitch			= 0;

//	char szGameEndedText[100] = {0};

	char szTitletext1[200];
	char szTitletext2[200];

	void switchGamestate(gamestate_e newGamestate)
	{
		int iTopTen;

		currentGamestate = newGamestate;
		
		switch (newGamestate)
		{
			case MENU_SCREEN:

//				bShowHighscores = false;
				bQuitGame		= false;
				iMainMenuScreen	= 0;
				iMainMenuItem	= 0;

				fMenuHighscoretime = 0;
				
				break;
			
			case GAME_STARTING:
			
				fStartupTime = 0.0f;
				
				break;
			
			case GAME_ENDED:
			
				iTopTen = highscores[0].insert(roundNK(fGametime, 2));
	
				if (iTopTen <= 10)
				{
//					sprintf(szGameEndedText, "Position %i in the Top 10 list!", iTopTen);
					highscores[0].save();
				}
				else
				{
//					sprintf(szGameEndedText, "Too slow for the Top 10");
				}
				
				break;
				
			default:
			
				break;
		}
	};

	void resetGametime()
	{
		fGametime = 0;
	}

	void addToGametime(float fSec)
	{
		fGametime += fSec;
		fGametime = MIN(fGametime, 99999.99f);
	}

	void renderPlayerHealth()
	{
		char text[200];
		
		sprintf(text, "%d", player.getHitpoints());

		font1.draw(text, 5, 0, 0xff0000ff, TEXT_LEFT);
	}

	void renderGametime()
	{
		char text[200];
		
		sprintf(text, "%.2f", fGametime);

if (!iTestSwitch)
		font1.draw(text, 265, 0, 0xff00ffff, TEXT_RIGHT);
else
		font1.draw(text, 265, 0, 0xffffffff, TEXT_RIGHT);
	}

	void loadMapArea(int iMapArea)
	{
		char info[200];

		if (!bVSync && !levelInfo.iLoadCompleteMap)
		{
			sprintf(info, "before loading area %i", iMapArea);
			writeFreeRam(info);
		}
			
		openMapFormatRun(g_szLoadedMap, CHANGE_MAP_AREA, iMapArea);
		flushCache();

		if (!bVSync && !levelInfo.iLoadCompleteMap)
		{
			sprintf(info, "after loading area %i", iMapArea);
			writeFreeRam(info);
		}
	}

	void renderLoadingScreen()
	{
		prepareRendering();
			begin2DRendering();
				render2DSprite(104, 272, 0, 272, clampMergeRGBA(130, 0, 0, 255), true, CLight::textureId, 0, 1, 0, 1, true);
			end2DRendering();

			startFontRendering(font1);
				font1.draw( "Loading", 240, 132, 0xff0000cc, TEXT_CENTER);
			endFontRendering();
		finishRendering(bVSync);
	}

	void resetMapGameplay()
	{
		player.setModels(0, 0, 0, 0, NULL, NULL);
		player.init(startPoint.orientation, false);
		ca_ai::init();
		shooting::reset();
		ca_projectiles::init();
		particles::resetAllParticles();
	}

	bool loadMap()
	{
		char szMap[100];

		if		(iCurrentMap <  10)	sprintf(szMap, "map00%d.d2", iCurrentMap);
		else if (iCurrentMap < 100) sprintf(szMap, "map0%d.d2" , iCurrentMap);
		else						sprintf(szMap, "map%d.d2"  , iCurrentMap);

		if (strcmp(g_szLoadedMap, szMap))
		{
			renderLoadingScreen();

			if (!openMapFormatRun(szMap, LOAD_MAP, 0))
				return false;

			strcpy(g_szLoadedMap, szMap);
		}
		else
		{
			resetCurrentMap(szMap);
		}

		resetMapGameplay();

		flushCache();

		if (!bVSync)
		{
			char text[200];
			sprintf(text, "After loading %s", g_szLoadedMap);
			writeFreeRam(text);
			printMemoryInfo(text);
			cout << "Total enemies: " << g_iEnemiesToKill << endl;
		}

		return true;
	}

	void resetGame()
	{
		switchGamestate(GAME_STARTING);

		iCurrentMap = 1;

		player.setHitpoints(iPlayerStartHitPoints);

		loadMap();

		resetGametime();
	}

	void loadNextMap()
	{
		iCurrentMap++;

		if (!loadMap())
			resetGame();
	}

	void updateEnemies(int iMapArea, float fElapsedTime)
	{
		int iEnemies = mapAreas[iMapArea].enemies.size();

		for (int i=0; i<iEnemies; i++)
			mapAreas[iMapArea].enemies[i].updateAnimation(fElapsedTime);
	}

	void checkDeadEnemies(int iMapArea)
	{
		int iEnemies = mapAreas[iMapArea].enemies.size();

		for (int i=0; i<iEnemies; i++)
		{
			OAnimatedMD3 *pEnemy = &mapAreas[iMapArea].enemies[i];
			
			if (!pEnemy->isAlive())
			{
				switch (pEnemy->iDeathAnim)
				{
					case 1:
						pEnemy->setTorsoAnimation("BOTH_DEAD1");
						pEnemy->setLegsAnimation("BOTH_DEAD1");
						break;
					case 2:
						pEnemy->setTorsoAnimation("BOTH_DEAD2");
						pEnemy->setLegsAnimation("BOTH_DEAD2");
						break;
					default:
						pEnemy->setTorsoAnimation("BOTH_DEAD3");
						pEnemy->setLegsAnimation("BOTH_DEAD3");
						break;
				}
			}
		}
	}

	void renderFPS(float fElapsedTime)
	{
		char fps[60];
		int iFPS = getFramesPerSec(fElapsedTime);
		
		if (iFPS == 0)
			strcpy(fps, "");
		else
			sprintf(fps, "%i", iFPS);

		font1.draw(fps, 475, 0, 0xff00ffff, TEXT_RIGHT);
	}

	void renderTestValue(int iVal)
	{
		char text[60];

		sprintf(text, "Enemies in fov: %.2d", iVal);

		font1.draw(text, 10, 10, 0xff00ffff, TEXT_LEFT);
	}

	void renderAreaLights(int iArea, const CCamera &cam)
	{
		multimap<float, int> sortedLights;
		multimap<float, int>::reverse_iterator it;

		Vector4 playerPos = cam.getPosition();

		for (unsigned int i=0; i<mapAreas[iArea].lights.size(); i++)
			sortedLights.insert(make_pair( vecDistanceSquared(mapAreas[iArea].lights[i].position, playerPos), i));

		for (it = sortedLights.rbegin(); it != sortedLights.rend(); it++)
			mapAreas[iArea].lights[(*it).second].render(true, false, false, cam);
	}

	void renderEnemyWeaponFlashs(int iMapArea, const CCamera &cam)
	{
		int iEnemies = mapAreas[iMapArea].enemies.size();

		Vector4 right=cam.getRightLook(), up=cam.getUpLook();

		render3DSpriteBegin(true, systemTextureId[SYSTEM_TEXTURE_WEAPONFLASH]);

		for (int i=0; i<iEnemies; i++)
		{
			OAnimatedMD3 *enemy = &mapAreas[iMapArea].enemies[i];
				
			enemy->renderWeaponFlash(right, up, false);
		}

		render3DSpriteEnd();
	}

	int renderEnemies(int iMapArea, const CCamera &cam)
	{
		int iEnemies = mapAreas[iMapArea].enemies.size();

		int iRenderedEnemies = 0;

		for (int i=0; i<iEnemies; i++)
		{
			// wenn hinter einer frustum plane -> nicht rendern

			bool bBehindOnePlane = false;
			
			OAnimatedMD3 *enemy = &mapAreas[iMapArea].enemies[i];

			AABB_t enemyAABB = enemy->getAABB();
			
			Vector4 enemyPos = enemy->getStandingPosition();
			
			enemyPos.y += (enemyAABB.max.y * 0.5f);
						
			float fEnemyRad = (enemyAABB.max.y * 0.5f);

			float fDelta	= 1.0f; // zur Sicherheit etwas frueher rendern, um ploetzliches aufpoppen zu verhindern...
			float fFogDelta = 3.0f; // zur Sicherheit etwas frueher rendern, um ploetzliches aufpoppen zu verhindern...

			for (int ip=0; ip<6; ip++)
			{
				if ( ( frustumPlane[ip].distanceToPoint(enemyPos) + fEnemyRad + fDelta) < 0)
				{
					ip = 6;
					bBehindOnePlane = true;
				}
			}

			if (mapAreas[iActMapArea].mapAreaInfo.iFog)
			{
				// Wenn Distanz zum Player > fFogEndDistance -> nicht mehr rendern
				
				Vector4 playerPos = cam.getPosition();
				
				if ( vecDistance(playerPos, enemyPos) > (mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance+fEnemyRad+fFogDelta) )
				{
					bBehindOnePlane = true;
				}
			}
			
			if (!bBehindOnePlane)
			{
				iRenderedEnemies++;
				enemy->render(false, false, false);
			}
		}
		
		return iRenderedEnemies;
	}

	bool bCheckCullQuad(Quad_t *pQ, Vector4 *pvPos)
	{
		// early backface culling
		
		float VtoPx, VtoPy, VtoPz;

		Vertex *pv = &pQ->verts[0];

		VtoPx = -pvPos->x + pv->x;
		VtoPy = -pvPos->y + pv->y;
		VtoPz = -pvPos->z + pv->z;

		return ( (VtoPx * pv->nx + VtoPy * pv->ny + VtoPz * pv->nz ) >= 0 );
	}

	void renderMap(const CCamera &cam)
	{
		if (mapAreas[iActMapArea].mapAreaInfo.iHasSkybox)
		{
			renderSkybox(&mapAreas[iActMapArea].mapAreaInfo.skyboxTextureMapIndex[0], cam.getPosition());
		}

		Vector4 vPos = cam.getPosition();

		for (unsigned int i=0; i<mapAreas[iActMapArea].visibleQuads.size(); i++)
		{
			if (!bCheckCullQuad(&mapAreas[iActMapArea].visibleQuads[i], &vPos))
				clipRenderQuad(&mapAreas[iActMapArea].visibleQuads[i], true, true, false, false);
		}
	}

	void renderGameScreen(float fElapsedTime)
	{
		Matrix4 viewMat, projMat;		

		if (mapAreas[iActMapArea].mapAreaInfo.iFog)
		{
			setFogParameters(mapAreas[iActMapArea].mapAreaInfo.uiFogColor, mapAreas[iActMapArea].mapAreaInfo.fFogStartDistance, mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance);
			switchFog(true);
		}

		projMat = setRenderingProjection(45.0f, (16.0f/9.0f), 0.1f, mapAreas[iActMapArea].mapAreaInfo.fZClippingDistance);

		viewMat = setRenderingView(player.moveInfo.cam);

		extractFrustumPlanes(viewMat, projMat);
		
		renderMap(player.moveInfo.cam);

		renderEnemies(iActMapArea, player.moveInfo.cam);

		renderAreaLights(iActMapArea, player.moveInfo.cam);

		switchFog(false);

		renderEnemyWeaponFlashs(iActMapArea, player.moveInfo.cam);

		particles::render(player.moveInfo.cam);

		ca_projectiles::render(player.moveInfo.cam);

		shooting::renderPlayerWeapon();

		startFontRendering(font1);
			if (!bVSync)
				renderFPS(fElapsedTime);
			renderGametime();
			renderPlayerHealth();
		endFontRendering();
	}

	bool handleGameInput(float fElapsedTime, int *piNewArea, bool *pbLevelExitCollision)
	{
		readInput(fElapsedTime, bVSync);

		if ( hardwareKeyOncePressed(BUTTON_START) )
		{
			switchGamestate(INGAME_MENU);
			return false;
		}

		float fForward=0, fStrafe=0, fYaw=0, fPitch=0;
		bool bJump = false;
		
		fForward -= ( actionKeyPressed[ACTION_WALK_BACKWARD] / 127.0f);
		fForward += ( actionKeyPressed[ACTION_WALK_FORWARD]  / 127.0f);
		fStrafe  += ( actionKeyPressed[ACTION_STRAFE_RIGHT]  / 127.0f);
		fStrafe  -= ( actionKeyPressed[ACTION_STRAFE_LEFT]   / 127.0f);
		fYaw     += ( actionKeyPressed[ACTION_ROTATE_LEFT]   / 127.0f);
		fYaw     -= ( actionKeyPressed[ACTION_ROTATE_RIGHT]  / 127.0f);
		fPitch   -= ( actionKeyPressed[ACTION_ROTATE_DOWN]   / 127.0f);
		fPitch   += ( actionKeyPressed[ACTION_ROTATE_UP]	 / 127.0f);

		if (player.moveInfo.bOnGround)
		{
			if ( hardwareKeyOncePressed(LEFT_TRIGGER) )
			{
				bJump		 = true;
//				iTestSwitch ^= 1;
			};
		}
		
		moveEntity(&player.moveInfo, fElapsedTime, fForward, fStrafe, fYaw, fPitch, bJump, piNewArea, pbLevelExitCollision, true, true);

		return true;
	}

	void renderMenuScreen(float fElapsedTime)
	{
		CCamera cam;
		Matrix4 viewMat, projMat;

		projMat = setRenderingProjection();
		viewMat = setRenderingView(cam);

		extractFrustumPlanes(viewMat, projMat);

		unsigned int textColor = 0xff00ffff, colorStart, colorInfo, colorExit;

/*
		if (bShowHighscores)
		{
			begin2DRendering();
				render2DSprite(104, 272, 0, 272, clampMergeRGBA(130, 0, 0, 255), true, CLight::textureId, 0, 1, 0, 1, true);
			end2DRendering();

			startFontRendering(font1);
				highscores[0].show(&font1, textColor);
				font1.draw("2008 by charnold", 480-4, 272-16, textColor, TEXT_RIGHT);

				if (!bVSync)
					renderFPS(fElapsedTime);
			endFontRendering();
		}
		else
*/
		{
			if (iMainMenuScreen == 0)
			{
				colorStart	= 0xff008888;
				colorInfo	= 0xff008888;
				colorExit	= 0xff008888;

				switch (iMainMenuItem)
				{
					case 0:
						colorStart	= 0xff00ffff;
					break;
					case 1:
						colorInfo	= 0xff00ffff;
					break;
					case 2:
						colorExit	= 0xff00ffff;
					break;
				}

				begin2DRendering();
					render2DSprite(104, 272, 0, 272, clampMergeRGBA(130, 0, 0, 255), true, CLight::textureId, 0, 1, 0, 1, true);
				end2DRendering();

				startFontRendering(font1);
					font1.draw( szTitletext1,	240, 40,		textColor,	TEXT_CENTER);
					font1.draw( szTitletext2,	240, 60,		textColor,	TEXT_CENTER);
					font1.draw( "Press start",	240, 120,		colorStart, TEXT_CENTER);
					font1.draw( "Info",			240, 120+20,	colorInfo,	TEXT_CENTER);
					font1.draw( "Exit",			240, 120+40,	colorExit,	TEXT_CENTER);
					if (!bVSync)
						renderFPS(fElapsedTime);
				endFontRendering();
			}
			else
			{
				begin2DRendering();
					render2DSprite(104, 272, 0, 272, clampMergeRGBA(130, 0, 0, 255), true, CLight::textureId, 0, 1, 0, 1, true);
				end2DRendering();

				startFontRendering(font1);
					font1.draw( "3D Engine:",						80, 100,	textColor, TEXT_LEFT);
					font1.draw( "Dungeons Engine 2 Version 1.0.6",	80, 100+20,	textColor, TEXT_LEFT);
					font1.draw( "2008 by charnold, www.caweb.de",	80, 100+40,	textColor, TEXT_LEFT);
					if (!bVSync)
						renderFPS(fElapsedTime);
				endFontRendering();
			}
		}

		fMenuHighscoretime += fElapsedTime;

		if (fMenuHighscoretime >= 5.0f)
		{
			fMenuHighscoretime = 0;
//			bShowHighscores ^= 1;
//			if (!bShowHighscores) bExitProgram = false;
		}
	}

	void handleMenuInput(float fElapsedTime)
	{
		readInput(fElapsedTime, bVSync);

		if ( hardwareKeyOncePressed(BUTTON_START) )
		{
			if (iMainMenuScreen == 0)
			{
				switch (iMainMenuItem)
				{
					case 0:
						resetGame();
					break;
					case 1:
						iMainMenuScreen = 1;
					break;
					case 2:
						g_bGameRunning = false;
					break;
				}
			}
			else
			{
				iMainMenuScreen = 0;
			}
		}

		if ( hardwareKeyOncePressed(DIGITAL_PAD_DOWN) || hardwareKeyOncePressed(BUTTON_CROSS))
		{
			fMenuHighscoretime = 0;
//			bShowHighscores = false;

			if (iMainMenuScreen == 0)
			{
				if (iMainMenuItem < 2) iMainMenuItem++;
			}
		}

		if ( hardwareKeyOncePressed(DIGITAL_PAD_UP) || hardwareKeyOncePressed(BUTTON_TRIANGLE))
		{
			fMenuHighscoretime = 0;
//			bShowHighscores = false;

			if (iMainMenuScreen == 0)
			{
				if (iMainMenuItem > 0) iMainMenuItem--;
			}
		}
	}

	void renderIngameMenuScreen(float fElapsedTime)
	{
		unsigned int colorCont, colorQuit;

		if (!bQuitGame)
		{
			colorCont = 0xff00ffff;
			colorQuit = 0xff008888;
		}
		else
		{
			colorCont = 0xff008888;
			colorQuit = 0xff00ffff;
		}

		startFontRendering(font1);
			font1.draw("Continue", 200, 155, colorCont, TEXT_LEFT);
			font1.draw("Quit", 220, 155+20, colorQuit, TEXT_LEFT);
		endFontRendering();
	}

	void handleIngameMenuInput(float fElapsedTime)
	{
		readInput(fElapsedTime, bVSync);

		if ( hardwareKeyOncePressed(BUTTON_START) )
		{
			if (!bQuitGame)
				switchGamestate(GAMEPLAY);
			else
				switchGamestate(MENU_SCREEN);
		}	

		if ( hardwareKeyOncePressed(DIGITAL_PAD_DOWN) || hardwareKeyOncePressed(BUTTON_CROSS))
			bQuitGame = true;

		if ( hardwareKeyOncePressed(DIGITAL_PAD_UP) || hardwareKeyOncePressed(BUTTON_TRIANGLE))
			bQuitGame = false;
	}

	void renderGameEndedMenuScreen(float fElapsedTime)
	{
		startFontRendering(font1);
//			font1.draw(szGameEndedText,				95, 160, 0xff00ffff, TEXT_LEFT);
//			font1.draw("Game over - Press start",	95, 180, 0xff00ffff, TEXT_LEFT);
			font1.draw("Game over - Press start",	240, 160, 0xff00ffff, TEXT_CENTER);
		endFontRendering();
	}

	void handleGameEndedMenuInput(float fElapsedTime)
	{
		readInput(fElapsedTime, bVSync);

		if ( hardwareKeyOncePressed(BUTTON_START) )
		{
			switchGamestate(MENU_SCREEN);
//			bShowHighscores = true;
		}	
	}

	void myShutdown(int code)
	{
		clearMap();
		unloadMD3Models();
		font1.unloadTexture();
		particles::destroy();
		shooting::unloadTextures();
		CLight::unloadTexture();
		caAudio::closeAudio();
	}

	void setupAudio()
	{
		caAudio::initAudio();
		caAudio::loadWave(caAudio::SOUND_STEP,			"step3.wav", 0);
		caAudio::loadWave(caAudio::SOUND_ENEMY_SPLAT,	"gibsplt1.wav", 0);
		caAudio::loadWave(caAudio::SOUND_SHOOT0,		"weapontype0.wav", 0);
		caAudio::loadWave(caAudio::SOUND_ENEMY_SHOOT0,	"weapontype0.wav", 0);
		caAudio::loadWave(caAudio::SOUND_ENEMY_WARNS,	"pain100_1.wav", 0);
	}

	void renderGameStartScreen(float fElapsedTime)
	{
		fStartupTime += fElapsedTime;

		if (fStartupTime > 1.30f)
		{
			switchGamestate(GAMEPLAY);
		}
		else
		{
			startFontRendering(font1);
				if (fStartupTime <= 1.00f)
					font1.draw("Get ready...", 202, 272/2-30, 0xff00ffff, TEXT_LEFT);
				else
					font1.draw("Go!", 227, 272/2-30, 0xff00ffff, TEXT_LEFT);
			endFontRendering();
		}
	}

	void runGame(float fElapsedTime)
	{
		int iNewArea = -1;
		bool bLevelExitCollision = false;

		switch (currentGamestate)
		{
			case MENU_SCREEN:
				prepareRendering();
					renderMenuScreen(fElapsedTime);
				finishRendering(bVSync);

				handleMenuInput(fElapsedTime);

				break;

			case INGAME_MENU:
				handleIngameMenuInput(fElapsedTime);

				prepareRendering();
					renderGameScreen(fElapsedTime);
					renderIngameMenuScreen(fElapsedTime);
				finishRendering(bVSync);

				break;
			
			case GAME_STARTING:
				prepareRendering();
					renderGameScreen(fElapsedTime);
					renderGameStartScreen(fElapsedTime);
				finishRendering(bVSync);
				
				break;
			
			case GAMEPLAY:

				prepareRendering();
					renderGameScreen(fElapsedTime);

					if (handleGameInput(fElapsedTime, &iNewArea, &bLevelExitCollision))
					{
						addToGametime(fElapsedTime);

						ca_ai::handle(fElapsedTime);
						particles::advanceParticles(fElapsedTime);
						ca_projectiles::advance(fElapsedTime);
						shooting::handlePlayerWeapon(fElapsedTime);
						updateEnemies(iActMapArea, fElapsedTime);
						textureEffects::advance(fElapsedTime);
					}
					
//					if (shooting::iEnemyKills == g_iEnemiesToKill && shooting::iEnemyKills > 0)
					if (!player.getHitpoints())
						switchGamestate(GAME_ENDED);

				finishRendering(bVSync);


				if (iNewArea != -1)
				{
					checkDeadEnemies(iActMapArea);
					ca_projectiles::init();
					particles::resetAllParticles();
					loadMapArea(iNewArea);
					getElapsedTime(); // Ladezeit nicht mitzählen...
				}

				if (bLevelExitCollision)
				{
					loadNextMap();
					getElapsedTime(); // Ladezeit nicht mitzählen...
				}

				
				break;
				
			case GAME_ENDED:
				handleGameEndedMenuInput(fElapsedTime);

//				ca_ai::handle(fElapsedTime);
//				particles::advanceParticles(fElapsedTime);
//				ca_projectiles::advance(fElapsedTime);
//				shooting::handlePlayerWeapon(fElapsedTime, false);
//				updateEnemies(iActMapArea, fElapsedTime);
//				textureEffects::advance(fElapsedTime);
				
				prepareRendering();
					renderGameScreen(fElapsedTime);
					renderGameEndedMenuScreen(fElapsedTime);
				finishRendering(bVSync);
				
				break;
			
		}
	};

	void loadConfigFile()
	{
		char szFile[256], szLine[256];

		sprintf(szFile, "%s/%s", SYSTEM_PATH, "config.txt");
		
		ifstream file(szFile, std::ios::in);

		strcpy(szTitletext1, "");
		strcpy(szTitletext2, "");
		bVSync						= false;
		ca_ai::bGodmode				= false;
		iPlayerStartHitPoints		= 10;

		if (file.fail())
		{
		}
		else
		{
			while(file.getline(szLine, 256))
			{
				if (cleanupLine(szLine) == 0) continue;

				if (!strcmp(szLine, "[TITLETEXT1]"))
				{
					file.getline( szLine, 256);
					cleanupLine(szLine);
					strcpy(szTitletext1, szLine);
					continue;
				}
				if (!strcmp(szLine, "[TITLETEXT2]"))
				{
					file.getline( szLine, 256);
					cleanupLine(szLine);
					strcpy(szTitletext2, szLine);
					continue;
				}
				if (!strcmp(szLine, "[VSYNC]"))
				{
					file.getline( szLine, 256);
					cleanupLine(szLine);
					if (!strcmp(szLine, "on"))
						bVSync = true;
					else
						bVSync = false;
					continue;
				}
				if (!strcmp(szLine, "[GODMODE]"))
				{
					file.getline( szLine, 256);
					cleanupLine(szLine);
					if (!strcmp(szLine, "on"))
						ca_ai::bGodmode = true;
					else
						ca_ai::bGodmode = false;
					continue;
				}
				if (!strcmp(szLine, "[PLAYER_HITPOINTS]"))
				{
					file.getline( szLine, 256);
					cleanupLine(szLine);
					sscanf(szLine, "%d", &iPlayerStartHitPoints);
					continue;
				}
			}
			file.clear();
			file.close();
		}
	}

	void makeStartupInits()
	{
		clearLog();

		loadConfigFile();

		customShutdownFct = myShutdown;

		initSystem(480, 272, false, "Dungeons");

		highscores[0].load();

		setupAudio();

		CLight::loadTexture();

		shooting::loadTextures();

		particles::init();

		font1.init("WST_Germ.bmp", "WST_Germ_met.dat", 16);

		switchGamestate(MENU_SCREEN);
	};
};
