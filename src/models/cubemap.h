#pragma once
#include <unordered_map>
#include <vector>

class Mesh;
struct Texture;

namespace Models {
class Cubemap {
public:
	explicit Cubemap(const char* const f[]);

	~Cubemap();

	using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
	[[nodiscard]] MeshMap* getMeshes() { return &meshes; }

	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture>>;
	[[nodiscard]] const TextureMap* getTextures() const { return &textures; }

private:
	MeshMap meshes;
	TextureMap textures;
};
}
