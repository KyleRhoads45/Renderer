#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_inverse.hpp>
#include "ecs/View.h"
#include "core/Components.h"
#include "core/Serializer.h"
#include "core/CameraSystem.h"
#include "renderer/Renderer.h"
#include "SceneCamera.h"
#include "core/Primatives.h"
#include "Editor.h"

#include "core/Input.h"
#include "imgui/imgui_internal.h"

void Editor::Init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ApplyEditorStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_VERSION_4_6));

	s_Window = window;
	s_CurWindowSize = s_SceneWindowSize = { 0, 0 };
	s_SelectionBuffer = FrameBuffer({0, 0}, FrameBuffer::RED_INTEGER);
	s_SelectedEntity = Entity::Null();
	CameraSystem::SetActiveCamera(&SceneCamera::s_Camera, &SceneCamera::s_Transform);
	s_TransGizmos = new TransformGizmos();
	s_ArrowMesh = Mesh::FromFile("Assets/Model/ArrowGizmo.fbx");
}

void Editor::OnPreRenderUpdate() {
	if (Input::OnKeyPress(GLFW_KEY_ESCAPE) && s_FullscreenEnabled) {
		s_FullscreenEnabled = false;
		s_CurWindowSize = s_SceneWindowSize;
	}

	if (s_CurWindowSize != Renderer::GetFrameBufferSize()) {
		Renderer::ResizeFrameBuffer(s_CurWindowSize);
	}
	if (s_CurWindowSize != s_SelectionBuffer.Size()) {
		s_SelectionBuffer.Resize(s_CurWindowSize);
	}
	
	SceneCamera::Update(s_CurWindowSize);
}

void Editor::OnPostRenderUpdate() {
	if (s_FullscreenEnabled) {
		Renderer::PresentFrame();
		return;
	}
	DrawEditor();
}

void Editor::DrawEditor() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	// ImGui::ShowDemoWindow();
	
	DrawMenuBar();
	DrawScene();
	DrawWorld();
	DrawInspector();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::DrawMenuBar() {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		
		ImGui::MenuItem("Save");
		
		if (ImGui::MenuItem("Import")) {
					
		}
		
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void Editor::DrawScene() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	ImGui::Begin("Scene");

	if (ImGui::Button("Fullscreen")) {
		OnFullScreen();
		return;
	}
	
	ImGui::BeginChild("Scene Frame Buffer");

	ImVec2 windowSize = ImGui::GetWindowSize();
	s_CurWindowSize = s_SceneWindowSize = { windowSize.x, windowSize.y };

	const glm::vec2 sceneWindowPos = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	const ImVec2 mousePos = ImGui::GetMousePos();

	const glm::vec2 minScreenPos(sceneWindowPos.x, sceneWindowPos.y);
	const glm::vec2 maxScreenPos(sceneWindowPos.x + s_SceneWindowSize.x, sceneWindowPos.y + s_SceneWindowSize.y);

	const bool mouseXInsideScene = mousePos.x >= minScreenPos.x && mousePos.x <= maxScreenPos.x;
	const bool mouseYInsideScene = mousePos.y >= minScreenPos.y && mousePos.y <= maxScreenPos.y;
	const bool leftMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool leftMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

	static bool selectedGizmo = false;
	static i32 gizmoId = 0;
	if (selectedGizmo && !leftMouseDown) {
		selectedGizmo = false;
	}

	static Shader s_SelectionShader("src/shaders/Selection.vert", "src/shaders/Selection.frag");

	if (mouseXInsideScene && mouseYInsideScene && leftMouseClicked) {
		s_SelectionBuffer.BindAndClear();
		s_SelectionBuffer.RedIntegerFill(-1);

		const auto view = View<LocalToWorld, MeshRenderer>();
		for (const auto entity : view) {
			auto& toWorld = Registry::Get<LocalToWorld>(entity);
			auto& meshRenderer = Registry::Get<MeshRenderer>(entity);
			s_SelectionShader.SetInt("entityId", static_cast<i32>(entity.Id()));
			Renderer::DrawMesh(meshRenderer, toWorld, s_SelectionShader);
		}

		if (s_SelectedEntity != Entity::Null()) {
			auto& ltw = s_SelectedEntity.Get<LocalToWorld>();
			auto gizmoTrans = ltw.ToTransform();

			gizmoTrans.rotation = glm::quatLookAt(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));

			s_TransGizmos->NotifyStartDrag();

			glDisable(GL_DEPTH_TEST);
			s_TransGizmos->SelectionDraw(gizmoTrans.position, s_SelectionShader);
			glEnable(GL_DEPTH_TEST);
		}

		const glm::vec2 pixelCoords(mousePos.x - sceneWindowPos.x, s_SceneWindowSize.y - (mousePos.y - sceneWindowPos.y));
		const i32 possibleEntityId = s_SelectionBuffer.ReadPixel(pixelCoords);

		if (possibleEntityId == -2 || possibleEntityId == -3 || possibleEntityId == -4) {
			selectedGizmo = true;
			gizmoId = possibleEntityId;
		}
		else {
			s_SelectedEntity = possibleEntityId >= 0 ? Registry::m_Entities[possibleEntityId] : Entity::Null();
			s_ShowInspectorEnvironment = false;
			gizmoId = 0;
		}

		s_SelectionBuffer.Unbind();
	}

	static Mesh plane = Primatives::Plane();
	static LocalToWorld planeLtw;
	planeLtw.matrix = glm::mat4(1.0f);

	static Shader depthVisualizer("src/shaders/DepthVisualizer.vert", "src/shaders/DepthVisualizer.frag");

	// glActiveTexture(GL_TEXTURE0);
	// ShadowMapper::m_ShadowMap.Bind();
	// depthVisualizer.SetInt("depthMap", 0);
	// Renderer::DrawMesh(plane, planeLtw, depthVisualizer);

	Renderer::ResumeFrame();
	glDisable(GL_DEPTH_TEST);

	if (s_SelectedEntity != Entity::Null()) {
		glm::vec3 gizmoPos = s_SelectedEntity.Get<LocalToWorld>().ToTransform().position;
		s_TransGizmos->Draw(gizmoPos);

		if (selectedGizmo) {
			glm::vec3 movePos = s_TransGizmos->TransformArrow(gizmoId, gizmoPos, sceneWindowPos);
			glm::mat4 invParentLTW = glm::inverse(s_SelectedEntity.Get<Parent>().entity.Get<LocalToWorld>().matrix);
			s_SelectedEntity.Get<Transform>().position = invParentLTW * glm::vec4(movePos, 1.0f);
		}
	}
	
	glEnable(GL_DEPTH_TEST);
	Renderer::EndFrame();

	auto frameBufferTexture = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(Renderer::FrameBufferTexture()));
	ImGui::Image(frameBufferTexture, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::PopStyleVar();

	// if (s_SceneWindowSize != Renderer::GetFrameBufferSize()) {
	// 	Renderer::ResizeFrameBuffer(s_SceneWindowSize);
	// }
	//
	// if (s_SceneWindowSize != s_SelectionBuffer.Size()) {
	// 	s_SelectionBuffer.Resize(s_SceneWindowSize);
	// }
}

void Editor::DrawWorld() {
	ImGui::Begin("World", 0, ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Environment")) {
		s_ShowInspectorEnvironment = true;
	}

	auto rootView = View<LocalToWorld, Transform, Children>().Exclude<Parent>();
	for (Entity& entity : rootView) {
		DrawEntityHierarchy(entity);
	}

	ImGui::End();
}

void Editor::DrawEntityHierarchy(Entity entity) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool isLeaf = !Registry::Has<Children>(entity);
	if (isLeaf) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (s_SelectedEntity != Entity::Null() && s_SelectedEntity == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity.Id()), flags, "Entity %d", entity.Id());

	if (ImGui::IsItemClicked()) {
		s_SelectedEntity = entity;
		s_ShowInspectorEnvironment = false;
	}

	if (open && !isLeaf) {
		for (Entity& child : Registry::Get<Children>(entity).entities) {
			DrawEntityHierarchy(child);
		}
	}

	if (open) {
		ImGui::TreePop();
	}
}

void Editor::DrawInspector() {
	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	if (s_ShowInspectorEnvironment) {
		static i32 shadowPcfWindowSize = Enviroment::Instance()->ShadowPcfWindowSize();
		static i32 shadowPcfFilterSize = Enviroment::Instance()->ShadowPcfFilterSize();
		static f32 shadowPcfFilterRadius = Enviroment::Instance()->ShadowPcfFilterRadius();
		static f32 shadowStrength = Enviroment::Instance()->ShadowStrength();
		
		if (ImGui::SliderInt("Shadow PCF Window", &shadowPcfWindowSize, 0, 100)) {
			Enviroment::Instance()->SetShadowPcf(shadowPcfWindowSize, shadowPcfFilterSize);
		}
		if (ImGui::SliderInt("Shadow PCF Filter", &shadowPcfFilterSize, 0, 100)) {
			Enviroment::Instance()->SetShadowPcf(shadowPcfWindowSize, shadowPcfFilterSize);
		}
		if (ImGui::SliderFloat("Shadow PCF Radius", &shadowPcfFilterRadius, 0.00f, 10.00f, "%.01f")) {
			Enviroment::Instance()->SetShadowPcfRadius(shadowPcfFilterRadius);
		}
		if (ImGui::SliderFloat("Shadow Strength", &shadowStrength, 0.0f, 1.0f, "%.01f")) {
			Enviroment::Instance()->SetShadowStrength(shadowStrength);
		}
		
		static f32 lightStrength = Enviroment::Instance()->LightStrength();	
		static f32 ambientStrength = Enviroment::Instance()->AmbientStrength();;	
		ImGui::SliderFloat("Light Strength", &lightStrength, 0.00f, 10.00f, "%.01f");
		ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.00f, 10.00f, "%.01f");

		static glm::vec3 initAmbientColor = Enviroment::Instance()->AmbientColor();
		static f32 ambientColor[3] = { initAmbientColor.x, initAmbientColor.y, initAmbientColor.z };
		ImGui::ColorPicker3("Ambient Color", ambientColor);
		glm::vec3 ambient = glm::vec3(ambientColor[0], ambientColor[1], ambientColor[2]);
		
		static glm::vec3 initLightColor = Enviroment::Instance()->LightColor();
		static f32 lightColor[3] = { initLightColor.x, initLightColor.y, initLightColor.z };
		ImGui::ColorPicker3("Light Color", lightColor);
		glm::vec3 light = glm::vec3(lightColor[0], lightColor[1], lightColor[2]);

		Enviroment::Instance()->SetLightStrength(lightStrength);
		Enviroment::Instance()->SetAmbientStrength(ambientStrength);
		Enviroment::Instance()->SetAmbientColor(ambient);
		Enviroment::Instance()->SetLightColor(light);

		ImGui::End();
		return;
	}
	
	if (s_SelectedEntity != Entity::Null()) {
		std::string entityName = "Selected Entity ";
		entityName.append(std::to_string(s_SelectedEntity.Id()));
		ImGui::Text(entityName.c_str());

		if (Registry::Has<Transform>(s_SelectedEntity)) {
			auto* trans = &Registry::Get<Transform>(s_SelectedEntity);
			ImGui::PushItemWidth(60);
			ImGui::DragFloat("x", &trans->position.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("y", &trans->position.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("z", &trans->position.z, 0.1f);
			ImGui::PopItemWidth();
		}

		if (s_SelectedEntity.Has<MeshRenderer>()) {
			auto& materials = s_SelectedEntity.Get<MeshRenderer>().materials;

			for (auto& material : materials) {
				ImGui::Separator();
				ImGui::Text(material->GetFilePath().c_str());

				f32 roughness = material->GetRoughness();
				f32 specularity = material->GetSpecularity();
				f32 metallicness = material->GetMetallicness();
				f32 alphaCutoff = material->GetAlphaCutoff();

				if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.00f, "%.01f")) {
					material->SetRoughness(roughness);		
					WriteMaterialToFile(*material);
				}
				if (ImGui::SliderFloat("Specularity", &specularity, 0.0f, 1.0f, "%.01f")) {
					material->SetSpecularity(specularity);		
					WriteMaterialToFile(*material);
				}
				if (ImGui::SliderFloat("Metallicness", &metallicness, 0.0f, 1.0f, "%.01f")) {
					material->SetMetallicness(metallicness);
					WriteMaterialToFile(*material);
				}
				if (ImGui::SliderFloat("Alpha Cutoff", &alphaCutoff, 0.0f, 1.0f, "%.01f")) {
					material->SetAlphaCutoff(alphaCutoff);
					WriteMaterialToFile(*material);
				}

				const char* renderOrders[] = { "Opaque", "Cutout", "Transparent" };
				const i32 curOrderIndex = static_cast<i32>(material->GetRenderOrder());
				const char* previewOrder = renderOrders[curOrderIndex];
				
				if (ImGui::BeginCombo("Render Order", previewOrder, ImGuiComboFlags_None)) {
					for (i32 i = 0; i < IM_ARRAYSIZE(renderOrders); i++) {
						if (ImGui::Selectable(renderOrders[i])) {
							material->SetRenderOrder(static_cast<RenderOrder>(i));
							WriteMaterialToFile(*material);
						}
					}
					ImGui::EndCombo();
				}
			}
		}
	}
	ImGui::End();
}

void Editor::WriteMaterialToFile(const Material& material) {
	Serializer serializer;
	serializer.Serialize(material);
	serializer.WriteToFile(material.GetFilePath());
}

void Editor::OnFullScreen() {
	i32 width, height;
	glfwGetWindowSize(s_Window, &width, &height);
	s_CurWindowSize = { width, height };
	s_FullscreenEnabled = true;
}

void Editor::ApplyEditorStyle() {
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 19, nullptr, io.Fonts->GetGlyphRangesDefault());

	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	style.FramePadding = ImVec2(20.0f, 8.0f);
	style.FrameBorderSize = 1.0f;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text]                   = ImVec4(0.78f, 0.78f, 0.81f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.03f, 0.03f, 0.04f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.00f, 0.19f, 0.75f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 0.19f, 0.75f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.00f, 0.19f, 0.75f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.19f, 0.75f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.00f, 0.19f, 0.75f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.00f, 0.19f, 0.74f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.03f, 0.03f, 0.04f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

