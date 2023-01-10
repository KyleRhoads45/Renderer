#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Components.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void Transform::SetRotation(glm::quat rot) {
	rotation = rot;

	model = glm::translate(glm::mat4(1.0f), position);
	model = model * glm::mat4_cast(rotation);
	model = glm::scale(model, scale);

	for (int i = 0; i < childCount; i++) {
		children[i]->UpdateModel(model);
	}
}

void Transform::SetAll(glm::vec3 p, glm::quat r, glm::vec3 s) {
	position = p;
	rotation = r;
	scale = s;

	model = glm::translate(glm::mat4(1.0f), position);
	model = model * glm::mat4_cast(rotation);
	model = glm::scale(model, scale);
}

void Transform::UpdateModel(glm::mat4 parentModel) {
	glm::mat4 localModel = glm::translate(glm::mat4(1.0f), position);
	localModel = localModel * glm::mat4_cast(rotation);
	localModel = glm::scale(localModel, scale);
	model = parentModel * localModel;

	for (int i = 0; i < childCount; i++) {
		children[i]->UpdateModel(model);
	}
}
