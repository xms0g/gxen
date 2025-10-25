#include "toneMapping.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"

ToneMapping::ToneMapping(const std::string& name, const bool enabled, const float exp)
	: IPostEffect(name, enabled),
	  exposure(exp) {
	shader = std::make_unique<Shader>("models/quad.vert", "post-processing/toneMapping.frag");
	shader->activate();
	shader->setInt("screenTexture", 0);
}

uint32_t ToneMapping::render(const uint32_t sceneTexture,
                             const uint32_t VAO,
                             const std::unique_ptr<FrameBuffer>* renderTargets,
                             int& toggle) const {
	renderTargets[toggle]->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	shader->activate();
	shader->setFloat("exposure", exposure);

	draw(sceneTexture, VAO);

	const uint32_t texture = renderTargets[toggle]->texture();
	renderTargets[toggle]->unbind();
	toggle = !toggle;
	return texture;
}
