#include "core/engine.h"
#include "core/camera.h"
#include "renderer/shader.h"
#include "resourceManager/resourceManager.h"
#include "io/filesystem.hpp"
#include "config/config.hpp"
#include "ECS/registry.h"
#include "ECS/components/directionalLight.hpp"
#include "ECS/components/material.hpp"
#include "ECS/components/mesh.hpp"
#include "ECS/components/pointLight.hpp"
#include "ECS/components/shader.hpp"
#include "ECS/components/spotLight.hpp"
#include "ECS/components/transform.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

int main() {
	XEngine xngn;
	Registry registry;
	ResourceManager& rm = ResourceManager::instance();

	xngn.init(&registry);

	auto backpack = registry.createEntity();
	backpack.addComponent<TransformComponent>(
		glm::vec3(0.0f),
		glm::vec3(0.0f, 45.0f, 0.0f),
		glm::vec3(1.0f));

	rm.loadModel(backpack.id(), fs::path(ASSET_DIR + "backpack/backpack.obj"));

	backpack.addComponent<MeshComponent>(rm.getMeshes(backpack.id()));

	backpack.addComponent<MaterialComponent>(rm.getTextures(backpack.id()), 32.0f);

	backpack.addComponent<ShaderComponent>(
		std::make_shared<Shader>(
			fs::path(SHADER_DIR + "model.vert"),
			fs::path(SHADER_DIR + "model.frag")));

	auto dirLight = registry.createEntity();
	dirLight.addComponent<DirectionalLightComponent>(
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.5f, 0.5f, 0.5f));

	auto pointLight = registry.createEntity();
	pointLight.addComponent<PointLightComponent>(
		glm::vec3(3.2f, 1.0f, 2.0f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f,
		0.09f,
		0.032f);

	registry.update();

	xngn.run();

	return 0;
}
