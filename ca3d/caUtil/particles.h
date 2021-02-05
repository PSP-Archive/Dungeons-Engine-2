// particles.h

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

#ifndef PARTICLES_H
#define PARTICLES_H

#include "vectorMath.h"
#include "camera.h"

#define MAX_PARTICLES		400
#define PARTICLE_LIFETIME	1.5f

struct particle_t
{
	bool bActive;
	Vector4 force;
	Vector4 position;
	float fTime;
	float fSize;
	unsigned int color;
};

namespace particles
{
	extern particle_t particleList[];

	void init();
	void render(const CCamera &cam);
	void destroy();
	void resetAllParticles();
	void createNewParticle(const Vector4 &position, const Vector4 &force);
	void advanceParticles(float fElapsedTime);
};

#endif
