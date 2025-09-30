#include "postProcess.h"
#include "shader.h"
#include "frameBuffer.h"
#include "../models/quad.h"

PostProcess::PostProcess(uint32_t width, uint32_t height) : mQuad(std::make_unique<Models::Quad>()),
                             mInverse(std::make_unique<Shader>("models/quad.vert", "post-processing/inverse.frag")),
                             mGrayScale(std::make_unique<Shader>("models/quad.vert", "post-processing/grayscale.frag")),
                             mGamma(std::make_unique<Shader>("models/quad.vert", "post-processing/gamma.frag")) {
	for (int i = 0; i < 2; i++) {
		pingPongBuffers[i] = std::make_unique<FrameBuffer>(width, height);
		pingPongBuffers[i]->withTexture().checkStatus();
	}
}

void PostProcess::render(const GLuint sceneTexture) const {
	int toggle = 0;
	GLuint inputTex = sceneTexture;

	if (mGrayScaleEnabled || mInverseEnabled) {
		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		if (mGrayScaleEnabled) {
			mGrayScale->activate();
			mGrayScale->setInt("screenTexture", 0);
		} else if (mInverseEnabled) {
			mInverse->activate();
			mInverse->setInt("screenTexture", 0);
		}

		draw(inputTex);
		inputTex = pingPongBuffers[toggle]->texture();
		pingPongBuffers[toggle]->unbind();
		toggle = !toggle;
	}

	if (mGammaEnabled) {
		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		mGamma->activate();
		mGamma->setInt("screenTexture", 0);

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
	glBindTexture(GL_TEXTURE_2D, sceneTexture); // use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
