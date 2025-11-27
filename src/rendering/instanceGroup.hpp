#pragma once
#include "material/material.hpp"

class Shader;
class Mesh;

struct InstanceGroup {
	const Entity* entity;
	const std::vector<float>* transforms; // ptr to P,R,S data
	std::vector<MaterialBatch> mbatch;
};
