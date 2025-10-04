#pragma once

#include "../../ECS/system.hpp"

class Camera;
class Shader;

class RenderSystem: public System {
public:
	RenderSystem();

protected:
	void opaquePass(const Entity& entity, const Shader& shader) const;

	void geometryPass(const Entity& entity, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	void drawPass(const Entity& entity) const;
};
