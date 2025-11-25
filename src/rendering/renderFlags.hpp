#pragma once
#include <cstdint>

enum class RenderFlags: uint32_t {
	Forward,
	Deferred,
	Instanced
};
