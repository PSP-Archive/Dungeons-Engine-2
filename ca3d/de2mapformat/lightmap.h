// lightmap.h

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

#ifndef CA_LIGHTMAP_H
#define CA_LIGHTMAP_H

#include "../caUtil/vectorMath.h"
#include "../caUtil/vertex.h"
#include "light.h"

// Light source is attenuated by multiplying the contribution of that source by an attenuation factor:

// attenuation factor = 1/(kc+(kl*d)+(kq*d*d))
// d = distance between light's position and pixel
// kc, kl, kq attenuation factors (default is no attenuation: kc=1, kl=0, kq=0)

// als Material wird i.M. für die Lightberechnung angenommen: ambient + diffuse color = Vector4(1,1,1,1)
// evtl. später Materialclass ergänzen + verschiedene materials verwenden (auch mit specular light und material color, shininess)

void clearCollisionquadsList();
void addCollisionQuad(Quad_t *quad);
void makeLightmap(Quad_t *quad, int iIsSide, bool bBlackEdge=false, bool bMakeLightTexture=false);

#endif
