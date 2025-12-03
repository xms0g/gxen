#pragma once
#include "IRenderPass.hpp"

class ForwardOpaquePass final : public IRenderPass {
public:
	~ForwardOpaquePass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};
