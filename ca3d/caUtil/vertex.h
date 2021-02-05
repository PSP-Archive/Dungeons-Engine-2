// vertex.h

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

#ifndef CA_VERTEX_H
#define CA_VERTEX_H

#include "plane.h"

struct Vertex
{
	float u,v;
	float ulocal, vlocal;
	float ul,vl;
	unsigned int color; // 0xaabbggrr
	float nx, ny, nz;
	float x,y,z;
};

struct VertexMd3
{
	float u,v;
	unsigned int color; // 0xaabbggrr
	float x,y,z;
};

struct textureSlot_t
{
	enum movementVersion_e
	{
		NONE,
		SLOW_POS,
		SLOW_NEG,
		FAST_POS,
		FAST_NEG
	};
	
	enum blendingVersion_e
	{
		ADD,
		GLOW_SIN_SLO,
		GLOW_SIN_MED,
		GLOW_SIN_FAS,
		GLOW_LIN_SLO,
		GLOW_LIN_FAS,
		ALPHA_BLEND
	};
	
	bool bActive;
	unsigned int textureMapIndex;
	movementVersion_e uMovementVersion;
	movementVersion_e vMovementVersion;
	blendingVersion_e blendingWithPrevSlots;
	float uOffsetLocal, vOffsetLocal;
};

#define MAX_POLY_VERTS 30

struct Polygon_t
{
	int iVertexCount;
	Vertex verts[MAX_POLY_VERTS];
	
	textureSlot_t *pQuadTextureSlot;

	bool bLightmap;
	unsigned int lightmapId;

	bool bAnimated;
	unsigned int animationIndex;
};

#define MAX_TEXTURE_SLOTS 5

struct Quad_t
{
	Vertex verts[4];

	bool bHidden;
	bool bOutside;
	bool bVisible;
	
	textureSlot_t textureSlot[MAX_TEXTURE_SLOTS];

	bool bLightmap;
	unsigned int lightmapId; // wird beim Laden neu zugewiesen

	bool bAnimated;
	unsigned int animationIndex;

	void init(const Vector4 &lowerLeftPos, const Vector4 &right, const Vector4 &up, float width, float height, bool bTextured, unsigned int iTextureMapIndex=0);
	void init(const Vector4 &lowerLeftPos, const Vector4 &lowerRightPos, const Vector4 &upperRightPos, const Vector4 &upperLeftPos, bool bTextured, unsigned int iTextureMapIndex=0);

	void updateUV(float uv0[2], float uv1[2], float uv2[2], float uv3[2]);
	void updateUVL(float uv0[2], float uv1[2], float uv2[2], float uv3[2]);
	void updateUVLocal(float uv0[2], float uv1[2], float uv2[2], float uv3[2]);
	void updateColor(unsigned int color);
	void updateNormalAndPos(const Vector4 &lowerLeftPos, const Vector4 &lowerRightPos, const Vector4 &upperRightPos, const Vector4 &upperLeftPos);
	void correctWorldUVCoordinates();

	// jeder Quad_t hat eine eigene lightmap texture, deshalb wird hier auch die texture aus dem Speicher gelöscht
	void clearLightmap();
	void setAnimation(unsigned int animationIndex);
	void removeAnimation();
	
private:

	void fillVertexUV(int iVertex, float uv[2]);
	void fillVertexUVL(int iVertex, float uv[2]);
	void fillVertexUVLocal(int iVertex, float uv[2]);
	void fillVertexColor(int iVertex, unsigned int color);
	void fillVertexNormalAndPos(int iVertex, const Vector4 &normal, const Vector4 &pos);
};

int clipToFrustum(Polygon_t *pin, Polygon_t *pout);

void assignVector(const Vertex &vert, Vector4 *vec);
void assignNormal(const Vertex &vert, Vector4 *n);

#endif
