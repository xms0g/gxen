#pragma once
#include "IRenderPass.hpp"

class BeginScenePass final : public IRenderPass {
public:
	~BeginScenePass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;
};