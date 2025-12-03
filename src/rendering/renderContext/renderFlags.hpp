#pragma once
#include <cstdint>

enum RenderFlags: uint32_t {
	Forward = 1 << 0,
	Deferred = 1 << 1,
	Instanced = 1 << 2
};
