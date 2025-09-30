#include "renderSystem.h"
#include <algorithm>
#include <iostream>
#include <SDL.h>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "glm/gtx/quaternion.hpp"
#include "lightSystem.h"
#include "shader.h"
#include "../mesh/mesh.h"
#include "../config/config.hpp"
#include "../core/camera.h"
#include "../ECS/registry.h"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/shader.hpp"
#include "../ECS/components/directionalLight.hpp"
#include "../ECS/components/material.hpp"
#include "../ECS/components/mesh.hpp"
#include "../ECS/components/pointLight.hpp"
#include "../ECS/components/spotLight.hpp"
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

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	mSceneBuffer = std::make_unique<FrameBuffer>(SCR_WIDTH, SCR_HEIGHT);
	mSceneBuffer->withTexture().withRenderBuffer().checkStatus();
}

void RenderSystem::render(const Camera* camera) {
	std::vector<std::pair<float, Entity> > transparentEntities;

	beginSceneRender();

	for (const auto& entity: getSystemEntities()) {
		if (collectTransparentEntities(entity, camera, transparentEntities))
			continue;

		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		opaquePass(entity, camera, *shader);
	}

	transparentPass(camera, transparentEntities);

	endSceneRender();
}

bool RenderSystem::collectTransparentEntities(const Entity& entity, const Camera* camera, TransEntityBucket& bucket) {
	const auto& tc = entity.getComponent<TransformComponent>();
	const auto& mtc = entity.getComponent<MaterialComponent>();

	if (mtc.isTransparent) {
		float distance = glm::length(camera->position() - tc.position);
		bucket.emplace_back(distance, entity);
		return true;
	}
	return false;
}

void RenderSystem::opaquePass(const Entity& entity, const Camera* camera, const Shader& shader) const {
	geometryPass(entity, camera, shader);
	materialPass(entity, shader);
	lightingPass(shader);
	drawPass(entity);
}

void RenderSystem::transparentPass(const Camera* camera, TransEntityBucket& bucket) {
	if (bucket.empty()) return;

	std::sort(bucket.begin(), bucket.end(),
					  [](const auto& a, const auto& b) { return a.first > b.first; });

	glDepthMask(GL_FALSE);
	for (auto& [dist, entity] : bucket) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();
		opaquePass(entity, camera, *shader);
	}
	glDepthMask(GL_TRUE);
}

void RenderSystem::geometryPass(const Entity& entity, const Camera* camera, const Shader& shader) const {
	const auto& tc = entity.getComponent<TransformComponent>();

	shader.setVec3("viewPos", camera->position());

	const glm::mat4 projectionMat = glm::perspective(glm::radians(camera->zoom()),
	                                                 static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
	                                                 ZNEAR, ZFAR);
	shader.setMat4("projection", projectionMat);
	shader.setMat4("view", camera->viewMatrix());

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

void RenderSystem::lightingPass(const Shader& shader) const {
	// Directional lights
	const auto& dirLights = mLightSystem->getDirLights();
	shader.setInt("numDirLights", dirLights.size());
	for (int i = 0; i < dirLights.size(); i++) {
		shader.setVec3("dirLights[" + std::to_string(i) + "].direction", dirLights[i]->direction);
		shader.setVec3("dirLights[" + std::to_string(i) + "].ambient", dirLights[i]->ambient);
		shader.setVec3("dirLights[" + std::to_string(i) + "].diffuse", dirLights[i]->diffuse);
		shader.setVec3("dirLights[" + std::to_string(i) + "].specular", dirLights[i]->specular);
	}
	// Point lights
	const auto& pointLights = mLightSystem->getPointLights();
	shader.setInt("numPointLights", pointLights.size());
	for (int i = 0; i < pointLights.size(); i++) {
		shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i]->position);
		shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i]->ambient);
		shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i]->diffuse);
		shader.setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i]->specular);
		shader.setFloat("pointLights[" + std::to_string(i) + "].Kc", pointLights[i]->Kc);
		shader.setFloat("pointLights[" + std::to_string(i) + "].Kl", pointLights[i]->Kl);
		shader.setFloat("pointLights[" + std::to_string(i) + "].Kq", pointLights[i]->Kq);
	}
	// Spot Lights
	const auto& spotLights = mLightSystem->getSpotLights();
	shader.setInt("numSpotLights", spotLights.size());
	for (int i = 0; i < spotLights.size(); i++) {
		shader.setVec3("spotLights[" + std::to_string(i) + "].position", spotLights[i]->position);
		shader.setVec3("spotLights[" + std::to_string(i) + "].direction", spotLights[i]->direction);
		shader.setFloat("spotLights[" + std::to_string(i) + "].cutOff",
		                 glm::cos(glm::radians(spotLights[i]->cutOff)));
		shader.setFloat("spotLights[" + std::to_string(i) + "].outerCutOff",
		                 glm::cos(glm::radians(spotLights[i]->outerCutOff)));
		shader.setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLights[i]->ambient);
		shader.setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLights[i]->diffuse);
		shader.setVec3("spotLights[" + std::to_string(i) + "].specular", spotLights[i]->specular);
		shader.setFloat("spotLights[" + std::to_string(i) + "].Kc", spotLights[i]->Kc);
		shader.setFloat("spotLights[" + std::to_string(i) + "].Kl", spotLights[i]->Kl);
		shader.setFloat("spotLights[" + std::to_string(i) + "].Kq", spotLights[i]->Kq);
	}
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

void RenderSystem::beginSceneRender() const {
	mSceneBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, mSceneBuffer->width(), mSceneBuffer->height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::endSceneRender() const {
	mSceneBuffer->unbind();
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
