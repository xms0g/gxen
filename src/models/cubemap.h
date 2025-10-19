#pragma once
#include <unordered_map>
#include <vector>

class Mesh;
struct Texture;

namespace Models {
class Cubemap {
public:
	explicit Cubemap(const char* const f[]);

	~Cubemap();

	[[nodiscard]] const std::vector<Mesh>* getMeshes() const { return &meshes; }
	[[nodiscard]] const std::unordered_map<uint32_t, std::vector<Texture>>* getTextures() const { return &textures; }

private:
	std::vector<Mesh> meshes;
	std::unordered_map<uint32_t, std::vector<Texture>> textures;
};
}
