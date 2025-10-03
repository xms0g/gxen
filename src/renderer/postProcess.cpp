#include "postProcess.h"
#include "shader.h"
#include "frameBuffer.h"

PostProcess::PostProcess(int width, int height) : mQuad(std::make_unique<Models::Quad>()) {
	mEffects = {
		{"Grayscale", std::make_shared<Shader>("models/quad.vert", "post-processing/grayscale.frag"), NONE, false},
		{"Sepia", std::make_shared<Shader>("models/quad.vert", "post-processing/sepia.frag"), NONE, false},
		{"Edge Detection", std::make_shared<Shader>("models/quad.vert", "post-processing/kernel.frag"), EDGE, false},
		{"Inverse", std::make_shared<Shader>("models/quad.vert", "post-processing/inverse.frag"), NONE, false},
		{"Sharpen", std::make_shared<Shader>("models/quad.vert", "post-processing/kernel.frag"), SHARPEN, false},
		{"Blur", std::make_shared<Shader>("models/quad.vert", "post-processing/kernel.frag"), BLUR, false},
		{"Gamma Correction", std::make_shared<Shader>("models/quad.vert", "post-processing/gamma.frag"), NONE, true},
	};

	for (auto& pingPongBuffer : pingPongBuffers) {
		pingPongBuffer = std::make_unique<FrameBuffer>(width, height);
		pingPongBuffer->withTexture().checkStatus();
	}
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
