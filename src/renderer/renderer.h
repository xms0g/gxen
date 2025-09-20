#pragma once

#include <memory>

class Entity;
class Window;
class Gui;
class Scene;

class Renderer {
public:
    Renderer(Scene* scene);

    Window* window() const { return mWindow.get(); }

    Gui* gui() const { return mGui.get(); }

    void render() const;

private:
    Scene* mScene;
    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Gui> mGui;
};
