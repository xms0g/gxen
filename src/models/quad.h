#pragma once
#include <memory>

class Shader;

namespace Models {
class Quad {
public:
	Quad();
	~Quad();

	[[nodiscard]] uint32_t VAO() const { return mVAO; }

	[[nodiscard]] const Shader& shader() const { return *mShader; }

private:
	uint32_t mVAO{0}, mVBO{0};
	std::unique_ptr<Shader> mShader;
};
}
