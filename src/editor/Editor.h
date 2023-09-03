#pragma once
#include <ecs/Entity.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "renderer/FrameBuffer.h"

class Editor {
public: 
	static void Init(GLFWwindow* window);
	static void Update();
private:
	static void DrawEditor();
	static void DrawMenuBar();
	static void DrawScene();
	static void DrawWorld();
	static void DrawInspector();
private: 
	inline static FrameBuffer s_SceneFrameBuffer;
	inline static Entity* s_SelectedEntity;
};

