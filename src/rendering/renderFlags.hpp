#pragma once
#include <cstdint>

enum RenderFlags: uint32_t {
	Forward = 1 << 0,
	Deferred = 1 << 1,
	Opaque = 1 << 2,
	Transparent = 1 << 3,
	Instanced = 1 << 4,
	TwoSided = 1 << 5,
	CastShadow = 1 << 6
};
