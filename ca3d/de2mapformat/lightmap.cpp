// lightmap.cpp

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

#include "lightmap.h"
#include "../caUtil/textures.h"
#include "../caUtil/calog.h"
#include "../caUtil/memory.h"
#include "de2mapformat.h"

#include <fstream>
#include <iostream>
using namespace std;

// 1) lightmap texture coordinates manuell/automatisch ermitteln, einfach: eigene textur pro face
// 2) calculate world position + normal for every pixel in every lightmap
// 3) calculate final color for every pixel (später mit collision detection, Achtung: nicht mit eigenem quad collidieren, evtl col. distanz > min. Wert)

// Vereinfachung: lightmap nur für quads, eigene lightmap pro quad

// zu 1) lightmap coordinates of quad momentan immer 0,1-1,1-1,0-0,0 (von l.u. ccw nach l.o)

// zu 2) rechnen mit 32*32 pixel pro lightmap
// world-coordinates links-rechts und oben-unten in 33 Teile
// links + vr = rechts -> vr/33 = rechtsstep
// oben + vd = unten -> vd/33 = downstep

// kann alles aus Quad_t ermittelt werden ( verts[i], i=0,1,2,3: 0=l.u., 1=r.u., 2=r.o., 3=l.o. )

// Worldcoordinates des 1.Pixels: l.o. + 0.5 rechtsstep + 0.5 downstep

// pixel der fertigen lightmaps an loadTexture übergeben und lightmap texture incl. mipmaps erzeugen

// rendern der lightmaps:	Möglichkeit 1: jedes poly jeweils komplett rendern
//							Möglichkeit 2: 1. Durchgang texture der polys rendern, dann multitxture an und im 2. Durchgang lightmaps aller polys rendern

vector<Quad_t*> pCollisionQuads;

void clearCollisionquadsList()
{
	pCollisionQuads.clear();
}

void addCollisionQuad(Quad_t *quad)
{
	pCollisionQuads.push_back(quad);
}

class CLightmap
{
public:
	// diese class nur zur Berechnung und Speichern der lightmaps verwenden, danach ähnlich wie normale textur aus Datei laden (aber alle Lightmaps aus 1 Datei)
	Vector4 normal;
	unsigned int pixels1[LIGHTMAP_SIZE*LIGHTMAP_SIZE];
	unsigned int pixels2[LIGHTMAP_SIZE*LIGHTMAP_SIZE];
	
	unsigned int border_up[LIGHTMAP_SIZE];
	unsigned int border_down[LIGHTMAP_SIZE];
	unsigned int border_left[LIGHTMAP_SIZE];
	unsigned int border_right[LIGHTMAP_SIZE];
};

CLightmap tempLightmap;

void setAlpha(int pix_u, int pix_v, unsigned int *color)
{
	int r, g, b, a;
	
	int center = LIGHTMAP_SIZE / 2;
	int diff_u = abs(center-pix_u);
	int diff_v = abs(center-pix_v);

	float rad = sqrtfCa(diff_u*diff_u+diff_v*diff_v);
	
	splitRGBA(*color, r, g, b, a);
	
	a = (unsigned int) (MAX( 0, ( (256 / center) * (center - rad - 1 ) ) ));

	*color = clampMergeRGBA(r, g, b, a);
}

bool sameNormal(Quad_t *q)
{
	if (tempLightmap.normal.x == q->verts[0].nx &&
		tempLightmap.normal.y == q->verts[0].ny &&
		tempLightmap.normal.z == q->verts[0].nz)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int checkCollision_old(const Vector4 &lightPos, const Vector4 &worldPos, Quad_t *pSrcQuad, bool bBorder)
{
	Vector4 light = (worldPos - lightPos);
	float t, u, v;
	int iHit=0, iTmpCollided=0;

	// Loop über alle CollisionQuads
	size_t iCollisionQuads = pCollisionQuads.size();

	for (size_t iCollisionQuad=0; iCollisionQuad<iCollisionQuads; iCollisionQuad++)
	{
		Quad_t *q = pCollisionQuads[iCollisionQuad];
		
		Vector4 vec[6];
		
		assignVector(q->verts[0], &vec[0]);
		assignVector(q->verts[1], &vec[1]);
		assignVector(q->verts[2], &vec[2]);
		assignVector(q->verts[2], &vec[3]);
		assignVector(q->verts[3], &vec[4]);
		assignVector(q->verts[0], &vec[5]);
		
		if (q != pSrcQuad)
		{
			// muss Collisions mit allen anderen quads prüfen, falls 1 Collision mit direkt anliegendem quad
			// dabei ist (Prüfung ohne culling), wird 2 zurückgegeben -> später nicht im Lightmap Filter verwenden

			iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[0], vec[1], vec[2], &t, &u, &v, false));

			if		(iTmpCollided == 1) iHit = 1;
			else if (iTmpCollided == 2 && !bBorder) return 2;
			else if (iTmpCollided == 2 && bBorder && !sameNormal(q)) return 2;

			iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[3], vec[4], vec[5], &t, &u, &v, false));

			if		(iTmpCollided == 1) iHit = 1;
			else if (iTmpCollided == 2 && !bBorder) return 2;
			else if (iTmpCollided == 2 && bBorder && !sameNormal(q)) return 2;
		}
	}

	return iHit;
}

int checkCollision(const Vector4 &lightPos, const Vector4 &worldPos, Quad_t *pSrcQuad, bool bBorder)
{
	Vector4 light = (worldPos - lightPos);
	float t, u, v;
	int iHit=0, iTmpCollided=0;

	// Loop über alle CollisionQuads
	size_t iCollisionQuads = pCollisionQuads.size();

	for (size_t iCollisionQuad=0; iCollisionQuad<iCollisionQuads; iCollisionQuad++)
	{
		Quad_t *q = pCollisionQuads[iCollisionQuad];
		
		Vector4 vec[6];
		
		assignVector(q->verts[0], &vec[0]);
		assignVector(q->verts[1], &vec[1]);
		assignVector(q->verts[2], &vec[2]);
		assignVector(q->verts[2], &vec[3]);
		assignVector(q->verts[3], &vec[4]);
		assignVector(q->verts[0], &vec[5]);
		
		if (q != pSrcQuad)
		{
			// muss Collisions mit allen anderen quads prüfen, falls 1 Collision mit direkt anliegendem quad
			// dabei ist (Prüfung ohne culling), wird 2 zurückgegeben -> später nicht im Lightmap Filter verwenden

			iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[0], vec[1], vec[2], &t, &u, &v, true));
			if		(iTmpCollided == 1) iHit = 1;
			
			if (iTmpCollided==0)
			{
				iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[0], vec[2], vec[1], &t, &u, &v, true));
				if (iTmpCollided == 2 && !bBorder) return 2;
				else if (iTmpCollided == 2 && bBorder && !sameNormal(q)) return 2;
			}

			iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[3], vec[4], vec[5], &t, &u, &v, true));
			if		(iTmpCollided == 1) iHit = 1;

			if (iTmpCollided==0)
			{
				iTmpCollided = (rayTriangleIntersectEnd(lightPos, worldPos, vec[3], vec[5], vec[4], &t, &u, &v, true));
				if (iTmpCollided == 2 && !bBorder) return 2;
				else if (iTmpCollided == 2 && bBorder && !sameNormal(q)) return 2;
			}
		}
	}

	return iHit;
}

bool partOfAnyCollisionQuad(const Vector4 &pos)
{
	// Loop über alle CollisionQuads
	size_t iCollisionQuads = pCollisionQuads.size();

	for (size_t iCollisionQuad=0; iCollisionQuad<iCollisionQuads; iCollisionQuad++)
	{
		Quad_t *q = pCollisionQuads[iCollisionQuad];

		Vector4 vec[6];
		
		assignVector(q->verts[0], &vec[0]);
		assignVector(q->verts[1], &vec[1]);
		assignVector(q->verts[2], &vec[2]);
		assignVector(q->verts[2], &vec[3]);
		assignVector(q->verts[3], &vec[4]);
		assignVector(q->verts[0], &vec[5]);

		if (pointInTriangle(pos, vec[0], vec[1], vec[2]))
			return true;

		if (pointInTriangle(pos, vec[3], vec[4], vec[5]))
			return true;
	}
	return false;
}

unsigned int calculatePixelColor(const Vector4 &worldPos, Quad_t *pSrcQuad, bool bBorder)
{
	Vector4 finalColor(0, 0, 0, 1), cThisLight, smallDelta(0.01f, 0.01f, 0.01f, 0.0f);
	int iColTest;

	// Loop über alle Lights
	size_t iLights = mapAreas[iActMapArea].lights.size();

	for (size_t iLight=0; iLight<iLights; iLight++)
	{
		CLight *l = &mapAreas[iActMapArea].lights[iLight];

		float d = vecDistance(worldPos, l->position + smallDelta);
		float attenuationFactor = ( 1.0f / ( 1.0f + (l->fLinearAttenuation * d) + (l->fQuadraticAttenuation * d * d) ) );
		
		iColTest = checkCollision(l->position + smallDelta, worldPos, pSrcQuad, bBorder);
		
		if (iColTest == 1)
		{
			attenuationFactor = 0;
		}

		cThisLight = l->getAmbientColor_v() + (l->getDiffuseColor_v() * attenuationFactor);

		finalColor = finalColor + cThisLight;

		if (bBorder)
		{
			if (!partOfAnyCollisionQuad(worldPos))
				finalColor.w = 0;
		}
	
		if (iColTest == 2) // worldPos liegt verdeckt
		{
			// ein verdecktes Pixel (d.h. iColTest == 2) wird hier behandelt wie keine Kollision,
			// bekommt aber Alphawert 0 (-> Verwendung im Filter)
			// finalColor.w = 0;
			
			// so wird es behandelt wie eine Kollision, aber ebenfalls Aplha 0 (d.h. Filter verwendet es nicht)
			finalColor = Vector4(0,0,0,0);
		}
		
	}
	return clampMergeRGBAv(finalColor);
}

void saveCurrentLightmap(char *szFilename)
{
	int pixel=0;
	ofstream outfile(szFilename, ios::out|ios::trunc|ios::binary);

	if (!outfile)
	{
		cout << "Error opening File: " << szFilename << endl;
		return;
	}

	for (int pix_v=0; pix_v<LIGHTMAP_SIZE; pix_v++)
	{
		for (int pix_u=0; pix_u<LIGHTMAP_SIZE; pix_u++)
		{
			outfile.write((char*)&tempLightmap.pixels2[pixel], sizeof(unsigned int));
			pixel++;
		}
	}

	outfile.clear();
	outfile.close();
}

void addFilterColor(unsigned char *src, unsigned int *sum, unsigned char oricol, int pos, int iComp, int fetchTexel, unsigned int *components)
{
	static unsigned char fetchTexelAlpha[4];

	if (iComp==0) fetchTexelAlpha[fetchTexel] = src[pos+3]; // in src in der Reihenfolge r,g,b,a gespeichert

	if (iComp == 3) return;

	// Texel mit alpha = 0 nicht für Filter verwenden
	if (fetchTexelAlpha[fetchTexel] > 0 )
	{
		*sum		+= src[pos];
		*components += 1;
		
		// weniger dunklen Bereich (Schatten) in den helleren Bereich filtern...
		if (oricol > src[pos])
		{
			for (int loop=0; loop<2; loop++)
			{
				*sum		+= oricol;
				*components += 1;
			}
		}
	}
}

void filterLightmapXY(unsigned char *src, unsigned char *dst)
{
	unsigned int c=0;

	for (int y=0; y<LIGHTMAP_SIZE; y++)
	{
		for (int x=0; x<LIGHTMAP_SIZE; x++)
		{
			for (int i=0; i<4; i++)
			{
				unsigned int sum = 0;
				unsigned int components = 0;

				if (y != 0)
					addFilterColor(src,											&sum, src[c], c-(LIGHTMAP_SIZE*4), i, 0, &components);
				else
					addFilterColor((unsigned char*)tempLightmap.border_up,		&sum, src[c], x*4+i,			   i, 0, &components);
				
				if (x != 0)
					addFilterColor(src,											&sum, src[c], c-4,				   i, 1, &components);
				else
					addFilterColor((unsigned char*)tempLightmap.border_left,	&sum, src[c], y*4+i,			   i, 1, &components);
				
				if (x != LIGHTMAP_SIZE-1)
					addFilterColor(src,											&sum, src[c], c+4,				   i, 2, &components);
				else
					addFilterColor((unsigned char*)tempLightmap.border_right,	&sum, src[c], y*4+i,			   i, 2, &components);
				
				if (y != LIGHTMAP_SIZE-1)
					addFilterColor(src,											&sum, src[c], c+(LIGHTMAP_SIZE*4), i, 3, &components);
				else
					addFilterColor((unsigned char*)tempLightmap.border_down,	&sum, src[c], x*4+i,			   i, 3, &components);

				if (components > 0)
					dst[c] = ( sum / components );
				else
					dst[c] = src[c];

				c++;
			}
		}
	}
}

void getQuadMinMaxComponents(Quad_t *quad, Vector4 *min, Vector4 *max)
{
	// x,y,z min + max ermitteln
	
	assignVector(quad->verts[0], min);
	assignVector(quad->verts[0], max);

	for (int i=1; i<4; i++)
	{
		if (quad->verts[i].x < min->x) min->x = quad->verts[i].x;
		if (quad->verts[i].y < min->y) min->y = quad->verts[i].y;
		if (quad->verts[i].z < min->z) min->z = quad->verts[i].z;

		if (quad->verts[i].x > max->x) max->x = quad->verts[i].x;
		if (quad->verts[i].y > max->y) max->y = quad->verts[i].y;
		if (quad->verts[i].z > max->z) max->z = quad->verts[i].z;
	}
}

void makeRectVertsFromMainDeviations(int iMidDev, int iMaxDev, const Vector4 &min, const Vector4 &max, Vector4 rectVerts[4])
{
	if		(iMidDev == 1)
	{
		rectVerts[0].x = min.x;
		rectVerts[1].x = min.x;
		rectVerts[2].x = max.x;
		rectVerts[3].x = max.x;
	}
	else if	(iMidDev == 2)
	{
		rectVerts[0].y = min.y;
		rectVerts[1].y = min.y;
		rectVerts[2].y = max.y;
		rectVerts[3].y = max.y;
	}
	else if	(iMidDev == 3)
	{
		rectVerts[0].z = min.z;
		rectVerts[1].z = min.z;
		rectVerts[2].z = max.z;
		rectVerts[3].z = max.z;
	}
	
	if		(iMaxDev == 1)
	{
		rectVerts[0].x = min.x;
		rectVerts[1].x = max.x;
		rectVerts[2].x = min.x;
		rectVerts[3].x = max.x;
	}
	else if (iMaxDev == 2)
	{
		rectVerts[0].y = min.y;
		rectVerts[1].y = max.y;
		rectVerts[2].y = min.y;
		rectVerts[3].y = max.y;
	}
	else if (iMaxDev == 3)
	{
		rectVerts[0].z = min.z;
		rectVerts[1].z = max.z;
		rectVerts[2].z = min.z;
		rectVerts[3].z = max.z;
	}
}

void assignMissingComponent(Quad_t *quad, Vector4 *rectVert, int iMinDev)
{
	for (int i=0; i<4; i++)
	{
		if (iMinDev == 1)
		{
			if (rectVert->y == quad->verts[i].y && rectVert->z == quad->verts[i].z)
			{
				rectVert->x = quad->verts[i].x;
				return;
			}
		}
		else if (iMinDev == 2)
		{
			if (rectVert->x == quad->verts[i].x && rectVert->z == quad->verts[i].z)
			{
				rectVert->y = quad->verts[i].y;
				return;
			}
		}
		else if (iMinDev == 3)
		{
			if (rectVert->x == quad->verts[i].x && rectVert->y == quad->verts[i].y)
			{
				rectVert->z = quad->verts[i].z;
				return;
			}
		}
	}
	
	cout << "error: no near quad vert found !!! iMinDev = " << iMinDev << endl;
	rectVert->print("rectVert");
}

void calcUlAndVl(Quad_t *quad, int iMidDev, int iMaxDev, const Vector4 &min, const Vector4 &max)
{
	// ul und vl des quad berechnen anhand der Position in den min/max bounds, iMidDev -> u, iMaxDev -> v
	if		(iMidDev == 1)
	{
		for (int i=0; i<4; i++)	
			quad->verts[i].ul = (quad->verts[i].x - min.x) / (max.x-min.x);
	}
	else if (iMidDev == 2)
	{
		for (int i=0; i<4; i++)	
			quad->verts[i].ul = (quad->verts[i].y - min.y) / (max.y-min.y);
	}
	else if (iMidDev == 3)
	{
		for (int i=0; i<4; i++)	
			quad->verts[i].ul = (quad->verts[i].z - min.z) / (max.z-min.z);
	}
	
	if		(iMaxDev == 1)
	{
		for (int i=0; i<4; i++) 
			quad->verts[i].vl = (quad->verts[i].x - min.x) / (max.x-min.x);
	}
	else if (iMaxDev == 2)
	{
		for (int i=0; i<4; i++)	
			quad->verts[i].vl = (quad->verts[i].y - min.y) / (max.y-min.y);
	}
	else if (iMaxDev == 3)
	{
		for (int i=0; i<4; i++)	
			quad->verts[i].vl = (quad->verts[i].z - min.z) / (max.z-min.z);
	}
}

void calculateQuadBoundingRectAndUVL(Quad_t *quad, int iIsSide, Vector4 rectVerts[4])
{
	// 4 Eckpunkte =	min/min/?
	//					min/max/?
	//					max/min/?
	//					max/max/?
	//
	// der Hauptabweichungen, 3. Komponente wird berechnet durch einsetzen in Ebenengleichung
	
	Vector4 min, max, dev;
	int iMinDev, iMaxDev, iMidDev;
	float fMinDevVal;

	// wer iMidDev und iMaxDev ist, ist eigentlich unwichtig (bestimmt später u und v Richtungen)
	switch (iIsSide)
	{
		case 0:
		case 1:
			iMinDev=1; iMidDev=2; iMaxDev=3;
			break;

		case 2:
		case 3:
			iMinDev=2; iMidDev=1; iMaxDev=3;
			break;
			
		case 4:
		case 5:
			iMinDev=3; iMidDev=1; iMaxDev=2;
			break;
	}
	
	// deviation Values ermitteln:
	getQuadMinMaxComponents(quad, &min, &max);

	dev = (max-min);

	if		(iMinDev == 1) fMinDevVal = dev.x;
	else if (iMinDev == 2) fMinDevVal = dev.y;
	else if (iMinDev == 3) fMinDevVal = dev.z;

	// rectVerts bestimmen:

	// a) Koordinaten der 2 Hauptabweichungen aus min, max Vektoren übernehmen
	makeRectVertsFromMainDeviations(iMidDev, iMaxDev, min, max, rectVerts);

	// b) Koordinaten der geringsten Abweichung berechnen
	
	// - bei Abweichung 0, kann man Koordinate irgendeinen (des ersten) quadverts nehmen
	// - bei Abweichung <> 0 Koordinate aus nähsten quadvert nehmen
	//   bei Abweichung <> 0 TODO: fehlende Koordinate mit Ebenengleichung berechnen (wenn Annahme von oben nicht gelten soll)
	
	if ( fabs(fMinDevVal) < 0.1f )
	{
		if		(iMinDev == 1)
		{
			for (int i=0; i<4; i++)
				rectVerts[i].x = quad->verts[0].x;
		}
		else if (iMinDev == 2)
		{
			for (int i=0; i<4; i++)
				rectVerts[i].y = quad->verts[0].y;
		}
		else if (iMinDev == 3)
		{
			for (int i=0; i<4; i++)
				rectVerts[i].z = quad->verts[0].z;
		}
	}
	else
	{
		// für alle 4 rectVerts den nähsten quad vert finden (mit den 2 Hauptkomponenten) und fehlende Komponente ergänzen
		for (int i=0; i<4; i++)
			assignMissingComponent(quad, &rectVerts[i], iMinDev);
	}
	
	calcUlAndVl(quad, iMidDev, iMaxDev, min, max);
}

void makeLightmap(Quad_t *quad, int iIsSide, bool bBlackEdge, bool bMakeLightTexture)
{
	Vector4 lu, ru, ro, lo, right, down, rightStep, downStep, worldPosStart, worldPos, boundingRectVerts[4];

	quad->clearLightmap();

	bool bLightmapAllowed;
	
	if (quad->bAnimated)
		bLightmapAllowed = mapTextures[textureAnimations[quad->animationIndex].frameTextureMapIndex[0]].bLightmapAllowed;
	else
		bLightmapAllowed = mapTextures[quad->textureSlot[0].textureMapIndex].bLightmapAllowed;

	if (quad->bVisible && bLightmapAllowed)
	{
		int pixel=0;
	
		calculateQuadBoundingRectAndUVL(quad, iIsSide, boundingRectVerts);
		
		lu = boundingRectVerts[1];
		ru = boundingRectVerts[3];
		ro = boundingRectVerts[2];
		lo = boundingRectVerts[0];

		right	= ro-lo;
		down	= lu-lo;

		rightStep	= right / (LIGHTMAP_SIZE+1);
		downStep	= down  / (LIGHTMAP_SIZE+1);
		
		tempLightmap.normal.x = quad->verts[0].nx;
		tempLightmap.normal.y = quad->verts[0].ny;
		tempLightmap.normal.z = quad->verts[0].nz;
		tempLightmap.normal.w = 1;

		// border_up
		worldPosStart = lo + ( rightStep * 0.5f) - (downStep * 0.5f);

		for (int borderpix=0; borderpix<LIGHTMAP_SIZE; borderpix++)
		{
			worldPos = worldPosStart + (borderpix * rightStep);
			tempLightmap.border_up[borderpix] = calculatePixelColor(worldPos, quad, true);
		}

		// border_down
		worldPosStart = lu + ( rightStep * 0.5f) + (downStep * 0.5f);

		for (int borderpix=0; borderpix<LIGHTMAP_SIZE; borderpix++)
		{
			worldPos = worldPosStart + (borderpix * rightStep);
			tempLightmap.border_down[borderpix] = calculatePixelColor(worldPos, quad, true);
		}

		// border_left
		worldPosStart = lo - ( rightStep * 0.5f) + (downStep * 0.5f);

		for (int borderpix=0; borderpix<LIGHTMAP_SIZE; borderpix++)
		{
			worldPos = worldPosStart + (borderpix * downStep);
			tempLightmap.border_left[borderpix] = calculatePixelColor(worldPos, quad, true);
		}

		// border_right
		worldPosStart = ro + ( rightStep * 0.5f) + (downStep * 0.5f);

		for (int borderpix=0; borderpix<LIGHTMAP_SIZE; borderpix++)
		{
			worldPos = worldPosStart + (borderpix * downStep);
			tempLightmap.border_right[borderpix] = calculatePixelColor(worldPos, quad, true);
		}

		worldPosStart = lo + ( rightStep * 0.5f) + (downStep * 0.5f);

		for (int pix_v=0; pix_v<LIGHTMAP_SIZE; pix_v++)
		{
			for (int pix_u=0; pix_u<LIGHTMAP_SIZE; pix_u++)
			{
				worldPos = worldPosStart + (pix_u * rightStep) + (pix_v * downStep);

				if ( bBlackEdge && (pix_u == 0 || pix_u == (LIGHTMAP_SIZE-1) || pix_v == 0 || pix_v == (LIGHTMAP_SIZE-1)) )
					tempLightmap.pixels1[pixel] = 0xff000000;
				else
					tempLightmap.pixels1[pixel] = calculatePixelColor(worldPos, quad, false);
							
				if (bMakeLightTexture)
					setAlpha(pix_u, pix_v, &tempLightmap.pixels1[pixel]);

				pixel++;
			}
		}
		
		unsigned char *src, *dst;
		
		src = (unsigned char*)tempLightmap.pixels1;
		dst = (unsigned char*)tempLightmap.pixels2;

		filterLightmapXY(src, dst);
		filterLightmapXY(dst, src);
		filterLightmapXY(src, dst);
		filterLightmapXY(dst, src);
		filterLightmapXY(src, dst);

		saveLightmapForQuad(quad, dst, LIGHTMAP_SIZE, true, iActMapArea);

		if (bMakeLightTexture)
			saveCurrentLightmap("xxxLighttexture02.lmp");
	}
}
