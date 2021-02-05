// component.cpp

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

#include "component.h"
#include "lightmap.h"

bool CComponent::isLegalComponent()
{
	// auf Gleichheit wird auch abgefragt, da sonst Vektoren aufeinanderliegen und
	// so für das Lightmapping eine momentan nicht handelbare Geometry entstehen würde.
	
	// TODO:
	// zusätzlich müsste noch geprüft werden, ob ein konkaves Polygon/Quad entstehen würde
	// (ist aber direkt im Editor sichtbar, später noch einbauen)

	if (vertices[0].x >= vertices[1].x) return false;
	if (vertices[3].x >= vertices[2].x) return false;
	if (vertices[4].x >= vertices[5].x) return false;
	if (vertices[7].x >= vertices[6].x) return false;

	if (vertices[0].y >= vertices[3].y) return false;
	if (vertices[1].y >= vertices[2].y) return false;
	if (vertices[4].y >= vertices[7].y) return false;
	if (vertices[5].y >= vertices[6].y) return false;

	if (vertices[4].z >= vertices[0].z) return false;
	if (vertices[5].z >= vertices[1].z) return false;
	if (vertices[6].z >= vertices[2].z) return false;
	if (vertices[7].z >= vertices[3].z) return false;

	return true;
}

void CComponent::moveVertices(eAxis axis3D, bool *bMoveVerts, float amount)
{
	Vector4 vertices_save[8];

	memcpy(vertices_save, vertices, sizeof(vertices));

	switch (axis3D)
	{
		case X_AXIS:

			for (int i=0; i<8; i++)
				if (bMoveVerts[i])
					vertices[i].x += amount;

			break;

		case Y_AXIS:

			for (int i=0; i<8; i++)
				if (bMoveVerts[i])
					vertices[i].y += amount;

			break;

		case Z_AXIS:

			for (int i=0; i<8; i++)
				if (bMoveVerts[i])
					vertices[i].z += amount;

			break;
	}
	
	alignVertices();

	if (!isLegalComponent())
		memcpy(vertices, vertices_save, sizeof(vertices));
}

void CComponent::alignVertices()
{
	// Raster notwendig? Werte immer neu an 0.1f (kleinste mögliche Abweichung) ausrichten?
	
	float val;

	for (int i=0; i<8; i++)
	{
		for (int c=0; c<3; c++)
		{
			if		(c==0)	val = vertices[i].x;
			else if (c==1)	val = vertices[i].y;
			else if (c==2)	val = vertices[i].z;
			
			val = roundNK(val, 1);

			if		(c==0)	vertices[i].x = val;
			else if (c==1)	vertices[i].y = val;
			else if (c==2)	vertices[i].z = val;
		}
	}
}

void CComponent::resize(int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl)
{
	// Verschiebungen abhängig von:
	// - activeControl
	// - viewType
	// - iAxis

	bool bMoveVert[8];

	for (int i=0; i<8; i++)
		bMoveVert[i]=false;

	switch (viewType)
	{
		case VIEWTYPE_RIGHT:

			switch (activeControl)
			{
				case EDGE_TOP_LEFT:
					bMoveVert[2] = true;
					bMoveVert[3] = true;
					break;

				case EDGE_TOP_RIGHT:
					bMoveVert[6] = true;
					bMoveVert[7] = true;
					break;

				case EDGE_BOTTOM_RIGHT:
					bMoveVert[4] = true;
					bMoveVert[5] = true;
					break;

				case EDGE_BOTTOM_LEFT:
					bMoveVert[0] = true;
					bMoveVert[1] = true;
					break;

				case FACE_TOP:
					bMoveVert[2] = true;
					bMoveVert[3] = true;
					bMoveVert[6] = true;
					bMoveVert[7] = true;
					break;

				case FACE_RIGHT:
					bMoveVert[4] = true;
					bMoveVert[5] = true;
					bMoveVert[6] = true;
					bMoveVert[7] = true;
					break;

				case FACE_BOTTOM:
					bMoveVert[0] = true;
					bMoveVert[1] = true;
					bMoveVert[4] = true;
					bMoveVert[5] = true;
					break;

				case FACE_LEFT:
					bMoveVert[0] = true;
					bMoveVert[1] = true;
					bMoveVert[2] = true;
					bMoveVert[3] = true;
					break;

				case OBJECT:

					for (int i=0; i<8; i++)
						bMoveVert[i] = true;

					break;
			}

			switch (iAxis2D)
			{
				case 1:
					moveVertices(Z_AXIS, bMoveVert, -amount);
					break;

				case 2:
					moveVertices(Y_AXIS, bMoveVert, amount);
					break;
			}

			break;
			
		case VIEWTYPE_FRONT:

			switch (activeControl)
			{
				case EDGE_TOP_LEFT:
					bMoveVert[3] = true;
					bMoveVert[7] = true;
					break;

				case EDGE_TOP_RIGHT:
					bMoveVert[2] = true;
					bMoveVert[6] = true;
					break;

				case EDGE_BOTTOM_RIGHT:
					bMoveVert[1] = true;
					bMoveVert[5] = true;
					break;

				case EDGE_BOTTOM_LEFT:
					bMoveVert[0] = true;
					bMoveVert[4] = true;
					break;

				case FACE_TOP:
					bMoveVert[2] = true;
					bMoveVert[3] = true;
					bMoveVert[6] = true;
					bMoveVert[7] = true;
					break;

				case FACE_RIGHT:
					bMoveVert[1] = true;
					bMoveVert[2] = true;
					bMoveVert[5] = true;
					bMoveVert[6] = true;
					break;

				case FACE_BOTTOM:
					bMoveVert[0] = true;
					bMoveVert[1] = true;
					bMoveVert[4] = true;
					bMoveVert[5] = true;
					break;

				case FACE_LEFT:
					bMoveVert[0] = true;
					bMoveVert[3] = true;
					bMoveVert[4] = true;
					bMoveVert[7] = true;
					break;

				case OBJECT:

					for (int i=0; i<8; i++)
						bMoveVert[i] = true;
					
					break;
			}

			switch (iAxis2D)
			{
				case 1:
					moveVertices(X_AXIS, bMoveVert, amount);
					break;

				case 2:
					moveVertices(Y_AXIS, bMoveVert, amount);
					break;
			}

			break;

		case VIEWTYPE_TOP:

			switch (activeControl)
			{
				case EDGE_TOP_LEFT:
					bMoveVert[4] = true;
					bMoveVert[7] = true;
					break;

				case EDGE_TOP_RIGHT:
					bMoveVert[5] = true;
					bMoveVert[6] = true;
					break;

				case EDGE_BOTTOM_RIGHT:
					bMoveVert[1] = true;
					bMoveVert[2] = true;
					break;

				case EDGE_BOTTOM_LEFT:
					bMoveVert[0] = true;
					bMoveVert[3] = true;
					break;

				case FACE_TOP:
					bMoveVert[4] = true;
					bMoveVert[5] = true;
					bMoveVert[6] = true;
					bMoveVert[7] = true;
					break;

				case FACE_RIGHT:
					bMoveVert[1] = true;
					bMoveVert[2] = true;
					bMoveVert[5] = true;
					bMoveVert[6] = true;
					break;

				case FACE_BOTTOM:
					bMoveVert[0] = true;
					bMoveVert[1] = true;
					bMoveVert[2] = true;
					bMoveVert[3] = true;
					break;

				case FACE_LEFT:
					bMoveVert[0] = true;
					bMoveVert[3] = true;
					bMoveVert[4] = true;
					bMoveVert[7] = true;
					break;

				case OBJECT:

					for (int i=0; i<8; i++)
						bMoveVert[i] = true;
					
					break;
			}

			switch (iAxis2D)
			{
				case 1:
					moveVertices(X_AXIS, bMoveVert, amount);
					break;

				case 2:
					moveVertices(Z_AXIS, bMoveVert, -amount);
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

	updateAllNormalsAndPositions();

	updateAllWorldUVCoordinates();
}

void CComponent::updateAllNormalsAndPositions()
{
	surfaces[0].updateNormalAndPos(vertices[4], vertices[0], vertices[3], vertices[7]);
	surfaces[1].updateNormalAndPos(vertices[1], vertices[5], vertices[6], vertices[2]);
	surfaces[2].updateNormalAndPos(vertices[4], vertices[5], vertices[1], vertices[0]);
	surfaces[3].updateNormalAndPos(vertices[3], vertices[2], vertices[6], vertices[7]);
	surfaces[4].updateNormalAndPos(vertices[5], vertices[4], vertices[7], vertices[6]);
	surfaces[5].updateNormalAndPos(vertices[0], vertices[1], vertices[2], vertices[3]);
}

void CComponent::updateWorldUVCoordinates(float uv0[2], float uv1[2], float uv2[2], float uv3[2], float fRep, eSurface wichSurface)
{
	switch (wichSurface)
	{
		case LEFT:
			uv0[0] = vertices[4].z; uv0[1] = vertices[4].y;
			uv1[0] = vertices[0].z; uv1[1] = vertices[0].y;
			uv2[0] = vertices[3].z; uv2[1] = vertices[3].y;
			uv3[0] = vertices[7].z; uv3[1] = vertices[7].y;
			break;
		case RIGHT:
			uv0[0] = -vertices[1].z; uv0[1] = vertices[1].y;
			uv1[0] = -vertices[5].z; uv1[1] = vertices[5].y;
			uv2[0] = -vertices[6].z; uv2[1] = vertices[6].y;
			uv3[0] = -vertices[2].z; uv3[1] = vertices[2].y;
			break;
		case BOTTOM:
			uv0[0] = vertices[4].x; uv0[1] = vertices[4].z;
			uv1[0] = vertices[5].x; uv1[1] = vertices[5].z;
			uv2[0] = vertices[1].x; uv2[1] = vertices[1].z;
			uv3[0] = vertices[0].x; uv3[1] = vertices[0].z;
			break;
		case TOP:
			uv0[0] = vertices[3].x; uv0[1] = -vertices[3].z;
			uv1[0] = vertices[2].x; uv1[1] = -vertices[2].z;
			uv2[0] = vertices[6].x; uv2[1] = -vertices[6].z;
			uv3[0] = vertices[7].x; uv3[1] = -vertices[7].z;
			break;
		case BACK:
			uv0[0] = -vertices[5].x; uv0[1] = vertices[5].y;
			uv1[0] = -vertices[4].x; uv1[1] = vertices[4].y;
			uv2[0] = -vertices[7].x; uv2[1] = vertices[7].y;
			uv3[0] = -vertices[6].x; uv3[1] = vertices[6].y;
			break;
		case FRONT:
			uv0[0] = vertices[0].x; uv0[1] = vertices[0].y;
			uv1[0] = vertices[1].x; uv1[1] = vertices[1].y;
			uv2[0] = vertices[2].x; uv2[1] = vertices[2].y;
			uv3[0] = vertices[3].x; uv3[1] = vertices[3].y;
			break;
	}

	uv0[0] /=  fRep; uv1[0] /=  fRep; uv2[0] /=  fRep; uv3[0] /=  fRep;
	uv0[1] /= -fRep; uv1[1] /= -fRep; uv2[1] /= -fRep; uv3[1] /= -fRep;
}

void CComponent::updateAllWorldUVCoordinates()
{
	float uv0[2], uv1[2], uv2[2], uv3[2];
	
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[0].textureSlot[0].textureMapIndex].fRepeat, LEFT );
	surfaces[0].updateUV(uv0, uv1, uv2, uv3);
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[1].textureSlot[0].textureMapIndex].fRepeat, RIGHT);
	surfaces[1].updateUV(uv0, uv1, uv2, uv3);
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[2].textureSlot[0].textureMapIndex].fRepeat, BOTTOM);
	surfaces[2].updateUV(uv0, uv1, uv2, uv3);
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[3].textureSlot[0].textureMapIndex].fRepeat, TOP);
	surfaces[3].updateUV(uv0, uv1, uv2, uv3);
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[4].textureSlot[0].textureMapIndex].fRepeat, BACK);
	surfaces[4].updateUV(uv0, uv1, uv2, uv3);
	updateWorldUVCoordinates(uv0, uv1, uv2, uv3, mapTextures[surfaces[5].textureSlot[0].textureMapIndex].fRepeat, FRONT);
	surfaces[5].updateUV(uv0, uv1, uv2, uv3);

	for (int i=0; i<6; i++)
		surfaces[i].correctWorldUVCoordinates();
}

void CComponent::init( float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bTextured, unsigned int iTextureMapId )
{
	bIsConnector = false;
	iConnectorIndex = 0;

	bIsCollisionObject = true;

	bIsLevelExit = false;

	vertices[0] = Vector4(xMin, yMin, zMax);
	vertices[1] = Vector4(xMax, yMin, zMax);
	vertices[2] = Vector4(xMax, yMax, zMax);
	vertices[3] = Vector4(xMin, yMax, zMax);
	vertices[4] = Vector4(xMin, yMin, zMin);
	vertices[5] = Vector4(xMax, yMin, zMin);
	vertices[6] = Vector4(xMax, yMax, zMin);
	vertices[7] = Vector4(xMin, yMax, zMin);

	alignVertices();

	// LEFT
	surfaces[0].init( vertices[4], vertices[0], vertices[3], vertices[7], bTextured, iTextureMapId );
	// RIGHT
	surfaces[1].init( vertices[1], vertices[5], vertices[6], vertices[2], bTextured, iTextureMapId );
	// BOTTOM
	surfaces[2].init( vertices[4], vertices[5], vertices[1], vertices[0], bTextured, iTextureMapId );
	// TOP
	surfaces[3].init( vertices[3], vertices[2], vertices[6], vertices[7], bTextured, iTextureMapId );
	// BACK
	surfaces[4].init( vertices[5], vertices[4], vertices[7], vertices[6], bTextured, iTextureMapId );
	// FRONT
	surfaces[5].init( vertices[0], vertices[1], vertices[2], vertices[3], bTextured, iTextureMapId );

	updateAllWorldUVCoordinates();
}

void CComponent::makeLightmaps()
{
	for (int i=0; i<6; i++)
		::makeLightmap(&surfaces[i], i, false);
}

void CComponent::render(bool bWithLightmap, bool bOnlyLines, bool bSelected)
{
	for (int i=0; i<6; i++)
	{
		bool lmpOnOff;
		
		lmpOnOff = (bWithLightmap && !bOnlyLines);
		
		if (surfaces[i].bVisible || bOnlyLines || !bWithLightmap)
			clipRenderQuad(&surfaces[i], lmpOnOff, true, bSelected, bOnlyLines);
	}
}

Vector4 CComponent::getMin()
{
	Vector4 vMin;

	vMin = vertices[0];

	for (int i=0; i<8; i++)
	{
		if (vertices[i].x < vMin.x) vMin.x = vertices[i].x;
		if (vertices[i].y < vMin.y) vMin.y = vertices[i].y;
		if (vertices[i].z < vMin.z) vMin.z = vertices[i].z;
	}
	
	return vMin;
}

Vector4 CComponent::getMid()
{
	Vector4 vMin, vMid, vMax;
	
	vMin = getMin();
	vMax = getMax();

	vMid.x = (vMin.x + ((vMax.x-vMin.x)/2.0f) );
	vMid.y = (vMin.y + ((vMax.y-vMin.y)/2.0f) );
	vMid.z = (vMin.z + ((vMax.z-vMin.z)/2.0f) );
	
	return vMid;
}

Vector4 CComponent::getMax()
{
	Vector4 vMax;

	vMax = vertices[0];

	for (int i=0; i<8; i++)
	{
		if (vertices[i].x > vMax.x) vMax.x = vertices[i].x;
		if (vertices[i].y > vMax.y) vMax.y = vertices[i].y;
		if (vertices[i].z > vMax.z) vMax.z = vertices[i].z;
	}
	
	return vMax;
}

bool CComponent::checkHit(const Vector4 &pos, const Vector4 &dir, float rayLength, float *tHit, int *surfHit)
{
	bool bHit = false;
	float tNearestCompHit = 9999999.0f;
	float t, u, v;

	for (int s=0; s<6; s++)
	{
		Quad_t *q = &surfaces[s];
				
		Vector4 vec[4];
		
		assignVector(q->verts[0], &vec[0]);
		assignVector(q->verts[1], &vec[1]);
		assignVector(q->verts[2], &vec[2]);
		assignVector(q->verts[3], &vec[3]);

		if ( rayTriangleIntersectDir(pos, dir, vec[0], vec[1], vec[2], &t, &u, &v) )
		{
			if (t <= rayLength)
			{
				bHit = true;

				if (t < tNearestCompHit)
				{
					*tHit			= t;
					*surfHit		= s;
					tNearestCompHit	= t;
				}
			}
		}

		if ( rayTriangleIntersectDir(pos, dir, vec[2], vec[3], vec[0], &t, &u, &v) )
		{
			if (t <= rayLength)
			{
				bHit = true;

				if (t < tNearestCompHit)
				{
					*tHit			= t;
					*surfHit		= s;
					tNearestCompHit	= t;
				}
			}
		}
	}

	return bHit;
}
