#pragma once

#include <memory>
#include <vector>
#include "../ECS/system.hpp"

class LightSystem;
class Window;
class Camera;
class guiSystem;
class Shader;

class RenderSystem final : public System {
public:
	explicit RenderSystem();

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

	LightSystem* mLightSystem{};
};
