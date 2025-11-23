#pragma once
#include <unordered_map>
#include <vector>
#include <array>

struct Texture;
class Mesh;
struct RenderItem;
struct Material;
class Shader;
class Entity;

namespace RenderCommon {
void setupTransform(const Entity& entity, const Shader& shader);

void setupMaterial(const Entity& entity, const Shader& shader);

void drawMesh(const RenderItem& item, const Shader& shader);

void drawQuad(uint32_t sceneTexture, uint32_t VAO);

using MaterialMap = std::unordered_map<uint32_t, Material>;

void bindTextures(const std::vector<Texture>& textures, const Shader& shader);

void unbindTextures(const std::vector<Texture>& textures);

void bindShadowMaps(const std::array<uint32_t, 3>& shadowMaps);
}
