#pragma once
#include <memory>
#include <array>
#include <string>

namespace Models {
class Quad;
}

class FrameBuffer;
class Shader;

enum EffectType { NONE = 0, BLUR = 1, SHARPEN = 2, EDGE = 3 };

struct PostEffect {
	std::string name;
	std::shared_ptr<Shader> shader;
	EffectType type{NONE};
	float exposure{1.0f};
	bool enabled{false};
};

class PostProcess {
public:
	PostProcess(int width, int height);

	~PostProcess();

	std::array<PostEffect, 8>& effects() { return mEffects; }

	void render(uint32_t sceneTexture) const;

private:
	void draw(uint32_t sceneTexture) const;

	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::array<PostEffect, 8> mEffects;
};
