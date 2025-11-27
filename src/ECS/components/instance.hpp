#pragma once
#include <vector>

struct InstanceComponent {
	std::vector<float>* transforms{};

	InstanceComponent() = default;
	explicit InstanceComponent(std::vector<float>* t) : transforms(t) {}
};
