#pragma once
#include <cstdint>
#include "IRenderPass.hpp"

class OpaqueInstancedPass final : public IRenderPass {
public:
	~OpaqueInstancedPass() override;

	void configure(const RenderContext& context) override;

	void execute(const RenderContext& context) override;

private:
	void prepareInstanceBuffer(const RenderContext& context);

	void prepareInstanceData(const RenderContext& context) const;

	struct {
		uint32_t buffer{};
		int offset{0};
	} vbo;
};