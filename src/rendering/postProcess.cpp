#include "postProcess.h"
#include "glad/glad.h"
#include "shader.h"
#include "buffers/frameBuffer.h"
#include "../models/quad.h"

PostProcess::PostProcess(int width, int height) : mQuad(std::make_unique<Models::Quad>()) {
	const auto kernel = std::make_shared<Shader>("models/quad.vert", "post-processing/kernel.frag");
	mEffects = {
		std::make_shared<GreyScale>("Grayscale", std::make_shared<Shader>("models/quad.vert", "post-processing/grayscale.frag"), false),
		std::make_shared<Sepia>("Sepia", std::make_shared<Shader>("models/quad.vert", "post-processing/sepia.frag"), false),
		std::make_shared<Inverse>("Inverse", std::make_shared<Shader>("models/quad.vert", "post-processing/inverse.frag"), false),
		std::make_shared<Kernel>("Edge Detection", kernel, false, EDGE),
		std::make_shared<Kernel>("Sharpen", kernel, false, SHARPEN),
		std::make_shared<Kernel>("Blur", kernel, false, BLUR),
		std::make_shared<ToneMapping>("Tone Mapping", std::make_shared<Shader>("models/quad.vert", "post-processing/toneMapping.frag"), false, 1.1f),
		std::make_shared<GammaCorrection>("Gamma Correction", std::make_shared<Shader>("models/quad.vert", "post-processing/gamma.frag"), true)
	};

	for (const auto& effect: mEffects) {
		effect->shader->setInt("screenTexture", 0);
	}

	for (auto& pingPongBuffer: pingPongBuffers) {
		pingPongBuffer = std::make_unique<FrameBuffer>(width, height);
		pingPongBuffer->withTexture()
				.checkStatus();
	}
}

PostProcess::~PostProcess() = default;


void PostProcess::render(const uint32_t sceneTexture) const {
	int toggle = 0;
	uint32_t inputTex = sceneTexture;

	for (const auto& effect: mEffects) {
		if (!effect->enabled) continue;

		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		effect->shader->activate();
		effect->applyUniforms();

		draw(inputTex);
		inputTex = pingPongBuffers[toggle]->texture();
		pingPongBuffers[toggle]->unbind();
		toggle = !toggle;
	}

	mQuad->shader().activate();
	draw(inputTex);
}

void PostProcess::draw(const uint32_t sceneTexture) const {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mQuad->VAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void ToneMapping::applyUniforms() const {
	shader->setFloat("exposure", exposure);
}

void Kernel::applyUniforms() const {
	shader->setInt("type", type);
}


