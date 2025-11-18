#pragma once
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>

#include "../../core/camera.h"
#include "glm/glm.hpp"

struct Texture;
class Entity;
class Window;
class Camera;
class guiSystem;
class Shader;

class ForwardRenderer {
public:
	explicit ForwardRenderer();

	~ForwardRenderer();

	void configure(const std::vector<Entity>& opaqueInstancedEntities,
	               const std::vector<Entity>& transparentInstancedEntities);

	void opaquePass(const std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches,
	                const std::array<uint32_t, 3>& shadowMaps) const;

	void transparentPass(const std::vector<Entity>& entities) const;

	void instancedPass(const std::vector<Entity>& entities, const std::array<uint32_t, 3>& shadowMaps) const;

	void transparentInstancedPass(const std::vector<Entity>& entities);

private:
	struct InstanceVBO {
		uint32_t buffer{};
		int offset{0};
	};
	InstanceVBO mOpaqueInstanceVBO, mTransparentInstanceVBO;

	void prepareInstanceData(const Entity& entity, const std::vector<glm::vec3>& positions,
	                         size_t instanceSize, uint32_t flags);

	void prepareInstanceBuffer(const std::vector<Entity>& entities, InstanceVBO& vbo);
};
