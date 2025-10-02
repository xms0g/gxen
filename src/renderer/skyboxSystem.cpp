#include "skyboxSystem.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.h"
#include "../config/config.hpp"
#include "../core/camera.h"
#include "../mesh/mesh.h"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/skyboxComponent.hpp"
#include "../resourceManager/texture.h"

SkyboxSystem::SkyboxSystem() {
	RequireComponent<SkyboxComponent>();
}

void SkyboxSystem::render(const Camera& camera) const {
	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;
		const auto& mc = entity.getComponent<MeshComponent>();
		const auto& mat = entity.getComponent<MaterialComponent>();

		const glm::mat4 projectionMat = glm::perspective(
			glm::radians(camera.zoom()),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
			ZNEAR, ZFAR);

		shader->activate();
		shader->setInt(mat.textures->front().type, 0);
		shader->setMat4("projection", projectionMat);

		auto view = glm::mat4(glm::mat3(camera.viewMatrix()));
		shader->setMat4("view", view);

		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, mat.textures->front().id);

		// Draw
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(mc.meshes->front().VAO());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}
}
