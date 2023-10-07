#include <string>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include "imgui/imgui_impl_opengl3.h"
#include "ecs/Entity.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "core/Components.h"
#include "core/Serializer.h"
#include "core/CameraSystem.h"
#include "renderer/Renderer.h"
#include "SceneCamera.h"
#include "Editor.h"

#include <iostream>

void Editor::Init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_VERSION_4_6));

	s_SceneFrameBuffer = FrameBuffer(glm::i32vec2(600, 800), FrameBuffer::RGB);
	Renderer::s_FrameBuffer = &s_SceneFrameBuffer;

	CameraSystem::SetActiveCamera(&SceneCamera::s_Camera, &SceneCamera::s_Transform);
}

void Editor::Update() {
	DrawEditor();
}

void Editor::DrawEditor() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	DrawMenuBar();
	DrawScene();
	DrawWorld();
	DrawInspector();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupCurrentContext);
	}
}

void Editor::DrawMenuBar() {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		ImGui::MenuItem("Save");
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void Editor::DrawScene() {
	ImGui::Begin("Scene");
	ImGui::BeginChild("Scene Frame Buffer");
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const glm::i32vec2 sceneWindowSize(windowSize.x, windowSize.y);

	const ImVec2 sceneWindowPos = ImGui::GetCursorScreenPos();
	const ImVec2 mousePos = ImGui::GetMousePos();

	const glm::vec2 minScreenPos(sceneWindowPos.x, sceneWindowPos.y);
	const glm::vec2 maxScreenPos(sceneWindowPos.x + sceneWindowSize.x, sceneWindowPos.y + sceneWindowSize.y);

	const bool mouseXInsideScene = mousePos.x >= minScreenPos.x && mousePos.x <= maxScreenPos.x;
	const bool mouseYInsideScene = mousePos.y >= minScreenPos.y && mousePos.y <= maxScreenPos.y;
	const bool leftMousePressed = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (mouseXInsideScene && mouseYInsideScene && leftMousePressed) {

		static Shader s_SelectionShader("src/shaders/Selection.vert", "src/shaders/Selection.frag");
		static FrameBuffer s_SelectionBuffer(sceneWindowSize, FrameBuffer::RED_INTEGER);

		if (sceneWindowSize != s_SelectionBuffer.Size()) {
			s_SelectionBuffer.Resize(sceneWindowSize);
		}

		s_SelectionBuffer.Bind();
		s_SelectionBuffer.RedIntegerFill(-1);

		const auto view = View<LocalToWorld, MeshRenderer>();
		for (const auto entity : view) {
			auto& toWorld = Registry::Get<LocalToWorld>(entity);
			auto& meshRenderer = Registry::Get<MeshRenderer>(entity);
			s_SelectionShader.SetInt("entityId", entity.Id());
			Renderer::DrawMesh(meshRenderer, toWorld, s_SelectionShader);
		}

		const glm::vec2 pixelCoords(mousePos.x - sceneWindowPos.x, sceneWindowSize.y - (mousePos.y - sceneWindowPos.y));
		const i32 possibleEntityId = s_SelectionBuffer.ReadPixel(pixelCoords);
		s_SelectionBuffer.UnBind();

		s_SelectedEntity = possibleEntityId >= 0 ? &Registry::m_Entities[possibleEntityId] : nullptr;
	}

	SceneCamera::Update(sceneWindowSize);

	if (sceneWindowSize != s_SceneFrameBuffer.Size()) {
		s_SceneFrameBuffer.Resize(sceneWindowSize);
	}
		
	Renderer::s_FrameBuffer = &s_SceneFrameBuffer;
	Renderer::RenderScene();

	ImGui::Image((ImTextureID)s_SceneFrameBuffer.Texture(), windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();
}

void Editor::DrawWorld() {
	ImGui::Begin("World", 0, ImGuiWindowFlags_NoCollapse);
	for (Entity& entity : Registry::m_Entities) {
		std::string entityName = "Entity ";
		entityName.append(std::to_string(entity.Id()));
		if (ImGui::Button(entityName.c_str())) {
			s_SelectedEntity = &entity;
		}
	}
	ImGui::End();
}

void Editor::DrawInspector() {
	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	if (s_SelectedEntity != nullptr) {
		std::string entityName = "Selected Entity ";
		entityName.append(std::to_string(s_SelectedEntity->Id()));
		ImGui::Text(entityName.c_str());

		if (Registry::Has<Transform>(*s_SelectedEntity)) {
			auto* trans = &Registry::Get<Transform>(*s_SelectedEntity);
			ImGui::PushItemWidth(60);
			ImGui::DragFloat("x", &trans->position.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("y", &trans->position.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("z", &trans->position.z, 0.1f);
			ImGui::PopItemWidth();
		}
	}
	ImGui::End();
}
