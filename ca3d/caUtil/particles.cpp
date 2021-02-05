// particles.cpp

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

#include "particles.h"
#include "textures.h"
#include "rendering.h"
#include "paths.h"
#include <iostream>

using namespace std;

namespace particles
{
	particle_t particleList[MAX_PARTICLES];

	unsigned int ParticleTextureId;

	int iLastActivatedParticle;
	
	int iActiveParticles;
	
	void init()
	{
		char szTexture[256];

		sprintf(szTexture, "%s/%s", SYSTEM_PATH, "blooddrop.tga");
		ParticleTextureId = loadTexture(szTexture, false, 4444, true);
	}

	void render(const CCamera &cam)
	{		
		if (!iActiveParticles) return;

		Vector4 right = cam.getRightLook(), up = cam.getUpLook();

		render3DSpriteBegin(true, ParticleTextureId);

		for (int i=0; i<MAX_PARTICLES; i++)
		{
			if (particleList[i].bActive)
				render3DSprite(right, up, particleList[i].position, particleList[i].fSize, 0x88888888);
		}
		
		render3DSpriteEnd();
	}

	void destroy()
	{
		unloadTexture(&ParticleTextureId);
	}

	void resetAllParticles()
	{
		for (int i=0; i<MAX_PARTICLES; i++)
			particleList[i].bActive = false;

		iLastActivatedParticle = -1;
		
		iActiveParticles = 0;
	}

	int findFreeParticleSlot()
	{
		int iSearchPos = iLastActivatedParticle;

		for (int i=0; i<MAX_PARTICLES; i++)
		{
			iSearchPos++;

			if (iSearchPos >= MAX_PARTICLES)
				iSearchPos = 0;

			if (!particleList[iSearchPos].bActive)
				return iSearchPos;
		}

		return -1;
	}

	void createNewParticle(const Vector4 &position, const Vector4 &force)
	{
		int iFreeSlot = findFreeParticleSlot();
		
		if (iFreeSlot != -1)
		{
			particle_t *part = &particleList[iFreeSlot];

			part->bActive	= true;
			part->force		= force;
			part->position	= position;
			part->fSize		= 0.15f;
			part->color		= 0xffffffff;
			part->fTime		= 0.0f;

			iLastActivatedParticle = iFreeSlot;

			iActiveParticles++;
		}
	}

	void advanceParticles(float fElapsedTime)
	{
		if (!iActiveParticles) return;
		
		float fMaxSize = 0.15f;

		for (int i=0; i<MAX_PARTICLES; i++)
		{
			if (particleList[i].bActive)
			{
				particleList[i].position	= (particleList[i].position+(particleList[i].force*fElapsedTime));
				particleList[i].fSize		= ((PARTICLE_LIFETIME-particleList[i].fTime)/PARTICLE_LIFETIME*fMaxSize);
				particleList[i].fTime		+= fElapsedTime;
				particleList[i].force.y		-= (9.81*fElapsedTime);

				if (particleList[i].fTime >= PARTICLE_LIFETIME)
				{
					particleList[i].bActive = false;
					iActiveParticles--;
				}
			}
		}
	}
};
