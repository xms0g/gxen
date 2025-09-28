#include "shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../io/filesystem.hpp"
#include "../config/config.hpp"

Shader::Shader(const char* vs, const char* fs) {
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// open files
		vShaderFile.open(fs::path(SHADER_DIR + vs));
		fShaderFile.open(fs::path(SHADER_DIR + fs));
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure& e) {
		throw std::runtime_error(std::string("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: ") + e.what());
	}

	std::unordered_set<std::string> includedFiles{};
	vertexCode = preprocess(vertexCode, includedFiles);
	fragmentCode = preprocess(fragmentCode, includedFiles);
	const char* vs_str = vertexCode.c_str();
	const char* fs_str = fragmentCode.c_str();

	GLuint vertex, fragment;
	// vertex shader
	vertex = createShader(&vs_str, GL_VERTEX_SHADER);

	// fragment Shader
	fragment = createShader(&fs_str, GL_FRAGMENT_SHADER);

	linkShader(vertex, fragment);

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(mID);
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

std::string Shader::preprocess(std::string& source, std::unordered_set<std::string>& includedFiles) {
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
					continue;  // Or throw error
				}
				includedFiles.insert(fullPath);

				// Load included file
				std::ifstream file(fullPath);
				if (file.is_open()) {
					std::stringstream includeSource;
					includeSource << file.rdbuf();
					auto includeSourceStr = includeSource.str();
					// Recurse
					result << preprocess(includeSourceStr, includedFiles);
				} else {
					// Handle error: file not found
					throw std::runtime_error(std::string("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: ") + "\n" + fullPath);
				}
			}
		} else {
			result << line << "\n";
		}
	}
	return result.str();
}

GLuint Shader::createShader(const char** source, const GLuint type) {
    const GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, source, nullptr);
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

        throw std::runtime_error(std::string("ERROR::SHADER_COMPILATION_ERROR:") + "\n" + infoLog);
    }

    return shader;
}

GLuint Shader::linkShader(const GLuint vertex, const GLuint fragment) {
    mID = glCreateProgram();

    glAttachShader(mID, vertex);
    glAttachShader(mID, fragment);

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
        glDeleteShader(mID);

        throw std::runtime_error(std::string("ERROR::PROGRAM_LINKING_ERROR:") + "\n" + infoLog);
    }

    return mID;
}