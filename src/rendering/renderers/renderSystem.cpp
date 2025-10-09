#include "renderSystem.h"
#include <iostream>
#include <SDL.h>
#include "glad/glad.h"
#include "../lightSystem.h"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../../mesh/mesh.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/shader.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../resourceManager/texture.h"
#include "../../math/utils.hpp"

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
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void RenderSystem::opaquePass(const Entity& entity, const Shader& shader) const {
	geometryPass(entity, shader);
	materialPass(entity, shader);
	drawPass(entity);
}

void RenderSystem::geometryPass(const Entity& entity, const Shader& shader) const {
	const auto& tc = entity.getComponent<TransformComponent>();

	auto [model, normal] = math::computeModelMatrices(tc.position, tc.rotation, tc.scale);
	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", normal);
}

void RenderSystem::materialPass(const Entity& entity, const Shader& shader) const {
	const auto& mtc = entity.getComponent<MaterialComponent>();

	if (mtc.flags & TwoSided) {
		glDisable(GL_CULL_FACE);
	}

	shader.setFloat("material.shininess", mtc.shininess);

	const auto textures = *mtc.textures;

	uint32_t diffuseCount = 1, specularCount = 1, normalCount = 1, heightCount = 1;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseCount++);
		else if (name == "texture_specular")
			number = std::to_string(specularCount++);
		else if (name == "texture_normal")
			number = std::to_string(normalCount++);
		else if (name == "texture_height")
			number = std::to_string(heightCount++);

		// now set the sampler to the correct texture unit
		shader.setInt(std::string("material.").append(name).append(number), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}

void RenderSystem::drawPass(const Entity& entity) const {
	const auto& mc = entity.getComponent<MeshComponent>();

	for (const auto& mesh: *mc.meshes) {
		glBindVertexArray(mesh.VAO());
		glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(mesh.indices().size()), GL_UNSIGNED_INT, nullptr);
	}

	if (!glIsEnabled(GL_CULL_FACE))
		glEnable(GL_CULL_FACE);
}