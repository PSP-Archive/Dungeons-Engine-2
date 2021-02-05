// rendering.h

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

#ifndef CA_RENDERING_H
#define CA_RENDERING_H

#include "camera.h"
#include "vectorMath.h"
#include "vertex.h"

enum renderLook_e
{
	RENDER_SOLID,
	RENDER_LINES
};

void initGraphicsUnit();

void prepareRendering(unsigned int clearColor = 0xff000000);
void finishRendering(bool vsync = false);

Matrix4 setRenderingProjection(float fFoV = 45.0f, float fAspectRatio = (16.0f/9.0f), float fZNear = 0.1f, float fZFar = 100.0f);
Matrix4 setRenderingProjectionOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
Matrix4 setRenderingView(CCamera &cam);

void matrixStackPush();
void matrixStackPop();
void matrixStackLoadIdentity();
void matrixStackMultiply(float matrix[16]);
void matrixStackTranslate(Vector4 &vT);
void matrixStackRotateX(float angle);
void matrixStackRotateY(float angle);
void matrixStackRotateZ(float angle);
void matrixStackScale(Vector4 &vS);

void begin2DRendering(int vpWidth=480, int vpHeight=272);

void render2DSprite(int iDestX, int iDestWidth, int iDestY, int iDestHeight, unsigned int color=0xffffffff, bool bTextured=false,
					unsigned int iTextureId=0, float fSourceX=0, float fSourceWidth=1, float fSourceY=0, float fSourceHeight=1, bool bAlphaBlended=false);

void end2DRendering();

void render3DSpriteBegin(bool bTextured, unsigned int iTextureId=0);
void render3DSprite(const Vector4 &right, const Vector4 &up, const Vector4 &pos, float fSize, unsigned int color=0xffffffff);
void render3DSpriteEnd();

void flushCache();

void switchTextureRendering(bool on);
void switchBlendTest(bool on);
void switchBlendTestX(bool on);
void switchAlphaTest(bool on, float alphaGrenze = 0.85f);

void clipRenderQuad(Quad_t *quad, bool bWithLightmap, bool bClip=true, bool bMark=false, bool bOnlyLines=false);	// wenn clipping, vorher extractFrustumPlanes aufrufen

void setPointSizeDefault();
void setPointSize(int iSize);
void renderPoint(const Vector4 &pos, unsigned int color);

void setLineWidthDefault();
void setLineWidth(int iWidth);
void renderLine(const Vector4 &begin, const Vector4 end, unsigned int color);

void renderAABB(const Vector4 *position, const AABB_t *aabb, unsigned int color, renderLook_e look);
void renderCube(const Vector4 *position, float fSize, unsigned int color, renderLook_e look);
void clearDepthBuffer();

void renderIndexedTrisTex(int VertexArrayStartPos, short *IndexBuffer, int iFacesCount, VertexMd3 *pVertices, bool bMark); // no clipping

void setViewMatrixIdentity();
void resetViewMatrix();

void setBlendFunctionFixedColor(float fBlendCol);

void renderSkybox(unsigned int *skyboxTextureMapIndex, Vector4 playerPos);

void setFogParameters(unsigned int fogColor, float fStart, float fEnd);
void switchFog(bool on);

#endif
