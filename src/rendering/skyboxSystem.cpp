#include "skyboxSystem.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.h"
#include "../config/config.hpp"
#include "../core/camera.h"
#include "../mesh/mesh.h"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/skybox.hpp"
#include "../resourceManager/texture.h"

SkyboxSystem::SkyboxSystem() {
	RequireComponent<SkyboxComponent>();
}

void SkyboxSystem::render(const Camera& camera) const {
	const auto& skybox = getSystemEntities().front();
	const auto& shader = skybox.getComponent<ShaderComponent>().shader;
	const uint32_t VAO = skybox.getComponent<MeshComponent>().meshes->at(0).front().VAO();
	const uint32_t texID = skybox.getComponent<MaterialComponent>().textures->at(0).front().id;

	const glm::mat4 projectionMat = glm::perspective(
		glm::radians(camera.zoom()),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		ZNEAR, ZFAR);

	shader->activate();
	shader->setInt("skybox", 0);
	shader->setMat4("projection", projectionMat);

	const auto view = glm::mat4(glm::mat3(camera.viewMatrix()));
	shader->setMat4("view", view);

	glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
	// and finally bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	// Draw
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}
