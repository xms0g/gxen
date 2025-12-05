#pragma once
#include <memory>
#include <vector>
#include "IPostEffect.hpp"
#include "../IRenderPass.hpp"

struct RenderContext;

namespace Models {
class Quad;
}

class FrameBuffer;
class Shader;

class PostProcessPass final : public IRenderPass {
public:
	~PostProcessPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;

	std::vector<std::shared_ptr<IPostEffect> >& effects() { return mEffects; }

private:
	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> renderTargets[2];
	std::vector<std::shared_ptr<IPostEffect> > mEffects;
};
