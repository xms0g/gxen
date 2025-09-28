#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex {
#define MAX_BONE_INFLUENCE 4
	// position
	glm::vec3 position;
	// normal
	glm::vec3 normal;
	// texCoords
	glm::vec2 texcoord;
	// tangent
	glm::vec3 tangent;
	// bitangent
	glm::vec3 bitangent;
	//bone indexes which will influence this vertex
	int boneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float weights[MAX_BONE_INFLUENCE];
};

class Mesh {
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	[[nodiscard]] const std::vector<Vertex>& vertices() const { return mVertices; }

	[[nodiscard]] const std::vector<unsigned int>& indices() const { return mIndices; }

	[[nodiscard]] unsigned int VAO() const { return mVAO; }

private:
	// mesh Data
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	unsigned int mVAO{}, mVBO{}, mEBO{};
};
