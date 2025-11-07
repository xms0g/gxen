#pragma once
#include <unordered_map>
#include <vector>

struct Texture;
class Shader;
class Entity;

namespace RenderCommon {
void setupTransform(const Entity& entity, const Shader& shader);

void drawMeshes(const Entity& entity, const Shader& shader);

using TextureMap = std::unordered_map<uint32_t, std::vector<Texture> >;

void bindTextures(
	uint32_t materialID,
	const TextureMap* texturesByMatID,
	const Shader& shader);

void unbindTextures(uint32_t materialID, const TextureMap* texturesByMatID);
}
