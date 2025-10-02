#pragma once

#include "glm/glm.hpp"

struct alignas(16) SpotLightComponent {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 cutOff; // (cutOff, outerCutOff, Kc, Kl)
	glm::vec4 attenuation; // (Kq, padding, padding, padding)

	SpotLightComponent() = default;

	explicit SpotLightComponent(
		const glm::vec4 pos,
		const glm::vec4 dir,
		const glm::vec4 a,
		const glm::vec4 dif,
		const glm::vec4 s,
		const glm::vec4 cut,
		const glm::vec4 att) : position(pos),
		                       direction(dir),
		                       ambient(a),
		                       diffuse(dif),
		                       specular(s),
		                       cutOff(cut),
		                       attenuation(att) {
	}
};
