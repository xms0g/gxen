#pragma once
#include <memory>
#include <vector>
#include "IPostEffect.hpp"

struct RenderContext;

namespace Models {
class Quad;
}

class FrameBuffer;
class Shader;

class PostProcessPass {
public:
	~PostProcessPass();

	void configure(const RenderContext& context);

	void execute(const RenderContext& context) const;

	std::vector<std::shared_ptr<IPostEffect> >& effects() { return mEffects; }

private:
	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> renderTargets[2];
	std::vector<std::shared_ptr<IPostEffect> > mEffects;
};
