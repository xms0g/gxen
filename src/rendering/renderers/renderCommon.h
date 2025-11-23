#pragma once
#include <unordered_map>
#include <vector>
#include <array>

struct Material;
class Shader;
class Entity;

namespace RenderCommon {
void setupTransform(const Entity& entity, const Shader& shader);

void setupMaterial(const Entity& entity, const Shader& shader);

void drawMeshes(const Entity& entity, const Shader& shader);

void drawQuad(uint32_t sceneTexture, uint32_t VAO);

using MaterialMap = std::unordered_map<uint32_t, Material>;

void bindTextures(
	uint32_t materialID,
	const MaterialMap* materials,
	const Shader& shader);

void unbindTextures(uint32_t materialID, const MaterialMap* materials);

void bindShadowMaps(const std::array<uint32_t, 3>& shadowMaps);
}
