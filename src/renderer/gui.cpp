#include "gui.h"
#include <string>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_impl_sdl.h"
#include "../../libs/imgui/imgui_impl_opengl3.h"
#include "../../libs/imgui/imgui_internal.h"

Gui::Gui(SDL_Window* window, SDL_GLContext gl_context) {
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

Gui::~Gui() {
	// Clean up ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void Gui::render() {
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

void Gui::updateFpsCounter(const float dt) {
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

void Gui::updateTransform(glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) const {
	position = mPosition;
	rotation = mRotation;
	scale = mScale;
}

void Gui::renderGraphicsInfo() const {
	if (ImGui::Begin("Graphics")) {
		ImGui::Text("%s FPS", std::to_string(mFPS).c_str());
		ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
		ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text("OpenGL Driver Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("OpenGL Renderer: %s", glGetString(GL_RENDERER));
	}
	ImGui::End();
}

void Gui::renderTransform() {
	static glm::vec3 _position{0.0}, _rotation{0.0}, _scale{1.0};

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Position");
		ImGui::DragFloat3("p", &_position.x, 0.01f);

		ImGui::Text("Rotation");
		ImGui::DragFloat3("r", &_rotation.x, 1.0f, -360.0f, 360.0f);

		ImGui::Text("Scale");
		ImGui::DragFloat3("s", &_scale.x);

		mPosition = _position;
		mRotation = _rotation;
		mScale = _scale;
	}
}