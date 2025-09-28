#pragma once

#include <vector>

class Mesh;
struct Vertex;
struct Texture;

class WindowModel {
public:
	explicit WindowModel(const char* texture = nullptr);

	const std::vector<Mesh>* getMeshes() const { return &meshes; }
	const std::vector<Texture>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
