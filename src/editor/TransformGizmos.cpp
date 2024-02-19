#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glad/glad.h>
#include "core/Input.h"
#include "editor/SceneCamera.h"
#include "core/CameraSystem.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "TransformGizmos.h"

#include <iostream>

TransformGizmos::TransformGizmos() {
	m_ArrowMesh = Mesh::FromFile("Assets/Model/ArrowGizmo.fbx");
}

void TransformGizmos::Draw(const glm::vec3& pos) {
	static Shader s_GizmoShader("src/shaders/Gizmo.vert", "src/shaders/Gizmo.frag");
	s_GizmoShader.Bind();

	f32 dist = glm::distance(CameraSystem::ActiveCamPos(), pos);
	glm::vec3 scale(dist * 0.03f);

	s_GizmoShader.SetVec3("gizmoColor", glm::vec3(1, 0, 0));
	LocalToWorld xLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(0.0, glm::radians(90.0), 0.0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, xLtw, s_GizmoShader);

	s_GizmoShader.SetVec3("gizmoColor", glm::vec3(0, 1, 0));
	LocalToWorld yLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(glm::radians(-90.0), 0.0, 0.0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, yLtw, s_GizmoShader);

	s_GizmoShader.SetVec3("gizmoColor", glm::vec3(0, 0, 1));
	LocalToWorld zLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, zLtw, s_GizmoShader);
}

void TransformGizmos::SelectionDraw(const glm::vec3& pos, Shader& selectionShader) {
	f32 dist = glm::distance(CameraSystem::ActiveCamPos(), pos);
	glm::vec3 scale(dist * 0.03f);

	selectionShader.SetInt("entityId", -2);
	LocalToWorld xLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(0.0, glm::radians(90.0), 0.0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, xLtw, selectionShader);

	selectionShader.SetInt("entityId", -3);
	LocalToWorld yLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(glm::radians(-90.0), 0.0, 0.0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, yLtw, selectionShader);

	selectionShader.SetInt("entityId", -4);
	LocalToWorld zLtw = LocalToWorld::From(pos, glm::quat(glm::vec3(0)), scale);
	Renderer::DrawMesh(m_ArrowMesh, zLtw, selectionShader);
}

void TransformGizmos::NotifyStartDrag() {
	m_initialOffset = glm::vec3(0.0);
}

glm::vec3 TransformGizmos::TransformArrow(i32 gizmoId, glm::vec3 pos, glm::vec2 sceneWindowPos) {
	i32 viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	glm::vec2 screenSize(viewportDimensions[2], viewportDimensions[3]);

	glm::vec2 mousePos = Input::MousePosition();

	const glm::vec2 pixelCoords(mousePos.x - sceneWindowPos.x, screenSize.y - (mousePos.y - sceneWindowPos.y));

	f32 xNDC = ((pixelCoords.x / screenSize.x) * 2.0f) - 1.0f;
	f32 yNDC = (((pixelCoords.y / screenSize.y) * 2.0f) - 1.0f);

	glm::vec4 ndc = glm::vec4(xNDC, yNDC, -1.0f, 1.0f);

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

	if (gizmoId == -2) {
		dragPos.y = selectedWorldPos.y;
		dragPos.z = selectedWorldPos.z;
	}
	else if (gizmoId == -3) {
		dragPos.x = selectedWorldPos.x;
		dragPos.z = selectedWorldPos.z;
	}
	else {
		dragPos.x = selectedWorldPos.x;
		dragPos.y = selectedWorldPos.y;
	}

	if (m_initialOffset == glm::vec3(0.0)) {
		m_initialOffset = pos - dragPos;
	}

	return dragPos + m_initialOffset;
}
