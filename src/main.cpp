#include <iostream>
#include <array>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include "core/Components.h"
#include "core/SceneCamera.h"
#include "core/Input.h"
#include "renderer/Renderer.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "renderer/CubeMap.h"
#include "renderer/Material.h"
#include "core/Primatives.h"
#include "ecs/Registry.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include "imgui/imgui_impl_opengl3.h"

void DrawMenuBar();
void SetTheme();
void SetupEnviroment();

int main() {
	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw\n";
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	auto window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to load OpenGL\n";
		return -1;
	}

	Input::Init(window);
	Renderer::Init();

	SetupEnviroment();

	SceneCamera camera;

	//Material mat = Material("Assets/Materials/Backpack.mat");
	//auto modelEntity = InstantiateModel("Assets/Model/Survival_BackPack_2.fbx", &mat);

	Material polygonCityMat = Material("Assets/Materials/PolygonCity.mat");
	auto buildingEntity = InstantiateModel("Assets/Model/City.fbx", &polygonCityMat);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_VERSION_4_6));

	SetTheme();

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		camera.Update();


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::RenderScene(camera);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawMenuBar();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void SetupEnviroment() {
	Enviroment enviorment;

	enviorment.skyboxMaterial = std::make_shared<Material>("Assets/Materials/Skybox.mat");
	enviorment.ambientColor = glm::vec3(1.0, 1.0, 1.0);
	enviorment.ambientStrength = 0.5;

	Renderer::SetEnviroment(enviorment);
}

void DrawMenuBar() {
	ImGui::BeginMainMenuBar();
	if (ImGui::Button("Save")) {
	}
	if (ImGui::Button("Create New Scene")) {
	}
	if (ImGui::Button("Load Scene")) {
	}
	if (ImGui::Button("Prefabs")) {
	}
	ImGui::EndMainMenuBar();
}

void SetTheme() {
	auto& style = ImGui::GetStyle();
	style.ChildRounding = 0;
	style.GrabRounding = 0;
	style.FrameRounding = 0;
	style.PopupRounding = 0;
	style.ScrollbarRounding = 0;
	style.TabRounding = 0;
	style.WindowRounding = 0;
	style.FramePadding = { 4, 4 };

	style.WindowTitleAlign = { 0.0, 0.5 };
	style.ColorButtonPosition = ImGuiDir_Left;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.00f };
	colors[ImGuiCol_TextDisabled] = { 0.25f, 0.25f, 0.25f, 1.00f };
	colors[ImGuiCol_WindowBg] = { 0.09f, 0.09f, 0.09f, 1.00f };
	colors[ImGuiCol_ChildBg] = { 0.11f, 0.11f, 0.11f, 1.00f };
	colors[ImGuiCol_PopupBg] = { 0.11f, 0.11f, 0.11f, 0.94f };
	colors[ImGuiCol_Border] = { 0.07f, 0.08f, 0.08f, 1.00f };
	colors[ImGuiCol_BorderShadow] = { 0.00f, 0.00f, 0.00f, 0.00f };
	colors[ImGuiCol_FrameBg] = { 0.35f, 0.35f, 0.35f, 0.54f };
	colors[ImGuiCol_FrameBgHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
	colors[ImGuiCol_FrameBgActive] = { 0.40f, 0.36f, 0.33f, 0.67f };
	colors[ImGuiCol_TitleBg] = { 0.1f, 0.1f, 0.1f, 1.00f };
	colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f, 0.3f, 1.00f };
	colors[ImGuiCol_TitleBgCollapsed] = { 0.0f, 0.0f, 0.0f, 0.61f };
	colors[ImGuiCol_MenuBarBg] = { 0.18f, 0.18f, 0.18f, 0.94f };
	colors[ImGuiCol_ScrollbarBg] = { 0.00f, 0.00f, 0.00f, 0.16f };
	colors[ImGuiCol_ScrollbarGrab] = { 0.24f, 0.22f, 0.21f, 1.00f };
	colors[ImGuiCol_ScrollbarGrabHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
	colors[ImGuiCol_ScrollbarGrabActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
	colors[ImGuiCol_CheckMark] = { 0.84f, 0.84f, 0.84f, 1.0f };
	colors[ImGuiCol_SliderGrab] = { 0.8f, 0.8f, 0.8f, 1.0f };
	colors[ImGuiCol_SliderGrabActive] = { 0.55f, 0.55f, 0.55f, 1.00f };
	colors[ImGuiCol_Button] = { 0.55f, 0.55f, 0.55f, 0.40f };
	colors[ImGuiCol_ButtonHovered] = { 0.15f, 0.15f, 0.15f, 0.62f };
	colors[ImGuiCol_ButtonActive] = { 0.60f, 0.60f, 0.60f, 1.00f };
	colors[ImGuiCol_Header] = { 0.84f, 0.36f, 0.05f, 0.0f };
	colors[ImGuiCol_HeaderHovered] = { 0.25f, 0.25f, 0.25f, 0.80f };
	colors[ImGuiCol_HeaderActive] = { 0.42f, 0.42f, 0.42f, 1.00f };
	colors[ImGuiCol_Separator] = { 0.35f, 0.35f, 0.35f, 0.50f };
	colors[ImGuiCol_SeparatorHovered] = { 0.31f, 0.29f, 0.27f, 0.78f };
	colors[ImGuiCol_SeparatorActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
	colors[ImGuiCol_ResizeGrip] = { 1.0f, 1.0f, 1.0f, 0.25f };
	colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.0f, 1.0f, 0.4f };
	colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.0f, 0.95f };
	colors[ImGuiCol_Tab] = { 0.18f, 0.18f, 0.18f, 1.0f };
	colors[ImGuiCol_TabHovered] = { 0.58f, 0.58f, 0.58f, 0.80f };
	colors[ImGuiCol_TabActive] = { 0.6f, 0.60f, 0.60f, 1.00f };
	colors[ImGuiCol_TabUnfocused] = { 0.07f, 0.10f, 0.15f, 0.97f };
	colors[ImGuiCol_TabUnfocusedActive] = { 0.14f, 0.26f, 0.42f, 1.00f };
	colors[ImGuiCol_PlotLines] = { 0.66f, 0.60f, 0.52f, 1.00f };
	colors[ImGuiCol_PlotLinesHovered] = { 0.98f, 0.29f, 0.20f, 1.00f };
	colors[ImGuiCol_PlotHistogram] = { 0.60f, 0.59f, 0.10f, 1.00f };
	colors[ImGuiCol_PlotHistogramHovered] = { 0.72f, 0.73f, 0.15f, 1.00f };
	colors[ImGuiCol_TextSelectedBg] = { 0.27f, 0.52f, 0.53f, 0.35f };
	colors[ImGuiCol_DragDropTarget] = { 0.60f, 0.59f, 0.10f, 0.90f };
	colors[ImGuiCol_NavHighlight] = { 0.51f, 0.65f, 0.60f, 1.00f };
	colors[ImGuiCol_NavWindowingHighlight] = { 1.00f, 1.00f, 1.00f, 0.70f };
	colors[ImGuiCol_NavWindowingDimBg] = { 0.80f, 0.80f, 0.80f, 0.20f };
	colors[ImGuiCol_ModalWindowDimBg] = { 0.11f, 0.13f, 0.13f, 0.35f };
}
