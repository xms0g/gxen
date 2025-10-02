#include "uniformBuffer.h"
#include "glad/glad.h"

UniformBuffer::UniformBuffer(const size_t size) {
	glGenBuffers(1, &mUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUBO, 0, size);
}

UniformBuffer::~UniformBuffer() {
	glDeleteBuffers(1, &mUBO);
}

void UniformBuffer::bind() const {
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);

}

void UniformBuffer::unbind() const {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::setData(const void* data, const size_t size, const size_t offset) {
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UniformBuffer::configure(const unsigned int program, const unsigned int uniformBlockBinding, const char* uniformBlockName) {
	const unsigned int ubidx = glGetUniformBlockIndex(program, uniformBlockName);
	glUniformBlockBinding(program, ubidx, uniformBlockBinding);
}
