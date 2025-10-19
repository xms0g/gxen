#pragma once
#include <unordered_map>
#include "../../ECS/system.hpp"

struct Texture;
class Camera;
class Shader;

class RenderSystem : public System {
public:
	RenderSystem();

protected:
	void opaquePass(const Entity& entity, const Shader& shader) const;

	void geometryPass(const Entity& entity, const Shader& shader) const;

	void materialPass(const Entity& entity, const Shader& shader) const;

	void drawPass(const Entity& entity, const Shader& shader) const;

	using TextureMap = std::unordered_map<uint32_t, std::vector<Texture>>;
	void bindTextures(
		uint32_t materialID,
		const TextureMap* texturesByMatID,
		const Shader& shader) const;

	void unbindTextures(
		uint32_t materialID,
		const TextureMap* texturesByMatID) const;
};
