#pragma once

struct Material;
class Mesh;
class Entity;

struct RenderItem {
	const Entity* entity;
	Mesh* mesh;
	const Material* material;
};
