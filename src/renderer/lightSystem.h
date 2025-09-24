#pragma once

#include "../ECS/system.hpp"

struct PointLightComponent;
struct DirectionalLightComponent;
struct SpotLightComponent;

class LightSystem final : public System {
public:
	LightSystem();

	std::vector<PointLightComponent*>& getPointLights() { return pointLights; }
	std::vector<DirectionalLightComponent*>& getDirLights() { return dirLights; }
	std::vector<SpotLightComponent*>& getSpotLights() { return spotLights; }

	void update();

private:
	std::vector<PointLightComponent*> pointLights;
	std::vector<DirectionalLightComponent*> dirLights;
	std::vector<SpotLightComponent*> spotLights;
};
