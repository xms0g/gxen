#include "postProcess.h"
#include "shader.h"
#include "../models/quad.h"

PostProcess::PostProcess() : mQuad(std::make_unique<Models::Quad>()) {}

void PostProcess::render(const GLuint sceneTexture) const {
	mQuad->shader().activate();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(mQuad->VAO());
	glBindTexture(GL_TEXTURE_2D, sceneTexture);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

