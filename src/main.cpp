#include "core/engine.h"
#include "core/camera.h"
#include "renderer/shader.h"
#include "model/model.h"
#include "io/filesystem.hpp"
#include "config/config.hpp"
#include "ECS/registry.h"
#include "ECS/components/model.hpp"
#include "ECS/components/shaderComponent.hpp"
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

	xngn.init(&registry);

	auto backpack = registry.createEntity();
	backpack.addComponent<TransformComponent>(glm::vec3(0.0f), glm::vec3(1.0f), 0.1f);
	backpack.addComponent<ModelComponent>(
		std::make_shared<Model>(
			fs::path(ASSET_DIR + "backpack/backpack.obj")));

	backpack.addComponent<ShaderComponent>(
		std::make_shared<Shader>(
			fs::path(SHADER_DIR + "model.vert"),
			fs::path(SHADER_DIR + "model_lit.frag")));
	// std::unique_ptr<Entity> light = std::make_unique<Light>(glm::vec3(3.2f, 1.0f, 2.0f),
	//                                                         fs::path(SHADER_DIR + "light.vert"),
	//                                                         fs::path(SHADER_DIR + "light.frag"));
	// light->scale = 0.2f;
	// scene->addEntity(light);

	registry.update();

	xngn.run();

	return 0;
}
