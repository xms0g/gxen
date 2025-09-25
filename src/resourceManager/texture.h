#pragma once

#include <vector>
#include <string>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

namespace texture {
GLuint load(const char* path);

GLuint loadCubemap(const std::vector<std::string>& faces);
}
