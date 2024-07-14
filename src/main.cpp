#include <iostream>
#include <filesystem>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "core/Base.h"
#include "core/Input.h"
#include "editor/Editor.h"
#include "renderer/Renderer.h"
#include "renderer/Model.h"
#include "renderer/CubeMap.h"
#include "renderer/Material.h"
#include "core/CameraSystem.h"
#include "core/TransformSystem.h"
#include "renderer/ShadowMapper.h"
#include "ecs/Registry.h"
#include "editor/importers/ModelImporter.h"

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

	//ModelImporter::Import("Assets/House/scene.gltf");
	auto house = Model::Instantiate("Assets/House/scene.gltf.model");
	house.Get<Transform>().scale = glm::vec3(0.01f);

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		Editor::Update();
		CameraSystem::Update();
		TransformSystem::Update();
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
	Enviroment::Instance()->SetLightStrength(0.01f);
	Enviroment::Instance()->SetAmbientStrength(0.0f);

	ShadowMapper::Init(4096, 1.3f);
}
