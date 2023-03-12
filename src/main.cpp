#include <iostream>
#include <array>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include "core/Components.h"
#include "core/Input.h"
#include "renderer/Renderer.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "renderer/CubeMap.h"
#include "renderer/Material.h"
#include "core/Primatives.h"
#include "ecs/Registry.h"
#include "core/CameraSystem.h"
#include "materials/CityMaterial.h"
#include <glm/gtx/quaternion.hpp>

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

	auto window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to load OpenGL\n";
		return -1;
	}

	Input::Init(window);
	Renderer::Init();

	SetupEnviroment();

	auto camEntity = Registry::Create();
	auto camTrans = Registry::Add<Transform>(camEntity);
	auto camera = Registry::Add<Camera>(camEntity);
	CameraSystem::activeCamEntity = camEntity;

	Shader litShader("src/shaders/lit.vert", "src/shaders/lit.frag");
	CityMaterial cityMaterial(litShader);
	auto modelEntity = InstantiateModel("Assets/Model/City.fbx", &cityMaterial);

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		CameraSystem::Update();
		Renderer::RenderScene(*Registry::Get<Camera>(CameraSystem::activeCamEntity));
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void SetupEnviroment() {
	Enviroment::Init();
	std::array<std::string, 6> textures {
		"Assets/Skybox/right.jpg",
		"Assets/Skybox/left.jpg",
		"Assets/Skybox/top.jpg",
		"Assets/Skybox/bottom.jpg",
		"Assets/Skybox/front.jpg",
		"Assets/Skybox/back.jpg"
	};
	Enviroment::m_Skybox = CubeMap(textures);
}
