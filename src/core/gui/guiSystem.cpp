#include "guiSystem.h"
#include "guiPanels.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/debug.hpp"
#include "../../ECS/components/directionalLight.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/instance.hpp"

GuiSystem::GuiSystem(SDL_Window* window, const SDL_GLContext gl_context) {
	RequireComponent<TransformComponent>(true);
	RequireComponent<PointLightComponent>(true);
	RequireComponent<SpotLightComponent>(true);
	RequireComponent<DirectionalLightComponent>(true);
	RequireComponent<DebugComponent>(true);
	// Initialize the ImGui context
	GuiBackend::init(window, gl_context, "#version 410");
}

GuiSystem::~GuiSystem() {
	// Clean up ImGui
	GuiBackend::shutdown();
}

void GuiSystem::update(const float dt) {
	updateFpsCounter(dt);
}

void GuiSystem::render(std::vector<PostEffect>& effects) const {
	// Start the Dear ImGui frame
	GuiBackend::newFrame();

	GuiPanels::renderGraphicsInfoPanel(mFPS);
	GuiPanels::renderPostProcessPanel(effects);

	for (const auto& entity: getSystemEntities()) {
		if (entity.hasComponent<InstanceComponent>()) continue;

		if (Ui::beginEntity(entity.name())) {
			GuiPanels::renderTransformPanel(entity);
			GuiPanels::renderDebugViewsPanel(entity);
			GuiPanels::renderLightPanel(entity);
			Ui::endEntity();
		}
	}

	//Render ImGui
	GuiBackend::renderFrame();
}

void GuiSystem::updateFpsCounter(const float dt) {
	mCurrentFrameCount++;

	mCurrentSeconds += dt;
	double elapsedSeconds = mCurrentSeconds - mPreviousSeconds;
	// limit text updates to 4 per second
	if (elapsedSeconds > 0.25) {
		mPreviousSeconds = mCurrentSeconds;
		mFPS = mCurrentFrameCount / elapsedSeconds;
		mCurrentFrameCount = 0;
	}
}

