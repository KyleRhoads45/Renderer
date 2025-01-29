#pragma once
#include <GLFW/glfw3.h>
#include "editor/TransformGizmos.h"
#include "ecs/Registry.h"

class Material;

class Editor {
public: 
	static void Init(GLFWwindow* window);
	static void OnPreRenderUpdate();
	static void OnPostRenderUpdate(Registry& registry);
private:
	static void DrawEditor(Registry& registry);
	static void DrawMenuBar();
	static void DrawScene(Registry& registry);
	static void DrawWorld(Registry& registry);
	static void DrawEntityHierarchy(Registry& registry, Entity entity);
	static void DrawInspector(Registry& registry);
	static void WriteMaterialToFile(const Material& material);
	static void OnFullScreen();
	static void ApplyEditorStyle();
private:
	inline static Registry s_EditorRegistry;
	inline static TransformGizmos* s_TransGizmos;
	inline static Mesh s_ArrowMesh;
	inline static GLFWwindow* s_Window;
	inline static glm::i32vec2 s_CurRenderingWinSize;
	inline static glm::i32vec2 s_SceneWindowSize;
	inline static bool s_ShowInspectorEnvironment = false;
	inline static bool s_ShowInspectorPostProcessing = false;
	inline static bool s_FullscreenEnabled = false;
	inline static bool s_ShowShadowMap = false;
};

