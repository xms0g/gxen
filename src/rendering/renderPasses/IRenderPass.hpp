#pragma once
#include <cstdint>

struct RenderContext;

class IRenderPass {
public:
	virtual ~IRenderPass() = default;

	virtual void configure(const RenderContext& context) = 0;

	virtual void execute(const RenderContext& context) = 0;
};
