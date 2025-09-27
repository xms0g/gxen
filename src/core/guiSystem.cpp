#include "guiSystem.h"
#include <string>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_impl_sdl.h"
#include "../../libs/imgui/imgui_impl_opengl3.h"
#include "../../libs/imgui/imgui_internal.h"
#include "../ECS/components/transform.hpp"

GuiSystem::GuiSystem(SDL_Window* window, SDL_GLContext gl_context) {
	RequireComponent<TransformComponent>(true);
	// Initialize the ImGui context
	const char* glsl_version = "#version 410";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

GuiSystem::~GuiSystem() {
	// Clean up ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void GuiSystem::update(const float dt) {
	updateFpsCounter(dt);
}

void GuiSystem::render() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

	renderGraphicsInfo();
	renderTransform();

	//Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void GuiSystem::renderGraphicsInfo() const {
	if (ImGui::Begin("Graphics")) {
		ImGui::Text("%s FPS", std::to_string(mFPS).c_str());
		ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
		ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("OpenGL Driver Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("OpenGL Renderer: %s", glGetString(GL_RENDERER));
	}
	ImGui::End();
}

void GuiSystem::renderTransform() {
	for (auto entity: getSystemEntities()) {
		auto& tc = entity.getComponent<TransformComponent>();

		ImGui::PushID(static_cast<int>(entity.id()));

		ImGui::Text("Entity %lu", entity.id());
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Position");ImGui::SameLine(80);
			ImGui::DragFloat3("##pos", glm::value_ptr(tc.position), 0.01f);

			ImGui::Text("Rotation");ImGui::SameLine(80);
			ImGui::DragFloat3("##rot", glm::value_ptr(tc.rotation), 1.0f, -360.0f, 360.0f);

			ImGui::Text("Scale");ImGui::SameLine(80);
			ImGui::DragFloat3("##scale", glm::value_ptr(tc.scale));

			ImGui::Separator();
		}
		ImGui::PopID();
	}
}