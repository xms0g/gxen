#include "cube.h"

#include "cubemap.h"
#include "../config/config.hpp"
#include "../mesh/mesh.h"
#include "../io/filesystem.hpp"
#include "../resourceManager/texture.h"

Models::Cube::Cube(const char* texture) {
	constexpr float v[] = {
		// Back face (-Z)
		-0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
		 0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
		 0.5f, 0.5f,-0.5f,  0,0,-1, 1,1,
		-0.5f, 0.5f,-0.5f,  0,0,-1, 0,1,

		// Front face (+Z)
		-0.5f,-0.5f, 0.5f,  0,0,1, 0,0,
		 0.5f,-0.5f, 0.5f,  0,0,1, 1,0,
		 0.5f, 0.5f, 0.5f,  0,0,1, 1,1,
		-0.5f, 0.5f, 0.5f,  0,0,1, 0,1,

		// Left face (-X)
		-0.5f,-0.5f,-0.5f, -1,0,0, 0,0,
		-0.5f, 0.5f,-0.5f, -1,0,0, 1,0,
		-0.5f, 0.5f, 0.5f, -1,0,0, 1,1,
		-0.5f,-0.5f, 0.5f, -1,0,0, 0,1,

		// Right face (+X)
		 0.5f,-0.5f,-0.5f,  1,0,0, 0,0,
		 0.5f, 0.5f,-0.5f,  1,0,0, 1,0,
		 0.5f, 0.5f, 0.5f,  1,0,0, 1,1,
		 0.5f,-0.5f, 0.5f,  1,0,0, 0,1,

		// Bottom face (-Y)
		-0.5f,-0.5f,-0.5f,  0,-1,0, 0,0,
		 0.5f,-0.5f,-0.5f,  0,-1,0, 1,0,
		 0.5f,-0.5f, 0.5f,  0,-1,0, 1,1,
		-0.5f,-0.5f, 0.5f,  0,-1,0, 0,1,

		// Top face (+Y)
		-0.5f, 0.5f,-0.5f,  0,1,0, 0,0,
		 0.5f, 0.5f,-0.5f,  0,1,0, 1,0,
		 0.5f, 0.5f, 0.5f,  0,1,0, 1,1,
		-0.5f, 0.5f, 0.5f,  0,1,0, 0,1
	};

	std::vector<uint32_t> indices = {
		// Back
		0, 2, 1,
		0, 3, 2,
		// Front
		4, 5, 6,
		4, 6, 7,
		// Left
		8, 10, 9,
		8, 11, 10,
		// Right
		12, 13, 14,
		12, 14, 15,
		// Bottom
		16, 17, 18,
		16, 18, 19,
		// Top
		20, 22, 21,
		20, 23, 22
	};


	std::vector<Vertex> vertices;

	for (int i = 0; i < 8 * 24; i += 8) {
		Vertex vertex;
		vertex.position = glm::vec3(v[i], v[i + 1], v[i + 2]);
		vertex.normal = glm::vec3(v[i + 3], v[i + 4], v[i + 5]);
		vertex.texcoord = glm::vec2(v[i + 6], v[i + 7]);

		vertices.emplace_back(vertex);
	}

	meshes.emplace_back(vertices, indices);

	if (texture) {
		textures.emplace_back(
		texture::load(fs::path(ASSET_DIR + texture).c_str(), "texture_diffuse"),
		"texture_diffuse",
		texture);
	}
}

Models::Cube::~Cube() = default;

