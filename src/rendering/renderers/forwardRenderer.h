#pragma once
#include <vector>
#include <array>
#include <unordered_map>

struct InstanceGroup;
struct RenderItem;
struct Texture;
class Entity;
class Window;
class Camera;
class guiSystem;
class Shader;

class ForwardRenderer {
public:
	explicit ForwardRenderer();

	~ForwardRenderer();

	void configure(const std::vector<InstanceGroup>& opaqueInstancedGroup,
	               const std::vector<InstanceGroup>& blendInstancedGroup);

	void opaquePass(const std::unordered_map<const Shader*, std::vector<RenderItem>>& renderItems,
	                const std::array<uint32_t, 3>& shadowMaps) const;

	void transparentPass(const std::unordered_map<const Shader*, std::vector<RenderItem>>& renderItems) const;

	void opaqueInstancedPass(const std::vector<InstanceGroup>& instancedGroup, const std::array<uint32_t, 3>& shadowMaps);

	void transparentInstancedPass(const std::vector<InstanceGroup>& instancedGroup);

private:
	struct InstanceVBO {
		uint32_t buffer{};
		int offset{0};
	};

	void instancedPass(const std::vector<InstanceGroup>& instancedGroup);

	void prepareInstanceData(const std::vector<InstanceGroup>& instancedGroup, const InstanceVBO& vbo);

	void prepareInstanceBuffer(const std::vector<InstanceGroup>& instancedGroup, InstanceVBO& vbo);

	InstanceVBO mOpaqueInstanceVBO, mTransparentInstanceVBO;
};
