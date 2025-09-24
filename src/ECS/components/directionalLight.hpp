#pragma once

#include "glm/glm.hpp"


struct DirectionalLightComponent {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	DirectionalLightComponent() = default;

	explicit DirectionalLightComponent(
		const glm::vec3 dir,
		const glm::vec3 a,
		const glm::vec3 dif,
		const glm::vec3 s) : direction(dir),
		                     ambient(a),
		                     diffuse(dif),
		                     specular(s) {
	}
};
