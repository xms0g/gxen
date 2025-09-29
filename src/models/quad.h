#pragma once
#include <memory>

class Shader;

namespace Models {
class Quad {
public:
	Quad();
	~Quad();

	[[nodiscard]] unsigned int VAO() const { return mVAO; }

	[[nodiscard]] const Shader& shader() const { return *mShader; }

private:
	unsigned int mVAO{0}, mVBO{0};
	std::unique_ptr<Shader> mShader;
};
}
