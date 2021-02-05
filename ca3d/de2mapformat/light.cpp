// light.cpp

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

#include "light.h"

void CLight::alignPosition()
{
	float val;
	
	for (int c=0; c<3; c++)
	{
		if		(c==0)	val = position.x;
		else if (c==1)	val = position.y;
		else if (c==2)	val = position.z;
		
		val = roundNK(val, 1);

		if		(c==0)	position.x = val;
		else if (c==1)	position.y = val;
		else if (c==2)	position.z = val;
	}
}

void CLight::movePosition(eAxis axis3D, float amount)
{
	switch (axis3D)
	{
		case X_AXIS:
			position.x += amount;

			break;

		case Y_AXIS:
			position.y += amount;

			break;

		case Z_AXIS:
			position.z += amount;

			break;
	}
	
	alignPosition();
}

void CLight::resize( int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl )
{
	switch (viewType)
	{
		case VIEWTYPE_RIGHT:

			switch (iAxis2D)
			{
				case 1:
					movePosition(Z_AXIS, -amount);
					break;

				case 2:
					movePosition(Y_AXIS, amount);
					break;
			}

			break;
			
		case VIEWTYPE_FRONT:

			switch (iAxis2D)
			{
				case 1:
					movePosition(X_AXIS, amount);
					break;

				case 2:
					movePosition(Y_AXIS, amount);
					break;
			}

			break;
			
		case VIEWTYPE_TOP:

			switch (iAxis2D)
			{
				case 1:
					movePosition(X_AXIS, amount);
					break;

				case 2:
					movePosition(Z_AXIS, -amount);
					break;
			}
			
			break;
			
		case VIEWTYPE_3D:
			break;
		case VIEWTYPE_BOTTOM:
			break;
		case VIEWTYPE_LEFT:
			break;
		case VIEWTYPE_BACK:
			break;
	}
}

Vector4 CLight::getAmbientColor_v()
{
	return Vector4( (float)ambientColor.r / 255.0f, (float)ambientColor.g / 255.0f, (float)ambientColor.b / 255.0f, (float)ambientColor.a / 255.0f);
}

Vector4 CLight::getDiffuseColor_v()
{
	return Vector4( (float)diffuseColor.r / 255.0f, (float)diffuseColor.g / 255.0f, (float)diffuseColor.b / 255.0f, (float)diffuseColor.a / 255.0f);
}

unsigned int CLight::getAmbientColor_ui()
{
	unsigned int final;

	final  = (ambientColor.r <<  0);
	final += (ambientColor.g <<  8);
	final += (ambientColor.b << 16);
	final += (ambientColor.a << 24);

	return final;
}

unsigned int CLight::getDiffuseColor_ui()
{
	unsigned int final;

	final  = (diffuseColor.r <<  0);
	final += (diffuseColor.g <<  8);
	final += (diffuseColor.b << 16);
	final += (diffuseColor.a << 24);

	return final;
}

unsigned int CLight::textureId;

void CLight::render(bool bWithLightmap, bool bOnlyLines, bool bSelected, const CCamera &cam )
{
	unsigned int color = (bSelected ? 0xff0000ff : 0xffff8888);
	renderLook_e look = (bSelected ? RENDER_SOLID : RENDER_LINES);
	
	float fLightRenderSize = fSize;

	if (!bWithLightmap || bOnlyLines || bSelected)
	{
		if (fLightRenderSize == 0.0f) fLightRenderSize = 0.3f;

		renderCube(&position, fLightRenderSize, color, look);
	}
	
	if (!bOnlyLines && (fSize > 0.0f))
	{
		render3DSpriteBegin(true, textureId);
		render3DSprite(cam.getRightLook(), cam.getUpLook(), position, fLightRenderSize, getDiffuseColor_ui());
		render3DSpriteEnd();
	}
}

void CLight::loadTexture()
{
	char szTexture[256];

	sprintf(szTexture, "%s/%s", SYSTEM_PATH, "lighttexture02.lmp");

	textureId = ::loadTexture(szTexture, true, 8888, true, 0, 64, mapTexture_t::ALIGN_LOCAL);
}

void CLight::unloadTexture()
{
	::unloadTexture(&textureId);
}
