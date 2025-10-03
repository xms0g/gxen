#pragma once

#include <string>
#include <unordered_set>

#include "glm/glm.hpp"
#include "glad/glad.h"

class Shader {
public:
	Shader(const char* vs, const char* fs, const char* gs = nullptr);

	~Shader();

	Shader(const Shader&) = delete;

	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other) noexcept {
		mID = other.mID;
		other.mID = 0;
	}

	Shader& operator=(Shader&& other) noexcept {
		if (this != &other) {
			glDeleteProgram(mID);
			mID = other.mID;
			other.mID = 0;
		}
		return *this;
	}

	[[nodiscard]] GLuint ID() const { return mID; }

	// use/activate the shader
	void activate() const;

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;

	void setFloat(const std::string& name, float value) const;

	void setVec2(const std::string& name, const glm::vec2& value) const;

	void setVec2(const std::string& name, float x, float y) const;

	void setVec3(const std::string& name, const glm::vec3& value) const;

	void setVec3(const std::string& name, float x, float y, float z) const;

	void setVec4(const std::string& name, const glm::vec4& value) const;

	void setVec4(const std::string& name, float x, float y, float z, float w) const;

	void setMat2(const std::string& name, const glm::mat2& mat) const;

	void setMat3(const std::string& name, const glm::mat3& mat) const;

	void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
	std::string preprocess(std::string& source, const char* fileName, std::unordered_set<std::string>& includedFiles);

	GLuint createShader(const char** source, GLuint type);

	GLuint linkShader(GLuint vertex, GLuint fragment, GLuint geometry = 0);

	// the program ID
	GLuint mID{};
};
