#pragma once
#include "glm/glm.hpp"

struct alignas(16) DirectionalLightComponent {
	glm::vec4 direction;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	DirectionalLightComponent() = default;

	explicit DirectionalLightComponent(
		const glm::vec4 dir,
		const glm::vec4 a,
		const glm::vec4 dif,
		const glm::vec4 s) : direction(dir),
		                     ambient(a),
		                     diffuse(dif),
		                     specular(s) {
	}
};
