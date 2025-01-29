#pragma once
#include "ecs/Registry.h"

class Registry;

class Selection {
public:
	static void Init();
	static void PreRenderUpdate(glm::i32vec2 windowSize);
	static void Update(Registry& gameRegistry, Registry& gizmoRegistry);
	static Entity SelectedEntity();
	static Entity GizmoEntity();
private:
	inline static FrameBuffer s_SelectionBuffer;
	inline static Entity s_SelectedEntity;
	inline static Entity s_SelectedGizmoEntity;
};