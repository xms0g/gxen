#include <iostream>
#include <ostream>
#include "core/engine.h"
#include "core/camera.h"
#include "rendering/shader.h"
#include "resourceManager/resourceManager.h"
#include "ECS/registry.h"
#include "ECS/components/debug.hpp"
#include "ECS/components/directionalLight.hpp"
#include "ECS/components/material.hpp"
#include "ECS/components/mesh.hpp"
#include "ECS/components/pointLight.hpp"
#include "ECS/components/shader.hpp"
#include "ECS/components/skybox.hpp"
#include "ECS/components/spotLight.hpp"
#include "ECS/components/transform.hpp"
#include "ECS/components/debug.hpp"
#include "ECS/components/instance.hpp"
#include "mesh/mesh.h"
#include "models/cube.h"
#include "models/cubemap.h"
#include "models/plane.h"
#include "models/window.h"
#include "rendering/renderFlags.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

static constexpr const char* faces[] = {
	"skybox/right.jpg",
	"skybox/left.jpg",
	"skybox/top.jpg",
	"skybox/bottom.jpg",
	"skybox/front.jpg",
	"skybox/back.jpg"
};

int main() {
	Registry registry;

	try {
		XEngine xngn;
		xngn.init(&registry);

		Models::Cubemap skyboxModel{faces};
		auto skybox = registry.createEntity("Skybox");
		skybox.addComponent<SkyboxComponent>();
		skybox.addComponent<MaterialComponent>(skyboxModel.getTextures());
		skybox.addComponent<MeshComponent>(skyboxModel.getMeshes());
		skybox.addComponent<ShaderComponent>(
			std::make_shared<Shader>("skybox.vert", "skybox.frag"));

		// Backpack
		auto backpack = registry.createEntity("Backpack");
		backpack.addComponent<TransformComponent>(
			glm::vec3(-3.2f, 0.8f, 0.0f),
			glm::vec3(0.0f),
			glm::vec3(1.0f));

		ResourceManager::instance().loadModel(backpack.id(), "backpack/backpack.obj");

		backpack.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(backpack.id()));
		backpack.addComponent<MaterialComponent>(ResourceManager::instance().getTextures(backpack.id()), 32.0f);

		backpack.addComponent<ShaderComponent>(
			std::make_shared<Shader>("object.vert", "object.frag"));

		backpack.addComponent<DebugComponent>(DebugMode::None);


		// Suzanne
		auto suzanne = registry.createEntity("Suzanne");
		suzanne.addComponent<TransformComponent>(
			glm::vec3(3.2f, 0.8f, 0.0f),
			glm::vec3(0.0f),
			glm::vec3(1.0f));

		ResourceManager::instance().loadModel(suzanne.id(), "suzanne/suzanne.obj");

		suzanne.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(suzanne.id()));
		suzanne.addComponent<MaterialComponent>(ResourceManager::instance().getTextures(suzanne.id()), 32.0f);

		suzanne.addComponent<ShaderComponent>(
			std::make_shared<Shader>("object.vert", "object.frag"));

		suzanne.addComponent<DebugComponent>(DebugMode::None);
		//
		// Plane
		Models::Plane planeModel{"textures/metal.png", "textures/metal_specular.png"};
		auto plane = registry.createEntity("Plane");
		plane.addComponent<TransformComponent>(
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(2.0f));

		plane.addComponent<MeshComponent>(planeModel.getMeshes());
		plane.addComponent<MaterialComponent>(planeModel.getTextures(), 32.0f, TwoSided);

		plane.addComponent<ShaderComponent>(
			std::make_shared<Shader>("models/plane.vert", "models/plane.frag"));

		// CUbe
		Models::Cube cubeModel{"textures/wall.jpg"};
		auto cube = registry.createEntity("Cube");
		cube.addComponent<TransformComponent>(
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(2.0f));

		cube.addComponent<MeshComponent>(cubeModel.getMeshes());
		cube.addComponent<MaterialComponent>(cubeModel.getTextures(), 32.0f);

		cube.addComponent<ShaderComponent>(
			std::make_shared<Shader>("models/cube.vert", "models/cube.frag"));

		cube.addComponent<DebugComponent>(DebugMode::None);

		// Blend
		std::vector<glm::vec3> windows{
			glm::vec3(-4.3f, 0.0f, 1.8f),
			glm::vec3(-0.45f, 0.9f, 1.9f),
		};

		Models::Window windowModel{"textures/window.png"};

		auto window = registry.createEntity("Window");
		window.addComponent<TransformComponent>(
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(2.0f));

		window.addComponent<MeshComponent>(windowModel.getMeshes());
		window.addComponent<MaterialComponent>(windowModel.getTextures(), 32.0f, TwoSided | Transparent | Instanced);
		window.addComponent<ShaderComponent>(
			std::make_shared<Shader>("instanced.vert", "models/blend.frag"));

		window.addComponent<InstanceComponent>(&windows, windows.size());


		auto dirLight = registry.createEntity("Directional Light");
		dirLight.addComponent<DirectionalLightComponent>(
			glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 0.0f),
			glm::vec4(0.4f, 0.4f, 0.4f, 0.0f),
			glm::vec4(0.5f, 0.5f, 0.5f, 0.0f));

		// auto pointLight = registry.createEntity("Point Light");
		// pointLight.addComponent<TransformComponent>(
		// 	glm::vec3(-3.2f, 5.0f, 0.0f),
		// 	glm::vec3(0.0f, 0.0f, 0.0f),
		// 	glm::vec3(0.2f));
		//
		// pointLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		// pointLight.addComponent<MaterialComponent>(glm::vec4(1.0f), 32.0f);
		//
		// pointLight.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("models/light.vert", "models/light.frag"));
		//
		// pointLight.addComponent<PointLightComponent>(
		// 	glm::vec4(0.0f),
		// 	glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),  // ambient
		// 	glm::vec4(0.5f, 0.5f, 0.5f,0.0f),  // diffuse
		// 	glm::vec4(0.5f, 0.5f, 0.5f,0.0f),
		// 	glm::vec4(1.0f, 0.09f, 0.032f,0.0f)
		// 	);


		// auto spotLight = registry.createEntity();
		// spotLight.addComponent<TransformComponent>(
		// 	glm::vec4(-3.2f, 5.0f, 0.0f),
		// 	glm::vec4(0.0f, 0.0f, 0.0f),
		// 	glm::vec4(0.2f));
		//
		// spotLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		// spotLight.addComponent<MaterialComponent>(glm::vec4(1.0f), 32.0f);
		//
		// spotLight.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("cube.vert", "cube.frag"));
		//
		// spotLight.addComponent<SpotLightComponent>(
		// 	glm::vec4(-3.2f, 5.0f, 0.0f),
		// 	glm::vec4(0.0f, -1.0f, 0.0f),
		// 	glm::cos(glm::radians(20.5f)),
		// 	glm::cos(glm::radians(25.0f)),
		// 	glm::vec4(0.1f, 0.1f, 0.1f),
		// 	glm::vec4(1.0f, 1.0f, 1.0f),
		// 	glm::vec4(1.0f, 1.0f, 1.0f),
		// 	 1.0f, 0.045f, 0.0075f);

		registry.update();

		xngn.configure();

		xngn.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
