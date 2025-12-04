#pragma once
#include "IRenderPass.hpp"

class FrustumCullingPass final : public IRenderPass {
public:
	~FrustumCullingPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};