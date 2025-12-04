#pragma once
#include "../material/material.hpp"

class Entity;

struct RenderGroup {
	const Entity* entity;
	std::vector<MaterialBatch> mbatch;
};
