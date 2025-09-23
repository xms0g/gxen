#pragma once

#include <memory>
#include "../ECS/system.hpp"

class Window;
class Camera;
class Gui;

class Renderer final : public System {
public:
    explicit Renderer();

    [[nodiscard]] Window* window() const { return mWindow.get(); }

    [[nodiscard]] Gui* gui() const { return mGui.get(); }

    void render(const Camera* camera) const;

private:
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
};
