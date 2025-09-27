#pragma once
#include <vector>

struct Texture;
class Mesh;

class Cube {
public:
	explicit Cube(const char* texture = nullptr);

	std::vector<Mesh>* getMeshes() { return &meshes; }
	std::vector<Texture>* getTextures() { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
