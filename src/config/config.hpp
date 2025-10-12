#pragma once
#include <string>
#include <cstdint>

#ifndef DEBUG
#define DEBUG
#endif

const std::string SHADER_DIR = "shaders/";
const std::string ASSET_DIR = "assets/";

// Window Settings
inline uint32_t SCR_WIDTH{0};
inline uint32_t SCR_HEIGHT{0};

// Camera Settings
constexpr float YAW{-90.0f};
constexpr float PITCH{0.0f};
constexpr float SPEED{10.0f};
constexpr float SENSITIVITY{0.1f};
constexpr float ZOOM{45.0f};
constexpr float ZNEAR{0.1f};
constexpr float ZFAR{100.0f};

// Render Settings
constexpr uint32_t MULTISAMPLED_COUNT{4};
constexpr uint32_t SHADOW_WIDTH{1024};
constexpr uint32_t SHADOW_HEIGHT{1024};
constexpr uint32_t SHADOWMAP_TEXTURE_SLOT{4};
constexpr float SHADOW_DIRECTIONAL_NEAR{1.0f};
constexpr float SHADOW_DIRECTIONAL_FAR{7.5f};
constexpr float SHADOW_DIRECTIONAL_LEFT{-10.0f};
constexpr float SHADOW_DIRECTIONAL_RIGHT{10.0f};
constexpr float SHADOW_DIRECTIONAL_BOTTOM{-10.0f};
constexpr float SHADOW_DIRECTIONAL_TOP{10.0f};
constexpr float SHADOW_OMNIDIRECTIONAL_NEAR{1.0f};
constexpr float SHADOW_OMNIDIRECTIONAL_FAR{25.0f};
constexpr float SHADOW_OMNIDIRECTIONAL_FOVY{90.0f};
constexpr float SHADOW_PERSPECTIVE_NEAR{1.0f};
constexpr float SHADOW_PERSPECTIVE_FAR{25.0f};

