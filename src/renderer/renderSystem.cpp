#include "renderSystem.h"
#include <iostream>
#include <SDL.h>
#include "glad/glad.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "lightSystem.h"
#include "shader.h"
#include "../mesh/mesh.h"
#include "../core/camera.h"
#include "../ECS/registry.h"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../resourceManager/texture.h"

RenderSystem::RenderSystem() {
	RequireComponent<MeshComponent>();
	RequireComponent<ShaderComponent>();
	RequireComponent<TransformComponent>();

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		throw std::runtime_error(std::string("ERROR::RENDERER::FAILED_TO_INIT_GLAD"));
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

bool RenderSystem::collectTransparentEntities(const Entity& entity, const Camera& camera, TransEntityBucket& bucket) {
	const auto& tc = entity.getComponent<TransformComponent>();
	const auto& mtc = entity.getComponent<MaterialComponent>();

	if (mtc.isTransparent) {
		float distance = glm::length(camera.position() - tc.position);
		bucket.emplace_back(distance, entity);
		return true;
	}
	return false;
}

void RenderSystem::transparentPass(TransEntityBucket& bucket) const {
	if (bucket.empty()) return;

	std::sort(bucket.begin(), bucket.end(),
			  [](const auto& a, const auto& b) { return a.first > b.first; });

	glDepthMask(GL_FALSE);
	for (auto& [dist, entity]: bucket) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		opaquePass(entity, *shader);
	}
	glDepthMask(GL_TRUE);
}

void RenderSystem::opaquePass(const Entity& entity, const Shader& shader) const {
	geometryPass(entity, shader);
	materialPass(entity, shader);
	drawPass(entity);
}

void RenderSystem::geometryPass(const Entity& entity, const Shader& shader) const {
	const auto& tc = entity.getComponent<TransformComponent>();

	auto modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, tc.position);
	modelMat *= glm::toMat4(glm::quat(glm::radians(tc.rotation)));
	modelMat *= glm::scale(glm::mat4(1.0f), tc.scale);
	shader.setMat4("model", modelMat);

	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMat)));
	shader.setMat3("normalMatrix", normalMatrix);
}

void RenderSystem::materialPass(const Entity& entity, const Shader& shader) const {
	const auto& mtc = entity.getComponent<MaterialComponent>();

	shader.setFloat("material.shininess", mtc.shininess);

	if (!mtc.textures) {
		shader.setBool("useTexture", false);
		shader.setVec3("material.color", mtc.color);
		return;
	}

	shader.setBool("useTexture", true);
	const auto textures = *mtc.textures;

	unsigned int diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		// now set the sampler to the correct texture unit
		shader.setInt(std::string("material.").append(name).append(number), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}

void RenderSystem::drawPass(const Entity& entity) const {
	const bool isCulling = glIsEnabled(GL_CULL_FACE);
	const auto& mc = entity.getComponent<MeshComponent>();

	if (mc.isTwoSided && isCulling)
		glDisable(GL_CULL_FACE);
	for (const auto& mesh: *mc.meshes) {
		glBindVertexArray(mesh.VAO());
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices().size()), GL_UNSIGNED_INT, nullptr);
	}
	if (mc.isTwoSided && isCulling)
		glEnable(GL_CULL_FACE);
}