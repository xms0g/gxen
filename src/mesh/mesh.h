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

struct InstanceData {
	glm::mat4 model;
	glm::mat3 normalMatrix;
};

class Mesh {
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	[[nodiscard]] const std::vector<Vertex>& vertices() const { return mVertices; }

	[[nodiscard]] const std::vector<unsigned int>& indices() const { return mIndices; }

	[[nodiscard]] uint32_t VAO() const { return mVAO; }

	void enableInstanceAttributes(uint32_t instanceVBO, size_t offset) const;

private:
	// mesh Data
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	uint32_t mVAO{}, mVBO{}, mEBO{};
};

void setupInstancing();
