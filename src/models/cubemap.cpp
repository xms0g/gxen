#include "cubemap.h"
#include "../config/config.hpp"
#include "../mesh/mesh.h"
#include "../io/filesystem.hpp"
#include "../resourceManager/texture.h"

Models::Cubemap::Cubemap(const char* const f[]) {
	constexpr float v[]= {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f
	};

	std::vector<Vertex> vertices;
	for (int i = 0; i < 36; i++) {
		Vertex vertex{};
		vertex.position = glm::vec3(v[i*3], v[i*3+1], v[i*3+2]);
		vertices.emplace_back(vertex);
	}

	std::vector<uint32_t> indices;
	meshes[0].emplace_back(vertices, indices);

	std::vector<std::string> faces;
	faces.reserve(6);

	for (int i = 0; i < 6; i++) {
		faces.emplace_back(fs::path(ASSET_DIR + f[i]));
	}

	std::vector<Texture> textures;
	textures.emplace_back(texture::loadCubemap(faces),"skybox","skybox.jpg");
	material[0] = {0, textures};
}

Models::Cubemap::~Cubemap() = default;

