#pragma once
#include <glm/glm.hpp>
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "ecs/Registry.h"

struct EditorWindowRect;

class TransformGizmos {
public:
	TransformGizmos();
	void TransformHandle(Registry& registry, glm::vec3* pos);
	void Draw(Registry& registry, const glm::vec3& pos);
	void NotifyStartDrag();
	glm::vec3 TransformArrow(Entity gizmoEntity, const glm::vec3& pos);

	// Temporary until events are implemented, same goes for it being static
	inline static glm::vec3 m_InitialOffset;
private:
	Entity CreateArrowEntity(Registry& registry);	
private:
	Mesh m_ArrowMesh;
	Entity m_RightEntity;
	Entity m_UpEntity;
	Entity m_ForwardEntity;
};