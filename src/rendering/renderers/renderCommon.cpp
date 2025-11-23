#include "renderCommon.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../renderFlags.hpp"
#include "../renderItem.hpp"
#include "../material.hpp"
#include "../../config/config.hpp"
#include "../../mesh/mesh.h"
#include "../../math/matrix.hpp"
#include "../../ECS/entity.hpp"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/material.hpp"
#include "../../ECS/components/mesh.hpp"
#include "../../resourceManager/texture.h"

void RenderCommon::setupTransform(const Entity& entity, const Shader& shader) {
	const auto& tc = entity.getComponent<TransformComponent>();

	const glm::mat4 model = math::computeModelMatrix(tc.position, tc.rotation, tc.scale);
	const glm::mat3 normal = math::computeNormalMatrix(model);

	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", normal);
}

void RenderCommon::setupMaterial(const Entity& entity, const Shader& shader) {
	const auto& mtc = entity.getComponent<MaterialComponent>();

	if (mtc.flags & TwoSided) {
		glDisable(GL_CULL_FACE);
	}

	shader.setFloat("material.shininess", mtc.shininess);
	shader.setFloat("material.heightScale", mtc.heightScale);

	if (!mtc.materials) {
		shader.setVec3("material.color", mtc.color);
	}
}

void RenderCommon::drawMesh(const RenderItem& item, const Shader& shader) {
	if (!item.mesh->isVisible()) return;

	bindTextures(item.material->textures, shader);

	glBindVertexArray(item.mesh->VAO());
	if (!item.mesh->indices().empty()) {
		glDrawElements(GL_TRIANGLES, static_cast<int32_t>(item.mesh->indices().size()), GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<int32_t>(item.mesh->vertices().size()));
	}

	unbindTextures(item.material->textures);
}

void RenderCommon::drawQuad(const uint32_t sceneTexture, const uint32_t VAO) {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}


void RenderCommon::bindTextures(const std::vector<Texture>& textures, const Shader& shader) {
	bool hasNormalMap{false}, hasHeightMap{false};

	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

		std::string name = textures[i].type;

		if (name == "texture_normal") {
			hasNormalMap = true;
		}

		if (name == "texture_height") {
			hasHeightMap = true;
		}

		// now set the sampler to the correct texture unit
		shader.setInt(std::string("material.").append(name), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	shader.setBool("material.hasNormalMap", hasNormalMap);
	shader.setBool("material.hasHeightMap", hasHeightMap);
}

void RenderCommon::unbindTextures(const std::vector<Texture>& textures) {
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void RenderCommon::bindShadowMaps(const std::array<uint32_t, 3>& shadowMaps) {
	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT);
	glBindTexture(GL_TEXTURE_2D, shadowMaps[0]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps[1]);

	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_SLOT + 2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps[2]);
}
