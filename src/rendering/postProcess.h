#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <string>

namespace Models {
class Quad;
}

class FrameBuffer;
class Shader;

enum EffectType { BLUR = 1, SHARPEN = 2, EDGE = 3 };

struct IPostEffect {
	std::string name;
	std::shared_ptr<Shader> shader;
	bool enabled{false};

	IPostEffect() = default;

	IPostEffect(std::string n, const std::shared_ptr<Shader>& s, const bool e) : name(std::move(n)), shader(s),
		enabled(e) {
	}

	virtual ~IPostEffect() = default;

	virtual void applyUniforms() const {}
};

struct GreyScale final : IPostEffect {
	GreyScale(const std::string& n, const std::shared_ptr<Shader>& s, const bool e) : IPostEffect(n, s, e) {
	}
};

struct Inverse final : IPostEffect {
	Inverse(const std::string& n, const std::shared_ptr<Shader>& s, const bool e) : IPostEffect(n, s, e) {
	}
};

struct Sepia final : IPostEffect {
	Sepia(const std::string& n, const std::shared_ptr<Shader>& s, const bool e) : IPostEffect(n, s, e) {
	}
};

struct GammaCorrection final : IPostEffect {
	GammaCorrection(const std::string& n, const std::shared_ptr<Shader>& s, const bool e) : IPostEffect(n, s, e) {
	}
};

struct ToneMapping final : IPostEffect {
	float exposure{1.0f};

	ToneMapping(const std::string& n, const std::shared_ptr<Shader>& s, const bool e, const float exp)
		: IPostEffect(n, s, e), exposure(exp) {
	}

	void applyUniforms() const override;
};

struct Kernel final : IPostEffect {
	EffectType type{};

	Kernel(const std::string& n, const std::shared_ptr<Shader>& s, const bool e, const EffectType t)
		: IPostEffect(n, s, e), type(t) {
	}
	void applyUniforms() const override;
};

class PostProcess {
public:
	PostProcess(int width, int height);

	~PostProcess();

	std::vector<std::shared_ptr<IPostEffect> >& effects() { return mEffects; }

	void render(uint32_t sceneTexture) const;

private:
	void draw(uint32_t sceneTexture) const;

	std::unique_ptr<Models::Quad> mQuad;
	std::unique_ptr<FrameBuffer> pingPongBuffers[2];
	std::vector<std::shared_ptr<IPostEffect> > mEffects;
};
