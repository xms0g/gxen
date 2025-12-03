#pragma once

namespace math {
inline float lerp(const float a, const float b, const float t) {
	return a + (b - a) * t;
}
}