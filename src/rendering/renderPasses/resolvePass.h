#pragma once
#include "IRenderPass.hpp"

class ResolvePass final : public IRenderPass {
public:
	~ResolvePass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};