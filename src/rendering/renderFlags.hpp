#pragma once
#include <cstdint>

enum RenderFlags: uint32_t {
	Opaque = 1 << 0,
	Transparent = 1 << 1,
	Instanced = 1 << 2,
	TwoSided = 1 << 3
};
