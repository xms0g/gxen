#pragma once
#include <memory>
#include <string>

class Shader;

class IPostEffect {
public:
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