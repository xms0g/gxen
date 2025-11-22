#pragma once
#include <unordered_map>
#include <vector>

struct Texture;
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

	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture>>;
	[[nodiscard]] const TextureMap* getTextures() const { return &textures; }

private:
	MeshMap meshes;
	TextureMap textures;
};
}
