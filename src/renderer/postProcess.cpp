#include "postProcess.h"
#include "shader.h"
#include "frameBuffer.h"
#include "../models/quad.h"

PostProcess::PostProcess(uint32_t width, uint32_t height) : mQuad(std::make_unique<Models::Quad>()) {
	for (int i = 0; i < 2; i++) {
		pingPongBuffers[i] = std::make_unique<FrameBuffer>(width, height);
		pingPongBuffers[i]->withTexture().checkStatus();
	}

	mEffects.emplace_back(
		"Grayscale",
		std::make_unique<Shader>("models/quad.vert", "post-processing/grayscale.frag"),
		NONE,
		false);

	mEffects.emplace_back(
		"Inverse",
		std::make_unique<Shader>("models/quad.vert", "post-processing/inverse.frag"),
		NONE,
		false);

	mEffects.emplace_back(
		"Blur",
		std::make_unique<Shader>("models/quad.vert", "post-processing/kernel.frag"),
		BLUR,
		false);

	mEffects.emplace_back(
		"Sharpen",
		std::make_unique<Shader>("models/quad.vert", "post-processing/kernel.frag"),
		SHARPEN,
		false);

	mEffects.emplace_back(
		"Edge Detection",
		std::make_unique<Shader>("models/quad.vert", "post-processing/kernel.frag"),
		EDGE,
		false);

	mEffects.emplace_back(
		"Gamma Correction",
		std::make_unique<Shader>("models/quad.vert", "post-processing/gamma.frag"),
		NONE,
		true);
}

void PostProcess::render(const GLuint sceneTexture) const {
	int toggle = 0;
	GLuint inputTex = sceneTexture;

	for (const auto& effect: mEffects) {
		if (!effect.enabled) continue;

		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		effect.shader->activate();
		effect.shader->setInt("type", effect.type);
		effect.shader->setInt("screenTexture", 0);

		draw(inputTex);
		inputTex = pingPongBuffers[toggle]->texture();
		pingPongBuffers[toggle]->unbind();
		toggle = !toggle;
	}

	mQuad->shader().activate();
	draw(inputTex);
}

void PostProcess::draw(const GLuint sceneTexture) const {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mQuad->VAO());
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
