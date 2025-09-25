#pragma once

#include <memory>
#include "../ECS/system.hpp"

class LightSystem;
class Window;
class Camera;
class Gui;
class Shader;

class RenderSystem final : public System {
public:
    explicit RenderSystem();

    [[nodiscard]] Window* window() const { return mWindow.get(); }

    [[nodiscard]] Gui* gui() const { return mGui.get(); }

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

    void render(const Camera* camera) const;

private:
	void geometryPass(const Entity& entity, const Camera* camera, const std::shared_ptr<Shader>& shader) const;
	void materialPass(const Entity& entity, const std::shared_ptr<Shader>& shader) const;
	void lightingPass(const std::shared_ptr<Shader>& shader) const;
	void drawPass(const Entity& entity, const std::shared_ptr<Shader>& shader) const;

	LightSystem* mLightSystem{};
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
};
