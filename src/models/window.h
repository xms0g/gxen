#pragma once
#include <vector>

class Mesh;
struct Vertex;
struct Texture;

namespace Models {
class Window {
public:
	explicit Window(const char* texture = nullptr);

	[[nodiscard]] const std::vector<Mesh>* getMeshes() const { return &meshes; }
	[[nodiscard]] const std::vector<Texture>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
}


