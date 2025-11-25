#pragma once
#include <unordered_map>
#include <vector>

struct Material;
class Mesh;

using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
using MaterialMap = std::unordered_map<uint32_t, Material>;

namespace Models {
class Cube {
public:
	explicit Cube(const char* diffuseTexture = nullptr,
				   const char* specularTexture = nullptr,
				   const char* normalTexture = nullptr,
				   const char* heightTexture = nullptr);

	~Cube();

	[[nodiscard]] MeshMap* getMeshes();

	[[nodiscard]] const MaterialMap* getMaterial() const;

private:
	MeshMap meshes;
	MaterialMap material;
};
}
