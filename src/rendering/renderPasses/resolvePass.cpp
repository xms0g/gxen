#include "resolvePass.h"
#include "glad/glad.h"
#include "../buffers/frameBuffer.h"
#include "../renderContext/renderContext.hpp"

ResolvePass::~ResolvePass() = default;

void ResolvePass::configure(const RenderContext& context) {
}

void ResolvePass::execute(const RenderContext& context) {
	context.sceneBuffer->bindForRead();
	context.intermediateBuffer->bindForDraw();
	glBlitFramebuffer(0, 0, context.sceneBuffer->width(), context.sceneBuffer->height(),
					  0, 0, context.intermediateBuffer->width(), context.intermediateBuffer->height(),
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	context.sceneBuffer = context.intermediateBuffer;
}
