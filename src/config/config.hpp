#pragma once

#include <string>
#include <cstdint>

#ifndef DEBUG
#define DEBUG
#endif

const std::string SHADER_DIR = "shaders/";
const std::string ASSET_DIR = "assets/";

// Window Settings
constexpr uint32_t SCR_WIDTH = 1600;
constexpr uint32_t SCR_HEIGHT = 900;
constexpr float ASPECT = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);

// Camera Settings
constexpr float YAW{-90.0f};
constexpr float PITCH{0.0f};
constexpr float SPEED{10.0f};
constexpr float SENSITIVITY{0.1f};
constexpr float ZOOM{45.0f};
constexpr float ZNEAR{0.1f};
constexpr float ZFAR{100.0f};