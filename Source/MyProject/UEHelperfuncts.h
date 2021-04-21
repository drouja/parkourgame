#pragma once
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include <string>
#include <algorithm>
#include <math.h>

template<class T>
inline T clamp(const T in, T min, T max)
{
	if (min > max) std::swap <T>(min, max);
	return std::max(min, std::min(in, max));
}

inline float normalize360(float angle)
{
	angle = fmod(angle, 360.0); // make sure angle is within 360 degrees
	if (angle < 0) angle += 360;
	return angle;
}

inline float clampangle(float ang, float min, float max)
{
	if (min > max) std::swap(min, max);
	float n_min = UKismetMathLibrary::NormalizeAxis(min - ang);
	float n_max = UKismetMathLibrary::NormalizeAxis(max - ang);
	if (n_min < 0 && n_max>0) return ang;
	if (std::abs(n_min) < std::abs(n_max)) return min;
	return max;
}

inline FVector normalizevector(FVector invector)
{
	float length = invector.Size();
	return invector / length;
}