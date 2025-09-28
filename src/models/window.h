#pragma once

#include <vector>

class Mesh;
struct Vertex;
struct Texture;

class WindowModel {
public:
	explicit WindowModel(const char* texture = nullptr);

	std::vector<Mesh>* getMeshes() { return &meshes; }
	std::vector<Texture>* getTextures() { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};