#pragma once
#include <array>
#include <memory>

#include "IRenderPass.hpp"

class Shader;

class DebugPass final : public IRenderPass {
public:
	~DebugPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;

private:
	std::array<std::shared_ptr<Shader>, 3> mDebugShaders;
};
