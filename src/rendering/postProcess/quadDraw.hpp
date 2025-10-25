#pragma once
#include "glad/glad.h"

class QuadDraw {
public:
	void draw(const uint32_t sceneTexture, const uint32_t VAO) const {
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
};