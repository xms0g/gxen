#pragma once

#include <vector>
#include <string>
#include "glad/glad.h"

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

namespace texture {
GLuint load(const char* path);

GLuint loadCubemap(std::vector<std::string>& faces);
}
