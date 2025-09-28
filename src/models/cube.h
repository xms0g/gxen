#pragma once
#include <vector>

struct Texture;
class Mesh;

namespace Models {
class Cube {
public:
	explicit Cube(const char* texture = nullptr);

	const std::vector<Mesh>* getMeshes() const { return &meshes; }
	const std::vector<Texture>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
}
