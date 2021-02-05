// component.h

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

#ifndef CA_COMPONENT_H
#define CA_COMPONENT_H

#include "../caUtil/caUtil.h"

// CComponent

// Vertices speichern wegen verschieben, resize...

//   7----6
//  /|   /|
// 3----2 |
// | 4--|-5
// |/   |/
// 0----1

// 8 vertices, 6 sides (quads), 12 edges

class CComponent
{
	public:

		enum eSurface
		{
			LEFT,
			RIGHT,
			BOTTOM,
			TOP,
			BACK,
			FRONT
		};

		void init( float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bTextured, unsigned int iTextureMapId );
		void resize( int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl );
		void updateAllWorldUVCoordinates();
		void render( bool bWithLightmap, bool bOnlyLines, bool bSelected );
		void makeLightmaps();
		bool checkHit(const Vector4 &pos, const Vector4 &dir, float rayLength, float *tHit, int *surfHit);

		Vector4 getMin();
		Vector4 getMid();
		Vector4 getMax();

		Quad_t surfaces[6];
		Vector4 vertices[8]; // nicht von auﬂen ‰ndern !!!
		bool bIsConnector;
		char filler1[3];
		int iConnectorIndex;
		bool bIsCollisionObject;
		char filler2[3];
		bool bIsLevelExit;
		char filler3[3];

	private:

		void updateWorldUVCoordinates(float uv0[2], float uv1[2], float uv2[2], float uv3[2], float fRep, eSurface wichSurface);
		void updateAllNormalsAndPositions();
		void moveVertices(eAxis axis3D, bool *bMoveVerts, float amount);
		void alignVertices();
		bool isLegalComponent();
};

struct CollisionBlock_t
{
	public:
		AABB_t collisionBB;			// wird beim Speichern des Run-Formats ermittelt

		bool bIsConnector;
		int iTargetArea;			// wird beim Speichern des Run-Formats ermittelt
		Vector4 vTargetPosition;	// wird beim Speichern des Run-Formats berechnet
		bool bIsLevelExit;
};

#endif
