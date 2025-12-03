#include "debugPass.h"
#include "../shader.h"
#include "../renderers/renderCommon.h"
#include "../buffers/uniformBuffer.h"
#include "../renderContext/renderContext.hpp"
#include "../renderContext/renderQueue.hpp"
#include "../renderContext/renderGroup.hpp"
#include "../../ECS/components/debug.hpp"
#include "../../ECS/entity.hpp"

DebugPass::~DebugPass() = default;

void DebugPass::configure(const RenderContext& context) {
	mDebugShaders = {
		nullptr, // for None
		std::make_shared<Shader>("debug/normal.vert", "debug/normal.frag", "debug/normal.geom"),
		std::make_shared<Shader>("debug/wireframe.vert", "debug/wireframe.frag", "debug/wireframe.geom")
	};

	for (const auto& shader: mDebugShaders) {
		if (!shader) continue;
		context.cameraUBO->configure(shader->ID(), 0, "CameraBlock");
	}
}

void DebugPass::execute(const RenderContext& context) {
	for (const auto& [entity, matBatches]: context.renderQueue->debugGroups) {
		const auto& db = entity->getComponent<DebugComponent>();
		if (db.mode == None)
			continue;

		const auto& dbgShader = mDebugShaders.at(db.mode);
		dbgShader->activate();

		for (const auto& [material, shader, meshes]: matBatches) {
			RenderCommon::setupTransform(*entity, *dbgShader);
			RenderCommon::drawMeshes(*meshes);
		}

	}
}
