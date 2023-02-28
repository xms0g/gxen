#pragma once

#include <filesystem>
#include <string>

struct filesystem {
    static std::string path(const std::string& p) {
        auto cwd = std::filesystem::current_path().string();
        cwd.erase(cwd.find_last_of('/'));
        return cwd + "/" + p;
    }
};
