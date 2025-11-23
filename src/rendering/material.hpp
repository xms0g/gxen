#pragma once
#include <cstdint>
#include <vector>

struct Texture;

struct Material {
	uint32_t flag;
	std::vector<Texture> textures;
};
