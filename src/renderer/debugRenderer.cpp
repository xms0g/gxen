#include "debugRenderer.h"
#include "shader.h"
#include "uniformBuffer.h"
#include "../ECS/components/debug.hpp"

DebugRenderer::DebugRenderer() {
	RequireComponent<DebugComponent>();
}

void DebugRenderer::configure(const UniformBuffer& cameraUBO) const {
	for (const auto& entity: getSystemEntities()) {
		const auto& dbShader = entity.getComponent<DebugComponent>().shader;

		cameraUBO.configure(dbShader->ID(), 0, "CameraBlock");
	}
}

void DebugRenderer::render() const {
	for (const auto& entity: getSystemEntities()) {
		const auto& db = entity.getComponent<DebugComponent>();
		if (db.mode == DebugMode::None)
			continue;

		const auto& dbShader = db.shader;
		dbShader->activate();
		geometryPass(entity, *dbShader);
		drawPass(entity);
	}
}
