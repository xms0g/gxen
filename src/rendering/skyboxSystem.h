#pragma once
#include "../ECS/system.hpp"

class Camera;

class SkyboxSystem final : public System {
public:
	SkyboxSystem();

	void render(const Camera& camera) const;
};
