#pragma once
#include "../ECS/system.hpp"

struct PointLightComponent;
struct DirectionalLightComponent;
struct SpotLightComponent;

class LightSystem final : public System {
public:
	LightSystem();

	[[nodiscard]] const std::vector<PointLightComponent*>& getPointLights() const  { return pointLights; }
	[[nodiscard]] const std::vector<DirectionalLightComponent*>& getDirLights() const { return dirLights; }
	[[nodiscard]] const std::vector<SpotLightComponent*>& getSpotLights() const { return spotLights; }

	void update();

private:
	std::vector<DirectionalLightComponent*> dirLights;
	std::vector<PointLightComponent*> pointLights;
	std::vector<SpotLightComponent*> spotLights;
};
