// de2mapformat.cpp

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

#include "de2mapformat.h"

// ============================================================================
// ============================================================================

startPoint_t startPoint;
int g_iEnemiesToKill;

levelInfo_t levelInfo;

void startPoint_t::init()
{
	orientation.position	= Vector4(2.5f, 0.0f, 2.5f);
	orientation.pitch		= 0.0f;
	orientation.yaw			= 0.0f;
	mapArea					= 0;
	startWeaponModelIndex	= -1;
}

// ============================================================================
// ============================================================================

void addNewEnemyToMapArea(int iMapArea, const orientation_t &orientation, int iModelIndex, int iSkinNr, int iWeaponModelIndex, int iBeginHitPoints)
{
	OAnimatedMD3 newEnemy;
	OAnimatedMD3 *pNewEnemy;

	mapAreas[iMapArea].enemies.push_back(newEnemy);

	pNewEnemy = &mapAreas[iMapArea].enemies[mapAreas[iMapArea].enemies.size()-1];

	pNewEnemy->setModels(iModelIndex, iSkinNr, iWeaponModelIndex, iBeginHitPoints, enemyModels[iModelIndex], weaponModels[iWeaponModelIndex]);
	pNewEnemy->init( orientation );
};

// ============================================================================
// ============================================================================

vector<mapConnector_t> mapConnectors;

mapConnector_t::mapConnector_t()
{
	init();
}

void mapConnector_t::init()
{
	bActive = false;
	
	b1Selected  = false;
	iArea1      = 0;
	iComponent1 = 0;

	b2Selected  = false;
	iArea2      = 0;
	iComponent2 = 0;
}

void clearMapConnectors()
{
	mapConnectors.clear();
}

void deleteConnector(int iConnectorIndex)
{
	// alle components aller MapAreas durchsuchen
	
	for (unsigned int iMapArea=0; iMapArea<mapAreas.size(); iMapArea++)
	{
		for ( unsigned int c=0; c<mapAreas[iMapArea].components.size(); c++)
		{
			CComponent *pComp = &mapAreas[iMapArea].components[c];
			
			if (pComp->iConnectorIndex == iConnectorIndex)
			{
				pComp->bIsConnector		= false;
				pComp->iConnectorIndex	= 0;
			}
			else if (pComp->iConnectorIndex > iConnectorIndex)
			{
				pComp->iConnectorIndex--;
			}
		}
	}
	
	mapConnectors.erase(mapConnectors.begin()+iConnectorIndex, mapConnectors.begin()+iConnectorIndex+1);
}

void updateConnectors()
{
	for (unsigned int i=0; i<mapConnectors.size(); i++)
	{
		mapConnectors[i].b1Selected  = false;
		mapConnectors[i].iArea1      = 0;
		mapConnectors[i].iComponent1 = 0;

		mapConnectors[i].b2Selected  = false;
		mapConnectors[i].iArea2      = 0;
		mapConnectors[i].iComponent2 = 0;
	}

	// iArea und iComponent Werte des mapConnector_t können sich durch löschen von MapArea oder Components ändern...
	
	for (unsigned int iMapArea=0; iMapArea<mapAreas.size(); iMapArea++)
	{
		for ( unsigned int c=0; c<mapAreas[iMapArea].components.size(); c++)
		{
			CComponent *pComp = &mapAreas[iMapArea].components[c];
			
			if (pComp->bIsConnector)
			{
				mapConnector_t *pConn = &mapConnectors[pComp->iConnectorIndex];
				
				if (!pConn->b1Selected)
				{
					pConn->b1Selected	= true;
					pConn->iArea1		= iMapArea;
					pConn->iComponent1	= c;
				}
				else if (!pConn->b2Selected)
				{
					pConn->b2Selected	= true;
					pConn->iArea2		= iMapArea;
					pConn->iComponent2	= c;
					
					pConn->bActive = true;
				}
			}
		}
	}

	for (unsigned int i=0; i<mapConnectors.size(); i++)
	{
		// mindestens eine Seite der Connection wurde gelöscht -> Connection komplett löschen
		if (mapConnectors[i].bActive && (!mapConnectors[i].b1Selected || !mapConnectors[i].b2Selected))
			deleteConnector(i);
	}
	
}

// ============================================================================
// ============================================================================

vector<mapArea_t> mapAreas;
int iActMapArea;

const char			szMapFormatEDIKennung[]	= "DE2EDICA";	// nur dann gibt es die Version im file, sonst ist Version = 0
//const unsigned int  iAktMapFormatEDIVersion	= 10000;		// 01.00.00 = 1.0.0
//const unsigned int  iAktMapFormatEDIVersion	= 10001;		// 01.00.01 = 1.0.1
//const unsigned int  iAktMapFormatEDIVersion	= 10002;		// 01.00.02 = 1.0.2
//const unsigned int  iAktMapFormatEDIVersion	= 10003;		// 01.00.03 = 1.0.3
const unsigned int  iAktMapFormatEDIVersion	= 10006;		// 01.00.06 = 1.0.6

const char			szMapFormatRUNKennung[]	= "DE2RUNCA";	// nur dann gibt es die Version im file, sonst ist Version = 0
//const unsigned int  iAktMapFormatRUNVersion	= 10000;		// 01.00.00 = 1.0.0
//const unsigned int  iAktMapFormatRUNVersion	= 10001;		// 01.00.01 = 1.0.1
//const unsigned int  iAktMapFormatRUNVersion	= 10002;		// 01.00.02 = 1.0.2
//const unsigned int  iAktMapFormatRUNVersion	= 10003;		// 01.00.03 = 1.0.3
const unsigned int  iAktMapFormatRUNVersion	= 10006;		// 01.00.06 = 1.0.6

const char			szMapFormatMOPKennung[]	= "DE2MOPCA";	// nur dann gibt es die Version im file, sonst ist Version = 0
const unsigned int  iAktMapFormatMOPVersion	= 10000;		// 01.00.00 = 1.0.0

void loadInitMapTextures()
{
	loadMapTexture("lighttexture02.lmp", true, 8888, true,	1.0f, mapTexture_t::ALIGN_LOCAL, true);
	loadMapTexture("black.bmp",			 true, 5650, false,	1.0f, mapTexture_t::ALIGN_LOCAL, false);
	clearAllTextureAnimations();
}

vector<CMD3*>			weaponModels;
vector<CAnimatedMD3*>	enemyModels;

void loadEnemyModel(char *sEnemyName, char *sEnemySkin, float fScale, Vector4 StartposDelta, float fBBWidth, float fBBHeight)
{
	char sEnemyPath[256];

	sprintf(sEnemyPath, "%s/%s", ENEMY_PATH, sEnemyName);

	CAnimatedMD3 *newModel = new CAnimatedMD3;
	
	int iIndex = enemyModels.size();

	enemyModels.push_back(newModel);

	enemyModels[iIndex]->load(sEnemyPath, sEnemyName, sEnemySkin, fScale, StartposDelta, fBBWidth, fBBHeight);
}

void addEnemySkin(char *sEnemyName, char *sEnemySkin)
{
	char sEnemyPath[256];

	sprintf(sEnemyPath, "%s/%s", ENEMY_PATH, sEnemyName);

	int iIndex = enemyModels.size()-1;

	enemyModels[iIndex]->load(sEnemyPath, sEnemyName, sEnemySkin, 0, Vector4(0,0,0), 0, 0);
}

void loadWeapon(char *sWeaponName)
{
	char sWeaponPath[256];

	sprintf(sWeaponPath, "%s/%s", WEAPON_PATH, sWeaponName);
	
	CMD3 *newModel = new CMD3;

	int iIndex = weaponModels.size();

	weaponModels.push_back(newModel);

	weaponModels[iIndex]->load(sWeaponPath, "weapon");
}

modelPack_t	activeModelPack;

void modelPack_t::loadPack(char *szName)
{
	char sPackPath[256];
	int iVal;
	char szKennung[12];
	unsigned int iVersion;

	sprintf(sPackPath, "%s/%s.d2p", MODEL_PATH, szName);
	
	strcpy(this->szName, szName);

	weaponModelParams.clear();

	enemyModelParams.clear();

	ifstream infile(sPackPath, ios::in|ios::binary);

	// filekennung
	infile.read( (char*)&szKennung, 12*sizeof(char) );
	
	// version
	if (strcmp(szKennung, szMapFormatMOPKennung))
	{
		infile.seekg(0, ios::beg);
		iVersion = 0;
	}
	else
	{
		infile.read( (char*)&iVersion, sizeof(unsigned int) );
	}

	infile.read( (char*)&iVal, sizeof(int) );
	
	for (int i=0; i<iVal; i++)
	{
		weaponModelParams_t temp;

		infile.read( (char*)&temp.szName[0], 80*sizeof(char) );
		
		if (iVersion >= 10000)
			infile.read( (char*)&temp.vFirstPersonOffset, sizeof(Vector4) );
		else
			temp.vFirstPersonOffset = Vector4(0,0,0);

		weaponModelParams.push_back(temp);
	};

	infile.read( (char*)&iVal, sizeof(int) );

	for (int i=0; i<iVal; i++)
	{
		enemyModelParams_t temp;

		infile.read( (char*)&temp.szName[0], 80*sizeof(char) );
		infile.read( (char*)&temp.iSkins, sizeof(int) );
		infile.read( (char*)&temp.skins[0][0], MAX_SKINS_PER_MODEL*80*sizeof(char) );
		infile.read( (char*)&temp.fScaling, sizeof(float) );
		infile.read( (char*)&temp.vShootStartPos, sizeof(Vector4) );
		infile.read( (char*)&temp.fBBWidth, sizeof(float) );
		infile.read( (char*)&temp.fBBHeight, sizeof(float) );

		enemyModelParams.push_back(temp);
	}

	infile.clear();
	infile.close();	
}

void modelPack_t::savePack()
{
	char sPackPath[256];
	int iVal;

	sprintf(sPackPath, "%s/%s.d2p", MODEL_PATH, szName);

	ofstream outfile(sPackPath, ios::out|ios::trunc|ios::binary);

	// filekennung
	outfile.write( (char*)&szMapFormatMOPKennung[0], 12*sizeof(char) );
	
	// version
	outfile.write( (char*)&iAktMapFormatMOPVersion, sizeof(unsigned int) );
	
	// weapons
	iVal = weaponModelParams.size();
	outfile.write( (char*)&iVal, sizeof(int) );
	
	for (unsigned int i=0; i<weaponModelParams.size(); i++)
	{
		outfile.write( (char*)&weaponModelParams[i], sizeof(weaponModelParams_t) );
	}

	// enemies
	iVal = enemyModelParams.size();
	outfile.write( (char*)&iVal, sizeof(int) );
	
	for (unsigned int i=0; i<enemyModelParams.size(); i++)
	{
		outfile.write( (char*)&enemyModelParams[i], sizeof(enemyModelParams_t) );
	}

	outfile.clear();
	outfile.close();
}

void modelPack_t::loadMD3Models()
{
	for (unsigned int i=0; i<weaponModelParams.size(); i++)
	{
		loadWeapon(weaponModelParams[i].szName);
	}

	for (unsigned int i=0; i<enemyModelParams.size(); i++)
	{
		loadEnemyModel(enemyModelParams[i].szName, enemyModelParams[i].skins[0], enemyModelParams[i].fScaling, enemyModelParams[i].vShootStartPos,
						enemyModelParams[i].fBBWidth, enemyModelParams[i].fBBHeight);

		for ( int j=1; j<enemyModelParams[i].iSkins; j++)
		{
			addEnemySkin(enemyModelParams[i].szName, enemyModelParams[i].skins[j]);
		}
	}
}

void modelPack_t::clear()
{
	strcpy(szName, "");
	weaponModelParams.clear();
	enemyModelParams.clear();
}

void unloadMD3Models()
{
	for (int i=enemyModels.size()-1; i>=0; i--)
	{
		enemyModels[i]->unload();
		delete enemyModels[i];
	}

	enemyModels.clear();

	for (int i=weaponModels.size()-1; i>=0; i--)
	{
		weaponModels[i]->unload();
		delete weaponModels[i];
	}

	weaponModels.clear();
}

void clearEnemies(int iMapArea)
{
	mapAreas[iMapArea].enemies.clear();
}

void clearComponents(int iMapArea)
{
	mapAreas[iMapArea].components.clear();
}

void clearLightsList(int iMapArea)
{
	mapAreas[iMapArea].lights.clear();
}

void clearAllLightmapPixels(int iMapArea)
{
	for (unsigned int i=0; i<mapAreas[iMapArea].allLightmapPixels.size(); i++)
	{
		memoryFree(mapAreas[iMapArea].allLightmapPixels[i]);
	}
	mapAreas[iMapArea].allLightmapPixels.clear();
}

void clearVisibleQuadsList(int iMapArea)
{
	mapAreas[iMapArea].visibleQuads.clear();
}

void clearCollisionBlocksList(int iMapArea)
{
	mapAreas[iMapArea].collisionBlocks.clear();
}

void clearLightmaps(int iMapArea)
{
	// die components list gibt es nur im edi format
	for ( unsigned int i1=mapAreas[iMapArea].components.size(); i1>0; i1-- )
	{
		for ( unsigned int s1=6; s1>0; s1--)
		{
			mapAreas[iMapArea].components[i1-1].surfaces[s1-1].clearLightmap();
		}
	}

	// auch diese list wird nur im edi format erzeugt
	clearAllLightmapPixels(iMapArea);
	
	// die visibleQuads list gibt es nur im run format
	for ( unsigned int i1=mapAreas[iMapArea].visibleQuads.size(); i1>0; i1-- )
	{
		mapAreas[iMapArea].visibleQuads[i1-1].clearLightmap();
	}
}

void switchMapArea(int iMapArea)
{
	iActMapArea = iMapArea;
}

void addNewMapArea(char *szName)
{
	mapArea_t mapArea;

	strcpy(mapArea.szName, szName);
	
	memset(&mapArea.mapAreaInfo, 0, sizeof(mapAreaInfo_t));
	
	// Standards setzen für mapAreaInfo
	mapArea.mapAreaInfo.iLightmapped		= 1;
	mapArea.mapAreaInfo.uiFogColor			= 0xff000000;
	mapArea.mapAreaInfo.fFogStartDistance	= 1.0f;
	mapArea.mapAreaInfo.fFogEndDistance		= 25.0f;
	mapArea.mapAreaInfo.fZClippingDistance	= 100.0f;
	
	mapAreas.push_back(mapArea);
}

void clearMapArea(int iMapArea)
{
	clearLightmaps(iMapArea);
	clearLightsList(iMapArea);
	clearComponents(iMapArea);
	clearVisibleQuadsList(iMapArea);
	clearCollisionBlocksList(iMapArea);
}

void deleteMapArea(int iMapArea)
{
	clearMapArea(iMapArea);

	mapAreas.erase(mapAreas.begin()+iMapArea, mapAreas.begin()+iMapArea+1);

	cout << "mapArea: " << iMapArea << " erased." << endl;

	updateConnectors();

	iActMapArea = 0;
}

void clearMap()
{
	for (int i=mapAreas.size(); i>0; i--)
	{
		clearMapArea(i-1);
		clearEnemies(i-1);
	}

	mapAreas.clear();

	clearAllTextureAnimations();

	clearAllMapTextures();

	clearMapConnectors();

	textureEffects::init();
}

void printOpenSaveInfo(MapHeader_Edi_t *header, char *szText)
{
	cout << szText << endl;
	cout << "formatID: " << header->formatID << endl;
	cout << "version: " << header->iVersion << endl;
	cout << "modelPack: " << header->modelPack << endl;
	cout << "startPoint:" << header->startPoint.mapArea << ", " << header->startPoint.startWeaponModelIndex << endl;
	cout << "iMapAreas: " << header->iMapAreas << endl;
	cout << "iConnectors: "	<< header->iConnectors << endl;
	cout << "iTextures: " << header->iTextures << endl;
	cout << "iTextureAnimations: " << header->iTextureAnimations << endl;
}

void loadTextureAnimations(ifstream &infile, int iTextureAnimations)
{
	for (int i=0; i<iTextureAnimations; i++)
	{
		textureAnimation_t textureAnimation_buff;

		int iFrames;
		
		// szName
		infile.read( (char*)&textureAnimation_buff.szName, 50 * sizeof(char) );
		
		// fFrameDelay
		infile.read( (char*)&textureAnimation_buff.fFrameDelay, sizeof(float) );
		
		// iFrames (nur für das Laden)
		infile.read( (char*)&iFrames, sizeof(int) );

		// frameTextureMapIndex
		for (int j=0; j<iFrames; j++)
		{
			unsigned int textureMapIndex;

			infile.read( (char*)&textureMapIndex, sizeof(unsigned int) );
			
			textureAnimation_buff.frameTextureMapIndex.push_back(textureMapIndex);
		}
		
		// iRunningFrameIndex
		textureAnimation_buff.iRunningFrameIndex = 0;
		
		// fRunningDelay
		textureAnimation_buff.fRunningDelay = 0.0f;

		textureAnimations.push_back(textureAnimation_buff);
	}
}

void saveTextureAnimations(ofstream &outfile, int iTextureAnimations)
{
	for (int i=0; i<iTextureAnimations; i++)
	{
		textureAnimation_t *pAnim = &textureAnimations[i];

		int iFrames = pAnim->frameTextureMapIndex.size();

		// szName
		outfile.write( (char*)&pAnim->szName, 50 * sizeof(char) );

		// fFrameDelay
		outfile.write( (char*)&pAnim->fFrameDelay, sizeof(float) );
		
		// iFrames (nur für das Laden)
		outfile.write( (char*)&iFrames, sizeof(int) );

		// frameTextureMapIndex
		for (int j=0; j<iFrames; j++)
		{
			outfile.write( (char*)&pAnim->frameTextureMapIndex[j], sizeof(unsigned int) );
		}
	}
}

void openMapFormatEdi(char *mapname)
{
	char szFilename[256];
	
	sprintf(szFilename, "%s/%s", MAP_PATH, mapname);

	clearMap();

	ifstream infile(szFilename, ios::in|ios::binary);

	// mapHeader

		MapHeader_Edi_t mapHeader;

		infile.read( (char*)&mapHeader.formatID[0], 9*sizeof(char) );
		infile.read( (char*)&mapHeader.modelPack[0], 80*sizeof(char) );
		infile.read( (char*)&mapHeader.filler, 3*sizeof(char) );

		if (strcmp(mapHeader.formatID, szMapFormatEDIKennung))
		{
			mapHeader.iVersion = 0;
		}
		else
		{
			infile.read( (char*)&mapHeader.iVersion, sizeof(unsigned int) );
		}
		
		if (mapHeader.iVersion >= 10002)
		{
			infile.read( (char*)&mapHeader.iLoadCompleteMap, sizeof(int) );
		}
		else
		{
			mapHeader.iLoadCompleteMap = 1;
		}

		levelInfo.iLoadCompleteMap = mapHeader.iLoadCompleteMap;

		infile.read( (char*)&mapHeader.startPoint, sizeof(startPoint_t) );
		
		infile.read( (char*)&mapHeader.iMapAreas, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iConnectors, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iTextures, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iTextureAnimations, sizeof(unsigned int) );
				
		printOpenSaveInfo(&mapHeader, "Loading EDI:");
				
		// modelPack
		
			unloadMD3Models();
			strcpy(activeModelPack.szName, "");

			if (strcmp(mapHeader.modelPack, ""))
			{
				activeModelPack.loadPack(mapHeader.modelPack);
				activeModelPack.loadMD3Models();
			}
		
		// startPoint
		
			startPoint = mapHeader.startPoint;
		
		// global connectors:
		
			mapConnector_t mapConnector_buff;
			
			for (unsigned int i=0; i<mapHeader.iConnectors; i++)
			{
				infile.read( (char*)&mapConnector_buff, sizeof(mapConnector_t) );
				mapConnectors.push_back(mapConnector_buff);
			}

		// global textures:
	
			mapTexture_t mapTexture_buff;
	
			for (unsigned int i=0; i<mapHeader.iTextures; i++)
			{
				infile.read( (char*)&mapTexture_buff, sizeof(mapTexture_t) );
				loadMapTexture(mapTexture_buff.szFile, mapTexture_buff.bMipmaps, mapTexture_buff.iPixelFormatT, mapTexture_buff.bVRAM, mapTexture_buff.fRepeat, mapTexture_buff.alignment, mapTexture_buff.bLightmapAllowed);
			}

		// global texture animations:
	
			loadTextureAnimations(infile, mapHeader.iTextureAnimations);

	// mapAreaHeader
	
		for (unsigned int iMapArea = 0; iMapArea < mapHeader.iMapAreas; iMapArea++)
		{
			MapAreaHeader_Edi_t mapAreaHeader;
			
			infile.read( (char*)&mapAreaHeader.szName[0], 80*sizeof(char) );
			infile.read( (char*)&mapAreaHeader.iComponents, sizeof(unsigned int) );
			infile.read( (char*)&mapAreaHeader.iEnemies, sizeof(unsigned int) );
			infile.read( (char*)&mapAreaHeader.iLights, sizeof(unsigned int) );
			infile.read( (char*)&mapAreaHeader.iLightmaps, sizeof(unsigned int) );
			infile.read( (char*)&mapAreaHeader.iLMPSize, sizeof(unsigned int) );

			addNewMapArea(mapAreaHeader.szName);
			
			if (mapHeader.iVersion >= 10000)
			{
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.iType, sizeof(int) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.iHasSkybox, sizeof(int) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.skyboxTextureMapIndex, 6*sizeof(unsigned int) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.iLightmapped, sizeof(int) );
			}

			if (mapHeader.iVersion >= 10001)
			{
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.iFog, sizeof(int) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.uiFogColor, sizeof(unsigned int) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.fFogStartDistance, sizeof(float) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.fFogEndDistance, sizeof(float) );
				infile.read( (char*)&mapAreas[iMapArea].mapAreaInfo.fZClippingDistance, sizeof(float) );
			}

			// components

				CComponent component_buff;

				for (unsigned int i=0; i<mapAreaHeader.iComponents; i++)
				{
//					infile.read( (char*)&component_buff, sizeof(CComponent) );

					infile.read( (char*)&component_buff.surfaces, 6 * sizeof(Quad_t) );
					infile.read( (char*)&component_buff.vertices, 8 * sizeof(Vector4) );
					infile.read( (char*)&component_buff.bIsConnector, sizeof(bool) );
					infile.read( (char*)&component_buff.filler1, 3 * sizeof(char) );
					infile.read( (char*)&component_buff.iConnectorIndex, sizeof(int) );
					infile.read( (char*)&component_buff.bIsCollisionObject, sizeof(bool) );
					infile.read( (char*)&component_buff.filler2, 3 * sizeof(char) );

					if (mapHeader.iVersion >= 10003)
					{
						infile.read( (char*)&component_buff.bIsLevelExit, sizeof(bool) );
						infile.read( (char*)&component_buff.filler3, 3 * sizeof(char) );
					}
					else
					{
						component_buff.bIsLevelExit = false;
					}

					mapAreas[iMapArea].components.push_back(component_buff);
				}

			// enemies

				enemyEditorInfo_t enemyEditorInfo;

				for (unsigned int i=0; i<mapAreaHeader.iEnemies; i++)
				{
					infile.read( (char*)&enemyEditorInfo.orientation, sizeof(orientation_t) );
					infile.read( (char*)&enemyEditorInfo.iModelIndex, sizeof(int) );
					infile.read( (char*)&enemyEditorInfo.iSkinNr, sizeof(int) );
					infile.read( (char*)&enemyEditorInfo.iWeaponModelIndex, sizeof(int) );

					if (mapHeader.iVersion >= 10006)
						infile.read( (char*)&enemyEditorInfo.iBeginHitPoints, sizeof(int) );
					else
						enemyEditorInfo.iBeginHitPoints = 1;

					addNewEnemyToMapArea(iMapArea, enemyEditorInfo.orientation, enemyEditorInfo.iModelIndex, enemyEditorInfo.iSkinNr, enemyEditorInfo.iWeaponModelIndex, enemyEditorInfo.iBeginHitPoints);
				}

			// lights

				CLight light_buff;

				for (unsigned int i=0; i<mapAreaHeader.iLights; i++)
				{
					infile.read( (char*)&light_buff, sizeof(CLight) );
					mapAreas[iMapArea].lights.push_back(light_buff);
				}

			// lightmaps

				unsigned char *lightmap_buff;

				if (mapAreaHeader.iLightmaps > 0)
				{
					unsigned int iLMPSize = mapAreaHeader.iLMPSize;
					
					lightmap_buff = (unsigned char*)memoryAllocate(iLMPSize * iLMPSize * 4);
					
					// schleife über lightmapped quads

					for ( unsigned int i1=0; i1<mapAreas[iMapArea].components.size(); i1++ )
					{
						for ( unsigned int s1=0; s1<6; s1++)
						{
							if (mapAreas[iMapArea].components[i1].surfaces[s1].bLightmap)
							{
								infile.read( (char*)lightmap_buff, iLMPSize * iLMPSize * 4 );
								saveLightmapForQuad(&mapAreas[iMapArea].components[i1].surfaces[s1], lightmap_buff, iLMPSize, true, iMapArea);
							}
						}
					}
		
					memoryFree(lightmap_buff);
				}
		}

	infile.clear();
	infile.close();

	switchMapArea(0);
}

unsigned int findVisibleQuads(int iMapArea)
{
	unsigned int i = 0;
	
	for ( unsigned int i1=0; i1<mapAreas[iMapArea].components.size(); i1++ )
	{
		for ( unsigned int s1=0; s1<6; s1++)
		{
			if (mapAreas[iMapArea].components[i1].surfaces[s1].bVisible)
				i++;
		}
	}
	return i;
}

unsigned int findLightmappedQuads(int iMapArea)
{
	unsigned int i = 0;
	
	for ( unsigned int i1=0; i1<mapAreas[iMapArea].components.size(); i1++ )
	{
		for ( unsigned int s1=0; s1<6; s1++)
		{
			if (mapAreas[iMapArea].components[i1].surfaces[s1].bLightmap)
				i++;
		}
	}
	return i;
}

void saveMapFormatEdi(char *mapname)
{
	char szFilename[256];
	
	sprintf(szFilename, "%s/%s", MAP_PATH, mapname);

	ofstream outfile(szFilename, ios::out|ios::trunc|ios::binary);

	// mapHeader

		unsigned int iMapAreas = mapAreas.size();
	
		MapHeader_Edi_t mapHeader;
	
		strcpy(mapHeader.formatID, szMapFormatEDIKennung);
		strcpy(mapHeader.modelPack, activeModelPack.szName);
		strcpy(mapHeader.filler, "");
		mapHeader.iVersion				= iAktMapFormatEDIVersion;
		mapHeader.iLoadCompleteMap		= levelInfo.iLoadCompleteMap;
		mapHeader.startPoint			= startPoint;
		mapHeader.iMapAreas				= iMapAreas;
		mapHeader.iConnectors			= mapConnectors.size();
		mapHeader.iTextures				= mapTextures.size();
		mapHeader.iTextureAnimations	= textureAnimations.size();

//		printOpenSaveInfo(&mapHeader, "Saving EDI:");

		outfile.write( (char*)&mapHeader, sizeof(MapHeader_Edi_t) );

		// global connectors:

			for (unsigned int i=0; i<mapHeader.iConnectors; i++)
				outfile.write( (char*)&mapConnectors[i], sizeof(mapConnector_t) );

		// global textures:
		
			for (unsigned int i=0; i<mapHeader.iTextures; i++)
				outfile.write( (char*)&mapTextures[i], sizeof(mapTexture_t) );

		// global texture animations:
		
			saveTextureAnimations(outfile, mapHeader.iTextureAnimations);
	
	// mapAreaHeader
		
		for (unsigned int iMapArea = 0; iMapArea < iMapAreas; iMapArea++)
		{
			MapAreaHeader_Edi_t mapAreaHeader;

			unsigned int iLightmappedQuads = findLightmappedQuads(iMapArea);
			
			strcpy(mapAreaHeader.szName, mapAreas[iMapArea].szName);
			mapAreaHeader.iComponents	= mapAreas[iMapArea].components.size();
			mapAreaHeader.iEnemies		= mapAreas[iMapArea].enemies.size();
			mapAreaHeader.iLights		= mapAreas[iMapArea].lights.size();
			
			if (iLightmappedQuads == mapAreas[iMapArea].allLightmapPixels.size())
				mapAreaHeader.iLightmaps = iLightmappedQuads;
			else
				mapAreaHeader.iLightmaps = 0;

			mapAreaHeader.iLMPSize		= LIGHTMAP_SIZE;
			
			memcpy(&mapAreaHeader.mapAreaInfo, &mapAreas[iMapArea].mapAreaInfo, sizeof(mapAreaInfo_t));
			
			outfile.write( (char*)&mapAreaHeader, sizeof(MapAreaHeader_Edi_t) );
			
			// components

				for (unsigned int i=0; i<mapAreas[iMapArea].components.size(); i++)
					outfile.write( (char*)&mapAreas[iMapArea].components[i], sizeof(CComponent) );

			// enemies

				for (unsigned int i=0; i<mapAreas[iMapArea].enemies.size(); i++)
				{
					enemyEditorInfo_t enemyEditorInfo;
					
					mapAreas[iMapArea].enemies[i].fillEnemyEditorInfo(&enemyEditorInfo);

					outfile.write( (char*)&enemyEditorInfo, sizeof(enemyEditorInfo_t) );
				}

			// lights

				for (unsigned int i=0; i<mapAreas[iMapArea].lights.size(); i++)
					outfile.write( (char*)&mapAreas[iMapArea].lights[i], sizeof(CLight) );

			// lightmaps
			
				// lightmapped quads müssen mit Anzahl Lightmaps übereinstimmen, sonst hier keine Lightmaps speichern...
	
				if (iLightmappedQuads == mapAreas[iMapArea].allLightmapPixels.size())
				{
					for (unsigned int i=0; i<iLightmappedQuads; i++)
						outfile.write( (char*)mapAreas[iMapArea].allLightmapPixels[i], LIGHTMAP_SIZE*LIGHTMAP_SIZE*4 );
				}
		}
	
	outfile.clear();
	outfile.close();
}

void loadMapAreaFormatRun(ifstream &infile, int iMapArea, bool bLoadData)
{
	MapAreaHeader_Run_t mapAreaHeader;

	infile.read( (char*)&mapAreaHeader, sizeof(MapAreaHeader_Run_t) );

	unsigned int iLMPSize = mapAreaHeader.iLMPSize;

	if (bLoadData)
	{
		strcpy(mapAreas[iMapArea].szName, mapAreaHeader.szName);
		
		memcpy(&mapAreas[iMapArea].mapAreaInfo, &mapAreaHeader.mapAreaInfo, sizeof(mapAreaInfo_t));
				
		// visible quads

		Quad_t quad_buff;

		for (unsigned int i=0; i<mapAreaHeader.iVisibleQuads; i++)
		{
			infile.read( (char*)&quad_buff, sizeof(Quad_t) );
			mapAreas[iMapArea].visibleQuads.push_back(quad_buff);
		}

		// collision blocks

		CollisionBlock_t cb_buff;

		for (unsigned int i=0; i<mapAreaHeader.iCollisionBlocks; i++)
		{
			infile.read( (char*)&cb_buff, sizeof(CollisionBlock_t) );
			mapAreas[iMapArea].collisionBlocks.push_back(cb_buff);
		}

		// enemies
		
		bool bLoadEnemies = (mapAreas[iMapArea].enemies.size() == 0); // wenn noch nicht geladen für diesen Area...

		enemyEditorInfo_t enemyEditorInfo;

		for (unsigned int i=0; i<mapAreaHeader.iEnemies; i++)
		{
			infile.read( (char*)&enemyEditorInfo, sizeof(enemyEditorInfo_t) );

			if (bLoadEnemies)
				addNewEnemyToMapArea(iMapArea, enemyEditorInfo.orientation, enemyEditorInfo.iModelIndex, enemyEditorInfo.iSkinNr, enemyEditorInfo.iWeaponModelIndex, enemyEditorInfo.iBeginHitPoints);
		}

		// lights

		CLight light_buff;

		for (unsigned int i=0; i<mapAreaHeader.iLights; i++)
		{
			infile.read( (char*)&light_buff, sizeof(CLight) );
			mapAreas[iMapArea].lights.push_back(light_buff);
		}

		// lightmaps

		if (mapAreaHeader.iLightmaps > 0)
		{
			unsigned char *lightmap_buff = (unsigned char*)memoryAllocate(iLMPSize * iLMPSize * 4);
			
			// schleife über visible quads (aber nicht alle vis. quads haben eine lmp, nur bLightmap)

			for ( unsigned int i1=0; i1<mapAreaHeader.iVisibleQuads; i1++ )
			{
				if (mapAreas[iMapArea].visibleQuads[i1].bLightmap)
				{
					infile.read( (char*)lightmap_buff, iLMPSize * iLMPSize * 4 );
					saveLightmapForQuad(&mapAreas[iMapArea].visibleQuads[i1], lightmap_buff, iLMPSize, false, iMapArea);
				}
			}
			
			memoryFree(lightmap_buff);
		}

	}
	else
	{
		infile.seekg( mapAreaHeader.iVisibleQuads * sizeof(Quad_t), ios::cur);				// visible quads
		infile.seekg( mapAreaHeader.iCollisionBlocks * sizeof(CollisionBlock_t), ios::cur);	// collision blocks
		infile.seekg( mapAreaHeader.iEnemies * sizeof(enemyEditorInfo_t), ios::cur);		// enemies
		infile.seekg( mapAreaHeader.iLights * sizeof(CLight), ios::cur);					// lights
		infile.seekg( mapAreaHeader.iLightmaps * iLMPSize * iLMPSize * 4, ios::cur);		// lightmaps
	}

}

void testLoadModels()
{
	char packName[] = "defaultPack";
	
	unloadMD3Models();

	strcpy(activeModelPack.szName, packName);

	activeModelPack.loadPack(packName);
	activeModelPack.loadMD3Models();
}

bool openMapFormatRun(char *mapname, loadingType_e loadingType, unsigned int iAreaToLoad)
{
	char szFilename[200];
	
	sprintf(szFilename, "%s/%s", MAP_PATH, mapname);

	ifstream infile(szFilename, ios::in|ios::binary);

	if (infile.fail())
		return false;

	if (loadingType == LOAD_MAP)
	{
		clearMap();

		// mapHeader

		MapHeader_Run_t mapHeader;

		infile.read( (char*)&mapHeader.formatID[0], 9*sizeof(char) );
		infile.read( (char*)&mapHeader.modelPack[0], 80*sizeof(char) );
		infile.read( (char*)&mapHeader.filler[0], 3*sizeof(char) );

		if (strcmp(mapHeader.formatID, szMapFormatRUNKennung))
		{
			mapHeader.iVersion = 0;
		}
		else
		{
			infile.read( (char*)&mapHeader.iVersion, sizeof(unsigned int) );
		}

		if (mapHeader.iVersion != iAktMapFormatRUNVersion)
		{
			// cannot load...
			writeLog("Old map format! Recreate runtime version in the editor.");
			return false;
		}

		infile.read( (char*)&mapHeader.iLoadCompleteMap, sizeof(int) );
		infile.read( (char*)&mapHeader.startPoint, sizeof(startPoint_t) );
		infile.read( (char*)&mapHeader.iEnemiesToKill, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iMapAreas, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iTextures, sizeof(unsigned int) );
		infile.read( (char*)&mapHeader.iTextureAnimations, sizeof(unsigned int) );
		
		levelInfo.iLoadCompleteMap = mapHeader.iLoadCompleteMap;
		
			// modelPack

				if (strcmp(activeModelPack.szName, mapHeader.modelPack))
				{
					unloadMD3Models();
					strcpy(activeModelPack.szName, "");

					if (strcmp("", mapHeader.modelPack))
					{
						activeModelPack.loadPack(mapHeader.modelPack);
						activeModelPack.loadMD3Models();
					}
				}

			// startPoint
		
				startPoint = mapHeader.startPoint;

				g_iEnemiesToKill = mapHeader.iEnemiesToKill;
				
				iAreaToLoad = startPoint.mapArea;

			// global textures:
		
				mapTexture_t mapTexture_buff;

				for (unsigned int i=0; i<mapHeader.iTextures; i++)
				{
					infile.read( (char*)&mapTexture_buff, sizeof(mapTexture_t) );
					loadMapTexture(mapTexture_buff.szFile, mapTexture_buff.bMipmaps, mapTexture_buff.iPixelFormatT, mapTexture_buff.bVRAM, mapTexture_buff.fRepeat, mapTexture_buff.alignment, mapTexture_buff.bLightmapAllowed);
				}

			// global texture animations:

				loadTextureAnimations(infile, mapHeader.iTextureAnimations);

		// mapAreaHeader
	
			for (unsigned int iMapArea = 0; iMapArea < mapHeader.iMapAreas; iMapArea++)
			{
				addNewMapArea("");

				// Offset für map areas merken für späteres direktes Laden von anderer map area

				mapAreas[iMapArea].areaPosition = infile.tellg();
				
				cout << "Position of map area " << iMapArea << ": " << mapAreas[iMapArea].areaPosition << endl;

				if (levelInfo.iLoadCompleteMap)
				{
					loadMapAreaFormatRun(infile, iMapArea, true );
				}
				else
				{
					loadMapAreaFormatRun(infile, iMapArea, (iMapArea == iAreaToLoad) );
				}
			}
	}
	else if (loadingType == CHANGE_MAP_AREA && !levelInfo.iLoadCompleteMap)
	{
		clearMapArea(iActMapArea);

		// Offset holen aus mapAreas
		
		infile.seekg(mapAreas[iAreaToLoad].areaPosition);
		
		loadMapAreaFormatRun(infile, iAreaToLoad, true);
	}
	
	infile.clear();
	infile.close();

	switchMapArea(iAreaToLoad);

	return true;
}

void resetCurrentMap(char *mapname)
{
	textureEffects::init();

	for (unsigned int iMapArea = 0; iMapArea < mapAreas.size(); iMapArea++)
	{
		for (unsigned int iEnemy = 0; iEnemy < mapAreas[iMapArea].enemies.size(); iEnemy++)
		{
			mapAreas[iMapArea].enemies[iEnemy].reset();
		}
	}
	
	openMapFormatRun(mapname, CHANGE_MAP_AREA, startPoint.mapArea);
}

void makeCollisionBlock(CollisionBlock_t *cb, int iSrcArea, int iSrcComponent)
{
	float fXMin, fXMax, fYMin, fYMax, fZMin, fZMax;

	CComponent *pSrcComp = &mapAreas[iSrcArea].components[iSrcComponent];
	
	// Boundingbox aus Mittelwerten der jeweils äußeren Vertices bilden
	
	fXMin = (pSrcComp->vertices[0].x + pSrcComp->vertices[3].x + pSrcComp->vertices[4].x + pSrcComp->vertices[7].x) / 4.0f ;
	fXMax = (pSrcComp->vertices[1].x + pSrcComp->vertices[2].x + pSrcComp->vertices[5].x + pSrcComp->vertices[6].x) / 4.0f ;
	fYMin = (pSrcComp->vertices[0].y + pSrcComp->vertices[1].y + pSrcComp->vertices[4].y + pSrcComp->vertices[5].y) / 4.0f ;
	fYMax = (pSrcComp->vertices[2].y + pSrcComp->vertices[3].y + pSrcComp->vertices[6].y + pSrcComp->vertices[7].y) / 4.0f ;
	fZMin = (pSrcComp->vertices[4].z + pSrcComp->vertices[5].z + pSrcComp->vertices[6].z + pSrcComp->vertices[7].z) / 4.0f ;
	fZMax = (pSrcComp->vertices[0].z + pSrcComp->vertices[1].z + pSrcComp->vertices[2].z + pSrcComp->vertices[3].z) / 4.0f ;
	
	cb->collisionBB		= AABB_t(Vector4(fXMin, fYMin, fZMin), Vector4(fXMax, fYMax, fZMax));
	cb->bIsConnector	= false;
	cb->iTargetArea		= 0;
	cb->vTargetPosition	= Vector4(0,0,0);
	cb->bIsLevelExit	= false;

	if (pSrcComp->bIsLevelExit)
	{
		cb->bIsLevelExit = true;
	}
	else if (pSrcComp->bIsConnector)
	{
		mapConnector_t *pConn = &mapConnectors[pSrcComp->iConnectorIndex];

		if (pConn->bActive)
		{			
			// bIsConnector:

			cb->bIsConnector = true;

			// iTargetArea bestimmen:

			CComponent *pDstComp;
			
			if (pConn->iArea1 != iSrcArea)
			{
				cb->iTargetArea = pConn->iArea1;
				pDstComp = &mapAreas[pConn->iArea1].components[pConn->iComponent1];
			}
			else
			{
				cb->iTargetArea = pConn->iArea2;
				pDstComp = &mapAreas[pConn->iArea2].components[pConn->iComponent2];
			}
			
			// vTargetPosition bestimmen:
			
			Vector4 vMidSrc = pSrcComp->getMid();
			Vector4 vMidDst = pDstComp->getMid();
			
			Vector4 vMinDst = pDstComp->getMin();
			Vector4 vMaxDst = pDstComp->getMax();
			
			Vector4 vSrcToDst = (vMidDst-vMidSrc);
			
			float fOffset = 0.5f;
			
			if (fabs(vSrcToDst.z) > fabs(vSrcToDst.x))
			{
				// north-south connector (z-axis)

				cb->vTargetPosition.x = vMidDst.x;
				
				if (vSrcToDst.z < 0)
					cb->vTargetPosition.z = vMinDst.z - fOffset;
				else
					cb->vTargetPosition.z = vMaxDst.z + fOffset;
			}
			else
			{
				// west-east connector (x-axis)

				cb->vTargetPosition.z = vMidDst.z;
				
				if (vSrcToDst.x < 0)
					cb->vTargetPosition.x = vMinDst.x - fOffset;
				else
					cb->vTargetPosition.x = vMaxDst.x + fOffset;
			}
		}
	}
}

int countCollisionBlocks(int iMapArea)
{
	int iCollisionBlocks = 0;
	
	for ( unsigned int i=0; i<mapAreas[iMapArea].components.size(); i++ )
	{
		if ( mapAreas[iMapArea].components[i].bIsCollisionObject )
			iCollisionBlocks++;
	}
	
	return iCollisionBlocks;
}

int countAllEnemies()
{
	int iEnemies = 0;

	for (unsigned int iMapArea = 0; iMapArea < mapAreas.size(); iMapArea++)
	{
		iEnemies += mapAreas[iMapArea].enemies.size();
	}

	return iEnemies;
}

void saveMapFormatRun(char *mapname)
{
	char szFilename[200];

	sprintf(szFilename, "%s/%s", MAP_PATH, mapname);

	ofstream outfile(szFilename, ios::out|ios::trunc|ios::binary);

	// mapHeader

		unsigned int iMapAreas = mapAreas.size();
	
		MapHeader_Run_t mapHeader;
	
		strcpy(mapHeader.formatID, szMapFormatRUNKennung);
		strcpy(mapHeader.modelPack, activeModelPack.szName);
		strcpy(mapHeader.filler, "");
		mapHeader.iVersion				= iAktMapFormatRUNVersion;
		mapHeader.iLoadCompleteMap		= levelInfo.iLoadCompleteMap;
		mapHeader.startPoint			= startPoint;
		mapHeader.iEnemiesToKill		= countAllEnemies();
		mapHeader.iMapAreas				= iMapAreas;
		mapHeader.iTextures				= mapTextures.size();
		mapHeader.iTextureAnimations	= textureAnimations.size();

		outfile.write( (char*)&mapHeader, sizeof(MapHeader_Run_t) );

		// global textures:
		
			for (unsigned int i=0; i<mapHeader.iTextures; i++)
				outfile.write( (char*)&mapTextures[i], sizeof(mapTexture_t) );

		// global texture animations:
		
			saveTextureAnimations(outfile, mapHeader.iTextureAnimations);
	
	// mapAreaHeader
		
		for (unsigned int iMapArea = 0; iMapArea < iMapAreas; iMapArea++)
		{
			MapAreaHeader_Run_t mapAreaHeader;

			unsigned int iVisibleQuads		= findVisibleQuads(iMapArea);
			unsigned int iLightmappedQuads	= findLightmappedQuads(iMapArea);
			
			strcpy(mapAreaHeader.szName, mapAreas[iMapArea].szName);

			mapAreaHeader.iVisibleQuads		= iVisibleQuads;
			mapAreaHeader.iCollisionBlocks	= countCollisionBlocks(iMapArea);
			mapAreaHeader.iEnemies			= mapAreas[iMapArea].enemies.size();
			mapAreaHeader.iLights			= mapAreas[iMapArea].lights.size();
			
			if (iLightmappedQuads == mapAreas[iMapArea].allLightmapPixels.size())
				mapAreaHeader.iLightmaps = iLightmappedQuads;
			else
				mapAreaHeader.iLightmaps = 0;

			mapAreaHeader.iLMPSize			= LIGHTMAP_SIZE;
			
			memcpy(&mapAreaHeader.mapAreaInfo, &mapAreas[iMapArea].mapAreaInfo, sizeof(mapAreaInfo_t));

			outfile.write( (char*)&mapAreaHeader, sizeof(MapAreaHeader_Run_t) );
			
			// visible quads

				for ( unsigned int i1=0; i1<mapAreas[iMapArea].components.size(); i1++ )
				{
					for (unsigned int s1=0; s1<6; s1++)
					{
						Quad_t *quad = &mapAreas[iMapArea].components[i1].surfaces[s1];
						
						if (quad->bVisible)
							outfile.write( (char*)quad, sizeof(Quad_t) );
					}
				}
			
			// collision blocks
				
				for ( int iLoop=0; iLoop<2; iLoop++ ) // zuerst die levelexits und connectors speichern (um zuerst mit ihnen zu kollidieren)
				{
					for ( unsigned int i=0; i<mapAreas[iMapArea].components.size(); i++ )
					{
						CComponent *comp = &mapAreas[iMapArea].components[i];
					
						if (iLoop == 0 && !comp->bIsConnector && !comp->bIsLevelExit)	// 1. Durchgang: levelexits und connectors speichern
							continue;
							
						if (iLoop == 1 && (comp->bIsConnector || comp->bIsLevelExit))	// 2. Durchgang: andere speichern
							continue;
						
						if ( comp->bIsCollisionObject )
						{
							CollisionBlock_t cb_buff;
							makeCollisionBlock(&cb_buff, iMapArea, i);
							outfile.write( (char*)&cb_buff, sizeof(CollisionBlock_t) );
						}
					}
				}

			// enemies

				for (unsigned int i=0; i<mapAreas[iMapArea].enemies.size(); i++)
				{
					enemyEditorInfo_t enemyEditorInfo;
					
					mapAreas[iMapArea].enemies[i].fillEnemyEditorInfo(&enemyEditorInfo);

					outfile.write( (char*)&enemyEditorInfo, sizeof(enemyEditorInfo_t) );
				}

			// lights

				for (unsigned int i=0; i<mapAreas[iMapArea].lights.size(); i++)
					outfile.write( (char*)&mapAreas[iMapArea].lights[i], sizeof(CLight) );

			// lightmaps
			
				// lightmapped quads müssen mit Anzahl Lightmaps übereinstimmen, sonst hier keine Lightmaps speichern...
	
				if (iLightmappedQuads == mapAreas[iMapArea].allLightmapPixels.size())
				{
					for (unsigned int i=0; i<iLightmappedQuads; i++)
						outfile.write( (char*)mapAreas[iMapArea].allLightmapPixels[i], LIGHTMAP_SIZE*LIGHTMAP_SIZE*4 );
				}
		}
	
	outfile.clear();
	outfile.close();
}

void saveLightmapForQuad(Quad_t *quad, unsigned char* pixels, unsigned int lightmapSize, bool bAddToVisQuadsLightmapList, int iMapArea)
{
	if (bAddToVisQuadsLightmapList)
	{
		unsigned int iNextFree = mapAreas[iMapArea].allLightmapPixels.size();

		unsigned char* tmp=0;
			
		mapAreas[iMapArea].allLightmapPixels.push_back(tmp);
			
		mapAreas[iMapArea].allLightmapPixels[iNextFree] = (unsigned char *)memoryAllocate(lightmapSize*lightmapSize*4);

		memcpy(mapAreas[iMapArea].allLightmapPixels[iNextFree], pixels, lightmapSize*lightmapSize*4);
	}
	quad->lightmapId = loadTexture("Lightmap", true, 8888, false, (unsigned int*)pixels, lightmapSize);
	quad->bLightmap  = true;
}
