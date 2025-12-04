#include "frustumCullingPass.h"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../mesh/mesh.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/bv.hpp"
#include "../../ECS/entity.hpp"
#include "../../ECS/components/transform.hpp"
#include "../../math/boundingVolume.h"

FrustumCullingPass::~FrustumCullingPass() = default;

void FrustumCullingPass::configure(const RenderContext& context) {
}

void FrustumCullingPass::execute(const RenderContext& context) {
	auto cullItems = [&](const std::vector<RenderGroup>& groups) {
		for (const auto& [entity, matBatch]: groups) {
			auto& bvc = entity->getComponent<BoundingVolumeComponent>();
			const auto& tc = entity->getComponent<TransformComponent>();
			const auto& aabb = bvc.bv;

			bvc.isVisible = aabb->isOnFrustum(*context.camera.frustum, tc.position, tc.rotation, tc.scale);
			if (!bvc.isVisible) return;

			for (auto& [material, shader, meshes]: matBatch) {
				for (auto& mesh: *meshes) {
					mesh.setVisible(aabb->isMeshInFrustum(*context.camera.frustum, mesh.min(), mesh.max(),
														  tc.position, tc.rotation, tc.scale));
				}
			}
		}
	};

	cullItems(context.renderQueue->forwardOpaqueGroups);
	cullItems(context.renderQueue->deferredGroups);
	cullItems(context.renderQueue->blendGroups);
}
