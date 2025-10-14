#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct ShadowData {
	struct IShadow {
		uint32_t shadowMap;
	};

	struct DirShadow: IShadow {
		glm::mat4 lightSpaceMatrix;

		DirShadow() = default;
		DirShadow(const glm::mat4& lsm, const uint32_t shadowMap):
			IShadow{shadowMap},
			lightSpaceMatrix{lsm}
		{}
	};

	struct OmnidirShadow: IShadow {
		float farPlane;

		OmnidirShadow(const float far, const uint32_t shadowMap):
			IShadow{shadowMap},
			farPlane{far}
		{}
	};

	struct PersShadow: DirShadow {
		PersShadow(const glm::mat4& lsm, const uint32_t shadowMap): DirShadow{lsm, shadowMap} {}
	};

	DirShadow dirShadow;
	std::vector<OmnidirShadow> omnidirShadows;
	std::vector<PersShadow> persShadows;
};