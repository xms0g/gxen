#pragma once
#include <vector>
#include <string>

struct Texture {
    uint32_t id;
    std::string type;
    std::string path;
};

namespace texture {
uint32_t load(const char* path);

uint32_t loadCubemap(const std::vector<std::string>& faces);

int info(const char* path);
}
