// geometry_calc.cpp

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

#include "geometry_calc.h"

void makeCSG_union_simple();
void findOutsideQuads();
void countVisibleQuads();

void fillCollisionquadsList()
{
	clearCollisionquadsList();

	for (int c=0; c<mapAreas[iActMapArea].components.size(); c++)
	{
		for (int s=0; s<6; s++)
		{

// wegen Prüfung auf Kollision mit verdeckten Quads bei Lightmaperzeugung können nur outside quads weglassen werden
// Achtung bei Mapediting, dass verdeckte Quads nicht versehentlich als outsider erkannt werden,
// (wenn Mitte des Quads ausserhalb der Map liegt, evtl dort nicht nur die Mitte prüfen?)

			if (!mapAreas[iActMapArea].components[c].surfaces[s].bOutside)
				addCollisionQuad(&mapAreas[iActMapArea].components[c].surfaces[s]);
		}
	}
}

void makeAllQuadsVisible()
{
	for ( int i1=0; i1<mapAreas[iActMapArea].components.size(); i1++ )
	{
		for (int s1=0; s1<6; s1++)
		{
			mapAreas[iActMapArea].components[i1].surfaces[s1].bVisible = true;
			mapAreas[iActMapArea].components[i1].surfaces[s1].bOutside = false;
		}
	}
}

void recalculateLightmaps()
{
	bool bRemoveOutsideQuads = (mapAreas[iActMapArea].mapAreaInfo.iType == 0);
	bool calculateLightmaps  = (mapAreas[iActMapArea].mapAreaInfo.iLightmapped == 1);

	clearLightmaps(iActMapArea);

	makeAllQuadsVisible();

	makeCSG_union_simple();

	if (bRemoveOutsideQuads)
	{
		findOutsideQuads();
	}
	
	if (calculateLightmaps)
	{
		fillCollisionquadsList();
		for (int i=0; i<mapAreas[iActMapArea].components.size(); i++) mapAreas[iActMapArea].components[i].makeLightmaps();
	}
	
	countVisibleQuads();
}

// TODO: Function evtl member von Quad_t

bool checkSurfaceQ1Hidden( Quad_t *q1, Quad_t *q2 )
{
	// wenn alle Vektoren von q1 in q2 liegen (gleiche Ebene), wird q1 unsichtbar...

	Vector4 q1_vec[4], q2_vec[4];
	
	for (int i=0; i<4; i++)
	{
		assignVector(q1->verts[i], &q1_vec[i]);
		assignVector(q2->verts[i], &q2_vec[i]);
	}

	for (int i=0; i<4; i++)
	{
		if ( !pointInTriangle(q1_vec[i], q2_vec[0], q2_vec[1], q2_vec[2]) &&
			 !pointInTriangle(q1_vec[i], q2_vec[2], q2_vec[3], q2_vec[0]) ) return false;
	}
	return true;
}

void makeCSG_union_simple()
{
	for ( int i1=0; i1<mapAreas[iActMapArea].components.size(); i1++ )
	{
		for (int s1=0; s1<6; s1++)
		{
			Quad_t *qSrc = &mapAreas[iActMapArea].components[i1].surfaces[s1];
			
			qSrc->bHidden = false; // nur unsichtbar, wenn es von anderem Quad verdeckt wird

			// jedes surface mit allen surfaces der anderen components vergleichen
			for ( int i2=0; i2<mapAreas[iActMapArea].components.size(); i2++ )
			{
				if (i1 != i2)
				{
					for (int s2=0; s2<6; s2++)
					{
						Quad_t *qDst = &mapAreas[iActMapArea].components[i2].surfaces[s2];

						if (checkSurfaceQ1Hidden(qSrc,  qDst))
						{
							qSrc->bHidden = true;

							qSrc->bVisible = false;	// 15.03.2008 (da bei open areas kein findOutsideQuads gemacht wird...)
							
							cout << "found hidden surf" << endl;
							
							goto check_next_surf;
						}
					}
				}
			}

check_next_surf: ;

		}
	}	
}

// TODO: Function evtl member von Quad_t

bool checkCollisionWithQuad(Quad_t *qDst, Vector4 *rayStart, Vector4 *rayDir)
{
	int iCollided;

	float t, u, v;

	Vector4 qDstVec[4];

	for (int i=0; i<4; i++)
		assignVector(qDst->verts[i], &qDstVec[i]);

	iCollided = rayTriangleIntersectDir( *rayStart, *rayDir, qDstVec[0], qDstVec[1], qDstVec[2], &t, &u, &v);

	if (iCollided == 1)
		return true;

	iCollided = rayTriangleIntersectDir( *rayStart, *rayDir, qDstVec[2], qDstVec[3], qDstVec[0], &t, &u, &v);

	if (iCollided == 1)
		return true;
	
	return false;
}

void findOutsideQuads()
{
	for ( int i1=0; i1<mapAreas[iActMapArea].components.size(); i1++ )
	{
		for (int s1=0; s1<6; s1++)
		{
			// vektor bilden von Quadmitte in Richtung normale, mit visible quads kollisieren,
			// wenn keine kollision -> quad ist nicht sichtbar

			Quad_t *qSrc = &mapAreas[iActMapArea].components[i1].surfaces[s1];

			// makeCSG_union_simple hat vorher die hidden quads markiert

			// wenn ein quad mit keinem sichtbaren (d.h. non hidden) kollidiert, ist er outside
			qSrc->bOutside = true;

			// nur sichtbar, wenn spaeter eine Kollision mit einem nonhidden quad entdeckt wird
			qSrc->bVisible = false;

			Vector4 rayStart, rayDir, qSrcVec[4];
			
			for (int i=0; i<4; i++)
				assignVector(qSrc->verts[i], &qSrcVec[i]);

//			rayStart = ( qSrcVec[0] + (0.5 * (qSrcVec[2] - qSrcVec[0])) );
			rayStart = ( qSrcVec[0] + (0.5 * (qSrcVec[1] - qSrcVec[0])) + (0.51 * (qSrcVec[3] - qSrcVec[0])) );

			assignNormal(qSrc->verts[0], &rayDir);

			for ( int i2=0; i2<mapAreas[iActMapArea].components.size(); i2++ )
			{
				for (int s2=0; s2<6; s2++)
				{
					Quad_t *qDst = &mapAreas[iActMapArea].components[i2].surfaces[s2];

					if (!qDst->bHidden)
					{						
						if ( checkCollisionWithQuad(qDst, &rayStart, &rayDir) )
						{
							// src quad ist sichtbar (aber hidden quads duerfen nicht mehr sichtbar werden)
							if (!qSrc->bHidden) qSrc->bVisible = true;

							qSrc->bOutside = false;
							goto check_next_quad;
						}
					}
				}
			}

check_next_quad: ;

		}
	}
}

void countVisibleQuads()
{
	int iVis=0, iInvis=0, iIn=0, iOut=0, iHid=0, iUnhid=0, iSum=0;

	for ( int i1=0; i1<mapAreas[iActMapArea].components.size(); i1++ )
	{
		for (int s1=0; s1<6; s1++)
		{
			if (mapAreas[iActMapArea].components[i1].surfaces[s1].bVisible)
				iVis++;
			else
				iInvis++;

			if (mapAreas[iActMapArea].components[i1].surfaces[s1].bOutside)
				iOut++;
			else
				iIn++;

			if (mapAreas[iActMapArea].components[i1].surfaces[s1].bHidden)
				iHid++;
			else
				iUnhid++;

			iSum++;
		}
	}

	char text[200];
	sprintf(text, "Total: %i Quads (%i visible, %i invisible, %i inside, %i outside, %i hidden, %i unhidden)", iSum, iVis, iInvis, iIn, iOut, iHid, iUnhid);
	writeLog(text);
}
