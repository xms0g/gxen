#pragma once
#include <unordered_map>
#include <vector>
#include "../rendering/material.hpp"

class Mesh;
struct Texture;

namespace Models {
class Cubemap {
public:
	explicit Cubemap(const char* const f[]);

	~Cubemap();

	using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
	[[nodiscard]] MeshMap* getMeshes() { return &meshes; }

	using MaterialMap = std::unordered_map<uint32_t, Material>;
	[[nodiscard]] const MaterialMap* getMaterial() const { return &material; }

private:
	MeshMap meshes;
	MaterialMap material;
};
}
