// light.h

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

#ifndef CA_LIGHT_H
#define CA_LIGHT_H

#include "../caUtil/caUtil.h"

#include <vector>
using namespace std;

// class for simple positional light: (später auch mal spotlights...)

struct rgba_t
{
	unsigned int r, g, b, a;
	rgba_t(unsigned int rr, unsigned int gg, unsigned int bb, unsigned int aa) {r=rr; g=gg; b=bb; a=aa;};
	rgba_t() {r=0, g=0; b=0; a=0;};
};

class CLight
{
public:
	Vector4 position;

	rgba_t ambientColor;
	rgba_t diffuseColor;

	float fLinearAttenuation;
	float fQuadraticAttenuation;
	float fSize;
	
	void resize( int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl );
	void render( bool bWithLightmap, bool bOnlyLines, bool bSelected, const CCamera &cam );
	
	Vector4 getAmbientColor_v();
	Vector4 getDiffuseColor_v();
	
	unsigned int getAmbientColor_ui();
	unsigned int getDiffuseColor_ui();
	
	static unsigned int textureId;
	static void loadTexture();
	static void unloadTexture();
private:

	void movePosition(eAxis axis3D, float amount);
	void alignPosition();
};

#endif
