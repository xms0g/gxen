#pragma once
#include "guiBackend.h"
#include "../../ECS/system.hpp"

struct IPostEffect;

class GuiSystem final : public System {
public:
	GuiSystem(SDL_Window* window, SDL_GLContext gl_context);

	~GuiSystem() override;

	void update(float dt);

	void render(std::vector<std::shared_ptr<IPostEffect>>& effects) const;

private:
	void updateFpsCounter(float dt);

	// Frame
	double mPreviousSeconds{0.0};
	double mCurrentSeconds{0.0};
	uint32_t mFPS{0};
	uint32_t mCurrentFrameCount{0};
};
