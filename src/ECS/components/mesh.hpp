#pragma once
#include <vector>
#include <unordered_map>

class Mesh;
struct MeshComponent {
	using MeshMap = std::unordered_map<uint32_t, std::vector<Mesh>>;
	MeshMap* meshes;

	MeshComponent() = default;
	explicit MeshComponent(MeshMap* m) : meshes(m) {}
};