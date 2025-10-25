#include "bloom.h"
#include "glad/glad.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../../models/quad.h"

Bloom::Bloom(const std::string& name, int width, int height, const bool enabled): IPostEffect(name, nullptr, enabled) {
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

uint32_t Bloom::render(const uint32_t sceneTexture, const uint32_t VAO) const {
	int toggle = 0;
	uint32_t inputTex = sceneTexture;

	pingPongBuffers[toggle]->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	brightFilter->activate();

	draw(inputTex, VAO);
	inputTex = pingPongBuffers[toggle]->texture();
	pingPongBuffers[toggle]->unbind();
	toggle = !toggle;

	bool horizontal = true;
	for (int i = 0; i < 10; i++) {
		pingPongBuffers[toggle]->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		blur->activate();
		blur->setBool("horizontal", horizontal);
		horizontal = !horizontal;
		draw(inputTex, VAO);
		inputTex = pingPongBuffers[toggle]->texture();
		pingPongBuffers[toggle]->unbind();
		toggle = !toggle;
	}

	pingPongBuffers[toggle]->bind();
	combine->activate();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	pingPongBuffers[toggle]->unbind();

	return pingPongBuffers[toggle]->texture();
}

void Bloom::draw(const uint32_t sceneTexture, const uint32_t VAO) const {
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
