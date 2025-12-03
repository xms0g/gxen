#include "forwardOpaquePass.h"
#include "../shader.h"
#include "../renderers/renderCommon.h"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/entity.hpp"

ForwardOpaquePass::~ForwardOpaquePass() = default;

void ForwardOpaquePass::configure(const RenderContext& context) {
}

void ForwardOpaquePass::execute(const RenderContext& context) {
	RenderCommon::bindShadowMaps(*context.shadowMap.textures);

	for (const auto& [entity, matBatches]: context.renderQueue->forwardOpaqueGroups) {
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
}


