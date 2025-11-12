#pragma once
#include <vector>
#include <unordered_map>

namespace Models {
class Quad;
}

class Entity;
class Shader;
class FrameBuffer;

class DeferredRenderer {
public:
	explicit DeferredRenderer(const Shader& lightingShader);

	~DeferredRenderer();

	void geometryPass(std::unordered_map<Shader*, std::vector<Entity> >& opaqueBatches,
	                  const FrameBuffer& gBuffer,
	                  const Shader& gShader) const;

	void lightingPass(const std::array<uint32_t, 3>& shadowMaps,
	                  const FrameBuffer& gBuffer,
	                  const FrameBuffer& sceneBuffer,
	                  const Shader& lightingShader) const;

private:
	std::unique_ptr<Models::Quad> mQuad;
};
