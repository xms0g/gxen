#include "guiPanels.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/debug.hpp"
#include "../../rendering/postProcess.h"

void GuiPanels::renderGraphicsInfoPanel(uint32_t fps) {
	if (ImGui::Begin("Graphics")) {
		ImGui::Text("%s FPS", std::to_string(fps).c_str());
		ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
		ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("OpenGL Driver Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("OpenGL Renderer: %s", glGetString(GL_RENDERER));
	}
	ImGui::End();
}

void GuiPanels::renderTransformPanel(const Entity& entity) {
	auto& tc = entity.getComponent<TransformComponent>();

	ImGui::PushID(static_cast<int>(entity.id()));
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Position");
		ImGui::SameLine(100);
		ImGui::DragFloat3("##pos", glm::value_ptr(tc.position), 0.01f);

		ImGui::Text("Rotation");
		ImGui::SameLine(100);
		ImGui::DragFloat3("##rot", glm::value_ptr(tc.rotation), 1.0f, -360.0f, 360.0f);

		ImGui::Text("Scale");
		ImGui::SameLine(100);
		ImGui::DragFloat3("##scale", glm::value_ptr(tc.scale));
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GuiPanels::renderDebugViewsPanel(const Entity& entity) {
	auto& db = entity.getComponent<DebugComponent>();

	ImGui::PushID(static_cast<int>(entity.id()));
	if (ImGui::TreeNodeEx("Debug Views", ImGuiTreeNodeFlags_DefaultOpen)) {
		const char* modes[] = {"None", "Normals", "Wireframe"};
		int currentMode = static_cast<int>(db.mode);
		ImGui::Text("Mode");
		ImGui::SameLine(70);
		if (ImGui::Combo("##mode", &currentMode, modes, IM_ARRAYSIZE(modes))) {
			db.mode = static_cast<DebugMode>(currentMode);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void GuiPanels::renderLightPanel(const Entity& entity) {
}

void GuiPanels::renderPostProcessPanel(std::vector<PostEffect>& effects) {
	if (ImGui::Begin("Post-Processing")) {
		for (auto& effect: effects) {
			ImGui::Checkbox(effect.name.c_str(), &effect.enabled);
		}
	}
	ImGui::End();
}
