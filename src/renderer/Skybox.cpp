#include <glad/glad.h>
#include "ecs/Registry.h"
#include "core/Components.h"
#include "Skybox.h"

Entity InstantiateSkybox() {
	auto entity = Registry::Create();
	auto trans = Registry::AddComponent<Transform>(entity);
	auto meshRenderer = Registry::AddComponent<MeshRenderer>(entity);

	meshRenderer->mesh = LoadMesh("Assets/Model/cube.fbx");

	return entity;
}
