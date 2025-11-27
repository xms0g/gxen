#pragma once
#include <vector>
#include <memory>
#include <SDL_stdinc.h>
#include <unordered_map>
#include "glm/glm.hpp"
#include "../../config/config.hpp"

struct RenderGroup;
struct SpotLightComponent;
struct DirectionalLightComponent;
struct PointLightComponent;
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

	[[nodiscard]] const std::array<uint32_t, 3>& getShadowMaps() const;

	[[nodiscard]] const UniformBuffer& getShadowUBO() const;

	void shadowPass(const std::vector<RenderGroup>& shadowCasters, const LightSystem& lights);

private:
	void directionalShadowPass(const std::vector<RenderGroup>& shadowCasters, const std::vector<DirectionalLightComponent*>& lights);

	void omnidirectionalShadowPass(const std::vector<RenderGroup>& shadowCasters, const std::vector<PointLightComponent*>& lights);

	void perspectiveShadowPass(const std::vector<RenderGroup>& shadowCasters, const std::vector<SpotLightComponent*>& lights);

	struct alignas(16) ShadowData {
		glm::mat4 lightSpaceMatrix;
		glm::mat4 persLightSpaceMatrix[MAX_SPOT_LIGHTS];
		glm::vec4 omniFarPlanes;
	};

	ShadowData mShadowData{};

	std::array<uint32_t, 3> mShadowMaps{};
	std::unique_ptr<DirectionalShadowPass> mDirShadowPass;
	std::unique_ptr<OmnidirectionalShadowPass> mOmnidirShadowPass;
	std::unique_ptr<PerspectiveShadowPass> mPerspectiveShadowPass;
	std::unique_ptr<UniformBuffer> mShadowUBO;
};
