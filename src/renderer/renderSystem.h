#pragma once

#include <memory>
#include <vector>
#include "frameBuffer.h"
#include "uniformBuffer.h"
#include "../ECS/system.hpp"

class LightSystem;
class Window;
class Camera;
class guiSystem;
class Shader;

class RenderSystem final : public System {
public:
	explicit RenderSystem();

	[[nodiscard]] GLuint getSceneTexture() const { return mSceneBuffer->texture(); }
	[[nodiscard]] uint32_t getSceneWidth() const { return mSceneBuffer->width(); }
	[[nodiscard]] uint32_t getSceneHeight() const { return mSceneBuffer->height(); }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void configureUB(const Camera* camera) const;

	void render(const Camera* camera);

	void beginSceneRender() const;

	void endSceneRender() const;

private:
	using TransEntityBucket = std::vector<std::pair<float, Entity> >;

	bool collectTransparentEntities(const Entity& entity, const Camera* camera, TransEntityBucket& bucket);

	void opaquePass(const Entity& entity, const Camera* camera, const Shader& shader) const;

	void transparentPass(const Camera* camera, TransEntityBucket& bucket);

	void geometryPass(const Entity& entity, const Camera* camera, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	void lightingPass(const Shader& shader) const;

	void drawPass(const Entity& entity) const;

	LightSystem* mLightSystem{};
	std::unique_ptr<FrameBuffer> mSceneBuffer;
	std::unique_ptr<UniformBuffer> mCameraUBO;
	std::unique_ptr<UniformBuffer> mLightUBO;
};
