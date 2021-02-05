// vectorMath.h

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

#ifndef CA_VECTORMATH_H
#define CA_VECTORMATH_H

#include <math.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define SMALL_VAL 0.000001f

const float kPi			= 3.141592654f;
const float k2Pi		= kPi * 2.0f;
const float kPiOver2	= kPi / 2.0f;
const float k1OverPi	= 1.0f / kPi;
const float k1Over2Pi	= 1.0f / k2Pi;
const float kPiOver180	= kPi / 180.0f;
const float k180OverPi	= 180.0f / kPi;

inline float degToRad(float deg) { return deg * kPiOver180; }
inline float radToDeg(float rad) { return rad * k180OverPi; }

inline float fabsfCa(float x) { return x >= 0.0f ? x : -x; }

#if __APPLE_CC__ | _WIN32

inline float sqrtfCa (float f) { return sqrtf(f); }
inline float sinfCa (float f) { return sinf(f); }
inline float cosfCa (float f) { return cosf(f); }
inline float asinfCa (float x)
{
	if      (x <= -1.0f) return -kPiOver2;
	else if (x >=  1.0f) return  kPiOver2;
	else				 return asinf(x);
}
inline float acosfCa(float x)
{
	if      (x <= -1.0f) return kPi;
	else if (x >=  1.0f) return 0.0f;
	else                 return acosf(x);
}
inline float atanfCa (float f) { return atanf(f); }
inline float atan2fCa (float y, float x) { return atan2f(y, x); }

#elif _PSP

#include "fastMathPSP.h"

#endif

class Vector4
{
public:
	float x, y, z, w;

	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

	Vector4(float X, float Y, float Z) : x(X), y(Y), z(Z), w(1.0f) {}

	Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

	Vector4 operator -() const { return Vector4(-x,-y,-z); }

	Vector4 operator +(const Vector4 &v) const { return Vector4(x + v.x, y + v.y, z + v.z); }

	Vector4 operator -(const Vector4 &v) const { return Vector4(x - v.x, y - v.y, z - v.z); }

	Vector4 operator *(float f) const { return Vector4(x*f, y*f, z*f); }

	Vector4 operator /(float f) const
	{
		f = 1.0f / f;
		return Vector4(x*f, y*f, z*f);
	}

	void normalize()
	{
		float len_2 = (x*x + y*y + z*z);
		if (len_2 > 0.0f) {
			float alen = 1.0f / sqrtfCa(len_2);
			x = x*alen;
			y = y*alen;
			z = z*alen;
		}
	}

	// dot product
	float operator *(const Vector4 &v) const { return (x*v.x + y*v.y + z*v.z); }

	Vector4 getRotatedX(float angle) const;
	Vector4 getRotatedY(float angle) const;
	Vector4 getRotatedZ(float angle) const;
	Vector4 getRotatedAxis(float angle, const Vector4 &axis_vector) const;
	
	void print(const char *text = "", bool bShowCleaned = true) const;
};

// Nonmember functions:

// zur Symmetrie hier auch float * Vector4 definiert
inline Vector4 operator *(float f, const Vector4 &v) { return Vector4(f*v.x, f*v.y, f*v.z); }

inline Vector4 crossProduct(const Vector4 &v1, const Vector4 &v2)
{
	return Vector4(
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x
	);
}

inline float vecLength(const Vector4 &v) { return sqrtfCa(v.x*v.x + v.y*v.y + v.z*v.z); }

inline float vecDistance(const Vector4 &v1, const Vector4 &v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;
	return sqrtfCa(dx*dx + dy*dy + dz*dz);
}

inline float vecDistanceSquared(const Vector4 &v1, const Vector4 &v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;
	return (dx*dx + dy*dy + dz*dz);
}

float findYawAngleBetweenVectors(const Vector4 &v1, const Vector4 &v2);

class Matrix4
{
public:
	float matrix[16];

	Matrix4() { makeIdentity(); }

	void makeIdentity();
	void setupProjection(float fovy, float aspect, float near_p, float far_p);
	void setupProjectionOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void setupLookAt(const Vector4 &pos, const Vector4 &forward, const Vector4 &up, const Vector4 &right);
	void setupTranslate(const Vector4 &Translate);
	void setupTranslate(float x, float y, float z);
    void setupRotateX(float angle);
    void setupRotateY(float angle);
    void setupRotateZ(float angle);
	void setupScaling(float x, float y, float z);
	void print(const char *text = "", bool bShowCleaned = true) const;
};

Vector4	operator*(const Matrix4 &m, const Vector4 &v);
Matrix4	operator*(const Matrix4 &a, const Matrix4 &b);

Matrix4 matrixInverse(const Matrix4 &m);

class Ray3D
{
public:
	Vector4 point;
	Vector4 dir_norm;

	Ray3D() : point(Vector4(0,0,0)), dir_norm(Vector4(0,0,-1)) {}
	Ray3D(Vector4 Point, Vector4 dir_norm) : point(Point), dir_norm(dir_norm) {}
};

class Sphere3D
{
public:
	Vector4 center;
	float	radius;

	Sphere3D() : center(Vector4(0,0,0)), radius(0.0f) {}
	Sphere3D(Vector4 Center, float Radius) : center(Center), radius(Radius) {}
};

bool rayrayIntersect2D(Ray3D *r1, Ray3D *r2, Vector4 *ip);

int raySphereIntersectNew(Ray3D *pRay, Sphere3D *pSphere, float *t1, float *t2, Vector4 *p1, Vector4 *p2);

bool raySphereIntersect(const Vector4 &rayOrigin, const Vector4 &rayDirection, float rayLength, const Vector4 &sphereCenter, float sphereRadius, float *pfDistToSphereCenter);

int rayTriangleIntersectDir( const Vector4 &orig, const Vector4 &dir, 
							 const Vector4 &vert0, const Vector4 &vert1, const Vector4 &vert2,
							 float *t, float *u, float *v, bool bTestCull = true, float fEpsilon = 0.000001f );

int rayTriangleIntersectEnd( const Vector4 &orig, const Vector4 &end, 
							 const Vector4 &vert0, const Vector4 &vert1, const Vector4 &vert2,
							 float *t, float *u, float *v, bool bTestCull = true, float fEpsilon = 0.000001f );

bool makeSphereCollision(const Vector4 &moverStart, Vector4* pMoverEnd, Vector4 blocker, float fMinDist);

void makePickingOriginAndDir(int hitX, int hitY, int vpWidth, int vpHeight, const Matrix4 &projMat, const Matrix4 &viewMat, Vector4 *rayOrigin, Vector4 *rayDir);
void makePickingOriginAndDirOrtho(int hitX, int hitY, const Vector4 &pos, const Vector4 &lookat, int vpWidth, int vpHeight, float left, float right, float bottom, float top, Vector4 *rayOrigin, Vector4 *rayDir);

void splitRGBA(unsigned int color, int &r, int &g, int &b, int &a);
unsigned int clampMergeRGBA(int r, int g, int b, int a);
unsigned int clampMergeRGBAv(const Vector4 &col);

bool pointInTriangle(const Vector4 &p, const Vector4 &p0, const Vector4 &p1, const Vector4 &p2);

float roundNK(float val, int iNK);

enum eAxis
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

struct AABB_t
{
	Vector4 min, max;

	AABB_t() {};
	
	AABB_t(const Vector4 &min, const Vector4 &max)
	{
		this->min = min;
		this->max = max;
	};
};

// This is our quaternion class
class CQuaternion 
{

public:

    // This is our default constructor, which initializes everything to an identity
    // quaternion.  An identity quaternion has x, y, z as 0 and w as 1.
    CQuaternion() { x = y = z = 0.0f;   w = 1.0f;  }
    
    // Creates a constructor that will allow us to initialize the quaternion when creating it
    CQuaternion(float X, float Y, float Z, float W) 
    { 
        x = X;      y = Y;      z = Z;      w = W;
    }

    // This takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
    void CreateMatrix(float *pMatrix);

    // This takes a 3x3 or 4x4 matrix and converts it to a quaternion, depending on rowColumnCount
    void CreateFromMatrix(float *pMatrix, int rowColumnCount);

    // This returns a spherical linear interpolated quaternion between q1 and q2, according to t
    CQuaternion Slerp(CQuaternion &q1, CQuaternion &q2, float t);

private:

    // This stores the 4D values for the quaternion
    float x, y, z, w;
};

struct orientation_t
{
	Vector4 position;
	float pitch, yaw;
};

// TODO: anders lösen?, eViewType, edgeControlButtons_e übergabe an CComponent, CLight...

enum eViewType
{
	VIEWTYPE_3D,
	VIEWTYPE_TOP,
	VIEWTYPE_BOTTOM,
	VIEWTYPE_LEFT,
	VIEWTYPE_RIGHT,
	VIEWTYPE_FRONT,
	VIEWTYPE_BACK
};

enum edgeControlButtons_e
{
	EDGE_TOP_LEFT		= 0,
	EDGE_TOP_RIGHT		= 1,
	EDGE_BOTTOM_RIGHT	= 2,
	EDGE_BOTTOM_LEFT	= 3,
	FACE_TOP			= 4,
	FACE_RIGHT			= 5,
	FACE_BOTTOM			= 6,
	FACE_LEFT			= 7,
	OBJECT				= 8
};

#endif
