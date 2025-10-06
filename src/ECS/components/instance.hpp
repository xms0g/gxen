#pragma once

#include <vector>
#include "glm/glm.hpp"

struct InstanceComponent {
	const std::vector<glm::vec3>* positions;
	int instancedCount;

	InstanceComponent() = default;
	explicit InstanceComponent(const std::vector<glm::vec3>* p, const int c) : positions(p), instancedCount(c) {}
};
