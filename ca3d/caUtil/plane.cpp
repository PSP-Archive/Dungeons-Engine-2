// plane.cpp

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

#include "plane.h"
#include "vectorMath.h"
#include <iostream>
using namespace std;

void Plane_t::print(const char *text, bool bShowCleaned) const
{
	Plane_t p = *this;
	
	if (bShowCleaned) {
		if (fabs(p.a) <= 0.000001f) p.a = 0;
		if (fabs(p.b) <= 0.000001f) p.b = 0;
		if (fabs(p.c) <= 0.000001f) p.c = 0;
		if (fabs(p.d) <= 0.000001f) p.d = 0;
	}

	cout << "Print plane: " << text << endl;
	cout << p.a << endl;
	cout << p.b << endl;
	cout << p.c << endl;
	cout << p.d << endl << endl;
}

float Plane_t::distanceToPoint(const Vector4 &p) const
{
	float pdiso = (a*p.x + b*p.y + c*p.z);

	return (pdiso - d);
}

Planeside_e Plane_t::classifyPoint(const Vector4 &p) const
{
	float pdiso = (a*p.x + b*p.y + c*p.z);

	if		(pdiso > d)	return PLANESIDE_FRONT;
	else if (pdiso < d)	return PLANESIDE_BACK;
	else				return PLANESIDE_ON_PLANE;
}

void Plane_t::normalize()
{
	float mag = sqrtfCa(a*a + b*b + c*c);

	if (mag == 0) mag = 1;

	a = a / mag;
	b = b / mag;
	c = c / mag;
	d = d / mag;
}

Plane_t frustumPlane[NUM_FRUSTUM_PLANES];

void extractFrustumPlanes(Matrix4 &viewMat, Matrix4 &projMat)
{
	Matrix4 cm;
	
	// Ergebnis in viewspace:
	//cm = projMat;
	
	// Ergebnis in worldspace:
	cm = projMat * viewMat; // beachten, wie ist Multiplikation definiert: M1*M2 (column vector compatible) = M2*M1 (row vector compatible)

	// left
	frustumPlane[0].a = (cm.matrix[3]  + cm.matrix[0]);
	frustumPlane[0].b = (cm.matrix[7]  + cm.matrix[4]);
	frustumPlane[0].c = (cm.matrix[11] + cm.matrix[8]);
	frustumPlane[0].d = (cm.matrix[15] + cm.matrix[12]);

	// right
	frustumPlane[1].a = (cm.matrix[3]  - cm.matrix[0]);
	frustumPlane[1].b = (cm.matrix[7]  - cm.matrix[4]);
	frustumPlane[1].c = (cm.matrix[11] - cm.matrix[8]);
	frustumPlane[1].d = (cm.matrix[15] - cm.matrix[12]);

	// top
	frustumPlane[2].a = (cm.matrix[3]  - cm.matrix[1]);
	frustumPlane[2].b = (cm.matrix[7]  - cm.matrix[5]);
	frustumPlane[2].c = (cm.matrix[11] - cm.matrix[9]);
	frustumPlane[2].d = (cm.matrix[15] - cm.matrix[13]);

	// bottom
	frustumPlane[3].a = (cm.matrix[3]  + cm.matrix[1]);
	frustumPlane[3].b = (cm.matrix[7]  + cm.matrix[5]);
	frustumPlane[3].c = (cm.matrix[11] + cm.matrix[9]);
	frustumPlane[3].d = (cm.matrix[15] + cm.matrix[13]);

	// near
	frustumPlane[4].a = (cm.matrix[3]  + cm.matrix[2]);
	frustumPlane[4].b = (cm.matrix[7]  + cm.matrix[6]);
	frustumPlane[4].c = (cm.matrix[11] + cm.matrix[10]);
	frustumPlane[4].d = (cm.matrix[15] + cm.matrix[14]);

	// far
	frustumPlane[5].a = (cm.matrix[3]  - cm.matrix[2]);
	frustumPlane[5].b = (cm.matrix[7]  - cm.matrix[6]);
	frustumPlane[5].c = (cm.matrix[11] - cm.matrix[10]);
	frustumPlane[5].d = (cm.matrix[15] - cm.matrix[14]);

	// fuer die Form ax+by+cz=d muss d negiert werden
	// z.B. far plane ohne negation (bei view identity): a=0,b=0,c=1,d=1500
	// Beispiel: vektor (0,0,-1500)*(0,0,1) = -1500 liegt genau auf der Farplane
	for (int i=0; i<6; i++)
	{
		frustumPlane[i].d  = -frustumPlane[i].d;
		frustumPlane[i].d -= 0.1f;
	}	

	for (int i=0; i<6; i++)
		frustumPlane[i].normalize();
}

void printFrustumPlanes(const char *text, bool bShowCleaned)
{
	for (int i=0; i<6; i++)
		frustumPlane[i].print(text, bShowCleaned);
}
