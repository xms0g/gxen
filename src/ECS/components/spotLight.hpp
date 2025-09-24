#pragma once

#include "glm/glm.hpp"

struct SpotLightComponent {
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float Kc;
	float Kl;
	float Kq;

	SpotLightComponent() = default;

	explicit SpotLightComponent(
		const glm::vec3 pos,
		const glm::vec3 dir,
		const float coff,
		const float ocoff,
		const glm::vec3 a,
		const glm::vec3 dif,
		const glm::vec3 s,
		const float c,
		const float l,
		const float q) : position(pos),
		           direction(dir),
		           cutOff(coff),
		           outerCutOff(ocoff),
		           ambient(a),
		           diffuse(dif),
		           specular(s),
		           Kc(c),
		           Kl(l),
		           Kq(q) {
	}
};
