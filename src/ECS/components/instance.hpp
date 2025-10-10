#pragma once

#include <vector>
#include "glm/glm.hpp"

struct InstanceComponent {
	const std::vector<glm::vec3>* positions;

	InstanceComponent() = default;
	explicit InstanceComponent(const std::vector<glm::vec3>* p) : positions(p) {}
};
