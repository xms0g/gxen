#pragma once
#include <memory>

#include "glad/glad.h"

namespace Models {
class Quad;
}

class PostProcess {
public:
	PostProcess();

	void render(GLuint sceneTexture) const;

private:
	std::unique_ptr<Models::Quad> mQuad;
};
