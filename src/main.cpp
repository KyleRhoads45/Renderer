#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "core/Input.h"
#include "editor/Editor.h"
#include "renderer/Renderer.h"
#include "renderer/Model.h"
#include "renderer/CubeMap.h"
#include "core/CameraSystem.h"
#include "core/TransformSystem.h"
#include "renderer/ShadowMapper.h"
#include "ecs/Registry.h"

void SetupEnviroment();

int main() {
	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw\n";
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL Renderer", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to load OpenGL\n";
		return -1;
	}

	SetupEnviroment();
	CameraSystem::Init();
	Input::Init(window);
	Renderer::Init();
	Editor::Init(window);

	// ModelImporter::Import("Assets/MedievalVillage/MedievalVillage.fbx");
	auto village = Model::Instantiate("Assets/MedievalVillage/MedievalVillage.fbx.model");
	village.Get<Transform>().scale = glm::vec3(0.001f);

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		CameraSystem::Update();
		TransformSystem::Update();
		Editor::OnPreRenderUpdate();
		Renderer::NewFrame();
		Renderer::RenderScene();
		Renderer::EndFrame();
		Editor::OnPostRenderUpdate();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void SetupEnviroment() {
	std::array<std::string, 6> textures {
		"Assets/Skybox/right.jpg", "Assets/Skybox/left.jpg",
		"Assets/Skybox/top.jpg", "Assets/Skybox/bottom.jpg",
		"Assets/Skybox/front.jpg", "Assets/Skybox/back.jpg"
	};
	Enviroment::Instance()->SetSkyBox(CubeMap::Load(textures));

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.33, -0.33, -0.33));
	Enviroment::Instance()->SetLightDir(lightDir);
	Enviroment::Instance()->SetLightColor({1.0f, 1.0f, 1.0f});
	Enviroment::Instance()->SetLightStrength(5.0f);
	Enviroment::Instance()->SetAmbientColor({0.130f, 0.155f, 0.186f});
	Enviroment::Instance()->SetAmbientStrength(5.5f);

	Enviroment::Instance()->SetShadowPcf(16, 10);
	Enviroment::Instance()->SetShadowPcfRadius(5.0f);
	Enviroment::Instance()->SetShadowStrength(0.8f);
	
	ShadowMapper::Init(8192, 2.8f);
}
