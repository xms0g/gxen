#include "blur.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../renderCommon.h"
#include "../buffers/frameBuffer.h"

Blur::Blur(const std::string& name, const bool enabled) : IPostEffect(name, enabled) {
	shader = std::make_unique<Shader>("models/quad.vert", "post-processing/kernel.frag");
	shader->activate();
	shader->setInt("screenTexture", 0);
}

uint32_t Blur::render(const uint32_t sceneTexture, const uint32_t VAO,
                      int& toggle, const std::unique_ptr<FrameBuffer>* renderTargets) const {
	renderTargets[toggle]->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	shader->activate();
	shader->setFloatArray("kernel", kernel, 9);

	RenderCommon::drawQuad(sceneTexture, VAO);

	const uint32_t texture = renderTargets[toggle]->texture();
	renderTargets[toggle]->unbind();
	toggle = !toggle;
	return texture;
}
