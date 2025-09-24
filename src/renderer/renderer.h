#pragma once

#include <memory>
#include "../ECS/system.hpp"

class LightSystem;
class Window;
class Camera;
class Gui;

class Renderer final : public System {
public:
    explicit Renderer();

	void setLightSystem(LightSystem* lightSystem) { mLightSystem = lightSystem; }

    [[nodiscard]] Window* window() const { return mWindow.get(); }

    [[nodiscard]] Gui* gui() const { return mGui.get(); }

    void update(const Camera* camera) const;

private:
	LightSystem* mLightSystem{};
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
};
