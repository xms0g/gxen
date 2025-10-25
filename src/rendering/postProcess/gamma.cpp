#include "gamma.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"

Gamma::Gamma(const std::string& name, const bool enabled) : IPostEffect(name, enabled) {
	shader = std::make_unique<Shader>("models/quad.vert", "post-processing/gamma.frag");
	shader->activate();
	shader->setInt("screenTexture", 0);
}

uint32_t Gamma::render(const uint32_t sceneTexture,
                       const uint32_t VAO,
                       const std::unique_ptr<FrameBuffer>* renderTargets,
                       int& toggle) const {
	renderTargets[toggle]->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	shader->activate();
	draw(sceneTexture, VAO);

	const uint32_t texture = renderTargets[toggle]->texture();
	renderTargets[toggle]->unbind();
	toggle = !toggle;
	return texture;
}
