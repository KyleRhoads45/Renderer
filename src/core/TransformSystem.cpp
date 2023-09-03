#include "Components.h"
#include "ecs/View.h"
#include "ecs/Registry.h"
#include "TransformSystem.h"

void TransformSystem::Update() {
	for (Entity entity : View<Transform>()) {
		auto& trans = Registry::Get<Transform>(entity);

		trans.model = glm::translate(glm::mat4(1.0f), trans.position);
		trans.model = trans.model * glm::mat4_cast(trans.rotation);
		trans.model = glm::scale(trans.model, trans.scale);
	}
}
