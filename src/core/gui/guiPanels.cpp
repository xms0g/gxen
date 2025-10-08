#include "guiPanels.h"
#include "guiBackend.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "../../ECS/registry.h"
#include "../../ECS/components/transform.hpp"
#include "../../ECS/components/debug.hpp"
#include "../../ECS/components/pointLight.hpp"
#include "../../ECS/components/spotLight.hpp"
#include "../../rendering/postProcess.h"

void GuiPanels::renderGraphicsInfoPanel(const uint32_t fps) {
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
	if (!entity.hasComponent<TransformComponent>()) return;

	auto& tc = entity.getComponent<TransformComponent>();

	ImGui::PushID(static_cast<int>(entity.id()));
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		Ui::dragFloat3("Position", tc.position);
		Ui::dragFloat3("Rotation", tc.rotation, 1.0f);
		Ui::dragFloat3("Scale", tc.scale);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void GuiPanels::renderDebugViewsPanel(const Entity& entity) {
	if (!entity.hasComponent<DebugComponent>()) return;

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
	if (!entity.hasComponent<SpotLightComponent>() && !entity.hasComponent<PointLightComponent>()) return;

	ImGui::PushID(static_cast<int>(entity.id()));
	if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (entity.hasComponent<SpotLightComponent>()) {
			renderSpotLight(entity);
		} else if (entity.hasComponent<PointLightComponent>()) {
			renderPointLight(entity);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void GuiPanels::renderSpotLight(const Entity& entity) {
	auto& splc = entity.getComponent<SpotLightComponent>();

	Ui::colorField3("Direction", splc.direction, 0.01f, 100);
	Ui::colorField3("Ambient", splc.ambient, 0.01f, 100);
	Ui::colorField3("Diffuse", splc.diffuse, 0.01f, 100);
	Ui::colorField3("Specular", splc.specular, 0.01f, 100);
	ImGui::Text("Cutoff");
	ImGui::SameLine(100);
	ImGui::DragFloat4("##Cutoff", glm::value_ptr(splc.cutOff));

	ImGui::Text("Attenua");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##att", glm::value_ptr(splc.attenuation));
}

void GuiPanels::renderPointLight(const Entity& entity) {
	auto& plc = entity.getComponent<PointLightComponent>();

	Ui::colorField3("Ambient", plc.ambient, 0.01f, 100);
	Ui::colorField3("Diffuse", plc.diffuse, 0.01f, 100);
	Ui::colorField3("Specular", plc.specular, 0.01f, 100);
	ImGui::Text("Attenua");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##att", glm::value_ptr(plc.attenuation));
}

void GuiPanels::renderPostProcessPanel(std::array<PostEffect, 7>& effects) {
	if (ImGui::Begin("Post-Processing")) {
		for (auto& effect: effects) {
			ImGui::Checkbox(effect.name.c_str(), &effect.enabled);
		}
	}
	ImGui::End();
}
