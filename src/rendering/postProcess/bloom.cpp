#include "bloom.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../../models/quad.h"

Bloom::Bloom(const std::string& name, int width, int height, const bool enabled) : IPostEffect(name, enabled) {
	brightFilter = std::make_unique<Shader>("models/quad.vert", "post-processing/bloom/brightFilter.frag");
	brightFilter->activate();
	brightFilter->setInt("screenTexture", 0);

	blur = std::make_unique<Shader>("models/quad.vert", "post-processing/bloom/blur.frag");
	blur->activate();
	blur->setInt("screenTexture", 0);

	combine = std::make_unique<Shader>("models/quad.vert", "post-processing/bloom/combine.frag");
	combine->activate();
	combine->setInt("screenTexture", 0);
	combine->setInt("bloomBlur", 1);

	for (auto& pingPongBuffer: pingPongBuffers) {
		pingPongBuffer = std::make_unique<FrameBuffer>(width, height);
#ifdef HDR
		pingPongBuffer->withTexture16F()
#else
		pingPongBuffer->withTexture()
#endif
		.checkStatus();
	}
}

uint32_t Bloom::render(const uint32_t sceneTexture,
                       const uint32_t VAO,
                       const std::unique_ptr<FrameBuffer>* renderTargets,
                       int& toggle) const {
	int toggle_ = 0;
	uint32_t inputTex = sceneTexture;

	inputTex = brightFilterPass(inputTex, VAO, toggle_);
	inputTex = blurPass(inputTex, VAO, toggle_);
	inputTex = combinePass(sceneTexture, inputTex, VAO, toggle_);

	return inputTex;
}

uint32_t Bloom::brightFilterPass(uint32_t sceneTexture, const uint32_t VAO, int& toggle) const {
	pingPongBuffers[toggle]->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	brightFilter->activate();

	draw(sceneTexture, VAO);

	sceneTexture = pingPongBuffers[toggle]->texture();
	toggle = !toggle;
	return sceneTexture;
}

uint32_t Bloom::blurPass(uint32_t sceneTexture, const uint32_t VAO, int& toggle) const {
	bool horizontal = true;

	for (int i = 0; i < 10; i++) {
		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		blur->activate();
		blur->setBool("horizontal", horizontal);
		horizontal = !horizontal;

		draw(sceneTexture, VAO);

		sceneTexture = pingPongBuffers[toggle]->texture();
		toggle = !toggle;
	}

	return sceneTexture;
}

uint32_t Bloom::combinePass(const uint32_t sceneTexture, const uint32_t bloomBlur, const uint32_t VAO,
                            const int& toggle) const {
	pingPongBuffers[toggle]->bind();
	combine->activate();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBlur);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	pingPongBuffers[toggle]->unbind();

	return pingPongBuffers[toggle]->texture();
}
