#pragma once

#include "glm/glm.hpp"

struct PointLightComponent {
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float Kc;
	float Kl;
	float Kq;

	PointLightComponent() = default;

	explicit PointLightComponent(
		const glm::vec3 pos,
		const glm::vec3 a,
		const glm::vec3 dif,
		const glm::vec3 s,
		const float c,
		const float l,
		const float q) : position(pos),
		                 ambient(a),
		                 diffuse(dif),
		                 specular(s),
		                 Kc(c),
		                 Kl(l),
		                 Kq(q) {
	}
};
