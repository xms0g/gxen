#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct ShadowData {
	struct IShadow {
		uint32_t shadowMap;
	};

	struct DirShadow : IShadow {
		glm::mat4 lightSpaceMatrix;

		DirShadow(const uint32_t shadowMap, const glm::mat4& lightSpaceMatrix) :
			IShadow(shadowMap),
			lightSpaceMatrix(lightSpaceMatrix) {
		}
	};

	struct PointShadow : IShadow {
		float farPlane;

		PointShadow(const uint32_t shadowMap, const float farPlane) :
			IShadow(shadowMap),
			farPlane(farPlane) {
		}
	};

	std::vector<DirShadow> dirShadows;
	std::vector<PointShadow> pointShadows;
};
