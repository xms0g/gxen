#pragma once
#include <vector>
#include <array>

struct RenderGroup;
struct RenderItem;

namespace Models {
class SingleQuad;
}

class Entity;
class Shader;
class FrameBuffer;

class DeferredRenderer {
public:
	explicit DeferredRenderer();

	~DeferredRenderer();

	const Shader& getGShader() const;

	const Shader& getLightingShader() const;

	void geometryPass(const std::vector<RenderGroup>& groups) const;

	void lightingPass(const std::array<uint32_t, 3>& shadowMaps, const FrameBuffer& sceneBuffer) const;

private:
	std::unique_ptr<Models::SingleQuad> mQuad;
	std::unique_ptr<FrameBuffer> mGBuffer;
	std::unique_ptr<Shader> mGShader;
	std::unique_ptr<Shader> mLigthingShader;
};
