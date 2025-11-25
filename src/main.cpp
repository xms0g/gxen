#include <iostream>
#include <ostream>
#include "core/engine.h"
#include "core/camera.h"
#include "rendering/shader.h"
#include "resourceManager/resourceManager.h"
#include "ECS/registry.h"
#include "ECS/components/bv.hpp"
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
#include "math/boundingVolume.h"
#include "rendering/mesh/mesh.h"
#include "rendering/models/cube.h"
#include "rendering/models/cubemap.h"
#include "rendering/models/plane.h"
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
		Engine engine;
		engine.init(&registry);

		Models::Cubemap skyboxModel{faces};
		auto skybox = registry.createEntity("Skybox");
		skybox.addComponent<SkyboxComponent>();
		skybox.addComponent<MaterialComponent>(skyboxModel.getMaterial(), 32.0f, 1.0f, RenderFlags::Forward);
		skybox.addComponent<MeshComponent>(skyboxModel.getMeshes());
		skybox.addComponent<ShaderComponent>(
			std::make_shared<Shader>("skybox.vert", "skybox.frag"));

		auto instancedObject = std::make_shared<Shader>("instanced.vert", "object.frag");
		auto object = std::make_shared<Shader>("object.vert", "object.frag");

		//
		// std::vector<glm::vec3> pos1;
		// float x = 5.0f, z = 0.0f;
		// for (int i = 0; i < 10; i++) {
		// 	pos1.push_back(glm::vec3(x, 0.0, z));
		// 	x += 5.0f;
		// 	if (x == 20) {
		// 		x = 0.0f;
		// 		z += 5;
		// 	}
		// }

		// Suzanne
		auto suzanne = registry.createEntity("Suzanne");
		suzanne.addComponent<TransformComponent>(
			glm::vec3(3.2f, 1.1f, 0.0f),
			glm::vec3(1.0f, 45.0f, 23.0f),
			glm::vec3(1.0f));

		ResourceManager::instance().loadModel(suzanne.id(), "Suzanne/glTF/Suzanne.gltf");

		suzanne.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(suzanne.id()));
		suzanne.addComponent<MaterialComponent>(ResourceManager::instance().getMaterial(suzanne.id()), 32.0f, 1.0, RenderFlags::Deferred);

		suzanne.addComponent<ShaderComponent>(object);

		suzanne.addComponent<BoundingVolumeComponent>(
			std::make_shared<math::AABB>(
				math::generateAABB(*ResourceManager::instance().getMeshes(suzanne.id()))));

		suzanne.addComponent<DebugComponent>();

		//suzanne.addComponent<InstanceComponent>(&pos1);

		//Plane
		// Models::Plane planeModel{"textures/wood.png", "textures/wood_specular.png"};
		// auto plane = registry.createEntity("Plane");
		// plane.addComponent<TransformComponent>(
		// 	glm::vec3(0.0f),
		// 	glm::vec3(0.0f),
		// 	glm::vec3(2.0f));
		//
		// plane.addComponent<MeshComponent>(planeModel.getMeshes());
		// plane.addComponent<MaterialComponent>(planeModel.getMaterial(), 32.0f, 1.0f, Deferred | TwoSided);
		//
		// plane.addComponent<ShaderComponent>(object);
		//
		// plane.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*planeModel.getMeshes())));
		//
		// plane.addComponent<DebugComponent>();


		// Cube
		Models::Cube cubeModel{
			"textures/brickwall.jpg",
			"textures/brickwall_specular.jpg",
			"textures/brickwall_normal.jpg"
		};
		// auto cube = registry.createEntity("Cube");
		// cube.addComponent<TransformComponent>(
		// 	glm::vec3(0.0f, 3.6f, 0.0f),
		// 	glm::vec3(0.0),
		// 	glm::vec3(2.0f));
		//
		// cube.addComponent<MeshComponent>(cubeModel.getMeshes());
		// cube.addComponent<MaterialComponent>(cubeModel.getMaterial(), 32.0f, 1.0f, CastShadow);
		//
		// cube.addComponent<ShaderComponent>(object);
		//
		// cube.addComponent<DebugComponent>();
		//
		// cube.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*cubeModel.getMeshes())));


		// Blend
		// std::vector<glm::vec3> windows{
		// 	glm::vec3(-4.3f, 2.0f, 1.8f),
		// 	glm::vec3(-0.45f, 1.9f, 1.9f),
		// 	glm::vec3(0.45f, 3.9f, 1.9f),
		// };
		//
		// Models::Window windowModel{"textures/window.png"};
		//
		// auto window = registry.createEntity("Window");
		// window.addComponent<TransformComponent>(
		// 	glm::vec3(0.0f),
		// 	glm::vec3(0.0f),
		// 	glm::vec3(2.0f));
		//
		// window.addComponent<MeshComponent>(windowModel.getMeshes());
		// window.addComponent<MaterialComponent>(windowModel.getTextures(), 32.0f, 1.0f,
		//                                        TwoSided | Instanced | Transparent);
		// window.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("instanced.vert", "models/blend.frag"));
		// window.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*windowModel.getMeshes())));
		//
		// window.addComponent<InstanceComponent>(&windows);
		//
		// std::vector<glm::vec3> grassess{
		// 	glm::vec3(-4.3f, 0.0f, 1.8f),
		// 	glm::vec3(-0.45f, 0.0f, 1.9f),
		// 	glm::vec3(0.45f, 0.0f, 1.9f),
		// };
		//
		// Models::Window grassModel{"textures/grass.png"};
		//
		// auto grass = registry.createEntity("Grass");
		// grass.addComponent<TransformComponent>(
		// 	glm::vec3(0.0f),
		// 	glm::vec3(0.0f),
		// 	glm::vec3(2.0f));
		//
		// grass.addComponent<MeshComponent>(grassModel.getMeshes());
		// grass.addComponent<MaterialComponent>(grassModel.getTextures(), 32.0f, 1.0f,
		// 									   TwoSided | Instanced | Transparent);
		// grass.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("instanced.vert", "models/blend.frag"));
		// grass.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*windowModel.getMeshes())));
		//
		// grass.addComponent<InstanceComponent>(&grassess);

		// auto angel = registry.createEntity("Anubis");
		// angel.addComponent<TransformComponent>(
		// 	glm::vec3(-1.2f, 3.3, 0.0f),
		// 	glm::vec3(0.0f),
		// 	glm::vec3(0.02f));
		//
		// ResourceManager::instance().loadModel(angel.id(), "anubis/Anubis.obj");
		//
		// angel.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(angel.id()));
		// angel.addComponent<MaterialComponent>(ResourceManager::instance().getTextures(angel.id()), 32.0f);
		//
		// angel.addComponent<ShaderComponent>(object);
		//
		// angel.addComponent<DebugComponent>(DebugMode::None);

		auto sponza = registry.createEntity("Sponza");
		sponza.addComponent<TransformComponent>(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f),
			glm::vec3(0.02f));

		ResourceManager::instance().loadModel(sponza.id(), "Sponza/glTF/Sponza.gltf");

		sponza.addComponent<MeshComponent>(ResourceManager::instance().getMeshes(sponza.id()));
		sponza.addComponent<MaterialComponent>(ResourceManager::instance().getMaterial(sponza.id()), 32.0f, 1.0f, RenderFlags::Deferred);

		sponza.addComponent<ShaderComponent>(object);

		sponza.addComponent<DebugComponent>();

		sponza.addComponent<BoundingVolumeComponent>(
			std::make_shared<math::AABB>(
				math::generateAABB(*ResourceManager::instance().getMeshes(sponza.id()))));


		auto dirLight = registry.createEntity("Directional Light");
		dirLight.addComponent<DirectionalLightComponent>(
			glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f),
			glm::vec4(0.01f, 0.01f, 0.01f, 0.0f),
			glm::vec4(0.4f, 0.4f, 0.4f, 0.0f),
			glm::vec4(0.5f, 0.5f, 0.5f, 0.0f));

		// auto pointLight = registry.createEntity("Point Light");
		// pointLight.addComponent<TransformComponent>(
		// 	glm::vec3(-3.2f, 5.0f, -2.4f),
		// 	glm::vec3(0.0f, 0.0f, 0.0f),
		// 	glm::vec3(0.2f));
		//
		// pointLight.addComponent<PointLightComponent>(
		// 	glm::vec4(0.0f),
		// 	glm::vec4(0.03f, 0.03f, 0.03f, 0.0f), // ambient
		// 	glm::vec4(2.3f, 2.2f, 2.5f, 0.0f), // diffuse
		// 	glm::vec4(1.3f, 1.2f, 1.5f, 0.0f), // specular
		// 	glm::vec3(1.0f, 0.14f, 0.07f), // attenuation
		// 	true
		// );
		//
		// pointLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		// pointLight.addComponent<MaterialComponent>(glm::vec4(4.0f), 32.0f, 1.0f, Forward);
		//
		// pointLight.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("models/light.vert", "models/light.frag"));
		//
		// pointLight.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*cubeModel.getMeshes())));

		// auto pointLight1 = registry.createEntity("Point Light1");
		// pointLight1.addComponent<TransformComponent>(
		// 	glm::vec3(3.2f, 5.0f, -2.4f),
		// 	glm::vec3(0.0f, 0.0f, 0.0f),
		// 	glm::vec3(0.2f));
		//
		// pointLight1.addComponent<PointLightComponent>(
		// 	glm::vec4(0.0f),
		// 	glm::vec4(0.01f, 0.01f, 0.01f, 0.0f), // ambient
		// 	glm::vec4(0.3f, 0.2f, 0.5f, 0.0f), // diffuse
		// 	glm::vec4(0.3f, 0.2f, 0.5f, 0.0f), // specular
		// 	glm::vec3(1.0f, 0.14f, 0.07f), // attenuation
		// 	false
		// );
		//
		// pointLight1.addComponent<MeshComponent>(cubeModel.getMeshes());
		// pointLight1.addComponent<MaterialComponent>(glm::vec4(1.0f), 32.0f);
		//
		// pointLight1.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("models/light.vert", "models/light.frag"));
		//
		// pointLight1.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*cubeModel.getMeshes())));

		// auto spotLight = registry.createEntity("Spot Light");
		// spotLight.addComponent<TransformComponent>(
		// 	glm::vec3(-3.2f, 5.0f, 0.0f),
		// 	glm::vec3(0.0f, 0.0f, 0.0f),
		// 	glm::vec3(0.2f));
		//
		// spotLight.addComponent<SpotLightComponent>(
		// 	glm::vec4(-3.2f, 5.0f, 0.0f, 0.0f),
		// 	glm::vec4(1.0f, -1.0f, 0.0f, 0.0f),
		// 	glm::vec4(0.1f, 0.1f, 0.1f, 0.0f),
		// 	glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
		// 	glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
		// 	glm::vec3(1.0f, 0.045f, 0.0075f),
		// 	true,
		// 	glm::vec4(glm::cos(glm::radians(20.5f)), glm::cos(glm::radians(25.0f)), 0.0f, 0.0f));
		//

		// spotLight.addComponent<MeshComponent>(cubeModel.getMeshes());
		// spotLight.addComponent<MaterialComponent>(glm::vec4(1.0f), 32.0f, 1.0f, Forward);
		//
		// spotLight.addComponent<ShaderComponent>(
		// 	std::make_shared<Shader>("models/light.vert", "models/light.frag"));
		//
		//
		// spotLight.addComponent<BoundingVolumeComponent>(
		// 	std::make_shared<math::AABB>(
		// 		math::generateAABB(*cubeModel.getMeshes())));

		registry.update();

		engine.configure();

		engine.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
