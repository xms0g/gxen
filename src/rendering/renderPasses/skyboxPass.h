#pragma once
#include "IRenderPass.hpp"

class SkyboxPass final : public IRenderPass {
public:
	~SkyboxPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};