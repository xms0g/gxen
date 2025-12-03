#pragma once
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "directionalShadowPass.h"
#include "omnidirectionalShadowPass.h"
#include "perspectiveShadowPass.h"
#include "../IRenderPass.hpp"
#include "../../../config/config.hpp"

struct RenderContext;
struct RenderGroup;
struct SpotLightComponent;
struct DirectionalLightComponent;
struct PointLightComponent;
class Shader;
class UniformBuffer;
class LightSystem;

class ShadowPass final: public IRenderPass {
public:
	~ShadowPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;

	[[nodiscard]] const std::array<uint32_t, 3>& getShadowMaps() const;

	[[nodiscard]] const UniformBuffer* getShadowUBO() const;

private:
	void directionalShadowPass(const RenderContext& context);

	void omnidirectionalShadowPass(const RenderContext& context);

	void perspectiveShadowPass(const RenderContext& context);

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
