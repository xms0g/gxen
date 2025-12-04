#pragma once
#include <vector>

struct InstanceGroup;
struct RenderGroup;
class Entity;

struct RenderQueue {
	std::vector<InstanceGroup> opaqueInstancedGroups;
	std::vector<InstanceGroup> blendInstancedGroups;
	std::vector<RenderGroup> debugGroups;
	std::vector<RenderGroup> shadowCasterGroups;
	std::vector<RenderGroup> forwardOpaqueGroups;
	std::vector<RenderGroup> deferredGroups;
	std::vector<RenderGroup> blendGroups;
	const Entity* skyboxEntity{};
};