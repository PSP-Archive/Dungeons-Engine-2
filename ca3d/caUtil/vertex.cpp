// vertex.cpp

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

#include "vertex.h"
#include "rendering.h"
#include "plattform_includes.h"
#include "vectorMath.h"
#include "textures.h"

void assignVector(const Vertex &vert, Vector4 *vec)
{
	vec->x = vert.x;
	vec->y = vert.y;
	vec->z = vert.z;
	vec->w = 1;
}

void assignNormal(const Vertex &vert, Vector4 *n)
{
	n->x = vert.nx;
	n->y = vert.ny;
	n->z = vert.nz;
	n->w = 1;
}

void Quad_t::setAnimation(unsigned int animationIndex)
{
	textureSlot[0].bActive			= true;
	textureSlot[0].textureMapIndex	= 0;

	this->animationIndex = animationIndex;
	bAnimated = true;
}

void Quad_t::removeAnimation()
{
	animationIndex = 0;
	bAnimated = false; 
}

void Quad_t::clearLightmap()
{
	if (bLightmap)
		unloadTexture(&lightmapId);
	
	lightmapId	= 0;
	bLightmap	= false;
}

void Quad_t::fillVertexUV(int iVertex, float uv[2])
{
	verts[iVertex].u = uv[0];
	verts[iVertex].v = uv[1];
}

void Quad_t::fillVertexUVL(int iVertex, float uv[2])
{
	verts[iVertex].ul = uv[0];
	verts[iVertex].vl = uv[1];
}

void Quad_t::fillVertexUVLocal(int iVertex, float uv[2])
{
	verts[iVertex].ulocal = uv[0];
	verts[iVertex].vlocal = uv[1];
}

void Quad_t::fillVertexColor(int iVertex, unsigned int color)
{
	verts[iVertex].color	= color;
}

void Quad_t::fillVertexNormalAndPos(int iVertex, const Vector4 &normal, const Vector4 &pos)
{
	verts[iVertex].nx		= normal.x;
	verts[iVertex].ny		= normal.y;
	verts[iVertex].nz		= normal.z;
	verts[iVertex].x		= pos.x;
	verts[iVertex].y		= pos.y;
	verts[iVertex].z		= pos.z;
}

void Quad_t::updateUV(float uv0[2], float uv1[2], float uv2[2], float uv3[2])
{
	fillVertexUV(0, uv0);
	fillVertexUV(1, uv1);
	fillVertexUV(2, uv2);
	fillVertexUV(3, uv3);
}

void Quad_t::updateUVL(float uv0[2], float uv1[2], float uv2[2], float uv3[2])
{
	fillVertexUVL(0, uv0);
	fillVertexUVL(1, uv1);
	fillVertexUVL(2, uv2);
	fillVertexUVL(3, uv3);
}

void Quad_t::updateUVLocal(float uv0[2], float uv1[2], float uv2[2], float uv3[2])
{
	fillVertexUVLocal(0, uv0);
	fillVertexUVLocal(1, uv1);
	fillVertexUVLocal(2, uv2);
	fillVertexUVLocal(3, uv3);
}

void Quad_t::updateColor(unsigned int color)
{
	for (int i=0; i<4; i++)
		fillVertexColor(i, color);
}

void Quad_t::updateNormalAndPos(const Vector4 &lowerLeftPos, const Vector4 &lowerRightPos, const Vector4 &upperRightPos, const Vector4 &upperLeftPos)
{
	Vector4 right, up, n;
	
	right	= lowerRightPos - lowerLeftPos;
	up		= upperLeftPos - lowerLeftPos;

	right.normalize();
	up.normalize();

	n = crossProduct(right, up);

	fillVertexNormalAndPos(0, n, lowerLeftPos);
	fillVertexNormalAndPos(1, n, lowerRightPos);
	fillVertexNormalAndPos(2, n, upperRightPos);
	fillVertexNormalAndPos(3, n, upperLeftPos);
}

void Quad_t::init(const Vector4 &lowerLeftPos, const Vector4 &lowerRightPos, const Vector4 &upperRightPos, const Vector4 &upperLeftPos, 
				bool bTextured, unsigned int iTextureMapIndex)
{
	float uv0[2]={0, 1}, uv1[2]={1, 1}, uv2[2]={1, 0}, uv3[2]={0, 0};
	unsigned int color = 0xffffffff;

	this->bHidden			= true;
	this->bOutside			= false;
	this->bVisible			= true;

	for (int i=0; i<MAX_TEXTURE_SLOTS; i++)
	{
		textureSlot[i].bActive = false;
		textureSlot[i].uMovementVersion			= textureSlot_t::NONE;
		textureSlot[i].vMovementVersion			= textureSlot_t::NONE;
		textureSlot[i].blendingWithPrevSlots	= textureSlot_t::ADD;
		textureSlot[i].uOffsetLocal				= 0.0f;
		textureSlot[i].vOffsetLocal				= 0.0f;
	}

	this->textureSlot[0].bActive		 = bTextured;
	this->textureSlot[0].textureMapIndex = iTextureMapIndex;

	this->bLightmap			= false;
	this->lightmapId		= 0;

	this->bAnimated			= false;
	this->animationIndex	= 0;

	updateUV(uv0, uv1, uv2, uv3);
	updateUVL(uv0, uv1, uv2, uv3);
	updateUVLocal(uv0, uv1, uv2, uv3);
	updateColor(color);
	updateNormalAndPos(lowerLeftPos, lowerRightPos, upperRightPos, upperLeftPos);
}

void Quad_t::init(const Vector4 &lowerLeftPos, const Vector4 &right, const Vector4 &up, float width, float height, 
				bool bTextured, unsigned int iTextureMapIndex)
{
	Vector4 lowerRightPos, upperRightPos, upperLeftPos;
	Vector4 rightN(right), upN(up);

	rightN.normalize();
	upN.normalize();

	lowerRightPos	= lowerLeftPos + rightN * width;
	upperRightPos	= lowerLeftPos + upN * height + rightN * width;
	upperLeftPos	= lowerLeftPos + upN * height;
	
	init(lowerLeftPos, lowerRightPos, upperRightPos, upperLeftPos, bTextured, iTextureMapIndex);
}

void Quad_t::correctWorldUVCoordinates()
{
	// Probleme auf PSP, wenn u,v viel größer oder kleiner als 0
	
	float u[4], v[4], fRep;
	
	// TODO ? verwendet standardmäßig das Repeat von Slot 0
	fRep = mapTextures[textureSlot[0].textureMapIndex].fRepeat;

	for (int k=0; k<4; k++)
	{
		u[k] = verts[k].u;
		v[k] = verts[k].v;
	}
		
	while ( u[0] >= fRep && u[1] >= fRep )
	{
		u[0] -= fRep;
		u[1] -= fRep;
	}
	while ( u[0] < 0 && u[1] < 0 )
	{
		u[0] += fRep;
		u[1] += fRep;
	}

	while ( u[3] >= fRep && u[2] >= fRep )
	{
		u[3] -= fRep;
		u[2] -= fRep;
	}
	while ( u[3] < 0 && u[2] < 0)
	{
		u[3] += fRep;
		u[2] += fRep;
	}

	while ( v[3] >= fRep && v[0] >= fRep )
	{
		v[3] -= fRep;
		v[0] -= fRep;
	}
	while ( v[3] < 0 && v[0] < 0 )
	{
		v[3] += fRep;
		v[0] += fRep;
	}

	while ( v[2] >= fRep && v[1] >= fRep )
	{
		v[2] -= fRep;
		v[1] -= fRep;
	}
	while ( v[2] < 0 && v[1] < 0 )
	{
		v[2] += fRep;
		v[1] += fRep;
	}
	
	for (int k=0; k<4; k++)
	{
		verts[k].u = u[k];
		verts[k].v = v[k];
	}
}

/////////////////////////////////////////////////////////////////////
// Clip a polygon to a plane.
/////////////////////////////////////////////////////////////////////
int clipToPlane(Polygon_t *pin, Plane_t *pplane, Polygon_t *pout)
{
    int     i, nextvert, curin, nextin;
    float   curdot, nextdot, scale;
    Vertex	*pinvert, *poutvert;

    pinvert		= pin->verts;
    poutvert	= pout->verts;

	curdot = ( pinvert->x*pplane->a + pinvert->y*pplane->b + pinvert->z*pplane->c );
    curin = ( curdot >= pplane->d );

	for (i=0 ; i<pin->iVertexCount ; i++)
    {
		nextvert = (i + 1) % pin->iVertexCount;

        // Keep the current vertex if it's inside the plane
        if (curin)
            *poutvert++ = *pinvert;

		nextdot = (	pin->verts[nextvert].x * pplane->a + pin->verts[nextvert].y * pplane->b + pin->verts[nextvert].z * pplane->c);

		nextin = (nextdot >= pplane->d);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
        {
			if ( (nextdot - curdot) == 0 )
				scale = 1;
			else
				scale = (pplane->d - curdot) / (nextdot - curdot);

            poutvert->u = pinvert->u + ((pin->verts[nextvert].u - pinvert->u) * scale);
            poutvert->v = pinvert->v + ((pin->verts[nextvert].v - pinvert->v) * scale);

            poutvert->ul = pinvert->ul + ((pin->verts[nextvert].ul - pinvert->ul) * scale);
            poutvert->vl = pinvert->vl + ((pin->verts[nextvert].vl - pinvert->vl) * scale);

            poutvert->ulocal = pinvert->ulocal + ((pin->verts[nextvert].ulocal - pinvert->ulocal) * scale);
            poutvert->vlocal = pinvert->vlocal + ((pin->verts[nextvert].vlocal - pinvert->vlocal) * scale);

			poutvert->color = pinvert->color;
			
			poutvert->nx = pinvert->nx;
			poutvert->ny = pinvert->ny;
			poutvert->nz = pinvert->nz;

            poutvert->x = pinvert->x + ((pin->verts[nextvert].x - pinvert->x) * scale);
            poutvert->y = pinvert->y + ((pin->verts[nextvert].y - pinvert->y) * scale);
            poutvert->z = pinvert->z + ((pin->verts[nextvert].z - pinvert->z) * scale);
			
            poutvert++;
        }

        curdot	= nextdot;
        curin	= nextin;
        pinvert++;
    }

	pout->iVertexCount = poutvert - pout->verts;

	if (pout->iVertexCount < 3) return 0;
	
	pout->pQuadTextureSlot	= pin->pQuadTextureSlot;

	pout->bLightmap			= pin->bLightmap;
	pout->lightmapId		= pin->lightmapId;

	pout->bAnimated			= pin->bAnimated;
	pout->animationIndex	= pin->animationIndex;

    return 1;
}

/////////////////////////////////////////////////////////////////////
// Clip a polygon to the frustum.
/////////////////////////////////////////////////////////////////////
int clipToFrustum(Polygon_t *pin, Polygon_t *pout)
{
    int         i, curpoly;
    Polygon_t	tpoly[2], *ppoly;

    curpoly = 0;
    ppoly = pin;

    for (i=0 ; i<(NUM_FRUSTUM_PLANES-1); i++)
    {
        if (!clipToPlane(ppoly, &frustumPlane[i], &tpoly[curpoly]))
        {
            return 0;
        }
        ppoly = &tpoly[curpoly];
        curpoly ^= 1;
    }

    return clipToPlane( ppoly, &frustumPlane[NUM_FRUSTUM_PLANES-1], pout );
}
