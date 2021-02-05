// font.cpp

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

#include "font.h"
#include "textures.h"
#include "calog.h"
#include "rendering.h"
#include "paths.h"

#define FONT_TEXTURE_SIZE 256

void CFont::loadTexture(char *szFontFile)
{
	char szFont[256];

	sprintf(szFont, "%s/%s", SYSTEM_PATH, szFontFile);

	TextureId = ::loadTexture(szFont, false, 4444, false);
}

void CFont::loadMetrics(char *szMetricsFile, int iHeight)
{
	char szMetrics[256];
	short buffer[1024];

	sprintf(szMetrics, "%s/%s", SYSTEM_PATH, szMetricsFile);

	mBase		= 0;
	mScale		= 1;
	mTracking	= 0;
	mHeight     = iHeight;

	FILE *file;
	if ((file = fopen(szMetrics, "rb")) == NULL)
	{
		writeLog("LoadFontMetrics failed!");
		return;
	}
	
	fread((unsigned char *)buffer, 1, 2048, file);
	fclose(file);
	
	int a, b, c;
    int y = 0;
	int x;

	int cellHeight = FONT_TEXTURE_SIZE/16;

	int n = 0;
	for (int i=0;i<16;i++)
	{
		for (int j=0;j<16;j++)
		{
			x = (int)buffer[n*4];	// x offset
			a = (int)buffer[n*4+1];	// character width
			b = (int)buffer[n*4+2];
			c = (int)buffer[n*4+3];

			mXPos[n]		= x;
			mYPos[n]		= y;
			mCharWidth[n]	= a+b+c;

			n++;
		}
		y += cellHeight;
	}
	
	// hack!!!
	if (!strcmp(szMetricsFile, "WST_Germ_met.dat"))
	{
		mCharWidth['W'-32] += 6;
		mCharWidth['X'-32] += 5;
		mCharWidth['m'-32] += 5;
	}
}

int CFont::getStringWidth(const char *string) const
{
	int len = 0;
	 
	const char *p = string;
	char ch;

	while (*p)
	{
		ch = *p - 32;
		p++;
		if (ch < 0) continue;
		len += mCharWidth[ch+mBase];
	} 
    return len*mScale;
}

void CFont::renderCharacter(int iSourceX, int iSourceY, int iWidth, int iDestX, int iDestY, unsigned int color)
{
	int destWidth		= iWidth*mScale;
	int destHeight		= mHeight*mScale;
	int iTextureSize	= FONT_TEXTURE_SIZE;

	float fSourceX	= (float)iSourceX / (float)iTextureSize;
	float fSourceY	= (float)iSourceY / (float)iTextureSize;
	float fWidth	= (float)iWidth   / (float)iTextureSize;
	float fHeight	= (float)mHeight  / (float)iTextureSize;

	render2DSprite(iDestX, destWidth, iDestY, destHeight, color, true, TextureId, fSourceX, fWidth, fSourceY, fHeight);
}

void CFont::init(char *szFontFile, char *szMetricsFile, int iHeight)
{
	loadTexture(szFontFile);
	loadMetrics(szMetricsFile, iHeight);
}

void CFont::draw(const char *string, int x, int y, unsigned int color, int align, int mScale)
{
	char *p = (char*)string;
	int dx = x, dy = y;
	
	this->mScale = mScale;

	int width = getStringWidth(string);
			  
	if		(align == TEXT_RIGHT)	dx -= width;
	else if (align == TEXT_CENTER)	dx -= width/2;

	int index;

	while (*p)
	{
		index = (*p - 32) + mBase;

		int iX		= mXPos[index];
		int iY		= mYPos[index];
		int iWidth	= mCharWidth[index];
		
		renderCharacter(iX, iY, iWidth, dx, dy, color);

		dx += (mCharWidth[index] + mTracking) * mScale;
		p++;
   }
}

void CFont::unloadTexture()
{
	::unloadTexture(&TextureId);
}

void startFontRendering(CFont &font, int vpWidth, int vpHeight)
{
	begin2DRendering(vpWidth, vpHeight);
	setCurrentTexture(font.TextureId);
	switchBlendTestX(true);
}

void endFontRendering()
{
	switchBlendTestX(false);
	end2DRendering();
}
