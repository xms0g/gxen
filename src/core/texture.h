#pragma once

#include <vector>
#include <string>
#include "glad/glad.h"

namespace texture {
GLuint load(const char* path);

GLuint loadCubemap(std::vector<std::string>& faces);
}
