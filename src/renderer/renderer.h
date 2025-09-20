#pragma once

#include <memory>

class Entity;
class Window;
class Gui;
class Scene;

class Renderer {
public:
    explicit Renderer(Scene* scene);

    [[nodiscard]] Window* window() const { return mWindow.get(); }

    [[nodiscard]] Gui* gui() const { return mGui.get(); }

    void render() const;

private:
    Scene* mScene;
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
};
