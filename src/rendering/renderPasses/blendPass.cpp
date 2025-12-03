#include "blendPass.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../renderers/renderCommon.h"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/entity.hpp"

BlendPass::~BlendPass() = default;

void BlendPass::configure(const RenderContext& context) {
}

void BlendPass::execute(const RenderContext& context) {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& [entity, matBatches]: context.renderQueue->blendGroups) {
		if (!entity->getComponent<BoundingVolumeComponent>().isVisible)
			continue;

		for (const auto& [material, shader, meshes]: matBatches) {
			shader->activate();
			shader->setInt("shadowMap", context.shadowMap.textureSlot);
			shader->setInt("shadowCubemap", context.shadowMap.textureSlot + 1);
			shader->setInt("persShadowMap", context.shadowMap.textureSlot + 2);

			RenderCommon::setupTransform(*entity, *shader);
			RenderCommon::setupMaterial(*entity, *shader);

			RenderCommon::bindTextures(material->textures, *shader);
			RenderCommon::drawMeshes(*meshes);
			RenderCommon::unbindTextures(material->textures);
		}
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}
