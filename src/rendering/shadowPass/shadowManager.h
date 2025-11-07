#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "glm/glm.hpp"

class ForwardRenderer;
class Shader;
class UniformBuffer;
class PerspectiveShadowPass;
class OmnidirectionalShadowPass;
class DirectionalShadowPass;
class LightSystem;
class Entity;

class ShadowManager {
public:
	explicit ShadowManager();

	~ShadowManager();

	std::array<uint32_t, 3>& getShadowMaps() { return mShadowMaps; }

	void configure(const std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches) const;

	void shadowPass(std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches, const LightSystem& lights);

private:
	struct alignas(16) ShadowData {
		glm::mat4 lightSpaceMatrix;
		glm::mat4 persLightSpaceMatrix;
		glm::vec4 omniFarPlanes;
	};

	ShadowData mShadowData{};

	std::array<uint32_t, 3> mShadowMaps{};
	std::unique_ptr<DirectionalShadowPass> mDirShadowPass;
	std::unique_ptr<OmnidirectionalShadowPass> mOmnidirShadowPass;
	std::unique_ptr<PerspectiveShadowPass> mPerspectiveShadowPass;
	std::unique_ptr<UniformBuffer> mShadowUBO;
};
