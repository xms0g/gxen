#include "debugRenderer.h"
#include "renderCommon.h"
#include "../shader.h"
#include "../buffers/uniformBuffer.h"
#include "../../ECS/components/debug.hpp"
#include "../../ECS/entity.hpp"

DebugRenderer::DebugRenderer() {
	mDebugShaders = {
		nullptr, // for None
		std::make_shared<Shader>("debug/normal.vert", "debug/normal.frag", "debug/normal.geom"),
		std::make_shared<Shader>("debug/wireframe.vert", "debug/wireframe.frag", "debug/wireframe.geom")
	};
}

void DebugRenderer::configure(const UniformBuffer& cameraUBO) const {
	for (const auto& shader: mDebugShaders) {
		if (!shader) continue;
		cameraUBO.configure(shader->ID(), 0, "CameraBlock");
	}
}

void DebugRenderer::render(const std::vector<Entity>& entities) const {
	for (const auto& entity: entities) {
		const auto& db = entity.getComponent<DebugComponent>();
		if (db.mode == None)
			continue;

		const auto& dbgShader = mDebugShaders.at(db.mode);
		dbgShader->activate();
		RenderCommon::setupTransform(entity, *dbgShader);
		RenderCommon::drawMeshes(entity, *dbgShader);
	}
}
