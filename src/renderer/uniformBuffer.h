#pragma once

#include <cstdlib>

class UniformBuffer {
public:
	explicit UniformBuffer(size_t size);

	~UniformBuffer();

	void bind() const;

	void unbind() const;

	void setData(const void* data, size_t size, size_t offset = 0);

	void configure(unsigned int program, unsigned int uniformBlockBinding,  const char* uniformBlockName);

private:
	unsigned int mUBO;
};