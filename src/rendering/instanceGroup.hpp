#pragma once

class Shader;
class Mesh;
struct Material;

struct MaterialBatch {
	const Material* material{};
	const Shader* shader{};
	std::vector<const Mesh*> meshes;
};

struct InstanceGroup {
	const Entity* entity;
	const std::vector<float>* transforms; // ptr to P,R,S data
	std::vector<MaterialBatch> mbatch;
};
