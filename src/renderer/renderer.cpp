#include "renderer.h"
#include <SDL.h>
#include <iostream>
#include "image/stb_image.h"
#include "glad/glad.h"
#include "window.h"
#include "gui.h"
#include "shader.h"
#include "../config/config.hpp"
#include "../core/camera.h"
#include "../model/model.h"
#include "../ECS/registry.h"
#include "../ECS/components/model.hpp"
#include "../ECS/components/transform.hpp"
#include "../ECS/components/materialComponent.hpp"

Renderer::Renderer() {
	RequireComponent<TransformComponent>();
	RequireComponent<ModelComponent>();
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

void Renderer::render(const Camera* camera) const {
	mWindow->clear(0.0f, 0.0f, 0.0f, 1.0f);

	for (const auto& entity: getSystemEntities()) {
		const auto& tc = entity.getComponent<TransformComponent>();
		const auto& mat = entity.getComponent<MaterialComponent>();

		const Shader* shader = mat.shader.get();
		shader->activate();

		// view/projection transformations
		glm::mat4 projectionMat = glm::perspective(glm::radians(camera->zoom()),
		                                           static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		                                           ZNEAR, ZFAR);
		shader->setMat4("projection", projectionMat);
		shader->setMat4("view", camera->viewMatrix());

		auto modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, tc.position);
		modelMat = glm::scale(modelMat, glm::vec3(tc.scale));

		if (tc.rotation != 0.0f) {
			static float angle = 0.0f;

			angle += tc.rotation;
			modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(0, 1, 0)); //rotation y = 0.0 degrees
		}
		shader->setMat4("model", modelMat);


		shader->setVec3("pointLight.position", glm::vec3(2.2f, 1.0f, 2.0f));
		shader->setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
		shader->setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);
		shader->setFloat("pointLight.Kc", 1.0f);
		shader->setFloat("pointLight.Kl", 0.09f);
		shader->setFloat("pointLight.Kq", 0.032f);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMat)));
		shader->setMat3("normalMatrix", normalMatrix);

		const auto& mc = entity.getComponent<ModelComponent>();
		mc.model->draw(shader);
	}

#ifdef DEBUG
	mGui->render();
#endif

	// SDL swap buffers
	mWindow->swapBuffer();
}
