#pragma once

#include <memory>
#include <vector>

#include "frameBuffer.h"
#include "../ECS/system.hpp"
#include "../models/quad.h"
#include "shader.h"

class FrameBuffer;
class LightSystem;
class Window;
class Camera;
class guiSystem;

class RenderSystem final : public System {
public:
	explicit RenderSystem();

	GLuint getSceneTexture() const { return mSceneBuffer->texture(); }
	uint32_t getSceneWidth() const { return mSceneBuffer->width(); }
	uint32_t getSceneHeight() const { return mSceneBuffer->height(); }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

	void render(const Camera* camera);

private:
	using TransEntityBucket = std::vector<std::pair<float, Entity> >;

	bool collectTransparentEntities(const Entity& entity, const Camera* camera, TransEntityBucket& bucket);

	void opaquePass(const Entity& entity, const Camera* camera, const Shader& shader) const;

	void transparentPass(const Camera* camera, TransEntityBucket& bucket);

	void geometryPass(const Entity& entity, const Camera* camera, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	void lightingPass(const Shader& shader) const;

	void drawPass(const Entity& entity) const;

	void beginSceneRender() const;

	void endSceneRender() const;

	LightSystem* mLightSystem{};
	std::unique_ptr<FrameBuffer> mSceneBuffer;
};
