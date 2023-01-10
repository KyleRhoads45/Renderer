#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include "core/Components.h"
#include "core/SceneCamera.h"
#include "core/Input.h"
#include "renderer/Renderer.h"
#include "renderer/Texture.h"
#include "renderer/Model.h"
#include "ecs/Registry.h"

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

	Shader litShader;
	auto modelEntity = InstantiateModel("Assets/Model/Survival_BackPack_2.fbx");
	auto trans = modelEntity.GetComponent<Transform>();

	Texture texture("Assets/Model/1001_albedo.jpg");
	Texture normalMap("Assets/Model/1001_normal.png");

	int map0 = 0;
	int map1 = 1;
	glUniform1iv(glGetUniformLocation(litShader.shaderId, "albedo"), 1, &map0);
	glUniform1iv(glGetUniformLocation(litShader.shaderId, "normalMap"), 1, &map1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap.id);

	SceneCamera camera;
	glEnable(GL_DEPTH_TEST);

	//float angle = 0.0;
	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		camera.Update();
		//angle += 0.0001;
		//trans->SetRotation(glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0)));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::RenderScene(camera);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}
