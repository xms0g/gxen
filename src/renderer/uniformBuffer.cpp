#include "uniformBuffer.h"
#include "glad/glad.h"

UniformBuffer::UniformBuffer(const int size, const int binding) {
	glGenBuffers(1, &mUBO);
	bind();
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	unbind();
	glBindBufferRange(GL_UNIFORM_BUFFER, binding, mUBO, 0, size);
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

void UniformBuffer::configure(const unsigned int program, const unsigned int uniformBlockBinding,
                              const char* uniformBlockName) const {
	const unsigned int ubidx = glGetUniformBlockIndex(program, uniformBlockName);
	glUniformBlockBinding(program, ubidx, uniformBlockBinding);
}
