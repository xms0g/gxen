#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
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

	using TransEntityBucket = std::vector<std::pair<float, Entity> >;
	void transparentPass(TransEntityBucket& entities) const;

	void instancedPass(const std::vector<Entity>& entities, const std::array<uint32_t, 3>& shadowMaps) const;

	void transparentInstancedPass(const std::vector<Entity>& entities);

	void geometryPass(const Entity& entity, const Shader& shader) const;

	void drawPass(const Entity& entity, const Shader& shader) const;

private:
	void opaquePass(const Entity& entity, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture> >;
	void bindTextures(
		uint32_t materialID,
		const TextureMap* texturesByMatID,
		const Shader& shader) const;

	void unbindTextures(
		uint32_t materialID,
		const TextureMap* texturesByMatID) const;

	void prepareInstanceData(const Entity& entity,
	                         const std::vector<glm::vec3>& positions,
	                         size_t instanceSize,
	                         uint32_t flags);
	struct InstanceVBO {
		uint32_t buffer{};
		int offset{0};
	};
	InstanceVBO mOpaqueInstanceVBO, mTransparentInstanceVBO;

	void prepareInstanceBuffer(const std::vector<Entity>& entities, InstanceVBO& vbo);
};
