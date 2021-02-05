// rendering.cpp

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

#include "rendering.h"
#include "vertex.h"
#include "plattform_includes.h"
#include "textures.h"

bool checkSetTextureRepeatOn(bool bLightmap, mapTexture_t *tex, textureSlot_t *slot);
void setLocalTextureOffset(textureSlot_t *pSlot);
void clearLocalTextureOffset(textureSlot_t *pSlot);

#if __APPLE_CC__ | _WIN32

void setViewMatrixIdentity()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void resetViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void matrixStackPush()
{
	glPushMatrix();
}

void matrixStackPop()
{
	glPopMatrix();
}

void matrixStackLoadIdentity()
{
	glLoadIdentity();
}

void matrixStackMultiply(float matrix[16])
{
	glMultMatrixf(matrix);
}

void matrixStackTranslate(Vector4 &vT)
{
	glTranslatef(vT.x, vT.y, vT.z);
}

void matrixStackRotateX(float angle)
{
	glRotatef(angle, 1, 0, 0);
}

void matrixStackRotateY(float angle)
{
	glRotatef(angle, 0, 1, 0);
}

void matrixStackRotateZ(float angle)
{
	glRotatef(angle, 0, 0, 1);
}

void matrixStackScale(Vector4 &vS)
{
	glScalef(vS.x, vS.y, vS.z);
}

void renderIndexedTrisTex(int VertexArrayStartPos, short *IndexBuffer, int iFacesCount, VertexMd3 *pVertices, bool bMark)
{
	int r, g, b, a;
	unsigned int color;

	switchTextureRendering(true);

	glBegin(GL_TRIANGLES);
	
	for (int i=0; i<iFacesCount; i++)
	{
		for (int j=0; j<3; j++)
		{	
			int index = VertexArrayStartPos + (*(IndexBuffer+i*3+j));
			
			if (bMark)
				color = 0xff2222ff;
			else
				color = pVertices[index].color;
				
			splitRGBA(color, r, g, b, a);

			glTexCoord2f( pVertices[index].u, pVertices[index].v );
			glColor4ub(r,g,b,a);
			glVertex3f( pVertices[index].x, pVertices[index].y, pVertices[index].z );
		}
	}
	glEnd();
}

void switchDepthBufferWrites(bool on)
{
	if (on)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
}

void switchMultitexture(bool on)
{
	if (on)
	{
		glEnable(GL_BLEND);
		switchDepthBufferWrites(false);
		glDepthFunc(GL_EQUAL);
	}
	else
	{
		glDisable(GL_BLEND);
		switchDepthBufferWrites(true);
		glDepthFunc(GL_LESS);
	}
}

void setBlendFunctionFixedColor(float fBlendCol)
{
	glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
	glBlendColor(fBlendCol, fBlendCol, fBlendCol, 0);
}

void setBlendFunctionAlpha()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setBlendFunctionMultiplySrcDst()
{
	glBlendFunc(GL_DST_COLOR, GL_ZERO);	
}

void flushCache()
{
}

// TODO: Aufrufe von switchTextureRendering prüfen (evtl. weniger aufrufen, nicht pro Polygon...)
void switchTextureRendering(bool on)
{
	if (on)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void switchTextureRepeatWrap(bool bRepeat)
{
	if (bRepeat)
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}
}

void switchBlendTest(bool on)
{
	if (on)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
		glBlendColor(1,1,1,0);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void switchBlendTestX(bool on)
{
	if (on)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void switchAlphaTest(bool on, float alphaGrenze)
{
	if (on)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, alphaGrenze);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}
}

void setFogParameters(unsigned int fogColor, float fStart, float fEnd)
{
	int r, g, b, a;
	
	splitRGBA(fogColor, r, g, b, a);
	
	float vfogColor[4];
	
	vfogColor[0] = (r/255.0f);
	vfogColor[1] = (g/255.0f);
	vfogColor[2] = (b/255.0f);
	vfogColor[3] = (a/255.0f);

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, vfogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, fStart);
	glFogf(GL_FOG_END, fEnd);
}

void switchFog(bool on)
{
	if (on)
	{
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}
}

void initGraphicsUnit()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void cullBack()
{
	glFrontFace(GL_CCW);
}

void cullFront()
{
	glFrontFace(GL_CW);
}

void prepareRendering(unsigned int clearColor)
{
	int r, g, b, a;
	
	splitRGBA(clearColor, r, g, b, a);
	
	glClearColor(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void clearDepthBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void finishRendering(bool vsync)
{
#if _NO_SDL
    glFlush();
#else
	SDL_GL_SwapBuffers();
#endif
}

Matrix4 setRenderingProjection(float fFoV, float fAspectRatio, float fZNear, float fZFar)
{
	Matrix4 projection;

	glMatrixMode(GL_PROJECTION);
	projection.setupProjection(fFoV, fAspectRatio, fZNear, fZFar);
	glLoadMatrixf((float*)&projection);
	
	return projection;
}

Matrix4 setRenderingProjectionOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Matrix4 projection;

	glMatrixMode(GL_PROJECTION);
	projection.setupProjectionOrtho(left, right, bottom, top, zNear, zFar);
	glLoadMatrixf((float*)&projection);
	
	return projection;
}

Matrix4 setRenderingView(CCamera &cam)
{
	Matrix4 view;
	Vector4 pos, forward, up, right;

	pos		= cam.getPosition();
	forward = cam.getForwardLook();
	up		= cam.getUpLook();
	right   = cam.getRightLook();

	glMatrixMode(GL_MODELVIEW);
	view.setupLookAt(pos, forward, up, right);
	glLoadMatrixf((float*)&view);

	return view;
}

void setTextureOffset(float fXOffset, float fYOffset)
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(fXOffset, fYOffset, 0);
}

void clearTextureOffset()
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

void renderPoly(Polygon_t &poly, bool bLightmap, bool bMark, bool bOnlyLines, int slotNr)
{
	int r, g, b, a, iVertexCount;
	unsigned int color;

	iVertexCount = poly.iVertexCount;

	if (iVertexCount)
	{
		textureSlot_t *pSlot = &poly.pQuadTextureSlot[slotNr];

		Vertex *v = &poly.verts[0];

		mapTexture_t *maptex = &mapTextures[pSlot->textureMapIndex];

		if (slotNr == 0)
		{
			if (poly.bAnimated)
			{
				unsigned int iAnimationIndex = poly.animationIndex;
				unsigned int iFrameIndex = textureAnimations[iAnimationIndex].iRunningFrameIndex;

				maptex = &mapTextures[textureAnimations[iAnimationIndex].frameTextureMapIndex[iFrameIndex]];
			}
		}

		if (bOnlyLines && !bMark)
			switchTextureRendering(false);
		else
			switchTextureRendering(pSlot->bActive);
		
		if (bLightmap)
			setCurrentTexture(poly.lightmapId);
		else if (pSlot->bActive)
			setCurrentTexture(maptex->iId);

		if (!bLightmap)
			setLocalTextureOffset(pSlot);

		if (!checkSetTextureRepeatOn(bLightmap, maptex, pSlot))
			switchTextureRepeatWrap(false);
		else
			switchTextureRepeatWrap(true);

			if (bOnlyLines && !bMark)
			{
				setLineWidth(3);
				glBegin(GL_LINE_STRIP);
			}
			else
				glBegin(GL_TRIANGLE_FAN);

			for (int i=0; i<iVertexCount; i++)
			{
				if (bMark)
					color = 0xff2222ff;
				else
				{
					if (bOnlyLines)
						color = 0xffff0000;
					else
						color = v->color;
				}
				
				splitRGBA(color, r, g, b, a);

				if (bLightmap)
					glTexCoord2f( v->ul, v->vl );
				else if (maptex->alignment == mapTexture_t::ALIGN_LOCAL)
					glTexCoord2f( v->ulocal, v->vlocal );
				else
					glTexCoord2f( v->u, v->v );

				glColor4ub(r,g,b,a);
				glNormal3f( v->nx, v->ny, v->nz );
				glVertex3f( v->x, v->y, v->z );
				v++;
			}
			glEnd();

		if (!bLightmap)
			clearLocalTextureOffset(pSlot);

		if (bOnlyLines && !bMark)
			setLineWidthDefault();
	}
}

void setPointSizeDefault()
{
	glPointSize(1);
}

void setPointSize(int iSize)
{
	glPointSize(3);
}

void renderPoint(const Vector4 &pos, unsigned int color)
{
	int r, g, b, a;

	splitRGBA(color, r, g, b, a);

	switchTextureRendering(false);

	glBegin(GL_POINTS);
		glColor4ub(r,g,b,a);
		glVertex3f( pos.x, pos.y, pos.z );
	glEnd();
}

void setLineWidthDefault()
{
	glLineWidth(1);
}

void setLineWidth(int iWidth)
{
	glLineWidth(3);
}

void renderLine(const Vector4 &begin, const Vector4 end, unsigned int color)
{
	int r, g, b, a;

	splitRGBA(color, r, g, b, a);

	switchTextureRendering(false);

	glBegin(GL_LINES);
		glColor4ub(r,g,b,a);
		glVertex3f( begin.x, begin.y, begin.z );
		glVertex3f( end.x, end.y, end.z );
	glEnd();
}

void rederBoxVerts(Vector4 *verts, unsigned int color, renderLook_e look)
{
	if (look == RENDER_LINES)
	{
		setLineWidth(3);

		renderLine(verts[0], verts[1], color);
		renderLine(verts[1], verts[2], color);
		renderLine(verts[2], verts[3], color);
		renderLine(verts[3], verts[0], color);

		renderLine(verts[4], verts[5], color);
		renderLine(verts[5], verts[6], color);
		renderLine(verts[6], verts[7], color);
		renderLine(verts[7], verts[4], color);

		renderLine(verts[0], verts[4], color);
		renderLine(verts[1], verts[5], color);
		renderLine(verts[2], verts[6], color);
		renderLine(verts[3], verts[7], color);

		setLineWidthDefault();
	}
	else
	{
		int r, g, b, a;

		splitRGBA(color, r, g, b, a);

		switchTextureRendering(false);

		glBegin(GL_QUADS);
			glColor4ub(r,g,b,a);
			glVertex3fv(&verts[0].x); 
			glVertex3fv(&verts[1].x);
			glVertex3fv(&verts[2].x);
			glVertex3fv(&verts[3].x);

			glVertex3fv(&verts[5].x); 
			glVertex3fv(&verts[4].x);
			glVertex3fv(&verts[7].x);
			glVertex3fv(&verts[6].x);

			glVertex3fv(&verts[1].x);
			glVertex3fv(&verts[5].x);
			glVertex3fv(&verts[6].x);
			glVertex3fv(&verts[2].x);

			glVertex3fv(&verts[4].x);
			glVertex3fv(&verts[0].x);
			glVertex3fv(&verts[3].x);
			glVertex3fv(&verts[7].x);

			glVertex3fv(&verts[3].x);
			glVertex3fv(&verts[2].x);
			glVertex3fv(&verts[6].x);
			glVertex3fv(&verts[7].x);

			glVertex3fv(&verts[4].x);
			glVertex3fv(&verts[5].x);
			glVertex3fv(&verts[1].x);
			glVertex3fv(&verts[0].x);
		glEnd();
	}
}

void renderAABB(const Vector4 *position, const AABB_t *aabb, unsigned int color, renderLook_e look)
{
	Vector4 vec[8];

	//   7----6
	//  /|   /|
	// 3----2 |
	// | 4--|-5
	// |/   |/
	// 0----1

	vec[0] = Vector4( (aabb->min.x+position->x), (aabb->min.y+position->y), (aabb->max.z+position->z) );
	vec[1] = Vector4( (aabb->max.x+position->x), (aabb->min.y+position->y), (aabb->max.z+position->z) );
	vec[2] = Vector4( (aabb->max.x+position->x), (aabb->max.y+position->y), (aabb->max.z+position->z) );
	vec[3] = Vector4( (aabb->min.x+position->x), (aabb->max.y+position->y), (aabb->max.z+position->z) );
	vec[4] = Vector4( (aabb->min.x+position->x), (aabb->min.y+position->y), (aabb->min.z+position->z) );
	vec[5] = Vector4( (aabb->max.x+position->x), (aabb->min.y+position->y), (aabb->min.z+position->z) );
	vec[6] = Vector4( (aabb->max.x+position->x), (aabb->max.y+position->y), (aabb->min.z+position->z) );
	vec[7] = Vector4( (aabb->min.x+position->x), (aabb->max.y+position->y), (aabb->min.z+position->z) );

	rederBoxVerts(vec, color, look);
}

void renderCube(const Vector4 *position, float fSize, unsigned int color, renderLook_e look)
{
	Vector4 vec[8];

	//   7----6
	//  /|   /|
	// 3----2 |
	// | 4--|-5
	// |/   |/
	// 0----1

	vec[0] = Vector4(position->x - (fSize/2.0f), position->y - (fSize/2.0f), position->z + (fSize/2.0f) );
	vec[1] = Vector4(position->x + (fSize/2.0f), position->y - (fSize/2.0f), position->z + (fSize/2.0f) );
	vec[2] = Vector4(position->x + (fSize/2.0f), position->y + (fSize/2.0f), position->z + (fSize/2.0f) );
	vec[3] = Vector4(position->x - (fSize/2.0f), position->y + (fSize/2.0f), position->z + (fSize/2.0f) );
	vec[4] = Vector4(position->x - (fSize/2.0f), position->y - (fSize/2.0f), position->z - (fSize/2.0f) );
	vec[5] = Vector4(position->x + (fSize/2.0f), position->y - (fSize/2.0f), position->z - (fSize/2.0f) );
	vec[6] = Vector4(position->x + (fSize/2.0f), position->y + (fSize/2.0f), position->z - (fSize/2.0f) );
	vec[7] = Vector4(position->x - (fSize/2.0f), position->y + (fSize/2.0f), position->z - (fSize/2.0f) );

	rederBoxVerts(vec, color, look);
}

void begin2DRendering(int vpWidth, int vpHeight)
{
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, vpWidth, vpHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void end2DRendering()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void render2DSprite(int iDestX, int iDestWidth, int iDestY, int iDestHeight, unsigned int color, bool bTextured,
					unsigned int iTextureId, float fSourceX, float fSourceWidth, float fSourceY, float fSourceHeight, bool bAlphaBlended)
{
	int r, g, b, a;

	switchTextureRendering(bTextured);
	
	if (bTextured)
		setCurrentTexture(iTextureId);

	if (bAlphaBlended)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
		
	splitRGBA(color, r, g, b, a);

	glBegin(GL_QUADS);
		glColor4ub(r,g,b,a);
		glTexCoord2f(fSourceX, fSourceY);
		glVertex2f(iDestX, iDestY);
		glTexCoord2f(fSourceX, fSourceY+fSourceHeight);
		glVertex2f(iDestX, iDestY+iDestHeight);
		glTexCoord2f(fSourceX+fSourceWidth, fSourceY+fSourceHeight);
		glVertex2f(iDestX+iDestWidth, iDestY+iDestHeight);
		glTexCoord2f(fSourceX+fSourceWidth, fSourceY);
		glVertex2f(iDestX+iDestWidth, iDestY);
	glEnd();
	
	if (bAlphaBlended)
	{
		glDisable(GL_BLEND);
	}
}

void render3DSpriteBegin(bool bTextured, unsigned int iTextureId)
{
	switchTextureRendering(bTextured);

	if (bTextured) setCurrentTexture(iTextureId);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	switchDepthBufferWrites(false);
}

void render3DSpriteEnd()
{
	glDisable(GL_BLEND);
	switchDepthBufferWrites(true);
}

void render3DSprite(const Vector4 &right, const Vector4 &up, const Vector4 &pos, float fSize, unsigned int color)
{
	Vector4 lu, ru, ro, lo;
	int r, g, b, a;

	float fSizeHalf = fSize * 0.5f;

	lu = (pos - up*fSizeHalf - right*fSizeHalf);
	ru = (pos - up*fSizeHalf + right*fSizeHalf);
	ro = (pos + up*fSizeHalf + right*fSizeHalf);
	lo = (pos + up*fSizeHalf - right*fSizeHalf);

	splitRGBA(color, r, g, b, a);

	glBegin(GL_QUADS);
		glColor4ub(r,g,b,a);
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f( lu.x, lu.y, lu.z );
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f( ru.x, ru.y, ru.z );
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( ro.x, ro.y, ro.z );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( lo.x, lo.y, lo.z );
	glEnd();
}

#elif _PSP

void convertMatrix(float *d, float *s)
{
	for (int i=0; i<16; i++)
		d[i] = s[i];
}

void setViewMatrixIdentity()
{
	sceGumMatrixMode(GU_VIEW);
	sceGumPushMatrix();
	sceGumLoadIdentity();
}

void resetViewMatrix()
{
	sceGumMatrixMode(GU_VIEW);
	sceGumPopMatrix();
}

void matrixStackPush()
{
	sceGumPushMatrix();
}

void matrixStackPop()
{
	sceGumPopMatrix();
}

void matrixStackLoadIdentity()
{
	sceGumLoadIdentity();
}

void matrixStackMultiply(float matrix[16])
{
	ScePspFMatrix4 final;
	convertMatrix( (float*)&final, matrix );
	sceGumMultMatrix( &final );
}

void matrixStackTranslate(Vector4 &vT)
{
	ScePspFVector3 v;
	v.x = vT.x; v.y = vT.y; v.z = vT.z;
	sceGumTranslate(&v);
}

void matrixStackRotateX(float angle)
{
	sceGumRotateX(degToRad(angle));
}

void matrixStackRotateY(float angle)
{
	sceGumRotateY(degToRad(angle));
}

void matrixStackRotateZ(float angle)
{
	sceGumRotateZ(degToRad(angle));
}

void matrixStackScale(Vector4 &vS)
{
	ScePspFVector3 v;
	v.x = vS.x; v.y = vS.y; v.z = vS.z;
	sceGumScale(&v);
}

void renderIndexedTrisTex(int VertexArrayStartPos, short *IndexBuffer, int iFacesCount, VertexMd3 *pVertices, bool bMark)
{
	switchTextureRendering(true);

	VertexMd3 *pVerticesStart = pVertices+VertexArrayStartPos;
	sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_INDEX_16BIT|GU_TRANSFORM_3D, iFacesCount*3, IndexBuffer, pVerticesStart);
}

void switchDepthBufferWrites(bool on)
{
	if (on)
		sceGuDepthMask(GU_FALSE);
	else
		sceGuDepthMask(GU_TRUE);
}

void switchMultitexture(bool on)
{
	if (on)
	{
		sceGuEnable(GU_BLEND);
		switchDepthBufferWrites(false);
		sceGuDepthFunc(GU_EQUAL);
	}
	else
	{
		sceGuDisable(GU_BLEND);
		switchDepthBufferWrites(true);
		sceGuDepthFunc(GU_GEQUAL);
	}
}

void setBlendFunctionFixedColor(float fBlendCol)
{
	int iCol = (int)(fBlendCol*255.0f);
	int iPSPBlendColor = ((iCol) + (iCol*256) + (iCol*256*256) + (iCol*256*256*256));
	sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, iPSPBlendColor, 0xffffffff);
}

void setBlendFunctionAlpha()
{
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0x00000000, 0x00000000);
}

void setBlendFunctionMultiplySrcDst()
{
	sceGuBlendFunc(GU_ADD, GU_DST_COLOR, GU_FIX, 0x00000000, 0x00000000);
}

void flushCache()
{
	sceKernelDcacheWritebackAll();
}

void switchTextureRendering(bool on)
{
	if (on)
		sceGuEnable(GU_TEXTURE_2D);
	else
		sceGuDisable(GU_TEXTURE_2D);
}

void switchTextureRepeatWrap(bool bRepeat)
{
	if (bRepeat)
		sceGuTexWrap(GU_REPEAT, GU_REPEAT);
	else
		sceGuTexWrap(GU_CLAMP, GU_CLAMP);
}

void switchBlendTest(bool on)
{
	if (on)
	{
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0xffffffff, 0xffffffff);
	}
	else
	{
		sceGuDisable(GU_BLEND);
	}
}

void switchBlendTestX(bool on)
{
	if (on)
	{
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0x00000000, 0x00000000);
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	}
	else
	{
		sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
		sceGuDisable(GU_BLEND);
	}
}

void switchAlphaTest(bool on, float alphaGrenze)
{
	if (on)
	{
		sceGuEnable(GU_ALPHA_TEST);
		sceGuAlphaFunc(GU_GREATER, (int)(255*alphaGrenze), 0xffffffff);
	}
	else
	{
		sceGuDisable(GU_ALPHA_TEST);
	}
}

unsigned int ALIGN_MEMORY list[262144];

void setFogParameters(unsigned int fogColor, float fStart, float fEnd)
{
	sceGuFog (fStart, fEnd, fogColor);
}

void switchFog(bool on)
{
	if (on)
	{
		sceGuEnable(GU_FOG);
	}
	else
	{
		sceGuDisable(GU_FOG);
	}
}

void initGraphicsUnit()
{
	sceGuInit();
	gumInit();

	sceGuStart(GU_DIRECT,list);

	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)(FRAME_SIZE), BUF_WIDTH);
	sceGuDepthBuffer((void*)(FRAME_SIZE*2), BUF_WIDTH);

	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(50000, 10000);

	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	
	sceGuEnable(GU_SCISSOR_TEST);
	
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);

	sceGuEnable(GU_TEXTURE_2D);

	switchTextureRepeatWrap(true);

	sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);

	sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);

	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);

	sceGuFrontFace(GU_CCW);
	sceGuEnable(GU_CULL_FACE);

	sceGuShadeModel(GU_SMOOTH);

	sceGuEnable(GU_CLIP_PLANES);

	sceGuAmbientColor(0xffffffff);

	sceGuFinish();
	sceGuSync(0,0);
	sceGuDisplay(GU_TRUE);
}

void cullBack()
{
	sceGuFrontFace(GU_CCW);
}

void cullFront()
{
	sceGuFrontFace(GU_CW);
}

void prepareRendering(unsigned int clearColor)
{
	sceGuStart(GU_DIRECT,list);
	sceGuClearColor(clearColor);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
}

void clearDepthBuffer()
{
	sceGuClearDepth(0);
	sceGuClear(GU_DEPTH_BUFFER_BIT);
}

void finishRendering(bool vsync)
{
	sceGuTexFlush();
	sceGuTexSync();

	sceGuFinish();

	sceGuSync(0,0);
	
	if (vsync)
		sceDisplayWaitVblankStart();

	sceGuSwapBuffers();
}

Matrix4 setRenderingProjection(float fFoV, float fAspectRatio, float fZNear, float fZFar)
{
	ScePspFMatrix4 projectionPsp;
	Matrix4 projection;

	projection.setupProjection(fFoV, fAspectRatio, fZNear, fZFar);
	convertMatrix( (float*)&projectionPsp, projection.matrix );

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadMatrix(&projectionPsp);
	
	return projection;
}

Matrix4 setRenderingProjectionOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Matrix4 projection;

	// TODO

	return projection;
}

Matrix4 setRenderingView(CCamera &cam)
{
	ScePspFMatrix4 viewPsp;
	Matrix4 view;

	Vector4 pos, forward, up, right;

	pos		= cam.getPosition();
	forward	= cam.getForwardLook();
	up		= cam.getUpLook();
	right   = cam.getRightLook();

	view.setupLookAt(pos, forward, up, right);
	convertMatrix( (float*)&viewPsp, view.matrix );

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadMatrix(&viewPsp);
    
	// set world
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	return view;
}

struct Vertex3D
{
	float u,v;
//	unsigned int color;
//	float nx, ny, nz;
	float x,y,z;
};

struct Vertex3Dcol
{
	float u,v;
	unsigned int color;
//	float nx, ny, nz;
	float x,y,z;
};

inline void copyVertices(int iVertexCount, Vertex source[], Vertex3D dest[], bool bLightmap, bool bMark, mapTexture_t::eTexAlignment alignment)
{
	for (int i=0; i<iVertexCount; i++)
	{
		if (bLightmap)
		{
			dest[i].u	= source[i].ul;
			dest[i].v	= source[i].vl;
		}
		else if (alignment == mapTexture_t::ALIGN_LOCAL)
		{
			dest[i].u	= source[i].ulocal;
			dest[i].v	= source[i].vlocal;
		}
		else
		{
			dest[i].u	= source[i].u;
			dest[i].v	= source[i].v;
		}

//		if (bMark)
//			dest[i].color	= 0xff0000ff;
//		else
//			dest[i].color	= source[i].color;

//		dest[i].nx		= source[i].nx;
//		dest[i].ny		= source[i].ny;
//		dest[i].nz		= source[i].nz;
		dest[i].x		= source[i].x;
		dest[i].y		= source[i].y;
		dest[i].z		= source[i].z;
	}
}

void setTextureOffset(float fXOffset, float fYOffset)
{
	sceGuTexOffset(fXOffset, fYOffset);
}

void clearTextureOffset()
{
	sceGuTexOffset(0.0f, 0.0f);
}

void renderPoly(Polygon_t &poly, bool bLightmap, bool bMark, bool bOnlyLines, int slotNr)
{
	int iVertexCount;

	iVertexCount = poly.iVertexCount;

	if (iVertexCount)
	{
		textureSlot_t *pSlot = &poly.pQuadTextureSlot[slotNr];

		Vertex *v = &poly.verts[0];

		mapTexture_t *maptex = &mapTextures[pSlot->textureMapIndex];

		if (slotNr == 0)
		{
			if (poly.bAnimated)
			{
				unsigned int iAnimationIndex = poly.animationIndex;
				unsigned int iFrameIndex = textureAnimations[iAnimationIndex].iRunningFrameIndex;

				maptex = &mapTextures[textureAnimations[iAnimationIndex].frameTextureMapIndex[iFrameIndex]];
			}
		}

		Vertex3D* vert = (Vertex3D*)sceGuGetMemory(iVertexCount * sizeof(struct Vertex3D));

		switchTextureRendering(pSlot->bActive);
		
		if (bLightmap)
			setCurrentTexture(poly.lightmapId);
		else if (pSlot->bActive)
			setCurrentTexture(maptex->iId);

		if (!bLightmap)
			setLocalTextureOffset(pSlot);

		if (!checkSetTextureRepeatOn(bLightmap, maptex, pSlot)) switchTextureRepeatWrap(false);

		copyVertices(iVertexCount, v, vert, bLightmap, bMark, maptex->alignment);
		
//		sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, iVertexCount, 0, vert);
		sceGumDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, iVertexCount, 0, vert);

		if (!checkSetTextureRepeatOn(bLightmap, maptex, pSlot)) switchTextureRepeatWrap(true);

		if (!bLightmap)
			clearLocalTextureOffset(pSlot);
	}
}

void setPointSizeDefault()
{
	// TODO
}

void setPointSize(int iSize)
{
	// TODO
}

void renderPoint(const Vector4 &pos, unsigned int color)
{
	// TODO
}

void setLineWidthDefault()
{
	// TODO
}

void setLineWidth(int iWidth)
{
	// TODO
}

void renderLine(const Vector4 &begin, const Vector4 end, unsigned int color)
{
	// TODO
}

void renderAABB(const Vector4 *position, const AABB_t *aabb, unsigned int color, renderLook_e look)
{
}

void renderCube(const Vector4 *position, float fSize, unsigned int color, renderLook_e look)
{
	// TODO
}

void begin2DRendering(int vpWidth, int vpHeight)
{
	sceGuDisable(GU_DEPTH_TEST);
}

void end2DRendering()
{
	sceGuEnable(GU_DEPTH_TEST);
}

#define SLICE_SIZE 64.0f

struct Vertex2D
{
	float u,v;
	unsigned int color;
	float x,y,z;
};

void render2DSprite(int iDestX, int iDestWidth, int iDestY, int iDestHeight, unsigned int color, bool bTextured,
					unsigned int iTextureId, float fSourceX, float fSourceWidth, float fSourceY, float fSourceHeight, bool bAlphaBlended)
{
	float fTextureSize = 0;

	if (bTextured)
		fTextureSize = (float)getTextureSize(iTextureId);

	if (bAlphaBlended)
	{
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0x00000000, 0x00000000);
	}

	fSourceX		*= fTextureSize;
	fSourceWidth	*= fTextureSize;
	fSourceY		*= fTextureSize;
	fSourceHeight	*= fTextureSize;

	float fDestX			= (float)iDestX;
	float fDestWidth		= (float)iDestWidth;
	float fDestY			= (float)iDestY;
	float fDestHeight		= (float)iDestHeight;
	float fRunningDestWidth = fDestWidth;
	float fSDRatio			= (fSourceWidth/fDestWidth);

	float fDestWidthToRender, fSourceWidthToRender;

	switchTextureRendering(bTextured);

	if (bTextured)
		setCurrentTexture(iTextureId);

	while (fRunningDestWidth > 0)
	{
		Vertex2D* vert = (Vertex2D*)sceGuGetMemory(2 * sizeof(struct Vertex2D));

		fDestWidthToRender   = (fRunningDestWidth < SLICE_SIZE ? fRunningDestWidth : SLICE_SIZE);
		fSourceWidthToRender = (fDestWidthToRender * fSDRatio);

		vert[0].u		= fSourceX;
		vert[0].v		= fSourceY;
		vert[0].color	= color;
		vert[0].x		= fDestX;
		vert[0].y		= fDestY;
		vert[0].z		= 0;

		vert[1].u		= fSourceX+fSourceWidthToRender;
		vert[1].v		= fSourceY+fSourceHeight;
		vert[1].color	= color;
		vert[1].x		= fDestX+fDestWidthToRender;
		vert[1].y		= fDestY+fDestHeight;
		vert[1].z		= 0;

		sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vert);

		fRunningDestWidth -= fDestWidthToRender;
		fSourceX          += fSourceWidthToRender;
		fDestX            += fDestWidthToRender;
	}
	
	if (bAlphaBlended)
	{
		sceGuDisable(GU_BLEND);
	}

}

void render3DSpriteBegin(bool bTextured, unsigned int iTextureId)
{
	switchTextureRendering(bTextured);

	if (bTextured) setCurrentTexture(iTextureId);

	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0x00000000, 0x00000000);
	switchDepthBufferWrites(false);
}

void render3DSpriteEnd()
{
	sceGuDisable(GU_BLEND);
	switchDepthBufferWrites(true);
}

void render3DSprite(const Vector4 &right, const Vector4 &up, const Vector4 &pos, float fSize, unsigned int color)
{
	Vector4 lu, ro;

	Vertex3Dcol* vert = (Vertex3Dcol*)sceGuGetMemory(2 * sizeof(struct Vertex3Dcol));

	float fSizeHalf = fSize * 0.5f;

	lu = (pos - up*fSizeHalf - right*fSizeHalf);
	ro = (pos + up*fSizeHalf + right*fSizeHalf);

	// lu
	vert[0].u		= 0.0f;
	vert[0].v		= 1.0f;
	vert[0].color	= color;
//	vert[0].nx		= 0.0f;
//	vert[0].ny		= 0.0f;
//	vert[0].nz		= -1.0f;
	vert[0].x		= lu.x;
	vert[0].y		= lu.y;
	vert[0].z		= lu.z;

	// ro
	vert[1].u		= 1.0f;
	vert[1].v		= 0.0f;
	vert[1].color	= color;
//	vert[1].nx		= 0.0f;
//	vert[1].ny		= 0.0f;
//	vert[1].nz		= -1.0f;
	vert[1].x		= ro.x;
	vert[1].y		= ro.y;
	vert[1].z		= ro.z;

//	sceGumDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 2, 0, vert);
	sceGumDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 2, 0, vert);
}

#endif

bool checkSetTextureRepeatOn(bool bLightmap, mapTexture_t *tex, textureSlot_t *slot)
{
	if (bLightmap) return false; // no repeat
	else if (	tex->alignment == mapTexture_t::ALIGN_WORLD) return true; // yes, repeat
	else if	(	tex->fRepeat			== 1.0f							&&
				slot->uMovementVersion	== textureSlot_t::NONE			&&
				slot->vMovementVersion	== textureSlot_t::NONE			&&
				slot->uOffsetLocal		== 0.0f							&&
				slot->vOffsetLocal		== 0.0f) return false; // no repeat
	else return true; // yes repeat
}

void setLocalTextureOffset(textureSlot_t *pSlot)
{
	if (pSlot->uMovementVersion != textureSlot_t::NONE || pSlot->vMovementVersion != textureSlot_t::NONE || pSlot->uOffsetLocal != 0 || pSlot->vOffsetLocal != 0)
	{
		float du=0, dv=0;
		
		if		(pSlot->uMovementVersion == textureSlot_t::SLOW_POS) du = textureEffects::fMovementValue[0];
		else if (pSlot->uMovementVersion == textureSlot_t::SLOW_NEG) du = textureEffects::fMovementValue[1];
		else if (pSlot->uMovementVersion == textureSlot_t::FAST_POS) du = textureEffects::fMovementValue[2];
		else if (pSlot->uMovementVersion == textureSlot_t::FAST_NEG) du = textureEffects::fMovementValue[3];

		if		(pSlot->vMovementVersion == textureSlot_t::SLOW_POS) dv = textureEffects::fMovementValue[0];
		else if (pSlot->vMovementVersion == textureSlot_t::SLOW_NEG) dv = textureEffects::fMovementValue[1];
		else if (pSlot->vMovementVersion == textureSlot_t::FAST_POS) dv = textureEffects::fMovementValue[2];
		else if (pSlot->vMovementVersion == textureSlot_t::FAST_NEG) dv = textureEffects::fMovementValue[3];
		
		setTextureOffset(-du-pSlot->uOffsetLocal, -dv-pSlot->vOffsetLocal);
	}
}

void clearLocalTextureOffset(textureSlot_t *pSlot)
{
	if (pSlot->uMovementVersion != textureSlot_t::NONE || pSlot->vMovementVersion != textureSlot_t::NONE || pSlot->uOffsetLocal != 0 || pSlot->vOffsetLocal != 0)
		clearTextureOffset();
}

void clipRenderQuad(Quad_t *quad, bool bWithLightmap, bool bClip, bool bMark, bool bOnlyLines)
{
	// quad in WorldSpace
	bool bMT = false, bAlphaBlend = false;
	
	Polygon_t unclippedPoly, clippedPoly;

	unclippedPoly.iVertexCount = 4;

	memcpy( unclippedPoly.verts, quad->verts, sizeof(Vertex)*4 );

	unclippedPoly.pQuadTextureSlot	= quad->textureSlot;
	unclippedPoly.bLightmap			= quad->bLightmap;
	unclippedPoly.lightmapId		= quad->lightmapId;
	unclippedPoly.bAnimated			= quad->bAnimated;
	unclippedPoly.animationIndex	= quad->animationIndex;

	if (bClip)
		if ( !clipToFrustum(&unclippedPoly, &clippedPoly) ) return;
	
	// render slot 0 + alpha blended slots before lightmap
	for (int slotNr=0; slotNr<5; slotNr++)
	{
		if (!quad->textureSlot[slotNr].bActive)
			break;
		
		if (slotNr > 0 && quad->textureSlot[slotNr].blendingWithPrevSlots != textureSlot_t::ALPHA_BLEND)
			continue;
		
		if (slotNr > 0 && !bMT)
		{
			bMT = true;
			switchMultitexture(true);
		}	
		
		if (slotNr > 0 && !bAlphaBlend)
		{
			setBlendFunctionAlpha();
			bAlphaBlend = true;
		}
		
		if (bClip)
			renderPoly(clippedPoly, false, bMark, bOnlyLines, slotNr);
		else
			renderPoly(unclippedPoly, false, bMark, bOnlyLines, slotNr);
	}
	
	// render Lightmap
	if (bWithLightmap && quad->bLightmap)
	{
		if (!bMT)
		{
			bMT = true;
			switchMultitexture(true);
		}
		
		setBlendFunctionMultiplySrcDst();
		
		if (bClip)
			renderPoly(clippedPoly, true, bMark, bOnlyLines, 0);
		else
			renderPoly(unclippedPoly, true, bMark, bOnlyLines, 0);
	}

	// render glowing textures
	for (int slotNr=1; slotNr<5; slotNr++)
	{
		if (!quad->textureSlot[slotNr].bActive)
			break;
		
		if (quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::ALPHA_BLEND)
			continue;

		if (!bMT)
		{
			bMT = true;
			switchMultitexture(true);
		}	
		
		float fBlendFactor = 1;
		
		if		(quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_SLO) fBlendFactor = textureEffects::fBlendValue[0];
		else if (quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_MED) fBlendFactor = textureEffects::fBlendValue[1];
		else if (quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_FAS) fBlendFactor = textureEffects::fBlendValue[2];
		else if (quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::GLOW_LIN_SLO) fBlendFactor = textureEffects::fBlendValue[3];
		else if (quad->textureSlot[slotNr].blendingWithPrevSlots == textureSlot_t::GLOW_LIN_FAS) fBlendFactor = textureEffects::fBlendValue[4];
		
		setBlendFunctionFixedColor(fBlendFactor);

		if (bClip)
			renderPoly(clippedPoly, false, bMark, bOnlyLines, slotNr);
		else
			renderPoly(unclippedPoly, false, bMark, bOnlyLines, slotNr);
	}
	
	if (bMT)
		switchMultitexture(false);
}

void renderSkybox(unsigned int *skyboxTextureMapIndex, Vector4 playerPos)
{
	Polygon_t unclippedPoly, clippedPoly;

	float fMin = -2.5;
	float fMax =  2.5;
	float size =  5.0;
	
	unclippedPoly.iVertexCount = 4;
	
	cullFront();

	switchDepthBufferWrites(false);

	for (int iSide = 0; iSide < 6; iSide++)
	{
		Vector4 lowerLeft, right, up;
		Quad_t quad;
		
		switch (iSide)
		{
			case 0: // front
				lowerLeft	= Vector4(fMin, fMin, fMax) + playerPos;
				right		= Vector4(1,0,0);
				up			= Vector4(0,1,0);				
				break;
				
			case 1: // back
				lowerLeft	= Vector4(fMax, fMin, fMin) + playerPos;
				right		= Vector4(-1,0,0);
				up			= Vector4(0,1,0);				
			break;

			case 2: // left
				lowerLeft	= Vector4(fMin, fMin, fMin) + playerPos;
				right		= Vector4(0,0,1);
				up			= Vector4(0,1,0);				
			break;

			case 3: // right
				lowerLeft	= Vector4(fMax, fMin, fMax) + playerPos;
				right		= Vector4(0,0,-1);
				up			= Vector4(0,1,0);				
			break;

			case 4: // top
				lowerLeft	= Vector4(fMin, fMax, fMax) + playerPos;
				right		= Vector4(1,0,0);
				up			= Vector4(0,0,-1);				
			break;

			case 5: // bottom
				lowerLeft	= Vector4(fMin, fMin, fMin) + playerPos;
				right		= Vector4(1,0,0);
				up			= Vector4(0,0,1);				
			break;
		}

		quad.init(lowerLeft, right, up, size, size, true, skyboxTextureMapIndex[iSide]);

		memcpy( unclippedPoly.verts, &quad.verts, sizeof(Vertex)*4 );

		unclippedPoly.pQuadTextureSlot	= quad.textureSlot;
		unclippedPoly.bLightmap			= quad.bLightmap;
		unclippedPoly.lightmapId		= quad.lightmapId;
		unclippedPoly.bAnimated			= quad.bAnimated;
		unclippedPoly.animationIndex	= quad.animationIndex;

		if ( clipToFrustum(&unclippedPoly, &clippedPoly) )
		{
			renderPoly(clippedPoly, false, false, false, 0);
		}
	}

	cullBack();

	switchDepthBufferWrites(true);
}
