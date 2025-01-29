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
#include "core/Input.h"
#include "imgui/imgui_internal.h"
#include "renderer/ShadowMapper.h"
#include "Selection.h"
#include "GuiUtils.h"
#include "Editor.h"

void Editor::Init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ApplyEditorStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_VERSION_4_6));

	Selection::Init();

	s_Window = window;
	s_CurRenderingWinSize = { 0, 0 };
	s_SceneWindowSize = { 0, 0 };
	
	CameraSystem::SetActiveCamera(&SceneCamera::s_Camera, &SceneCamera::s_Transform);
	s_TransGizmos = new TransformGizmos();
	s_ArrowMesh = Mesh::FromFile("Assets/Model/ArrowGizmo.fbx");
}

void Editor::OnPreRenderUpdate() {
	if (Input::OnKeyPress(GLFW_KEY_ESCAPE) && s_FullscreenEnabled) {
		s_FullscreenEnabled = false;
		s_CurRenderingWinSize = s_SceneWindowSize;
	}

	if (s_CurRenderingWinSize != Renderer::GetFrameBufferSize()) {
		Renderer::ResizeFrameBuffer(s_CurRenderingWinSize);
	}

	Selection::PreRenderUpdate(s_CurRenderingWinSize);
	SceneCamera::Update(s_CurRenderingWinSize);
}

void Editor::OnPostRenderUpdate(Registry& registry) {
	if (s_FullscreenEnabled) {
		Renderer::PresentFrame();
		return;
	}
	DrawEditor(registry);
}

void Editor::DrawEditor(Registry& registry) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	// ImGui::ShowDemoWindow();
	
	DrawMenuBar();
	DrawScene(registry);
	DrawWorld(registry);
	DrawInspector(registry);

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

void Editor::DrawScene(Registry& registry) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	ImGui::Begin("Scene");

	if (ImGui::Button("Fullscreen")) {
		OnFullScreen();
		return;
	}
	
	ImGui::BeginChild("Scene Frame Buffer");

	const glm::vec2 windowSize = GuiUtils::CurrentWindowSize();
	s_SceneWindowSize = windowSize;
	s_CurRenderingWinSize = windowSize;

	Renderer::NewGizmosFrame();
	glDisable(GL_DEPTH_TEST);
	
	if (Input::OnKeyPress(GLFW_KEY_M)) {
		s_ShowShadowMap = !s_ShowShadowMap;		
	}
	
	if (s_ShowShadowMap) {
		static Shader depthVisualizer("src/shaders/DepthVisualizer.vert", "src/shaders/DepthVisualizer.frag");
		depthVisualizer.Bind();
		ShadowMapper::BindShadowMap(0);
		depthVisualizer.SetInt("depthMap", 0);
		Renderer::DrawFullScreenQuad(depthVisualizer);
	}
	
	Entity selectedEntity = Selection::SelectedEntity();

	if (selectedEntity != Entity::Null()) {
		glm::vec3 gizmoPos = registry.Get<LocalToWorld>(selectedEntity).ToTransform().position;
		s_TransGizmos->TransformHandle(s_EditorRegistry, &gizmoPos);
		Entity parent = registry.Get<Parent>(selectedEntity).entity;
		glm::mat4 invParentLTW = glm::inverse(registry.Get<LocalToWorld>(parent).matrix);
		registry.Get<Transform>(selectedEntity).position = invParentLTW * glm::vec4(gizmoPos, 1.0f);
	}

	glEnable(GL_DEPTH_TEST);
	Renderer::EndGizmosFrame();

	Selection::Update(registry, s_EditorRegistry);

	auto frameBufferTexture = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(Renderer::FrameBufferTexture()));
	ImGui::Image(frameBufferTexture, { windowSize.x, windowSize.y }, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::PopStyleVar();

	s_EditorRegistry.FreeAll();
}

void Editor::DrawWorld(Registry& registry) {
	ImGui::Begin("World", 0, ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Environment")) {
		s_ShowInspectorEnvironment = true;
		s_ShowInspectorPostProcessing = false;
	}
	
	if (ImGui::Button("PostProcessing")) {
		s_ShowInspectorPostProcessing = true;
		s_ShowInspectorEnvironment = false;
	}

	auto rootView = View<LocalToWorld, Transform, Children>(registry).Exclude<Parent>();
	for (Entity& entity : rootView) {
		DrawEntityHierarchy(registry, entity);
	}

	ImGui::End();
}

void Editor::DrawEntityHierarchy(Registry& registry, Entity entity) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool isLeaf = !registry.Has<Children>(entity);
	if (isLeaf) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	Entity selectedEntity = Selection::SelectedEntity();
	if (selectedEntity != Entity::Null() && selectedEntity == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity.Id()), flags, "Entity %d", entity.Id());

	if (ImGui::IsItemClicked()) {
		selectedEntity = entity;
		s_ShowInspectorEnvironment = false;
		s_ShowInspectorPostProcessing = false;
	}

	if (open && !isLeaf) {
		for (Entity& child : registry.Get<Children>(entity).entities) {
			DrawEntityHierarchy(registry, child);
		}
	}

	if (open) {
		ImGui::TreePop();
	}
}

void Editor::DrawInspector(Registry& registry) {
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
		ImGui::SliderFloat("Light Strength", &lightStrength, 0.00f, 100.00f, "%.01f");
		ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.00f, 100.00f, "%.01f");

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

	if (s_ShowInspectorPostProcessing) {
		Renderer::PostProcessingParams params = Renderer::GetPostProcessingParams();
		ImGui::SliderFloat("Contrast", &params.contrast, 0.00f, 10.00f, "%.01f");
		ImGui::SliderFloat("Saturation", &params.saturation, 0.00f, 10.00f, "%.01f");
		ImGui::SliderFloat("Exposure", &params.exposure, 0.00f, 10.00f, "%.01f");
		Renderer::SetPostProcessingParams(params);
		ImGui::End();
		return;
	}

	Entity selectedEntity = Selection::SelectedEntity();

	if (selectedEntity != Entity::Null()) {
		std::string entityName = "Selected Entity ";
		entityName.append(std::to_string(selectedEntity.Id()));
		ImGui::Text(entityName.c_str());

		if (registry.Has<Transform>(selectedEntity)) {
			auto* trans = &registry.Get<Transform>(selectedEntity);
			ImGui::PushItemWidth(60);
			ImGui::DragFloat("x", &trans->position.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("y", &trans->position.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("z", &trans->position.z, 0.1f);
			ImGui::PopItemWidth();
		}

		if (registry.Has<MeshRenderer>(selectedEntity)) {
			auto& materials = registry.Get<MeshRenderer>(selectedEntity).materials;

			for (size_t i = 0; i < materials.size(); i++) {
				Material* material = materials[i];

				ImGui::Dummy({0, 20});
				ImGui::Text(material->GetFilePath().c_str());

				f32 roughness = material->GetRoughness();
				f32 specularity = material->GetSpecularity();
				f32 metallicness = material->GetMetallicness();
				f32 alphaCutoff = material->GetAlphaCutoff();
				glm::vec2 tiling = material->GetTiling();

				ImGui::PushID(static_cast<i32>(i));
				
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

				ImGui::Text("Tiling");
				if (ImGui::InputFloat("Tiling X", &tiling.x)) {
					material->SetTiling(tiling);
					WriteMaterialToFile(*material);
				}
				ImGui::SameLine();
				if (ImGui::InputFloat("Tiling Y", &tiling.y)) {
					material->SetTiling(tiling);
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

				ImGui::PopID();
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
	s_CurRenderingWinSize = { width, height };
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
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
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

