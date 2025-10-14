#pragma once
#include "../ECS/system.hpp"

struct PointLightComponent;
struct DirectionalLightComponent;
struct SpotLightComponent;

class LightSystem final : public System {
public:
	LightSystem();

	[[nodiscard]] const std::vector<PointLightComponent*>& getPointLights() const  { return pointLights; }
	[[nodiscard]] const DirectionalLightComponent* getDirLight() const { return dirLight; }
	[[nodiscard]] const std::vector<SpotLightComponent*>& getSpotLights() const { return spotLights; }

	void update();

private:
	std::vector<PointLightComponent*> pointLights;
	DirectionalLightComponent* dirLight;
	std::vector<SpotLightComponent*> spotLights;
};
