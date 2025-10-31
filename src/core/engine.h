#pragma once
#include <memory>

class RenderPipeline;
class GuiSystem;
class Window;
class Registry;
class Input;
class Camera;

class Engine {
public:
	Engine();

	~Engine();

	void init(Registry* registry);

	void configure() const;

	void run();

private:
	float mDeltaTime{};
	uint32_t mMillisecsPreviousFrame{0};
	// Engine
	bool isRunning{true};

	Registry* mRegistry{};
	GuiSystem* mGuiSystem{};
	std::unique_ptr<RenderPipeline> mRenderPipeline;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;
	std::unique_ptr<Input> mInput;
};
