#include "boundingVolume.h"
#include <algorithm>
#include "frustum.hpp"
#include "matrix.hpp"
#include "../mesh/mesh.h"

bool math::Sphere::isOnFrustum(const Frustum& camFrustum,
                               const glm::vec3& position,
                               const glm::vec3& rotation,
                               const glm::vec3& scale) const {
	const glm::mat4 modelMatrix = computeModelMatrix(position, rotation, scale);
	const glm::vec3 globalScale = {
		glm::length(modelMatrix[0]),
		glm::length(modelMatrix[1]),
		glm::length(modelMatrix[2])
	};

	//Get our global center with process it with the global model matrix of our transform
	const glm::vec3 globalCenter{modelMatrix * glm::vec4(mCenter, 1.f)};

	//To wrap correctly our shape, we need the maximum scale scalar.
	const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

	//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
	const Sphere globalSphere(globalCenter, mRadius * (maxScale * 0.5f));

	//Check Firstly the result that have the most chance to failure to avoid to call all functions.
	return globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
	       globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
	       globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
	       globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
	       globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
	       globalSphere.isOnOrForwardPlane(camFrustum.bottomFace);
}

bool math::Sphere::isOnOrForwardPlane(const Plane& plane) const {
	return plane.computeSignedDistanceToPlan(mCenter) > -mRadius;
}

bool math::AABB::isOnFrustum(const Frustum& camFrustum,
                             const glm::vec3& position,
                             const glm::vec3& rotation,
                             const glm::vec3& scale) const {
	const glm::mat4 modelMatrix = computeModelMatrix(position, rotation, scale);
	const glm::vec3 globalCenter{modelMatrix * glm::vec4(center, 1.f)};
	// Scaled orientation
	const glm::vec3 right = modelMatrix[0] * extents.x;
	const glm::vec3 up = modelMatrix[1] * extents.y;
	const glm::vec3 forward = -modelMatrix[2] * extents.z;

	const float newIi = std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, right)) +
	                    std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, up)) +
	                    std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, right)) +
	                    std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, up)) +
	                    std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, right)) +
	                    std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, up)) +
	                    std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, forward));

	const AABB globalAABB(globalCenter, newIi, newIj, newIk);

	return globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
	       globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
	       globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
	       globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
	       globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
	       globalAABB.isOnOrForwardPlane(camFrustum.farFace);
}

bool math::AABB::isOnOrForwardPlane(const Plane& plane) const {
	const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
					extents.z * std::abs(plane.normal.z);

	return -r <= plane.computeSignedDistanceToPlan(center);
}

math::Sphere math::generateSphereBV(const std::unordered_map<uint32_t, std::vector<Mesh>>& meshesByMatID) {
	auto minAABB = glm::vec3(std::numeric_limits<float>::max());
	auto maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto& [matID, meshes]: meshesByMatID) {
		for (auto& mesh: meshes) {
			for (auto& vertex: mesh.vertices()) {
				minAABB.x = std::min(minAABB.x, vertex.position.x);
				minAABB.y = std::min(minAABB.y, vertex.position.y);
				minAABB.z = std::min(minAABB.z, vertex.position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.position.z);
			}
		}
	}

	return {(maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB)};
}

math::AABB math::generateAABB(const std::unordered_map<uint32_t, std::vector<Mesh>>& meshesByMatID) {
	auto minAABB = glm::vec3(std::numeric_limits<float>::max());
	auto maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto& [matID, meshes]: meshesByMatID) {
		for (auto& mesh: meshes) {
			for (auto& vertex: mesh.vertices()) {
				minAABB.x = std::min(minAABB.x, vertex.position.x);
				minAABB.y = std::min(minAABB.y, vertex.position.y);
				minAABB.z = std::min(minAABB.z, vertex.position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.position.z);
			}
		}
	}

	return {minAABB, maxAABB};
}
