#pragma once

#include <vector>

class Mesh;
struct MeshComponent {
	const std::vector<Mesh>* meshes;
	bool isTwoSided;

	MeshComponent() = default;
	explicit MeshComponent(const std::vector<Mesh>* m, const bool its = false) : meshes(m), isTwoSided(its) {}
};