#pragma once

#include <SDL.h>
#include "../ECS/system.hpp"

struct PostEffect;

class GuiSystem final : public System {
public:
	GuiSystem(SDL_Window* window, SDL_GLContext gl_context);

	~GuiSystem() override;

	void update(float dt);

	void render(std::vector<PostEffect>& effects);

private:
	void updateFpsCounter(float dt);

	void renderGraphicsInfo() const;

	void renderTransform() const;

	void renderPostProcess(std::vector<PostEffect>& effects);

	// Frame
	double mPreviousSeconds{0.0};
	double mCurrentSeconds{0.0};
	uint32_t mFPS{0};
	uint32_t mCurrentFrameCount{0};
};
