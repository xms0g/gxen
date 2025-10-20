#pragma once
#include <unordered_map>
#include <vector>

class Mesh;
struct Vertex;
struct Texture;

namespace Models {
class Plane {
public:
	explicit Plane(const char* diffuseTexture = nullptr,
	               const char* specularTexture = nullptr,
	               const char* normalTexture = nullptr,
	               const char* heightTexture = nullptr);

	~Plane();

	using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
	[[nodiscard]] const MeshMap* getMeshes() const { return &meshes; }

	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture>>;
	[[nodiscard]] const TextureMap* getTextures() const { return &textures; }

private:
	MeshMap meshes;
	TextureMap textures;
};
}
