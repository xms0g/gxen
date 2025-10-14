#pragma once
#include <vector>
#include <memory>
#include "shadowData.hpp"
#include "../../ECS/system.hpp"

class PerspectiveShadowPass;
class OmnidirectionalShadowPass;
class DirectionalShadowPass;
class LightSystem;
class Entity;

class ShadowSystem final : public System {
public:
	ShadowSystem();

	~ShadowSystem() override;

	ShadowData& getShadowData();

	void shadowPass(const LightSystem& lights);

private:
	ShadowData mShadowData{};
	std::vector<Entity> mEntities;
	std::unique_ptr<DirectionalShadowPass> mDirShadowPass;
	std::unique_ptr<OmnidirectionalShadowPass> mOmnidirShadowPass;
	std::unique_ptr<PerspectiveShadowPass> mPerspectiveShadowPass;
};
