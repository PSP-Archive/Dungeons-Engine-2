// vectorMath.cpp

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

#include "vectorMath.h"
#include <iostream>
using namespace std;

// Für die Rotationen (Vector4+Matrix4) gilt:
// Daumen der rechten Hand zeigt in Richtung der Rotationsachse im right-handed CO-System,
// die Finger zeigen dann die Richtung der positiven Rotation an.

Vector4 Vector4::getRotatedX(float angle) const
{
	angle = degToRad(angle);

	float sinAngle = sinfCa(angle);
	float cosAngle = cosfCa(angle);

	return Vector4(x, y * cosAngle - z * sinAngle, y * sinAngle + z * cosAngle, w);
}

Vector4 Vector4::getRotatedY(float angle) const
{
	angle = degToRad(angle);

	float sinAngle = sinfCa(angle);
	float cosAngle = cosfCa(angle);

	return Vector4(x * cosAngle + z * sinAngle, y, -x * sinAngle + z * cosAngle, w);
}

Vector4 Vector4::getRotatedZ(float angle) const
{
	angle = degToRad(angle);

	float sinAngle = sinfCa(angle);
	float cosAngle = cosfCa(angle);

	return Vector4(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle, z, w);
}

Vector4 Vector4::getRotatedAxis(float angle, const Vector4 &axis_vector) const
{
	Vector4 axis = axis_vector;

	angle = degToRad(angle);

	axis.normalize();

	Vector4 RotationRow1, RotationRow2, RotationRow3;

	float newAngle = angle;
	float sinAngle = sinfCa(newAngle);
	float cosAngle = cosfCa(newAngle);
	float oneSubCos = 1.0f - cosAngle;

	RotationRow1.x = (axis.x) * (axis.x) + cosAngle * (1 - (axis.x) * (axis.x));
	RotationRow1.y = (axis.x) * (axis.y) * (oneSubCos) - sinAngle * axis.z;
	RotationRow1.z = (axis.x) * (axis.z) * (oneSubCos) + sinAngle * axis.y;

	RotationRow2.x = (axis.x) * (axis.y) * (oneSubCos) + sinAngle * axis.z;
	RotationRow2.y = (axis.y) * (axis.y) + cosAngle * (1 - (axis.y) * (axis.y));
	RotationRow2.z = (axis.y) * (axis.z) * (oneSubCos) - sinAngle * axis.x;
	
	RotationRow3.x = (axis.x) * (axis.z) * (oneSubCos) - sinAngle * axis.y;
	RotationRow3.y = (axis.y) * (axis.z) * (oneSubCos) + sinAngle * axis.x;
	RotationRow3.z = (axis.z) * (axis.z) + cosAngle * (1 - (axis.z) * (axis.z));

	return Vector4(*this * RotationRow1, *this * RotationRow2, *this * RotationRow3);
}

void Vector4::print(const char *text, bool bShowCleaned) const
{
	Vector4 v = *this;
	
	if (bShowCleaned) {
		if (fabs(v.x) <= SMALL_VAL) v.x = 0;
		if (fabs(v.y) <= SMALL_VAL) v.y = 0;
		if (fabs(v.z) <= SMALL_VAL) v.z = 0;
		if (fabs(v.w) <= SMALL_VAL) v.w = 0;
	}

	cout << "Print vector: " << text << endl;
	cout << v.x << endl;
	cout << v.y << endl;
	cout << v.z << endl;
	cout << v.w << endl << endl;
}

float findYawAngleBetweenVectors(const Vector4 &v1, const Vector4 &v2)
{
	// - v1, v2 must be normalized!
	// - v1, v2 must have the same y value!
	
	float fAngleRad = acosfCa(v1 * v2);
	float fAngleDeg = radToDeg(fAngleRad);
	
//	Vector4 v1Pos = v1.getRotatedY(0.01f);
	
//	float fAngleRadPos = acosfCa(v1Pos * v2);
//	float fAngleDegPos = radToDeg(fAngleRadPos);
	
//	float fAngleBetween = fAngleDeg;
	
//	if (fAngleDegPos < fAngleDeg)
//		return fAngleBetween;	// rotate v1 positive to reach v2
//	else
//		return -fAngleBetween;	// rotate v1 negative to reach v2
	
	// eleganter und schneller: mit crossproduct
	
	Vector4 cp = crossProduct(v1, v2);

	if (cp.y > 0)
		return fAngleDeg;	// rotate v1 positive to reach v2
	else
		return -fAngleDeg;	// rotate v1 negative to reach v2
}

// Matrizen:

// Xo = A*Xi + B*Yi + C*Zi + D*Wi
// Yo = E*Xi + F*Yi + G*Zi + H*Wi
// Zo = I*Xi + J*Yi + K*Zi + L*Wi
// Wo = M*Xi + N*Yi + O*Zi + P*Wi

// 1) STORING

// Die coefficients liegen bei mir dieser Reihenfolge im Speicher: A,E,I,M,B,F,J,N,C,G,K,O,D,H,L,P
// = the derivatives with respect to Xi first (code-compatible mit OpenGL und DirectX)

// 2) PRINTING

// Meine Print-Funktion gibt die Matrix folgendermaßen aus
// A B C D
// E F G H
// I J K L
// M N O P
// = column-vector-compatible (wie in den OpenGL manuals)

// Hinweis: DirectX manuals verwenden row-vector-compatible notation, d.h. die Matrix erscheint dann so im manual:
// A E I M
// B F J N
// C G K O
// D H L P

// siehe auch: http://panda3d.org/wiki/index.php/Matrix_Representation

// Zur Info: Die Elemente 12,13,14 (D,H,L) zeigen die x,y,z Translation

void Matrix4::print(const char *text, bool bShowCleaned) const
{
	Matrix4 m = *this;
	
	if (bShowCleaned)
	{
		for (int i=0; i<16; i++)
			if (fabs(m.matrix[i]) <= SMALL_VAL) m.matrix[i] = 0;
	}

	cout << "Print matrix: " << text << endl;
	cout << m.matrix[0] << ", " <<  m.matrix[4] << ", " <<  m.matrix[8]  << ", " <<  m.matrix[12] << endl;
	cout << m.matrix[1] << ", " <<  m.matrix[5] << ", " <<  m.matrix[9]  << ", " <<  m.matrix[13] << endl;
	cout << m.matrix[2] << ", " <<  m.matrix[6] << ", " <<  m.matrix[10] << ", " <<  m.matrix[14] << endl;
	cout << m.matrix[3] << ", " <<  m.matrix[7] << ", " <<  m.matrix[11] << ", " <<  m.matrix[15] << endl << endl;
}

void Matrix4::makeIdentity()
{
	matrix[0] = 1.0f; matrix[4] = 0.0f; matrix[8]  = 0.0f; matrix[12] = 0.0f;
	matrix[1] = 0.0f; matrix[5] = 1.0f; matrix[9]  = 0.0f; matrix[13] = 0.0f;
	matrix[2] = 0.0f; matrix[6] = 0.0f; matrix[10] = 1.0f; matrix[14] = 0.0f;
	matrix[3] = 0.0f; matrix[7] = 0.0f; matrix[11] = 0.0f; matrix[15] = 1.0f;
}

void Matrix4::setupProjection(float fovy, float aspect, float near_p, float far_p)
{
	makeIdentity();

	fovy = degToRad(fovy);

	float angle = (fovy / 2.0f);
	float cotangent = cosfCa(angle) / sinfCa(angle);

	matrix[0]  = cotangent / aspect;
	matrix[5]  = cotangent;
	matrix[10] = ((far_p + near_p) / (near_p - far_p));
	matrix[11] = -1.0f;
	matrix[14] = (2.0f * (far_p * near_p) / (near_p - far_p));
	matrix[15] = 0.0f;
}

void Matrix4::setupProjectionOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	makeIdentity();
	
	float a,b,c,tx,ty,tz;

	a = (2.0f/(right-left));
	b = (2.0f/(top-bottom));
	c = (2.0f/(zFar-zNear));
	tx = (right+left)/(right-left);
	ty = (top+bottom)/(top-bottom);
	tz = (zFar+zNear)/(zFar-zNear);

	matrix[0]	= a;
	matrix[5]	= b;
	matrix[10]	= c;
	matrix[12]	= tx;
	matrix[13]	= ty;
	matrix[14]	= tz;
}

void Matrix4::setupLookAt(const Vector4 &pos, const Vector4 &forward, const Vector4 &up, const Vector4 &right)
{
	makeIdentity();

	// first row = right vector
	matrix[0] = right.x;
	matrix[4] = right.y;
	matrix[8] = right.z;

	// second row = up vector
	matrix[1] = up.x;
	matrix[5] = up.y;
	matrix[9] = up.z;

	// third row = neg forward
	matrix[2]  = -forward.x;
	matrix[6]  = -forward.y;
	matrix[10] = -forward.z;

	// translation matrix, that takes camera location to the origin
	Matrix4 trans;

	trans.setupTranslate(-pos.x, -pos.y, -pos.z);

	*this = *this * trans;
}

void Matrix4::setupTranslate(const Vector4 &translate)
{
	makeIdentity();

	matrix[12] = translate.x;
	matrix[13] = translate.y;
	matrix[14] = translate.z;
	matrix[15] = 1.0f;
}

void Matrix4::setupTranslate(float x, float y, float z)
{
	makeIdentity();

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;
}

void Matrix4::setupScaling(float x, float y, float z)
{
	makeIdentity();

	matrix[0] = x;
	matrix[5] = y;
	matrix[10] = z;
	matrix[15] = 1.0f;
}

void Matrix4::setupRotateX(float angle)
{
	angle = degToRad(angle);

	float sine   = sinfCa(angle);
	float cosine = cosfCa(angle);

	matrix[5]  = cosine;
	matrix[6]  =   sine;
	matrix[9]  =  -sine;
	matrix[10] = cosine;
}

void Matrix4::setupRotateY(float angle)
{
	angle = degToRad(angle);

	float sine   = sinfCa(angle);
	float cosine = cosfCa(angle);

	matrix[0]  = cosine;
	matrix[2]  =  -sine;
	matrix[8]  =   sine;
	matrix[10] = cosine;
}

void Matrix4::setupRotateZ(float angle)
{
	angle = degToRad(angle);

	float sine   = sinfCa(angle);
	float cosine = cosfCa(angle);

	matrix[0] = cosine;
	matrix[1] =   sine;
	matrix[4] =  -sine;
	matrix[5] = cosine;
}

Vector4	operator*(const Matrix4 &m, const Vector4 &v)
{
	Vector4 out;

	out.x = v.x * m.matrix[0] + v.y * m.matrix[4] + v.z * m.matrix[8]  + v.w * m.matrix[12];
	out.y = v.x * m.matrix[1] + v.y * m.matrix[5] + v.z * m.matrix[9]  + v.w * m.matrix[13];
	out.z = v.x * m.matrix[2] + v.y * m.matrix[6] + v.z * m.matrix[10] + v.w * m.matrix[14];
	out.w = v.x * m.matrix[3] + v.y * m.matrix[7] + v.z * m.matrix[11] + v.w * m.matrix[15];

	return out;
}

// Unterschied der Matrix-Multiplikation column-vector-compatibler und row-vector-compatibler Matrizen:

// A B C D   A B C D    A=(A1A2+B1E2+C1I2+D1M2) B=(A1B2+B1F2+C1J2+D1N2) usw.
// E F G H * E F G H ->
// I J K L   I J K L
// M N O P   M N O P

// A E I M   A E I M    A=(A1A2+E1B2+I1C2+M1D2) B=(B1A2+F1B2+J1C2+N1D2) usw.
// B F J N * B F J N ->
// C G K O   C G K O
// D H L P   D H L P

// das bedeutet: M1*M2 (column vector compatible) = M2*M1 (row vector compatible)

// Ich verwende hier die column-vector-compatible Version:

Matrix4	operator*(const Matrix4 &a, const Matrix4 &b)
{
	Matrix4 out;

	out.matrix[0]	= a.matrix[0] * b.matrix[0]  + a.matrix[4] * b.matrix[1]  + a.matrix[8]  * b.matrix[2]  + a.matrix[12] * b.matrix[3];
	out.matrix[1]	= a.matrix[1] * b.matrix[0]  + a.matrix[5] * b.matrix[1]  + a.matrix[9]  * b.matrix[2]  + a.matrix[13] * b.matrix[3];
	out.matrix[2]	= a.matrix[2] * b.matrix[0]  + a.matrix[6] * b.matrix[1]  + a.matrix[10] * b.matrix[2]  + a.matrix[14] * b.matrix[3];
	out.matrix[3]	= a.matrix[3] * b.matrix[0]  + a.matrix[7] * b.matrix[1]  + a.matrix[11] * b.matrix[2]  + a.matrix[15] * b.matrix[3];

	out.matrix[4]	= a.matrix[0] * b.matrix[4]  + a.matrix[4] * b.matrix[5]  + a.matrix[8]  * b.matrix[6]  + a.matrix[12] * b.matrix[7];
	out.matrix[5]	= a.matrix[1] * b.matrix[4]  + a.matrix[5] * b.matrix[5]  + a.matrix[9]  * b.matrix[6]  + a.matrix[13] * b.matrix[7];
	out.matrix[6]	= a.matrix[2] * b.matrix[4]  + a.matrix[6] * b.matrix[5]  + a.matrix[10] * b.matrix[6]  + a.matrix[14] * b.matrix[7];
	out.matrix[7]	= a.matrix[3] * b.matrix[4]  + a.matrix[7] * b.matrix[5]  + a.matrix[11] * b.matrix[6]  + a.matrix[15] * b.matrix[7];

	out.matrix[8]	= a.matrix[0] * b.matrix[8]  + a.matrix[4] * b.matrix[9]  + a.matrix[8]  * b.matrix[10] + a.matrix[12] * b.matrix[11];
	out.matrix[9]	= a.matrix[1] * b.matrix[8]  + a.matrix[5] * b.matrix[9]  + a.matrix[9]  * b.matrix[10] + a.matrix[13] * b.matrix[11];
	out.matrix[10]	= a.matrix[2] * b.matrix[8]  + a.matrix[6] * b.matrix[9]  + a.matrix[10] * b.matrix[10] + a.matrix[14] * b.matrix[11];
	out.matrix[11]	= a.matrix[3] * b.matrix[8]  + a.matrix[7] * b.matrix[9]  + a.matrix[11] * b.matrix[10] + a.matrix[15] * b.matrix[11];

	out.matrix[12]	= a.matrix[0] * b.matrix[12] + a.matrix[4] * b.matrix[13] + a.matrix[8]  * b.matrix[14] + a.matrix[12] * b.matrix[15];
	out.matrix[13]	= a.matrix[1] * b.matrix[12] + a.matrix[5] * b.matrix[13] + a.matrix[9]  * b.matrix[14] + a.matrix[13] * b.matrix[15];
	out.matrix[14]	= a.matrix[2] * b.matrix[12] + a.matrix[6] * b.matrix[13] + a.matrix[10] * b.matrix[14] + a.matrix[14] * b.matrix[15];
	out.matrix[15]	= a.matrix[3] * b.matrix[12] + a.matrix[7] * b.matrix[13] + a.matrix[11] * b.matrix[14] + a.matrix[15] * b.matrix[15];

	return out;
}

// Der Code zur Berechnung der inversen Matrix kommt aus http://gamemath.com/ (Buch "3D Math Primer for Games and Graphics")

// Compute the inverse of a matrix.  We use the classical adjoint divided
// by the determinant method.

// Zur Info: Die Matrixelemente 3,7,11,15 werden nicht in die Berechnung der inversen Matrix einbezogen (bleiben immer 0,0,0,1)

static float matrixDeterminant(const Matrix4 &m)
{
	// Compute the determinant of the 3x3 portion of the matrix.
	return
		  m.matrix[0] * (m.matrix[5]*m.matrix[10] - m.matrix[6]*m.matrix[9])
		+ m.matrix[1] * (m.matrix[6]*m.matrix[8]  - m.matrix[4]*m.matrix[10])
		+ m.matrix[2] * (m.matrix[4]*m.matrix[9]  - m.matrix[5]*m.matrix[8]);
}

Matrix4 matrixInverse(const Matrix4 &m)
{
	Matrix4	r;

	// Compute the determinant
	float det = matrixDeterminant(m);

	// If we're singular, then the determinant is zero and there's no inverse
	if ( fabsf(det) <= SMALL_VAL ) return r;

	// Compute one over the determinant, so we divide once and can *multiply* per element
	float	oneOverDet = 1.0f / det;

	// Compute the 3x3 portion of the inverse, by dividing the adjoint by the determinant
	r.matrix[0]  = (m.matrix[5]*m.matrix[10] - m.matrix[6]*m.matrix[9])  * oneOverDet;
	r.matrix[1]  = (m.matrix[2]*m.matrix[9]  - m.matrix[1]*m.matrix[10]) * oneOverDet;
	r.matrix[2]  = (m.matrix[1]*m.matrix[6]  - m.matrix[2]*m.matrix[5])  * oneOverDet;

	r.matrix[4]  = (m.matrix[6]*m.matrix[8]  - m.matrix[4]*m.matrix[10]) * oneOverDet;
	r.matrix[5]  = (m.matrix[0]*m.matrix[10] - m.matrix[2]*m.matrix[8])  * oneOverDet;
	r.matrix[6]  = (m.matrix[2]*m.matrix[4]  - m.matrix[0]*m.matrix[6])  * oneOverDet;

	r.matrix[8]  = (m.matrix[4]*m.matrix[9]  - m.matrix[5]*m.matrix[8])  * oneOverDet;
	r.matrix[9]  = (m.matrix[1]*m.matrix[8]  - m.matrix[0]*m.matrix[9])  * oneOverDet;
	r.matrix[10] = (m.matrix[0]*m.matrix[5]  - m.matrix[1]*m.matrix[4])  * oneOverDet;

	// Compute the translation portion of the inverse
	r.matrix[12] = -(m.matrix[12]*r.matrix[0] + m.matrix[13]*r.matrix[4] + m.matrix[14]*r.matrix[8]);
	r.matrix[13] = -(m.matrix[12]*r.matrix[1] + m.matrix[13]*r.matrix[5] + m.matrix[14]*r.matrix[9]);
	r.matrix[14] = -(m.matrix[12]*r.matrix[2] + m.matrix[13]*r.matrix[6] + m.matrix[14]*r.matrix[10]);

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...
	return r;
}

// Source: http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/

bool rayrayIntersect2D(Ray3D *r1, Ray3D *r2, Vector4 *ip)
{
	Vector4 p1 = (r1->point);
	Vector4 p2 = (r1->point + r1->dir_norm);
	Vector4 p3 = (r2->point);
	Vector4 p4 = (r2->point + r2->dir_norm);
	
	float uA_Zaehler = (p4.x - p3.x) * (p1.z - p3.z) - (p4.z - p3.z) * (p1.x - p3.x);
	float uA_Nenner  = (p4.z - p3.z) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.z - p1.z);

	float uB_Zaehler = (p2.x - p1.x) * (p1.z - p3.z) - (p2.z - p1.z) * (p1.x - p3.x);
//	float uB_Nenner  = uA_Nenner;
	
	// coincident
	if  ((uA_Zaehler < SMALL_VAL && uA_Zaehler > -SMALL_VAL) &&
		 (uB_Zaehler < SMALL_VAL && uB_Zaehler > -SMALL_VAL) &&
		 (uA_Nenner  < SMALL_VAL && uA_Nenner  > -SMALL_VAL))
		return false;
	
	// parallel
	if (uA_Nenner < SMALL_VAL && uA_Nenner > -SMALL_VAL)
		return false;

	float uA = (uA_Zaehler / uA_Nenner);
//	float uB = (uB_Zaehler / uB_Nenner);

	ip->x = p1.x + uA * (p2.x - p1.x);
	ip->y = 0.0f;
	ip->z = p1.z + uA * (p2.z - p1.z);

	return true;
}

int raySphereIntersectNew(Ray3D *pRay, Sphere3D *pSphere, float *t1, float *t2, Vector4 *p1, Vector4 *p2)
{
	Vector4 m = pRay->point - pSphere->center;
	
	float b = m * pRay->dir_norm;
	
	float c = (m * m) - (pSphere->radius * pSphere->radius);
	
	float discr = b * b - c;
	
	// a negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f) return 0;
	
	// ray now found to intersect sphere
	*t1 = -b - sqrtfCa(discr);
	*t2 = -b + sqrtfCa(discr);
	
	*p1 = pRay->point + *t1 * pRay->dir_norm;

	if (*t1 == *t2) return 1;
	
	*p2 = pRay->point + *t2 * pRay->dir_norm;
	
	return 2;	
}

bool raySphereIntersect(const Vector4 &rayOrigin, const Vector4 &rayDirection, float rayLength, 
						const Vector4 &sphereCenter, float sphereRadius, float *pfDistToSphereCenter)
{
	// easy collision detection ...

	float l_quad, s, s_quad, m_quad, r_quad;
	Vector4 l = ( sphereCenter - rayOrigin );

	l_quad = l * l;
	if ( l_quad > ((rayLength + sphereRadius) * (rayLength + sphereRadius)) ) return false;
	s = l * rayDirection;
	r_quad = ( sphereRadius * sphereRadius );
	if ( s < 0 && l_quad > r_quad) return false;
	s_quad = ( s * s );
	m_quad = ( l_quad - s_quad );
	if ( m_quad > r_quad ) return false;

	if (pfDistToSphereCenter)
		*pfDistToSphereCenter = vecLength(l);

	return true;
}

int rayTriangleIntersectDir( const Vector4 &orig, const Vector4 &dir,
							 const Vector4 &vert0, const Vector4 &vert1, const Vector4 &vert2,
							 float *t, float *u, float *v, bool bTestCull, float fEpsilon)
{
	Vector4 edge1, edge2, tvec, pvec, qvec;

	float det, inv_det;
	
	/* find vectors for two edges sharing vert0 */
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;

	/* begin calculating determinant - also used to calculate U parameter */
	pvec = crossProduct(dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = edge1 * pvec;

	if (bTestCull)
	{
		/* if culling is desired */

		if (det < fEpsilon)
			return 0;

		/* calculate distance from vert0 to ray origin */
		tvec = orig - vert0;

		/* calculate U parameter and test bounds */
		*u = tvec * pvec;
		if (*u < 0.0 || *u > det)
			return 0;

		/* prepare to test V parameter */
		qvec = crossProduct(tvec, edge1);

		/* calculate V parameter and test bounds */
		*v = dir * qvec;
		if (*v < 0.0 || *u + *v > det)
			return 0;

		/* calculate t, scale parameters, ray intersects triangle */
		*t = edge2 * qvec;
		inv_det = 1.0 / det;
		*t *= inv_det;
		*u *= inv_det;
		*v *= inv_det;
	}
	else
	{
		/* the non-culling branch */

		if (det > -fEpsilon && det < fEpsilon)
			return 0;
			
		inv_det = 1.0f / det;

		/* calculate distance from vert0 to ray origin */
		tvec = orig - vert0;

		/* calculate U parameter and test bounds */
		*u = (tvec * pvec) * inv_det;
		if (*u < 0.0 || *u > 1.0)
			return 0;

		/* prepare to test V parameter */
		qvec = crossProduct(tvec, edge1);

		/* calculate V parameter and test bounds */
		*v = (dir * qvec) * inv_det;
		if (*v < 0.0 || *u + *v > 1.0)
			return 0;

		/* calculate t, ray intersects triangle */
		*t = (edge2 * qvec) * inv_det;
	}
	
	// *t der direction entgegengesetzt
	if (*t < 0)
		return 0;

	return 1;
}

int rayTriangleIntersectEnd( const Vector4 &orig, const Vector4 &end,
							 const Vector4 &vert0, const Vector4 &vert1, const Vector4 &vert2,
							 float *t, float *u, float *v, bool bTestCull, float fEpsilon )
{
	int iRet;
	float rayLength;
	
	rayLength = vecDistance(orig, end);
	
	Vector4 dir = (end - orig);

	dir.normalize();
	
	iRet = rayTriangleIntersectDir( orig, dir, vert0, vert1, vert2, t, u, v, bTestCull, fEpsilon );

	// Kollision mit Punkt auf der rayLength evtl. Sonderbehandlung
	if ( iRet == 1 && *t > (rayLength - 0.01f) && *t < (rayLength + 0.01f) )
		return 2;
	// Kollision mit Punkt weit hinter der rayLength darf nicht gezählt werden
	else if (*t > (rayLength - 0.01f) ) 
		return 0;
	else
		return iRet;
}

bool makeSphereCollision(const Vector4 &moverStart, Vector4* pMoverEnd, Vector4 blocker, float fMinDist)
{
	Vector4 posCorrected, distBefore = (moverStart - blocker), distAfter  = (*pMoverEnd - blocker);

	float fDistB = vecLength(distBefore), fDistA = vecLength(distAfter);

	if (fDistA < fDistB && fDistA < fMinDist)
	{
		posCorrected = *pMoverEnd - blocker;
		posCorrected.normalize();
		posCorrected = posCorrected * fMinDist;
		*pMoverEnd = blocker + posCorrected;
		return true;
	}
	return false;
}

void makePickingOriginAndDirOrtho(int hitX, int hitY, const Vector4 &pos, const Vector4 &lookat, int vpWidth, int vpHeight, float left, float right, float bottom, float top, Vector4 *rayOrigin, Vector4 *rayDir)
{
	*rayOrigin	= pos;
	*rayDir		= lookat;

	float xCor =  ( hitX * ((right-left) / vpWidth)  + left);
	float yCor = -( hitY * ((top-bottom) / vpHeight) + bottom);
		
	if (rayDir->x < -0.9f)
	{
		// VIEWTYPE_RIGHT
		rayOrigin->z -= xCor;
		rayOrigin->y += yCor;
	}
	else if (rayDir->z < -0.9f)
	{
		// VIEWTYPE_FRONT
		rayOrigin->x += xCor;
		rayOrigin->y += yCor;
	}
	else if (rayDir->y < -0.9f)
	{
		// VIEWTYPE_TOP
		rayOrigin->x += xCor;
		rayOrigin->z -= yCor;
	}
}

void makePickingOriginAndDir(int hitX, int hitY, int vpWidth, int vpHeight, const Matrix4 &projMat, const Matrix4 &viewMat, Vector4 *rayOrigin, Vector4 *rayDir)
{
	//from http://www.toymaker.info/Games/html/picking.html

	Vector4 v;

	v.x =   ( ( (2.0f*hitX) / vpWidth)  -1 ) / projMat.matrix[0];
	v.y =  -( ( (2.0f*hitY) / vpHeight) -1 ) / projMat.matrix[5];
	v.z =  -1;
	v.w =   1;

	Matrix4 view_i = matrixInverse(viewMat);

	rayDir->x = v.x * view_i.matrix[0] + v.y * view_i.matrix[4] + v.z * view_i.matrix[8];
	rayDir->y = v.x * view_i.matrix[1] + v.y * view_i.matrix[5] + v.z * view_i.matrix[9];
	rayDir->z = v.x * view_i.matrix[2] + v.y * view_i.matrix[6] + v.z * view_i.matrix[10];
	rayDir->w = 1;

	rayDir->normalize();

	// rayOrigin ( gleichbedeutend mit cam.getPosition() )
	rayOrigin->x = view_i.matrix[12];
	rayOrigin->y = view_i.matrix[13];
	rayOrigin->z = view_i.matrix[14];
	rayOrigin->w = 1;
}

void splitRGBA(unsigned int color, int &r, int &g, int &b, int &a)
{
	r =  (color & 0x000000ff);
	g = ((color & 0x0000ff00) >>  8);
	b = ((color & 0x00ff0000) >> 16);
	a = ((color & 0xff000000) >> 24);
}

unsigned int clampMergeRGBAv(const Vector4 &col)
{
	unsigned int r, g, b, a, final;

	r = (unsigned int)(col.x * 255);
	g = (unsigned int)(col.y * 255);
	b = (unsigned int)(col.z * 255);
	a = (unsigned int)(col.w * 255);
	
	// clamp
	r = MIN(255, r);
	g = MIN(255, g);
	b = MIN(255, b);
	a = MIN(255, a);

	// minimum color
	r = MAX(35, r);
	g = MAX(35, g);
	b = MAX(35, b);
//	a = MAX(35, a);	// alpha soll auch null werden können
	
	// merge
	final  = (r <<  0);
	final += (g <<  8);
	final += (b << 16);
	final += (a << 24);

	return final;
}

unsigned int clampMergeRGBA(int r, int g, int b, int a)
{
	unsigned int final;

	// clamp
	r = MIN(255, r);
	g = MIN(255, g);
	b = MIN(255, b);
	a = MIN(255, a);

	// merge
	final  = (r <<  0);
	final += (g <<  8);
	final += (b << 16);
	final += (a << 24);

	return final;
}

static bool pointOnSamePlaneAsTriangle(const Vector4 &p, const Vector4 &p0, const Vector4 &p1, const Vector4 &p2)
{
	Vector4 n;

	float deltaD, eps(0.01f);

	n = crossProduct(p1-p0, p2-p0);

	// n kann 0 Vektor sein, falls Tri zur Line degeneriert ist, dann keine Ebenennormale vorhanden
	// -> Ergebnis = Point nicht auf gleicher Ebene
	if ( n.x < eps && n.x > -eps &&
		 n.y < eps && n.y > -eps &&
		 n.z < eps && n.z > -eps )
		return false;

	n.normalize();
	
	// Differenz der Distanzen
	deltaD = (n*p0) - (n*p);

	if ( deltaD < eps && deltaD > -eps )
		return true;
	else
		return false;
}

/*
Point in Triangle Test
Quelle: http://www.blackpawn.com/texts/pointinpoly/default.html
*/

static bool pointOnSameTriangleSide( const Vector4 &pt1, const Vector4 &pt2, const Vector4 &a, const Vector4 &b)
{
	Vector4 cp1, cp2;

    cp1 = crossProduct(b-a, pt1-a);
    cp2 = crossProduct(b-a, pt2-a);

    if ( (cp1 * cp2) >= 0 )
		return true;
    else 
		return false;
}

bool pointInTriangle(const Vector4 &p, const Vector4 &p0, const Vector4 &p1, const Vector4 &p2)
{
	// ergänzt von ca
	if ( !pointOnSamePlaneAsTriangle(p,p0,p1,p2) )
		return false;
	//

	if ( pointOnSameTriangleSide(p,p0,p1,p2) && 
		 pointOnSameTriangleSide(p,p1,p0,p2) && 
		 pointOnSameTriangleSide(p,p2,p0,p1) )
		return true;
    else
		return false;
}

float roundNK(float val, int iNK)
{
	float d = powf(10,iNK);

	return ( floorf(val * d + 0.5f) / d);
}

////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This function converts a quaternion to a rotation matrix
/////
////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 

void CQuaternion::CreateMatrix(float *pMatrix)
{
    // Make sure the matrix has allocated memory to store the rotation data
    if(!pMatrix) return;
    
    // Fill in the rows of the 4x4 matrix, according to the quaternion to matrix equations

    // First row
    pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z );  
    pMatrix[ 1] = 2.0f * ( x * y - w * z );  
    pMatrix[ 2] = 2.0f * ( x * z + w * y );  
    pMatrix[ 3] = 0.0f;  

    // Second row
    pMatrix[ 4] = 2.0f * ( x * y + w * z );  
    pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z );  
    pMatrix[ 6] = 2.0f * ( y * z - w * x );  
    pMatrix[ 7] = 0.0f;  

    // Third row
    pMatrix[ 8] = 2.0f * ( x * z - w * y );  
    pMatrix[ 9] = 2.0f * ( y * z + w * x );  
    pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );  
    pMatrix[11] = 0.0f;  

    // Fourth row
    pMatrix[12] = 0;  
    pMatrix[13] = 0;  
    pMatrix[14] = 0;  
    pMatrix[15] = 1.0f;

    // Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}


///////////////////////////////// CREATE FROM MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This creates a quaternion from a 3x3 or a 4x4 matrix, depending on rowColumnCount
/////
///////////////////////////////// CREATE FROM MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuaternion::CreateFromMatrix(float *pTheMatrix, int rowColumnCount)
{
    // Make sure the matrix has valid memory and it's not expected that we allocate it.
    // Also, we do a check to make sure the matrix is a 3x3 or a 4x4 (must be 3 or 4).
    if(!pTheMatrix || ((rowColumnCount != 3) && (rowColumnCount != 4))) return;

    // This function is used to take in a 3x3 or 4x4 matrix and convert the matrix
    // to a quaternion.  If rowColumnCount is a 3, then we need to convert the 3x3
    // matrix passed in to a 4x4 matrix, otherwise we just leave the matrix how it is.
    // Since we want to apply a matrix to an OpenGL matrix, we need it to be 4x4.

    // Point the matrix pointer to the matrix passed in, assuming it's a 4x4 matrix
    float *pMatrix = pTheMatrix;

    // Create a 4x4 matrix to convert a 3x3 matrix to a 4x4 matrix (If rowColumnCount == 3)
    float m4x4[16] = {0};

    // If the matrix is a 3x3 matrix (which it is for Quake3), then convert it to a 4x4
    if(rowColumnCount == 3)
    {
        // Set the 9 top left indices of the 4x4 matrix to the 9 indices in the 3x3 matrix.
        // It would be a good idea to actually draw this out so you can visualize it.
        m4x4[0]  = pTheMatrix[0];   m4x4[1]  = pTheMatrix[1];   m4x4[2]  = pTheMatrix[2];
        m4x4[4]  = pTheMatrix[3];   m4x4[5]  = pTheMatrix[4];   m4x4[6]  = pTheMatrix[5];
        m4x4[8]  = pTheMatrix[6];   m4x4[9]  = pTheMatrix[7];   m4x4[10] = pTheMatrix[8];

        // Since the bottom and far right indices are zero, set the bottom right corner to 1.
        // This is so that it follows the standard diagonal line of 1's in the identity matrix.
        m4x4[15] = 1;

        // Set the matrix pointer to the first index in the newly converted matrix
        pMatrix = &m4x4[0];
    }

    // The next step, once we made sure we are dealing with a 4x4 matrix, is to check the
    // diagonal of the matrix.  This means that we add up all of the indices that comprise
    // the standard 1's in the identity matrix.  If you draw out the identity matrix of a
    // 4x4 matrix, you will see that they 1's form a diagonal line.  Notice we just assume
    // that the last index (15) is 1 because it is not effected in the 3x3 rotation matrix.

    // Find the diagonal of the matrix by adding up it's diagonal indices.
    // This is also known as the "trace", but I will call the variable diagonal.
    float diagonal = pMatrix[0] + pMatrix[5] + pMatrix[10] + 1;
    float scale = 0.0f;

    // Below we check if the diagonal is greater than zero.  To avoid accidents with
    // floating point numbers, we substitute 0 with 0.000001.  If the diagonal is
    // great than zero, we can perform an "instant" calculation, otherwise we will need
    // to identify which diagonal element has the greatest value.  Note, that it appears
    // that %99 of the time, the diagonal IS greater than 0 so the rest is rarely used.

    // If the diagonal is greater than zero
    if(diagonal > SMALL_VAL)
    {
        // Calculate the scale of the diagonal
        scale = float(sqrtfCa(diagonal ) * 2);

        // Calculate the x, y, x and w of the quaternion through the respective equation
        x = ( pMatrix[9] - pMatrix[6] ) / scale;
        y = ( pMatrix[2] - pMatrix[8] ) / scale;
        z = ( pMatrix[4] - pMatrix[1] ) / scale;
        w = 0.25f * scale;
    }
    else 
    {
        // If the first element of the diagonal is the greatest value
        if ( pMatrix[0] > pMatrix[5] && pMatrix[0] > pMatrix[10] )  
        {   
            // Find the scale according to the first element, and double that value
            scale  = sqrtfCa( 1.0f + pMatrix[0] - pMatrix[5] - pMatrix[10] ) * 2.0f;

            // Calculate the x, y, x and w of the quaternion through the respective equation
            x = 0.25f * scale;
            y = (pMatrix[4] + pMatrix[1] ) / scale;
            z = (pMatrix[2] + pMatrix[8] ) / scale;
            w = (pMatrix[9] - pMatrix[6] ) / scale; 
        } 
        // Else if the second element of the diagonal is the greatest value
        else if ( pMatrix[5] > pMatrix[10] ) 
        {
            // Find the scale according to the second element, and double that value
            scale  = sqrtfCa( 1.0f + pMatrix[5] - pMatrix[0] - pMatrix[10] ) * 2.0f;
            
            // Calculate the x, y, x and w of the quaternion through the respective equation
            x = (pMatrix[4] + pMatrix[1] ) / scale;
            y = 0.25f * scale;
            z = (pMatrix[9] + pMatrix[6] ) / scale;
            w = (pMatrix[2] - pMatrix[8] ) / scale;
        } 
        // Else the third element of the diagonal is the greatest value
        else 
        {   
            // Find the scale according to the third element, and double that value
            scale  = sqrtfCa( 1.0f + pMatrix[10] - pMatrix[0] - pMatrix[5] ) * 2.0f;

            // Calculate the x, y, x and w of the quaternion through the respective equation
            x = (pMatrix[2] + pMatrix[8] ) / scale;
            y = (pMatrix[9] + pMatrix[6] ) / scale;
            z = 0.25f * scale;
            w = (pMatrix[4] - pMatrix[1] ) / scale;
        }
    }
}


/////////////////////////////////////// SLERP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   Returns a spherical linear interpolated quaternion between q1 and q2, with respect to t
/////
/////////////////////////////////////// SLERP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuaternion CQuaternion::Slerp(CQuaternion &q1, CQuaternion &q2, float t)
{
    // Create a local quaternion to store the interpolated quaternion
    CQuaternion qInterpolated;

    // This function is the milk and honey of our quaternion code, the rest of
    // the functions are an appendage to what is done here.  Everyone understands
    // the terms, "matrix to quaternion", "quaternion to matrix", "create quaternion matrix",
    // "quaternion multiplication", etc.. but "SLERP" is the stumbling block, even a little 
    // bit after hearing what it stands for, "Spherical Linear Interpolation".  What that
    // means is that we have 2 quaternions (or rotations) and we want to interpolate between 
    // them.  The reason what it's called "spherical" is that quaternions deal with a sphere.  
    // Linear interpolation just deals with 2 points primarily, where when dealing with angles
    // and rotations, we need to use sin() and cos() for interpolation.  If we wanted to use
    // quaternions for camera rotations, which have much more instant and jerky changes in 
    // rotations, we would use Spherical-Cubic Interpolation.  The equation for SLERP is this:
    //
    // q = (((b.a)^-1)^t)a
    //
    // Go here for an a detailed explanation and proofs of this equation:
    //
    // http://www.magic-software.com/Documentation/quat.pdf
    //
    // Now, Let's code it

    // Here we do a check to make sure the 2 quaternions aren't the same, return q1 if they are
    if(q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w) 
        return q1;

    // Following the (b.a) part of the equation, we do a dot product between q1 and q2.
    // We can do a dot product because the same math applied for a 3D vector as a 4D vector.
    float result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

    // If the dot product is less than 0, the angle is greater than 90 degrees
    if(result < 0.0f)
    {
        // Negate the second quaternion and the result of the dot product
        q2 = CQuaternion(-q2.x, -q2.y, -q2.z, -q2.w);
        result = -result;
    }

    // Set the first and second scale for the interpolation
    float scale0 = 1 - t, scale1 = t;

    // Next, we want to actually calculate the spherical interpolation.  Since this
    // calculation is quite computationally expensive, we want to only perform it
    // if the angle between the 2 quaternions is large enough to warrant it.  If the
    // angle is fairly small, we can actually just do a simpler linear interpolation
    // of the 2 quaternions, and skip all the complex math.  We create a "delta" value
    // of 0.1 to say that if the cosine of the angle (result of the dot product) between
    // the 2 quaternions is smaller than 0.1, then we do NOT want to perform the full on 
    // interpolation using.  This is because you won't really notice the difference.

    // Check if the angle between the 2 quaternions was big enough to warrant such calculations
    if(1 - result > 0.1f)
    {
        // Get the angle between the 2 quaternions, and then store the sin() of that angle
        float theta    = acosfCa(result);
        float sinTheta = sinfCa(theta);

        // Calculate the scale for q1 and q2, according to the angle and it's sine value
        scale0 = sinfCa( ( 1 - t ) * theta) / sinTheta;
        scale1 = sinfCa( ( t * theta) ) / sinTheta;
    }   

    // Calculate the x, y, z and w values for the quaternion by using a special
    // form of linear interpolation for quaternions.
    qInterpolated.x = (scale0 * q1.x) + (scale1 * q2.x);
    qInterpolated.y = (scale0 * q1.y) + (scale1 * q2.y);
    qInterpolated.z = (scale0 * q1.z) + (scale1 * q2.z);
    qInterpolated.w = (scale0 * q1.w) + (scale1 * q2.w);

    // Return the interpolated quaternion
    return qInterpolated;
}
