#pragma once
#include "material/material.hpp"

class Mesh;
class Entity;

struct RenderGroup {
	const Entity* entity;
	std::vector<MaterialBatch> mbatch;
};
