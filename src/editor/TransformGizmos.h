#pragma once
#include "ecs/Registry.h"
#include "core/Components.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

class TransformGizmos {
public:
	enum GizmoUsage { Selection, Translation};

	TransformGizmos();
	void Draw(const glm::vec3& pos);
	void SelectionDraw(const glm::vec3& pos, Shader& selectionShader);
	void NotifyStartDrag();
	glm::vec3 TransformArrow(i32 gizmoId, glm::vec3 pos, glm::vec2 sceneWindowPos);

private:
	Mesh m_ArrowMesh;
	glm::vec3 m_initialOffset;
};

