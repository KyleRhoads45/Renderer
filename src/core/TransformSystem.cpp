#include "Components.h"
#include "ecs/View.h"
#include "TransformSystem.h"

void TransformSystem::Update(Registry& registry) {
	 auto rootView = View<LocalToWorld, Transform, Children>(registry).Exclude<Parent>();
	for (const Entity entity : rootView) {
		UpdateLocalToWorld(registry, entity);
	}
}

void TransformSystem::UpdateLocalToWorld(Registry& registry, const Entity entity) {
	auto& toWorld = registry.Get<LocalToWorld>(entity);
	auto& trans = registry.Get<Transform>(entity);

	toWorld.matrix = glm::translate(glm::mat4(1.0f), trans.position);
	toWorld.matrix = toWorld.matrix * glm::mat4_cast(trans.rotation);
	toWorld.matrix = glm::scale(toWorld.matrix, trans.scale);

	if (registry.Has<Parent>(entity)) {
		auto& parent = registry.Get<Parent>(entity);
		auto& parentLTW = registry.Get<LocalToWorld>(parent.entity);
		toWorld.matrix = parentLTW.matrix * toWorld.matrix;
	}

	if (registry.Has<Children>(entity)) {
		auto& children = registry.Get<Children>(entity);
		for (auto& childEntity : children.entities) {
			UpdateLocalToWorld(registry, childEntity);
		}
	}
}
