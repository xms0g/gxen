#include <iostream>
#include <ostream>
#include "core/engine.h"
#include "core/camera.h"
#include "renderer/shader.h"
#include "resourceManager/resourceManager.h"
#include "ECS/registry.h"
#include "ECS/components/directionalLight.hpp"
#include "ECS/components/material.hpp"
#include "ECS/components/mesh.hpp"
#include "ECS/components/pointLight.hpp"
#include "ECS/components/shader.hpp"
#include "ECS/components/spotLight.hpp"
#include "ECS/components/transform.hpp"
#include "mesh/mesh.h"
#include "models/cube.h"
#include "models/plane.h"
#include "models/window.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

int main() {
	Registry registry;

	try {
		XEngine xngn;
		xngn.init(&registry);

		auto backpack = registry.createEntity();
		backpack.addComponent<TransformComponent>(
			glm::vec3(-3.2f, 0.8f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f));

		ResourceManager::instance().loadModel(backpack.id(), "backpack/backpack.obj");

		backpack.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(backpack.id()));
		backpack.addComponent<MaterialComponent>(ResourceManager::instance().getTextures(backpack.id()), 32.0f);

		backpack.addComponent<ShaderComponent>(
			std::make_shared<Shader>("model.vert", "model.frag"));

		// Plane
		Models::Plane planeModel{"textures/metal.png", "textures/metal_specular.png"};
		auto plane = registry.createEntity();
		plane.addComponent<TransformComponent>(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f));

		plane.addComponent<MeshComponent>(planeModel.getMeshes(), true);
		plane.addComponent<MaterialComponent>(planeModel.getTextures(), 32.0f);

		plane.addComponent<ShaderComponent>(
			std::make_shared<Shader>("models/plane.vert", "models/plane.frag"));

		// CUbe
		Models::Cube cubeModel{"textures/wall.jpg"};
		auto cube = registry.createEntity();
		cube.addComponent<TransformComponent>(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f));

		cube.addComponent<MeshComponent>(cubeModel.getMeshes());
		cube.addComponent<MaterialComponent>(cubeModel.getTextures(), 32.0f);

		cube.addComponent<ShaderComponent>(
			std::make_shared<Shader>("models/cube.vert", "models/cube.frag"));

		// std::vector<glm::vec3> windows{
		// 	glm::vec3(-4.3f, 0.0f, 1.8f),
		// 	glm::vec3(-0.45f, 0.9f, 1.9f),
		// };
		//
		// Models::Window windowModel{"textures/window.png"};
		// auto windowShader = std::make_shared<Shader>("blend.vert", "blend.frag");
		//
		// for (auto& pos: windows) {
		// 	auto w = registry.createEntity();
		// 	w.addComponent<TransformComponent>(
		// 		pos,
		// 		glm::vec3(0.0f, 0.0f, 0.0f),
		// 		glm::vec3(2.0f));
		//
		// 	w.addComponent<MeshComponent>(windowModel.getMeshes(), true);
		// 	w.addComponent<MaterialComponent>(windowModel.getTextures(), 32.0f, true);
		// 	w.addComponent<ShaderComponent>(windowShader);
		// }

		// auto dirLight = registry.createEntity();
		// dirLight.addComponent<DirectionalLightComponent>(
		// 	glm::vec3(-0.2f, -1.0f, -0.3f),
		// 	glm::vec3(0.05f, 0.05f, 0.05f),
		// 	glm::vec3(0.4f, 0.4f, 0.4f),
		// 	glm::vec3(0.5f, 0.5f, 0.5f));

		auto pointLight = registry.createEntity();
		pointLight.addComponent<TransformComponent>(
			glm::vec3(-3.2f, 5.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.2f));

		pointLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		pointLight.addComponent<MaterialComponent>(glm::vec3(1.0f), 32.0f);

		pointLight.addComponent<ShaderComponent>(
			std::make_shared<Shader>("models/light.vert", "models/light.frag"));

		pointLight.addComponent<PointLightComponent>(
			glm::vec3(0.0f),
			glm::vec3(0.2f, 0.2f, 0.2f),  // ambient
			glm::vec3(0.5f, 0.5f, 0.5f),  // diffuse
			glm::vec3(0.3f, 0.3f, 0.3f),
			1.0f,
			0.09f,
			0.032f);


		// auto spotLight = registry.createEntity();
		// spotLight.addComponent<TransformComponent>(
		// 	glm::vec3(-3.2f, 5.0f, 0.0f),
		// 	glm::vec3(0.0f, 0.0f, 0.0f),
		// 	glm::vec3(0.2f));
		//
		// spotLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		// spotLight.addComponent<MaterialComponent>(glm::vec3(1.0f), 32.0f);
		//
		// spotLight.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("cube.vert", "cube.frag"));
		//
		// spotLight.addComponent<SpotLightComponent>(
		// 	glm::vec3(-3.2f, 5.0f, 0.0f),
		// 	glm::vec3(0.0f, -1.0f, 0.0f),
		// 	glm::cos(glm::radians(20.5f)),
		// 	glm::cos(glm::radians(25.0f)),
		// 	glm::vec3(0.1f, 0.1f, 0.1f),
		// 	glm::vec3(1.0f, 1.0f, 1.0f),
		// 	glm::vec3(1.0f, 1.0f, 1.0f),
		// 	 1.0f, 0.045f, 0.0075f);

		registry.update();

		xngn.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
