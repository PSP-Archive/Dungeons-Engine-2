// font.h

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

#ifndef CA_FONT_H
#define CA_FONT_H

#include <iostream>
#include <fstream>
using namespace std;

#define MAX_CHAR 256

enum
{
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
};

class CFont
{
public:
	void init(char *szFontFile, char *szMetricsFile, int iHeight);
	void draw(const char *string, int x, int y, unsigned int color=0xffffffff, int align=TEXT_LEFT, int mScale=1);
	void unloadTexture();
	unsigned int TextureId;
	
private:	
	int		mXPos[MAX_CHAR];
	int		mYPos[MAX_CHAR];
	int		mCharWidth[MAX_CHAR];
	int		mHeight;
	int		mBase;
	int		mTracking;
	int		mScale;
	void	loadTexture(char *szFontFile);
	void	loadMetrics(char *szMetricsFile, int iHeight);
	int		getStringWidth(const char *string) const;
	void	renderCharacter(int iSourceX, int iSourceY, int iWidth, int iDestX, int iDestY, unsigned int color);
};

void startFontRendering(CFont &font, int vpWidth=480, int vpHeight=272);
void endFontRendering();

#endif
