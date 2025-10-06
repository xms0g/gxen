#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) : mVertices(std::move(vertices)),
	mIndices(std::move(indices)) {
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	// create vao
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// create buffers
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	// bind the buffer to be used
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0],
	             GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, normal)));

	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, texcoord)));

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, tangent)));

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,
		                       boneIDs)));

	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, weights)));

	glBindVertexArray(0);
}

void Mesh::enableInstanceAttributes(const GLuint instanceVBO, const size_t offset) const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	// Model matrix attributes (7–10)
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(7 + i);
		glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
		                      reinterpret_cast<void*>(offset + sizeof(glm::vec4) * i));
		glVertexAttribDivisor(7 + i, 1);
	}

	// Normal matrix attributes (11–13)
	for (int i = 0; i < 3; i++) {
		glEnableVertexAttribArray(11 + i);
		glVertexAttribPointer(11 + i, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
		                      reinterpret_cast<void*>(offset + sizeof(glm::mat4) + sizeof(glm::vec3) * i));
		glVertexAttribDivisor(11 + i, 1);
	}
}
