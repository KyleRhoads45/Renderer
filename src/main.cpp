#include <iostream>
#include <array>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include "core/Components.h"
#include "core/SceneCamera.h"
#include "core/Input.h"
#include "renderer/Renderer.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "renderer/CubeMap.h"
#include "renderer/Material.h"
#include "core/Primatives.h"
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

	SceneCamera camera;

	Material mat = Material("Assets/Materials/Backpack.mat");
	auto modelEntity = InstantiateModel("Assets/Model/Survival_BackPack_2.fbx", &mat);
	auto trans = modelEntity.GetComponent<Transform>();

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		camera.Update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::RenderScene(camera);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void SetupEnviroment() {
	Enviroment enviorment;

	enviorment.skyboxMaterial = std::make_shared<Material>("Assets/Materials/Skybox.mat");
	enviorment.ambientColor = glm::vec3(0.734, 0.918, 1.0);
	enviorment.ambientStrength = 0.8;

	Renderer::SetEnviroment(enviorment);
}
