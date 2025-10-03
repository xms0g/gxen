#pragma once

#include "../../ECS/system.hpp"

class Camera;
class Shader;

class RenderSystem: public System {
public:
	RenderSystem();

protected:
	using TransEntityBucket = std::vector<std::pair<float, Entity> >;

	bool collectTransparentEntities(const Entity& entity, const Camera& camera, TransEntityBucket& bucket);

	void transparentPass(TransEntityBucket& bucket) const;

	void opaquePass(const Entity& entity, const Shader& shader) const;

	void geometryPass(const Entity& entity, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	void drawPass(const Entity& entity) const;
};
