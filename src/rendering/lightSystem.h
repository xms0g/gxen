#pragma once
#include "../ECS/system.hpp"

class UniformBuffer;
struct PointLightComponent;
struct DirectionalLightComponent;
struct SpotLightComponent;

class LightSystem final : public System {
public:
	LightSystem();

	[[nodiscard]] const UniformBuffer& getLightUBO() const { return *mLightUBO; }

	[[nodiscard]] const std::vector<PointLightComponent*>& getPointLights() const  { return pointLights; }
	[[nodiscard]] const std::vector<DirectionalLightComponent*>& getDirLights() const { return dirLights; }
	[[nodiscard]] const std::vector<SpotLightComponent*>& getSpotLights() const { return spotLights; }

	void update();

private:
	void updateLightUBO() const;

	std::unique_ptr<UniformBuffer> mLightUBO;
	std::vector<DirectionalLightComponent*> dirLights;
	std::vector<PointLightComponent*> pointLights;
	std::vector<SpotLightComponent*> spotLights;
};
