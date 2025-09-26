#pragma once

#include <SDL.h>

#include "glm/vec3.hpp"
#include "../ECS/system.hpp"

struct TransformComponent;

class GuiSystem final : public System {
public:
	GuiSystem(SDL_Window* window, SDL_GLContext gl_context);

	~GuiSystem() override;

	void update(float dt);

	void render();

private:
	void updateFpsCounter(float dt);

	void updateTransform(TransformComponent& tc) const;

	void renderGraphicsInfo() const;

	void renderTransform();

	struct {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	} transform;

	// Frame
	double mPreviousSeconds{0.0};
	double mCurrentSeconds{0.0};
	uint32_t mFPS{0};
	uint32_t mCurrentFrameCount{0};
};
