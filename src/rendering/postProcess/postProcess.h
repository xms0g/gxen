#pragma once
#include <memory>
#include <vector>
#include "IPostEffect.hpp"
#include "quadDraw.hpp"

namespace Models {
class Quad;
}

class FrameBuffer;
class Shader;

class PostProcess : public QuadDraw {
public:
	PostProcess(int width, int height);

	~PostProcess();

	std::vector<std::shared_ptr<IPostEffect> >& effects() { return mEffects; }

	void render(uint32_t sceneTexture) const;

private:
	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::vector<std::shared_ptr<IPostEffect> > mEffects;
};
