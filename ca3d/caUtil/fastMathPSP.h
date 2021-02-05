// fastMathPSP.h

#ifndef CA_FATSMATHPSP_H
#define CA_FATSMATHPSP_H

#include "pspvfpu.h"
#include <math.h>

// Fast square root calculation using the cpu
// param x: value to calc sqrt
// return square root of x
inline float sqrtfCa(float x)
{
	float output;

	asm("sqrt.s %0,%1" : "=f" (output) : "f" (x));
	return output;
}

// Fast sine calculation using the vfpu
// param x: value in radiants
// return value between -1 and 1
inline float sinfCa(float x)
{
	float ret;

	__asm__ volatile ( 
	"mtv %1, s531\n" 
	"vcst.s s431, VFPU_2_PI\n" 
	"vmul.s s631, s431, s531\n" 
	"vsin.s s531, s631\n" // or vcos.s 
	"mfv %0, s531\n" 
	: "=r"(ret) : "r"(x)); 

	return ret;
}

// Fast cosine calculation using the vfpu
// param x: value in radiants
// return value between -1 and 1
inline float cosfCa(float x)
{
	float ret;

	__asm__ volatile ( 
	"mtv %1, s531\n" 
	"vcst.s s431, VFPU_2_PI\n" 
	"vmul.s s631, s431, s531\n" 
	"vcos.s s531, s631\n" // or vcos.s 
	"mfv %0, s531\n" 
	: "=r"(ret) : "r"(x)); 

	return ret; 
}

// Fast arcsine calculation using the vfpu
// param x: value between -1 and 1
// return value between -pi/2 and pi/2
inline float asinfCa(float x)
{
	if      (x <= -1.0f) return -kPiOver2;
	else if (x >=  1.0f) return  kPiOver2;
	else
	{
		float ret;

		__asm__ volatile ( 
		"mtv %1, s531\n" 
		"vasin.s s531, s531\n"
		"vcst.s s431, VFPU_PI_2\n"
		"vmul.s s531, s531, s431\n"
		"mfv %0, s531\n" 
		: "=r"(ret) : "r"(x)); 

		return ret;
	}
}

// Fast arccosine calculation using the vfpu
// param x: value between -1 and 1
// return value between 0 and pi
inline float acosfCa(float x)
{
	if      (x <= -1.0f) return kPi;
	else if (x >=  1.0f) return 0.0f;
	else
	{
		float ret;

		__asm__ volatile (
		"mtv %1, s531\n"
		"vasin.s s531, s531\n"
		"vcst.s s431, VFPU_PI_2\n"
		"vmul.s s531, s431, s531\n"
		"vsub.s s531, s431, s531\n"
		"mfv %0, s531\n" : "=r" ( ret ) : "r" ( x ));

		return ret;
	}
}

// Fast arctangent calculation using the vfpu
// param x: value in R
// return arcotangent of x
inline float atanfCa(float x)
{
	float ret;

	__asm__ volatile (
	"mtv %1, s531\n"								// t = x
	"vone.s s532\n"
	"vsgn.s s431, s531\n"							// z = sign(x)
	"vmul.s s531, s531, s531\n"						// t = t*t
	"vadd.s s531, s531, s532\n"						// t++
	"vsqrt.s s531, s531\n"							// t = sqrt(t)
	"vdiv.s s531, s532, s531\n"						// t = 1/t
	"vasin.s s531, s531\n"							// t = asin(t)
	"vcst.s s521, VFPU_PI_2\n"						// r = PI/2
	"vmul.s s531, s531, s521\n"
	"vsub.s s521, s521, s531\n"						// r = pi/2 - asin = acos
	"vmul.s s521, s521, s431\n"						// r = r*z ( set sign ) 
	"mfv %0, s521" : "=r" ( ret ) : "r" ( x ));		// store result

	return ret;
}

// Fast atan2 calculation using the vfpu
// param x: x coord
// param y: y coord
// return arctangent of y/x using the signs of both arguments to determine the quadrant of the return value
inline float atan2fCa(float y, float x)
{
	if (x > 0.0f) return atanfCa(y/x);

	return atan2f(y, x);
}

#endif
