#pragma once
#include <vector>

struct Texture;
class Mesh;

namespace Models {
class Cube {
public:
	explicit Cube(const char* diffuseTexture = nullptr,
				   const char* specularTexture = nullptr,
				   const char* normalTexture = nullptr);

	~Cube();

	[[nodiscard]] const std::vector<Mesh>* getMeshes() const { return &meshes; }
	[[nodiscard]] const std::vector<Texture>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
}
