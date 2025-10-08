#pragma once

#include <cstdlib>

class UniformBuffer {
public:
	explicit UniformBuffer(int size, int binding);

	~UniformBuffer();

	void bind() const;

	void unbind() const;

	void setData(const void* data, size_t size, size_t offset = 0);

	void configure(unsigned int program, unsigned int uniformBlockBinding, const char* uniformBlockName) const;

private:
	uint32_t mUBO{};
};
