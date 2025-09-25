#include "renderSystem.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "glm/gtx/quaternion.hpp"
#include "window.h"
#include "gui.h"
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
	RequireComponent<MaterialComponent>();

	mWindow = std::make_unique<Window>();
	mWindow->init("XEngine");

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}

	mGui = std::make_unique<Gui>(mWindow->nativeHandle(), mWindow->glContext());

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void RenderSystem::render(const Camera* camera) const {
	mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

	for (const auto& entity: getSystemEntities()) {
		const auto& shader = entity.getComponent<ShaderComponent>().shader;

		shader->activate();

		geometryPass(entity, camera, shader);
		materialPass(entity, shader);
		lightingPass(shader);
		drawPass(entity);
	}
#ifdef DEBUG
	mGui->render();
#endif

	// SDL swap buffers
	mWindow->swapBuffer();
}

void RenderSystem::geometryPass(const Entity& entity, const Camera* camera,
                                const std::shared_ptr<Shader>& shader) const {
	auto& tc = entity.getComponent<TransformComponent>();

	mGui->updateTransform(tc.position, tc.rotation, tc.scale);

	shader->setVec3("viewPos", camera->position());

	const glm::mat4 projectionMat = glm::perspective(glm::radians(camera->zoom()),
	                                                 static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
	                                                 ZNEAR, ZFAR);
	shader->setMat4("projection", projectionMat);
	shader->setMat4("view", camera->viewMatrix());

	auto modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, tc.position);
	modelMat *= glm::toMat4(glm::quat(glm::radians(tc.rotation)));
	modelMat *= glm::scale(glm::mat4(1.0f), tc.scale);
	shader->setMat4("model", modelMat);

	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMat)));
	shader->setMat3("normalMatrix", normalMatrix);
}

void RenderSystem::materialPass(const Entity& entity, const std::shared_ptr<Shader>& shader) const {
	unsigned int diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1;
	const auto& mtc = entity.getComponent<MaterialComponent>();
	const auto textures = *mtc.textures;

	shader->setFloat("material.shininess", mtc.shininess);

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to string
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to string
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to string

		// now set the sampler to the correct texture unit
		shader->setInt(std::string("material.") + name + number, i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}

void RenderSystem::lightingPass(const std::shared_ptr<Shader>& shader) const {
	// Directional lights
	const auto& dirLights = mLightSystem->getDirLights();
	shader->setInt("numDirLights", dirLights.size());
	for (int i = 0; i < dirLights.size(); i++) {
		shader->setVec3("dirLights[" + std::to_string(i) + "].direction", dirLights[i]->direction);
		shader->setVec3("dirLights[" + std::to_string(i) + "].ambient", dirLights[i]->ambient);
		shader->setVec3("dirLights[" + std::to_string(i) + "].diffuse", dirLights[i]->diffuse);
		shader->setVec3("dirLights[" + std::to_string(i) + "].specular", dirLights[i]->specular);
	}
	// Point lights
	const auto& pointLights = mLightSystem->getPointLights();
	shader->setInt("numPointLights", pointLights.size());
	for (int i = 0; i < pointLights.size(); i++) {
		shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i]->position);
		shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i]->ambient);
		shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i]->diffuse);
		shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i]->specular);
		shader->setFloat("pointLights[" + std::to_string(i) + "].Kc", pointLights[i]->Kc);
		shader->setFloat("pointLights[" + std::to_string(i) + "].Kl", pointLights[i]->Kl);
		shader->setFloat("pointLights[" + std::to_string(i) + "].Kq", pointLights[i]->Kq);
	}
	// Spot Lights
	const auto& spotLights = mLightSystem->getSpotLights();
	shader->setInt("numSpotLights", spotLights.size());
	for (int i = 0; i < spotLights.size(); i++) {
		shader->setVec3("spotLights[" + std::to_string(i) + "].position", spotLights[i]->position);
		shader->setVec3("spotLights[" + std::to_string(i) + "].direction", spotLights[i]->direction);
		shader->setFloat("spotLights[" + std::to_string(i) + "].cutOff",
		                 glm::cos(glm::radians(spotLights[i]->cutOff)));
		shader->setFloat("spotLights[" + std::to_string(i) + "].outerCutOff",
		                 glm::cos(glm::radians(spotLights[i]->outerCutOff)));
		shader->setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLights[i]->ambient);
		shader->setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLights[i]->diffuse);
		shader->setVec3("spotLights[" + std::to_string(i) + "].specular", spotLights[i]->specular);
		shader->setFloat("spotLights[" + std::to_string(i) + "].Kc", spotLights[i]->Kc);
		shader->setFloat("spotLights[" + std::to_string(i) + "].Kl", spotLights[i]->Kl);
		shader->setFloat("spotLights[" + std::to_string(i) + "].Kq", spotLights[i]->Kq);
	}
}

void RenderSystem::drawPass(const Entity& entity) const {
	const auto& mc = entity.getComponent<MeshComponent>();

	for (auto mesh: *mc.meshes) {
		glBindVertexArray(mesh.VAO());
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices().size()), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}
