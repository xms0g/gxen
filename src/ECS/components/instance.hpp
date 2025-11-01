#pragma once
#include <vector>
#include "glm/glm.hpp"

struct InstanceComponent {
	std::vector<glm::vec3>* positions;

	InstanceComponent() = default;
	explicit InstanceComponent(std::vector<glm::vec3>* p) : positions(p) {}
};
