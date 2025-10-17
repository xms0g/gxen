#include "cube.h"

#include "cubemap.h"
#include "../config/config.hpp"
#include "../mesh/mesh.h"
#include "../io/filesystem.hpp"
#include "../resourceManager/texture.h"

Models::Cube::Cube(const char* diffuseTexture,
				   const char* specularTexture,
				   const char* normalTexture) {
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
		Vertex vertex{};
		vertex.position = glm::vec3(v[i], v[i + 1], v[i + 2]);
		vertex.normal = glm::vec3(v[i + 3], v[i + 4], v[i + 5]);
		vertex.texcoord = glm::vec2(v[i + 6], v[i + 7]);

		vertices.emplace_back(vertex);
	}

	for (uint32_t i = 0; i < indices.size(); i += 3) {
		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		glm::vec3 pos0 = vertices[i0].position;
		glm::vec3 pos1 = vertices[i1].position;
		glm::vec3 pos2 = vertices[i2].position;

		glm::vec2 uv0 = vertices[i0].texcoord;
		glm::vec2 uv1 = vertices[i1].texcoord;
		glm::vec2 uv2 = vertices[i2].texcoord;

		glm::vec3 edge1 = pos1 - pos0;
		glm::vec3 edge2 = pos2 - pos0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		vertices[i0].tangent = vertices[i1].tangent = vertices[i2].tangent = glm::vec3(
														  f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
														  f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
														  f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z));

		vertices[i0].bitangent = vertices[i1].bitangent = vertices[i2].bitangent = glm::vec3(
															  f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
															  f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
															  f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z));
	}

	meshes.emplace_back(vertices, indices);

	if (diffuseTexture) {
		textures.emplace_back(
		texture::load(fs::path(ASSET_DIR + diffuseTexture).c_str()),
		"texture_diffuse",
		diffuseTexture);
	}

	if (specularTexture) {
		textures.emplace_back(
			texture::load(fs::path(ASSET_DIR + specularTexture).c_str()),
			"texture_specular",
			specularTexture);
	}

	if (normalTexture) {
		textures.emplace_back(
			texture::load(fs::path(ASSET_DIR + normalTexture).c_str()),
			"texture_normal",
			normalTexture);
	}
}

Models::Cube::~Cube() = default;

