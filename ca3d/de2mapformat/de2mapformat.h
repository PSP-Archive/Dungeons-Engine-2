// de2mapformat.h

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

#ifndef DE2MAPFORMAT_H
#define DE2MAPFORMAT_H

#include "component.h"
#include "light.h"
#include "../caUtil/caUtil.h"

// ============================================================================
// ============================================================================

struct startPoint_t
{
	orientation_t orientation;
	int mapArea;
	int startWeaponModelIndex;
	
	void init();
};

extern startPoint_t startPoint;

extern int g_iEnemiesToKill;

// ============================================================================
// ============================================================================

struct mapConnector_t
{
	bool bActive;
	
	bool b1Selected;
	int iArea1;
	int iComponent1;
	
	bool b2Selected;
	int iArea2;
	int iComponent2;
	
	mapConnector_t();

	void init();
};

extern vector<mapConnector_t> mapConnectors;

void clearMapConnectors();

void deleteConnector(int iConnectorIndex);

void updateConnectors();

// ============================================================================
// ============================================================================

struct weaponModelParams_t
{
	char szName[80];
	Vector4	vFirstPersonOffset;
};

struct enemyModelParams_t
{
	char	szName[80];
	int		iSkins;
	char	skins[MAX_SKINS_PER_MODEL][80];
	float	fScaling;
	Vector4	vShootStartPos;
	float	fBBWidth, fBBHeight;
};

struct modelPack_t
{
	char szName[80];
	vector<weaponModelParams_t>	weaponModelParams;
	vector<enemyModelParams_t> enemyModelParams;

	void loadPack(char *szName);
	void savePack();
	void clear();

	void loadMD3Models();
};

extern modelPack_t	activeModelPack;

extern vector<CMD3*>			weaponModels;
extern vector<CAnimatedMD3*>	enemyModels;

void unloadMD3Models();

void addNewEnemyToMapArea(int iMapArea, const orientation_t &orientation, int iModelIndex, int iSkinNr, int iWeaponModelIndex, int iBeginHitPoints);

struct mapAreaInfo_t
{
	int				iType;						// 0=sealed, 1=open
	int				iHasSkybox;					// 0=no, 1=yes
	unsigned int	skyboxTextureMapIndex[6];	// 0=front, 1=back, 2=left, 3=right, 4=top, 5=bottom;
	int				iLightmapped;				// 0=no, 1=yes

	int				iFog;						// 0=no, 1=yes
	unsigned int 	uiFogColor;
	float			fFogStartDistance;
	float			fFogEndDistance;
	
	float			fZClippingDistance;			// bei schwarzem Nebel kann man fZClippingDistance mind. auf fFogEndDistance setzen (oder noch etwas naeher)
};

struct mapArea_t
{
	char						szName[80];
	
	mapAreaInfo_t				mapAreaInfo;
	
	vector<CComponent>			components;
	vector<OAnimatedMD3>		enemies;
	vector<CLight>				lights;
	vector<unsigned char *>		allLightmapPixels;	// nur in MapFormat EDI, berechnete Lightmaps der Quads (die bVisible + bLightmapAllowed)
	
	vector<Quad_t>				visibleQuads;		// nur in MapFormat RUN
	vector<CollisionBlock_t>	collisionBlocks;	// nur in MapFormat RUN
	
	streampos					areaPosition;		// erst beim Laden der map im run format ermitteln...
};

extern vector<mapArea_t> mapAreas;
extern int iActMapArea;

struct levelInfo_t
{
	int iLoadCompleteMap;	// 0=nein, 1=ja
};

extern levelInfo_t levelInfo;

struct MapHeader_Edi_t
{
	char			formatID[9];			// ab sofort nur noch Vielfache von 4 !!! (sonst kommen autom. filler, oder pack aendern)
	char			modelPack[80];
	char			filler[3];
	unsigned int	iVersion;
	int				iLoadCompleteMap;
	startPoint_t	startPoint;
	unsigned int	iMapAreas;
	unsigned int	iConnectors;
	unsigned int	iTextures;
	unsigned int	iTextureAnimations;
};

struct MapHeader_Run_t
{
	char			formatID[9];			// ab sofort nur noch Vielfache von 4 !!! (sonst kommen autom. filler, oder pack aendern)
	char			modelPack[80];
	char			filler[3];
	unsigned int	iVersion;
	int				iLoadCompleteMap;
	startPoint_t	startPoint;
	unsigned int	iEnemiesToKill;
	unsigned int	iMapAreas;
	unsigned int	iTextures;
	unsigned int	iTextureAnimations;
};

struct MapAreaHeader_Edi_t
{
	char szName[80];
	unsigned int	iComponents;
	unsigned int	iEnemies;
	unsigned int	iLights;
	unsigned int	iLightmaps;
	unsigned int	iLMPSize;
	mapAreaInfo_t	mapAreaInfo;
};

struct MapAreaHeader_Run_t
{
	char szName[80];
	unsigned int	iVisibleQuads;
	unsigned int	iCollisionBlocks;
	unsigned int	iEnemies;
	unsigned int	iLights;
	unsigned int	iLightmaps;
	unsigned int	iLMPSize;
	mapAreaInfo_t	mapAreaInfo;
};

enum loadingType_e
{
	LOAD_MAP,
	CHANGE_MAP_AREA
};

void openMapFormatEdi(char *mapname);
void saveMapFormatEdi(char *mapname);
bool openMapFormatRun(char *mapname, loadingType_e loadingType, unsigned int iAreaToLoad);
void saveMapFormatRun(char *mapname);

#define LIGHTMAP_SIZE 32

void saveLightmapForQuad(Quad_t *quad, unsigned char* pixels, unsigned int lightmapSize, bool bAddToVisQuadsLightmapList, int iMapArea);

void loadInitMapTextures();
void clearLightmaps(int iMapArea);
void clearMap();

void addNewMapArea(char *szName);
void deleteMapArea(int iMapArea);
void switchMapArea(int iMapArea);
void resetCurrentMap(char *mapname);

extern bool g_bWholeLevelLoaded;

void testLoadModels();

#endif
