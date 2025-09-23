#pragma once

#include <vector>

class Model;
struct ModelComponent {
	std::shared_ptr<Model> model;

	ModelComponent() = default;
	explicit ModelComponent(const std::shared_ptr<Model>& m) : model(m) {}
};