#pragma once
#include <unordered_map>
#include <vector>

struct Material;
class Mesh;
struct Texture;

using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
using MaterialMap = std::unordered_map<uint32_t, Material>;

namespace Models {
class Cubemap {
public:
	explicit Cubemap(const char* const f[]);

	~Cubemap();

	[[nodiscard]] MeshMap* getMeshes();

	[[nodiscard]] const MaterialMap* getMaterial() const;

private:
	MeshMap meshes;
	MaterialMap material;
};
}
