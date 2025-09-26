#pragma once

#include <memory>
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

    void render(const Camera* camera) const;

private:
	void geometryPass(const Entity& entity, const Camera* camera, const std::shared_ptr<Shader>& shader) const;
	void materialPass(const Entity& entity, const std::shared_ptr<Shader>& shader) const;
	void lightingPass(const std::shared_ptr<Shader>& shader) const;
	void drawPass(const Entity& entity) const;

	LightSystem* mLightSystem{};
};
