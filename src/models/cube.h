#pragma once
#include <unordered_map>
#include <vector>
#include "../rendering/material.hpp"

class Mesh;

namespace Models {
class Cube {
public:
	explicit Cube(const char* diffuseTexture = nullptr,
				   const char* specularTexture = nullptr,
				   const char* normalTexture = nullptr,
				   const char* heightTexture = nullptr);

	~Cube();

	using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
	[[nodiscard]] MeshMap* getMeshes() { return &meshes; }

	using MaterialMap = std::unordered_map<uint32_t, Material>;
	[[nodiscard]] const MaterialMap* getMaterial() const { return &material; }

private:
	MeshMap meshes;
	MaterialMap material;
};
}
