#pragma once
#include <cstdint>
#include <vector>

enum MaterialFlags: uint32_t {
	Opaque = 1 << 0,
	Blend = 1 << 1,
	Cutout = 1 << 2,
	TwoSided = 1 << 3,
	CastShadow = 1 << 4,
};

struct Texture;

struct Material {
	uint32_t flag;
	std::vector<Texture> textures;
};
