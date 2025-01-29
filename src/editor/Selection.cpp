#include <imgui/imgui.h>
#include "GuiUtils.h"
#include "renderer/Shader.h"
#include "renderer/renderer.h"
#include "ecs/View.h"
#include "ecs/Registry.h"
#include "TransformGizmos.h"
#include "Selection.h"

void Selection::Init() {
	s_SelectionBuffer = FrameBuffer({0, 0}, FrameBuffer::RED_INTEGER);
	s_SelectedEntity = Entity::Null();
}

void Selection::PreRenderUpdate(const glm::i32vec2 windowSize) {
	if (windowSize != s_SelectionBuffer.Size()) {
		s_SelectionBuffer.Resize(windowSize);
	}
}

void Selection::Update(Registry& gameRegistry, Registry& gizmoRegistry) {
	static Shader s_SelectionShader("src/shaders/Selection.vert", "src/shaders/Selection.frag");
	
	GuiRect sceneRect = GuiUtils::CurrentWindow();
	bool mouseIsOverScene = sceneRect.ContainsPoint(GuiUtils::MousePosition());
	const bool leftMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (mouseIsOverScene && leftMouseClicked) {
		s_SelectionBuffer.BindAndClear();
		s_SelectionBuffer.RedIntegerFill(-1);

		{
			const auto view = View<LocalToWorld, MeshRenderer>(gameRegistry);
			for (const auto entity : view) {
				auto& toWorld = gameRegistry.Get<LocalToWorld>(entity);
				auto& meshRenderer = gameRegistry.Get<MeshRenderer>(entity);
				s_SelectionShader.SetInt("entityId", static_cast<i32>(entity.Id()));
				Renderer::DrawMesh(meshRenderer, toWorld, s_SelectionShader);
			}
		}

		i32 gizmoIdOffset = gameRegistry.GetEntityCount();

		glDisable(GL_DEPTH_TEST);
		{
			const auto view = View<Transform, MeshRenderer>(gizmoRegistry);
			for (const auto entity : view) {
				auto& transform = gizmoRegistry.Get<Transform>(entity);
				auto& meshRenderer = gizmoRegistry.Get<MeshRenderer>(entity);
				s_SelectionShader.SetInt("entityId", static_cast<i32>(entity.Id() + gizmoIdOffset));
				Renderer::DrawMesh(meshRenderer, LocalToWorld::FromTransform(transform), s_SelectionShader);
			}
		}
		glEnable(GL_DEPTH_TEST);
		
		const glm::vec2 pixelCoords = sceneRect.RelativeCoordinates(GuiUtils::MousePosition());
		const i32 possibleEntityId = s_SelectionBuffer.ReadPixel(pixelCoords);

		if (possibleEntityId >= gizmoIdOffset) {
			s_SelectedGizmoEntity = possibleEntityId - gizmoIdOffset;
			TransformGizmos::m_InitialOffset = glm::vec3(0);
		}
		else {
			s_SelectedEntity = possibleEntityId >= 0 ? gameRegistry.m_Entities[possibleEntityId] : Entity::Null();
			s_SelectedGizmoEntity = Entity::Null();
		}

		s_SelectionBuffer.Unbind();
	}
}

Entity Selection::SelectedEntity() {
	return s_SelectedEntity;
}

Entity Selection::GizmoEntity() {
	return s_SelectedGizmoEntity;
}
