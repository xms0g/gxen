#pragma once
#include <cstdint>

enum RenderFlags: uint32_t {
	Forward = 1 << 0,
	Deferred = 1 << 1,
	Transparent = 1 << 2,
	Instanced = 1 << 3,
	TwoSided = 1 << 4,
	CastShadow = 1 << 5
};
