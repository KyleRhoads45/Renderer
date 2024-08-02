#pragma once
#include <GLFW/glfw3.h>
#include "ecs/Registry.h"
#include "renderer/FrameBuffer.h"
#include "editor/TransformGizmos.h"

class Editor {
public: 
	static void Init(GLFWwindow* window);
	static void OnPreRenderUpdate();
	static void OnPostRenderUpdate();
private:
	static void DrawEditor();
	static void DrawMenuBar();
	static void DrawScene();
	static void DrawWorld();
	static void DrawEntityHierarchy(Entity entity);
	static void DrawInspector();
	static void WriteMaterialToFile(const Material& material);
	static void OnFullScreen();
	static void ApplyEditorStyle();
private:
	inline static FrameBuffer s_SelectionBuffer;
	inline static Entity s_SelectedEntity;
	inline static TransformGizmos* s_TransGizmos;
	inline static Mesh s_ArrowMesh;
	inline static GLFWwindow* s_Window;
	inline static glm::i32vec2 s_CurWindowSize;
	inline static glm::i32vec2 s_SceneWindowSize;
	inline static bool s_ShowInspectorEnvironment;
	inline static bool s_FullscreenEnabled;
};

