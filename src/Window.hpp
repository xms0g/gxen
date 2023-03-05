#pragma once

class IWindow {
public:
    virtual ~IWindow() = default;
    virtual void updateFpsCounter(float dt) = 0;
    virtual void clear() = 0;
    virtual void swapBuffer() = 0;
};
