#pragma once
#include "glm/glm.hpp"

struct alignas(16) PointLightComponent {
	glm::vec4 position;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 attenuation;

	PointLightComponent() = default;

	explicit PointLightComponent(
		const glm::vec4 pos,
		const glm::vec4 a,
		const glm::vec4 dif,
		const glm::vec4 s,
		const glm::vec4 att) : position(pos),
		                       ambient(a),
		                       diffuse(dif),
		                       specular(s),
		                       attenuation(att) {
	}
};
