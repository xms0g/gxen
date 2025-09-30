#pragma once

#include <memory>
#include <vector>
#include <string>
#include "glad/glad.h"
#include "../models/quad.h"

class FrameBuffer;
class Shader;

enum EffectType { NONE = 0, BLUR = 1, SHARPEN = 2, EDGE = 3 };

struct PostEffect {
	std::string name;
	std::shared_ptr<Shader> shader;
	EffectType type{NONE};
	bool enabled{false};
};

class PostProcess {
public:
	PostProcess(uint32_t width, uint32_t height);

	std::vector<PostEffect>& effects() { return mEffects; }

	void render(GLuint sceneTexture) const;

private:
	void draw(GLuint sceneTexture) const;

	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::vector<PostEffect> mEffects;
};
