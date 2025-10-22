#include "guiBackend.h"

#include <string>

#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

void GuiBackend::init(SDL_Window* window, const SDL_GLContext context, const char* glsl_version) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void GuiBackend::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void GuiBackend::newFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void GuiBackend::renderFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Ui::beginEntity(const char* label) {
	return ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);
}

void Ui::endEntity() {
	ImGui::Separator();
}

void Ui::colorField4(const char* label, glm::vec4& value, const float speed, const float sameLineOffset) {
	ImGui::Text("%s", label);
	ImGui::SameLine(sameLineOffset);
	ImGui::DragFloat3(("##" + std::string(label) + "v").c_str(), glm::value_ptr(value), speed);
	ImGui::SameLine();
	ImGui::ColorEdit3(("##" + std::string(label) + "c").c_str(), glm::value_ptr(value),
	                  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
}

void Ui::dragFloat3(const char* label, glm::vec3& value, const float speed, const float sameLineOffset) {
	ImGui::Text("%s", label);
	ImGui::SameLine(sameLineOffset);
	ImGui::DragFloat3(("##" + std::string(label) + "v").c_str(), glm::value_ptr(value), speed);
}

void Ui::dragFloat4(const char* label, glm::vec4& value, const float speed, const float sameLineOffset) {
	ImGui::Text("%s", label);
	ImGui::SameLine(sameLineOffset);
	ImGui::DragFloat4(("##" + std::string(label) + "v").c_str(), glm::value_ptr(value), speed);
}

void Ui::sliderFloat(const char* label, float* value, const float sameLineOffset) {
	ImGui::Text("%s", label);
	ImGui::SameLine(sameLineOffset);
	ImGui::SliderFloat(("##" + std::string(label) + "v").c_str(), value, 0.0f, 10.0f);
}
