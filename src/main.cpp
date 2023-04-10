#include <iostream>
#include <filesystem>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include "core/Base.h"
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
#include "renderer/ShadowMapper.h"

void SetupEnviroment();
void InstantiateDemo();

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

	auto window = glfwCreateWindow(1920, 1080, "OpenGL Renderer", NULL, NULL);
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
	Input::Init(window);
	Renderer::Init();
	CameraSystem::Init();

	auto camEntity = Registry::Create();
	auto camTrans = Registry::Add<Transform>(camEntity);
	auto camera = Registry::Add<Camera>(camEntity);
	CameraSystem::SetActiveCameraEntity(camEntity);

	InstantiateDemo();

	while (!glfwWindowShouldClose(window)) {
		Input::Update(window);
		CameraSystem::Update();
		Renderer::RenderScene();
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
	Enviroment::Instance()->m_Skybox = CubeMap::Load(textures);

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.33, -0.33, -0.33));
	Enviroment::Instance()->SetLightDir(lightDir);
	Enviroment::Instance()->SetAmbientStrength(0.5f);

	ShadowMapper::Init(8192, 60.0f);
}

void InstantiateDemo() {
	static std::vector<Ref<Material>> mats;
	Shader litShader("src/shaders/lit.vert", "src/shaders/lit.frag");

	auto entity = Model::Instantiate("Assets/PolygonCity/City.fbx", NULL);
	auto trans = Registry::Get<Transform>(entity);
	trans->SetScale(glm::vec3(0.01f));

	i32 childIndex = 0;
	auto directory = std::filesystem::directory_iterator("Assets/PolygonCity/Textures");
	for (auto& file : directory) {
		auto mat = MakeRef<CityMaterial>(litShader, file.path().string());
		mats.push_back(mat);
		Registry::Get<MeshRenderer>(trans->GetChild(childIndex++).entity)->material = mat.get();
	}
}
