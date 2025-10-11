#pragma once
#include <vector>
#include <memory>
#include "shadowData.hpp"

class OmnidirectionalShadowPass;
class DirectionalShadowPass;
class LightSystem;
class Entity;

class ShadowManager {
public:
	ShadowManager();

	~ShadowManager();

	ShadowData& getShadowData();

	void render(const std::vector<Entity>& entities, const LightSystem& lights);

private:
	ShadowData mShadowData{};
	std::unique_ptr<DirectionalShadowPass> mDirShadowPass;
	std::unique_ptr<OmnidirectionalShadowPass> mOmnidirShadowPass;
};
