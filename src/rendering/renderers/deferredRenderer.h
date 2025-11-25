#pragma once
#include <vector>
#include <unordered_map>

struct RenderItem;

namespace Models {
class SingleQuad;
}

class Entity;
class Shader;
class FrameBuffer;

class DeferredRenderer {
public:
	explicit DeferredRenderer(const Shader& lightingShader);

	~DeferredRenderer();

	void geometryPass(const std::unordered_map<const Shader*, std::vector<RenderItem>>& renderItems, const FrameBuffer& gBuffer,
	                  const Shader& gShader) const;

	void lightingPass(const std::array<uint32_t, 3>& shadowMaps, const FrameBuffer& gBuffer,
	                  const FrameBuffer& sceneBuffer, const Shader& lightingShader) const;

private:
	std::unique_ptr<Models::SingleQuad> mQuad;
};
