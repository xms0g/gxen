#pragma once
#include <memory>

namespace math {
class BoundingVolume;
}

struct BoundingVolumeComponent {
	std::shared_ptr<math::BoundingVolume> bv;
	bool isVisible{false};

	BoundingVolumeComponent() = default;
	explicit BoundingVolumeComponent(const std::shared_ptr<math::BoundingVolume>& b) : bv(b) {}
};
