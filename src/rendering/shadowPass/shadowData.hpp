#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct ShadowData {
	uint32_t shadowMap;
	uint32_t shadowCubemap;
	glm::mat4 lightSpaceMatrix;
	float omniFarPlane;
};