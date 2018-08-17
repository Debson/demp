#include "md_math.h"


namespace mdEngine
{
	namespace Math
	{

	}

	float Math::Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}
}