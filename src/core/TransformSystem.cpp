#include "Components.h"
#include "ecs/View.h"
#include "ecs/Registry.h"
#include "TransformSystem.h"

void TransformSystem::Update() {
	auto rootView = View<LocalToWorld, Transform, Children>().Exclude<Parent>();
	for (const Entity entity : rootView) {
		UpdateLocalToWorld(entity);
	}
}

void TransformSystem::UpdateLocalToWorld(const Entity entity) {
	auto& toWorld = Registry::Get<LocalToWorld>(entity);
	auto& trans = Registry::Get<Transform>(entity);

	toWorld.matrix = glm::translate(glm::mat4(1.0f), trans.position);
	toWorld.matrix = toWorld.matrix * glm::mat4_cast(trans.rotation);
	toWorld.matrix = glm::scale(toWorld.matrix, trans.scale);

	if (Registry::Has<Parent>(entity)) {
		auto& parent = Registry::Get<Parent>(entity);
		auto& parentLTW = Registry::Get<LocalToWorld>(parent.entity);
		toWorld.matrix = parentLTW.matrix * toWorld.matrix;
	}

	if (Registry::Has<Children>(entity)) {
		auto& children = Registry::Get<Children>(entity);
		for (auto& childEntity : children.entities) {
			UpdateLocalToWorld(childEntity);
		}
	}
}
