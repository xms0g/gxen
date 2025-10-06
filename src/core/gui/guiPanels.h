#pragma once

#include <vector>

struct PostEffect;
class Entity;

namespace  GuiPanels {
void renderGraphicsInfoPanel(uint32_t fps);

void renderTransformPanel(const Entity& entity);

void renderDebugViewsPanel(const Entity& entity);

void renderLightPanel(const Entity& entity);

void renderPostProcessPanel(std::vector<PostEffect>& effects);

}
