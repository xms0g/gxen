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
	               const char* normalTexture = nullptr);

	~Plane();

	[[nodiscard]] const std::vector<Mesh>* getMeshes() const { return &meshes; }
	[[nodiscard]] const std::unordered_map<uint32_t, std::vector<Texture>>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::unordered_map<uint32_t, std::vector<Texture>> textures;
};
}
