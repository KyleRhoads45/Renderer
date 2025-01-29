#include <glm/gtx/vector_angle.hpp>
#include <glad/glad.h>
#include "GuiUtils.h"
#include "editor/SceneCamera.h"
#include "core/CameraSystem.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "TransformGizmos.h"
#include "core/Components.h"
#include "Selection.h"

TransformGizmos::TransformGizmos() {
	m_ArrowMesh = Mesh::FromFile("Assets/Model/ArrowGizmo.fbx");
}

void TransformGizmos::TransformHandle(Registry& registry, glm::vec3* pos) {
	Draw(registry, *pos);
	*pos = TransformArrow(Selection::GizmoEntity(), *pos);
}

void TransformGizmos::Draw(Registry& registry, const glm::vec3& pos) {
	static Shader s_GizmoShader("src/shaders/Gizmo.vert", "src/shaders/Gizmo.frag");
	s_GizmoShader.Bind();

	f32 dist = glm::distance(CameraSystem::ActiveCamPos(), pos);
	glm::vec3 scale(dist * 0.03f);

	{
		m_RightEntity = CreateArrowEntity(registry);
		auto& rightTrans = registry.Get<Transform>(m_RightEntity);
		rightTrans.position = pos;
		rightTrans.rotation = glm::quat(glm::vec3(0.0, glm::radians(90.0), 0.0));
		rightTrans.scale = scale;
		
		auto& rightMeshRenderer = registry.Get<MeshRenderer>(m_RightEntity);
		rightMeshRenderer.meshes.push_back(m_ArrowMesh);
		
		s_GizmoShader.SetVec3("gizmoColor", glm::vec3(1, 0, 0));
		Renderer::DrawMesh(rightMeshRenderer, LocalToWorld::FromTransform(rightTrans), s_GizmoShader);
	}

	{
		m_UpEntity = CreateArrowEntity(registry);
		auto& upTrans = registry.Get<Transform>(m_UpEntity);
		upTrans.position = pos;
		upTrans.rotation = glm::quat(glm::vec3(glm::radians(-90.0), 0.0, 0.0));
		upTrans.scale = scale;
		
		auto& upMeshRenderer = registry.Get<MeshRenderer>(m_UpEntity);
		upMeshRenderer.meshes.push_back(m_ArrowMesh);

		s_GizmoShader.SetVec3("gizmoColor", glm::vec3(0, 1, 0));
		Renderer::DrawMesh(upMeshRenderer, LocalToWorld::FromTransform(upTrans), s_GizmoShader);
	}

	{
		m_ForwardEntity = CreateArrowEntity(registry);
		auto& forwardTrans = registry.Get<Transform>(m_ForwardEntity);
		forwardTrans.position = pos;
		forwardTrans.rotation = glm::quat(glm::vec3(0.0));
		forwardTrans.scale = scale;
		
		auto& forwardMeshRenderer = registry.Get<MeshRenderer>(m_ForwardEntity);
		forwardMeshRenderer.meshes.push_back(m_ArrowMesh);

		s_GizmoShader.SetVec3("gizmoColor", glm::vec3(0, 0, 1));
		Renderer::DrawMesh(forwardMeshRenderer, LocalToWorld::FromTransform(forwardTrans), s_GizmoShader);
	}
}

void TransformGizmos::NotifyStartDrag() {
	m_InitialOffset = glm::vec3(0.0);
}

glm::vec3 TransformGizmos::TransformArrow(Entity gizmoEntity, const glm::vec3& pos) {
	bool draggingRight   = gizmoEntity == m_RightEntity;
	bool draggingUp      = gizmoEntity == m_UpEntity;
	bool draggingForward = gizmoEntity == m_ForwardEntity;

	if (!draggingRight && !draggingUp && !draggingForward) {
		return pos;
	}
	
	i32 viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	glm::vec2 screenSize(viewportDimensions[2], viewportDimensions[3]);

	glm::vec2 mousePos = GuiUtils::MousePosition();

	GuiRect sceneRect = GuiUtils::CurrentWindow();
	const glm::vec2 pixelCoords = sceneRect.RelativeCoordinates(mousePos);

	f32 xNdc = ((pixelCoords.x / screenSize.x) * 2.0f) - 1.0f;
	f32 yNdc = (((pixelCoords.y / screenSize.y) * 2.0f) - 1.0f);
	glm::vec4 ndc = glm::vec4(xNdc, yNdc, -1.0f, 1.0f);

	glm::vec4 viewSpace = glm::inverse(CameraSystem::ActiveCamProjection()) * ndc;
	viewSpace.z = -1.0f;
	viewSpace.w = 0.0f;

	glm::vec4 world = glm::inverse(CameraSystem::ActiveCamView()) * viewSpace;
	glm::vec3 rayDir = glm::normalize(glm::vec3(world));

	glm::vec3 selectedWorldPos = pos;

	glm::vec3 dirToObject = selectedWorldPos - SceneCamera::s_Transform.position;
	dirToObject = glm::normalize(dirToObject);

	glm::vec3 flatRayDir = rayDir;
	flatRayDir = glm::normalize(flatRayDir);

	f32 zDist = glm::distance(selectedWorldPos, SceneCamera::s_Transform.position);
	f32 projectedZDist = zDist / glm::cos(glm::angle(dirToObject, flatRayDir));

	glm::vec3 dragPos = SceneCamera::s_Transform.position + rayDir * projectedZDist;

	if (draggingRight) {
		dragPos.y = selectedWorldPos.y;
		dragPos.z = selectedWorldPos.z;
	}
	else if (draggingUp) {
		dragPos.x = selectedWorldPos.x;
		dragPos.z = selectedWorldPos.z;
	}
	else if (draggingForward) {
		dragPos.x = selectedWorldPos.x;
		dragPos.y = selectedWorldPos.y;
	}

	if (m_InitialOffset == glm::vec3(0.0)) {
		m_InitialOffset = pos - dragPos;
	}

	return dragPos + m_InitialOffset;
}

Entity TransformGizmos::CreateArrowEntity(Registry& registry) {
	Entity entity = registry.Create();
	registry.Add<Transform>(entity);
	registry.Add<MeshRenderer>(entity);
	return entity;
}
