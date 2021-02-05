// textures.h

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

#ifndef CA_TEXTURES_H
#define CA_TEXTURES_H

#include <vector>
using namespace std;

struct mapTexture_t
{
	enum eTexAlignment
	{
		ALIGN_LOCAL,
		ALIGN_WORLD
	};

	bool bMipmaps;
	bool bLightmapAllowed;
	bool bVRAM;
	char szFile[200];
	unsigned int iId;
	int iPixelFormatT;
	eTexAlignment alignment;
	float fRepeat;

	void loadTextureFile(const char *szFile);	// properties von oben müssen vorher gesetzt worden sein, werden verwendet...
	void unload();
};

extern vector<mapTexture_t> mapTextures;

struct textureAnimation_t
{
	char szName[50];
	float fFrameDelay;
	vector<unsigned int> frameTextureMapIndex;
	
	unsigned int iRunningFrameIndex;
	float fRunningDelay;
};

extern vector<textureAnimation_t> textureAnimations;

namespace textureEffects
{
	extern float fMovementValue[];
	extern float fBlendValue[];
	extern float c[];
	extern float fAim[];
	
	void init();
	void advance(float fElapsedTime);
};

void clearAllTextureAnimations();

void loadMapTexture(const char *szFile, bool bMipmaps, int iPixelFormatT, bool bVRAM, float fRepeat, mapTexture_t::eTexAlignment alignment, bool bLightmapAllowed);
void clearAllMapTextures();

void resetVRAM();
unsigned int loadTexture(const char *szFile, bool bMipmaps=false, int iPixelFormatT=5650, bool bVRAM=false, unsigned int *pLightmap=0, int iLightmapSize=32, mapTexture_t::eTexAlignment alignment = mapTexture_t::ALIGN_LOCAL);
void unloadTexture(unsigned int *id);
int getTextureSize(unsigned int id);
void setCurrentTexture(unsigned int iTextureId);
void printTextureInfo();

#endif
