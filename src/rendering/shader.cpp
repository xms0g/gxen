#include "shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../io/filesystem.hpp"
#include "../config/config.hpp"

Shader::Shader(const char* vs, const char* fs, const char* gs) {
	std::unordered_set<std::string> includedFiles{};

	try {
		std::string vertexCode = preprocess(loadFile(vs), vs, includedFiles);
		includedFiles.clear();
		std::string fragmentCode = preprocess(loadFile(fs), fs, includedFiles);
		includedFiles.clear();
		std::string geometryCode = gs ? preprocess(loadFile(gs), gs, includedFiles) : "";
		includedFiles.clear();

		const GLuint vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
		const GLuint fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
		GLuint geometry = 0;

		if (gs) {
			geometry = compileShader(geometryCode, GL_GEOMETRY_SHADER);
		}

		linkShader(vertex, fragment, geometry);
	} catch (std::runtime_error& e) {
		throw std::runtime_error(std::string("Shader error: ") + e.what());
	}
}

Shader::~Shader() {
	if (mID)
		glDeleteProgram(mID);
}

Shader::Shader(Shader&& other) noexcept {
	mID = other.mID;
	other.mID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		glDeleteProgram(mID);
		mID = other.mID;
		other.mID = 0;
	}
	return *this;
}

void Shader::activate() const {
	glUseProgram(mID);
}

void Shader::setBool(const std::string& name, const bool value) const {
	glUniform1i(glGetUniformLocation(mID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, const int value) const {
	glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value) const {
	glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, const float x, const float y) const {
	glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const {
	glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const float x, const float y, const float z, const float w) const {
	glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

std::string Shader::loadFile(const char* sf) {
	std::ifstream file(fs::path(SHADER_DIR + sf));
	if (!file.is_open()) {
		throw std::runtime_error(std::string("Failed to open shader file: ") + sf);
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Shader::preprocess(std::string source, const char* fileName,
                               std::unordered_set<std::string>& includedFiles) {
	std::stringstream result;
	std::istringstream stream(source);
	std::string line;

	while (std::getline(stream, line)) {
		if (line.find("#include") == 0) {
			// Parse the include (e.g., #include "lighting.glsl")
			size_t start = line.find('"');
			size_t end = line.rfind('"');

			if (start != std::string::npos && end != std::string::npos && start != end) {
				std::string includeFile = line.substr(start + 1, end - start - 1);
				std::string fullPath = fs::path(SHADER_DIR + includeFile);

				// Prevent cyclic includes
				if (includedFiles.contains(fullPath)) {
					throw std::runtime_error(std::string("File already included: ") + includeFile + "\nin " + fileName);
				}
				includedFiles.insert(fullPath);
				// Load included file
				if (std::ifstream file(fullPath); file.is_open()) {
					std::stringstream includeSource;
					includeSource << file.rdbuf();
					// Recurse
					result << preprocess(includeSource.str(), fileName, includedFiles) << "\n";
				} else {
					// Handle error: file not found
					throw std::runtime_error(std::string("File not open:\n") + fullPath);
				}
			}
		} else {
			result << line << "\n";
		}
	}
	return result.str();
}

GLuint Shader::compileShader(std::string& source, const GLuint type) {
	const GLuint shader = glCreateShader(type);
	const char* code = source.c_str();

	glShaderSource(shader, 1, &code, nullptr);
	glCompileShader(shader);

	// Check compile errors
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		std::string infoLog;
		GLint maxLength = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		infoLog.resize(maxLength);

		glGetShaderInfoLog(shader, maxLength, nullptr, infoLog.data());

		// The program is useless now. So delete it.
		glDeleteShader(shader);

		throw std::runtime_error(std::string("Compilation error:") + "\n" + infoLog);
	}

	return shader;
}

GLuint Shader::linkShader(const GLuint vertex, const GLuint fragment, const GLuint geometry) {
	mID = glCreateProgram();

	glAttachShader(mID, vertex);
	glAttachShader(mID, fragment);

	if (geometry != 0)
		glAttachShader(mID, geometry);

	glLinkProgram(mID);

	// Check linking errors
	GLint success;
	glGetProgramiv(mID, GL_LINK_STATUS, &success);

	if (!success) {
		std::string infoLog;
		GLint maxLength = 0;

		glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &maxLength);
		infoLog.resize(maxLength);

		glGetProgramInfoLog(mID, maxLength, nullptr, infoLog.data());

		// The program is useless now. So delete it.
		glDeleteProgram(mID);

		throw std::runtime_error(std::string("Linking error:\n") + infoLog);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (geometry)
		glDeleteShader(geometry);

	return mID;
}
