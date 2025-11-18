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
	float padding[3];
};

class Mesh {
public:
	Mesh(uint32_t materialID, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	[[nodiscard]] uint32_t materialID() const { return mMaterialID; }

	[[nodiscard]] const std::vector<Vertex>& vertices() const { return mVertices; }

	[[nodiscard]] const std::vector<uint32_t>& indices() const { return mIndices; }

	[[nodiscard]] uint32_t VAO() const { return mVAO; }

	void enableInstanceAttributes(uint32_t instanceVBO, size_t offset) const;

private:
	// mesh Data
	uint32_t mMaterialID;
	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
	uint32_t mVAO{}, mVBO{}, mEBO{};
};

void setupInstancing();
