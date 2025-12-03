#pragma once
#include "IRenderPass.hpp"

class BlendPass final : public IRenderPass {
public:
	~BlendPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};