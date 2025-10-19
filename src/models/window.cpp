#include "window.h"
#include "../config/config.hpp"
#include "../mesh/mesh.h"
#include "../io/filesystem.hpp"
#include "../resourceManager/texture.h"

Models::Window::Window(const char* texture) {
	constexpr float v[] = {
		// Positions       // Normals        // TexCoords
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 0
		0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 1
		1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 2
		1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f // 3
	};

	std::vector<uint32_t> indices = {
		0, 2, 1, // First triangle
		0, 3, 2 // Second triangle
	};

	std::vector<Vertex> vertices;

	for (int i = 0; i < 8 * 4; i += 8) {
		Vertex vertex{};
		vertex.position = glm::vec3(v[i], v[i + 1], v[i + 2]);
		vertex.normal = glm::vec3(v[i + 3], v[i + 4], v[i + 5]);
		vertex.texcoord = glm::vec2(v[i + 6], v[i + 7]);

		vertices.emplace_back(vertex);
	}

	meshes.emplace_back(0, vertices, indices);

	if (texture) {
		textures[0].emplace_back(
			texture::load(fs::path(ASSET_DIR + texture).c_str()),
			"texture_diffuse",
			texture
		);
	}
}
