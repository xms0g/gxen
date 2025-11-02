#include "debugRenderer.h"
#include "../shader.h"
#include "../buffers/uniformBuffer.h"
#include "../../ECS/components/debug.hpp"
#include "../../ECS/entity.hpp"

DebugRenderer::DebugRenderer() {
	mDebugShaders = {
		{
			DebugMode::Normals,
			std::make_shared<Shader>("debug/normal.vert", "debug/normal.frag", "debug/normal.geom")
		},
		{
			DebugMode::Wireframe,
			std::make_shared<Shader>("debug/wireframe.vert", "debug/wireframe.frag", "debug/wireframe.geom")
		}
	};
}

void DebugRenderer::configure(const UniformBuffer& cameraUBO) const {
	for (const auto& [mode, shader]: mDebugShaders) {
		cameraUBO.configure(shader->ID(), 0, "CameraBlock");
	}
}

void DebugRenderer::render(std::vector<Entity>& entities) const {
	for (const auto& entity: entities) {
		const auto& db = entity.getComponent<DebugComponent>();
		if (db.mode == DebugMode::None)
			continue;

		const auto& dbShader = mDebugShaders.at(db.mode);
		dbShader->activate();
		geometryPass(entity, *dbShader);
		drawPass(entity, *dbShader);
	}
	entities.clear();
}
