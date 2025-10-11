#pragma once
#include <vector>

class Mesh;
struct Texture;

namespace Models {
class Cubemap {
public:
	explicit Cubemap(const char* const f[]);

	~Cubemap();

	[[nodiscard]] const std::vector<Mesh>* getMeshes() const { return &meshes; }
	[[nodiscard]] const std::vector<Texture>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};
}
