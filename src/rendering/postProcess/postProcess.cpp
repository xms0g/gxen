#include "postProcess.h"
#include "grayscale.h"
#include "Sepia.h"
#include "bloom.h"
#include "blur.h"
#include "gamma.h"
#include "sharpen.h"
#include "toneMapping.h"
#include "edgeDetection.h"
#include "../shader.h"
#include "../buffers/frameBuffer.h"
#include "../../models/quad.h"

PostProcess::PostProcess(int width, int height) : mQuad(std::make_unique<Models::Quad>()) {
	mEffects = {
		std::make_shared<Grayscale>("Grayscale", false),
		std::make_shared<Sepia>("Sepia", false),
		std::make_shared<EdgeDetection>("Edge Detection", false),
		std::make_shared<Sharpen>("Sharpen", false),
		std::make_shared<Blur>("Blur", false),
		std::make_shared<Bloom>("Bloom", width, height, false),
		std::make_shared<ToneMapping>("Tone Mapping", false, 1.1f),
		std::make_shared<Gamma>("Gamma Correction", true)
	};

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

PostProcess::~PostProcess() = default;

void PostProcess::render(const uint32_t sceneTexture) const {
	uint32_t inputTex = sceneTexture;
	int toggle = 0;

	for (const auto& effect: mEffects) {
		if (!effect->enabled) continue;

		inputTex = effect->render(inputTex, mQuad->VAO(), pingPongBuffers, toggle);
	}

	mQuad->shader().activate();
	draw(inputTex, mQuad->VAO());
}
