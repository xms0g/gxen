#pragma once
#include <utility>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace math {
inline std::pair<glm::mat4, glm::mat3> computeModelMatrices(const glm::vec3& position, const glm::vec3& rotation,
                                                            const glm::vec3& scale) {
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model *= glm::toMat4(glm::quat(glm::radians(rotation)));
	model *= glm::scale(glm::mat4(1.0f), scale);

	const glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(model)));
	return {model, normal};
}
}
