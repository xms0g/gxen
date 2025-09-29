#include "lightSystem.h"
#include "../ECS/registry.h"
#include "../ECS/components/directionalLight.hpp"
#include "../ECS/components/pointLight.hpp"
#include "../ECS/components/spotLight.hpp"
#include "../ECS/components/transform.hpp"

LightSystem::LightSystem() {
	RequireComponent<DirectionalLightComponent>(true);
	RequireComponent<PointLightComponent>(true);
	RequireComponent<SpotLightComponent>(true);
}

void LightSystem::update() {
	dirLights.clear();
	pointLights.clear();
	spotLights.clear();

	for (auto& entity: getSystemEntities()) {
		TransformComponent tc;

		if (entity.hasComponent<TransformComponent>()) {
			tc = entity.getComponent<TransformComponent>();
		}

		if (entity.hasComponent<DirectionalLightComponent>()) {
			auto& light = entity.getComponent<DirectionalLightComponent>();
			dirLights.push_back(&light);
		} else if (entity.hasComponent<PointLightComponent>()) {
			auto& light = entity.getComponent<PointLightComponent>();
			light.position = tc.position;
			pointLights.push_back(&light);
		} else if (entity.hasComponent<SpotLightComponent>()) {
			auto& light = entity.getComponent<SpotLightComponent>();
			light.position = tc.position;
			spotLights.push_back(&light);
		}
	}
}

