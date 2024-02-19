#pragma once
#include <GLFW/glfw3.h>
#include "ecs/Registry.h"
#include "renderer/FrameBuffer.h"
#include "editor/TransformGizmos.h"

class Editor {
public: 
	static void Init(GLFWwindow* window);
	static void Update();
private:
	static void DrawEditor();
	static void DrawMenuBar();
	static void DrawScene();
	static void DrawWorld();
	static void DrawEntityHierarchy(Entity entity);
	static void DrawInspector();
private: 
	inline static FrameBuffer s_SceneFrameBuffer;
	inline static Entity s_SelectedEntity;
	inline static TransformGizmos* s_TransGizmos;
	inline static Mesh s_ArrowMesh;
};

