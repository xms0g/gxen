#pragma once

#include <vector>

class Mesh;
struct MeshComponent {
	const std::vector<Mesh>* meshes;

	MeshComponent() = default;
	explicit MeshComponent(const std::vector<Mesh>* m) : meshes(m) {}
};