#pragma once
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "../../ECS/system.hpp"

class UniformBuffer;
class PerspectiveShadowPass;
class OmnidirectionalShadowPass;
class DirectionalShadowPass;
class LightSystem;
class Entity;

class ShadowSystem final : public System {
public:
	ShadowSystem();

	~ShadowSystem() override;

	std::array<uint32_t, 3>& getShadowMaps() { return mShadowMaps; }

	void configure() const;

	void shadowPass(const LightSystem& lights);

private:
	struct alignas(16) ShadowData {
		glm::mat4 lightSpaceMatrix;
		glm::mat4 persLightSpaceMatrix;
		glm::vec4 omniFarPlanes;
	};
	ShadowData mShadowData{};

	std::array<uint32_t, 3> mShadowMaps{};
	std::vector<Entity> mEntities;
	std::unique_ptr<DirectionalShadowPass> mDirShadowPass;
	std::unique_ptr<OmnidirectionalShadowPass> mOmnidirShadowPass;
	std::unique_ptr<PerspectiveShadowPass> mPerspectiveShadowPass;
	std::unique_ptr<UniformBuffer> mShadowUBO;
};
