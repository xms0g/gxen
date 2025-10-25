#pragma once
#include <memory>
#include <string>

class FrameBuffer;

class IPostEffect {
public:
	std::string name;
	bool enabled{false};

	IPostEffect() = default;

	IPostEffect(std::string n, const bool e) : name(std::move(n)), enabled(e) {
	}

	virtual ~IPostEffect() = default;

	virtual void applyUniforms() const {
	}

	virtual uint32_t render(uint32_t sceneTexture,
	                        uint32_t VAO,
	                        const std::unique_ptr<FrameBuffer>* renderTargets,
	                        int& toggle) const = 0;
};
