#include <glm/gtx/quaternion.hpp>
#include "Transform.h"

void Transform::SetPosition(glm::vec3 pos) {
	position = pos;
	RecalculateModel();
}

void Transform::SetRotation(glm::quat rot) {
	rotation = rot;
	RecalculateModel();
}

void Transform::SetScale(glm::vec3 scl) {
	scale = scl;
	RecalculateModel();
}

void Transform::Set(glm::vec3 pos, glm::quat rot, glm::vec3 scl) {
	position = pos;
	rotation = rot;
	scale = scl;
	RecalculateModel();
}

void Transform::RecalculateModel() {
	model = glm::translate(glm::mat4(1.0f), position);
	model = model * glm::mat4_cast(rotation);
	model = glm::scale(model, scale);

	for (int i = 0; i < children.size(); i++) {
		children[i]->UpdateModelRecursive(model);
	}
}

void Transform::UpdateModelRecursive(glm::mat4 parentModel) {
	glm::mat4 localModel = glm::translate(glm::mat4(1.0f), position);
	localModel = localModel * glm::mat4_cast(rotation);
	localModel = glm::scale(localModel, scale);
	model = parentModel * localModel;

	for (int i = 0; i < children.size(); i++) {
		children[i]->UpdateModelRecursive(model);
	}
}

glm::vec3 Transform::TranformPosition(const glm::vec3& pos) {
	glm::vec4 globalPos = model * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
	return glm::vec3(globalPos.x, globalPos.y, globalPos.z);
}

glm::vec3 Transform::InverseTransformPosition(const glm::vec3& pos) {
	glm::vec4 globalPos = glm::inverse(model) * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
	return glm::vec3(globalPos.x, globalPos.y, globalPos.z);
}

uint32_t Transform::GetChildCount() {
	return children.size();
}

Transform& Transform::GetChild(const uint32_t index) {
	assert(index < children.size());
	return *children[index];
}

void Transform::AddChild(Transform* child) {
	children.push_back(child);
}

void Transform::SetParent(Transform* newParent) {
	parent = newParent;
	UpdateModelRecursive(parent->Model());
}

glm::vec3 Transform::Position() {
	if (parent == nullptr) {
		return position;
	}

	// Position is stored in the 3rd column of TRS matrix
	// Each column in glm matrix is a vector4
	return glm::vec3(model[3]);
}

glm::quat Transform::Rotation() {
	if (parent == nullptr) {
		return rotation;
	}

	// Rotation data is stored in the top 3 cols and rows of matrix
	// Covert mat4 to mat3 then cast that to quaternion
	return glm::quat_cast(glm::mat3(model));
}

glm::vec3 Transform::Scale() {
	if (parent == nullptr) {
		return scale;
	}

	// x 0 0 0
	// 0 y 0 0
	// 0 0 z 0
	// 0 0 0 1
	// Scale is stored in vectors 0, 1, 2
	// The length of these vectors are equivalent to scale

	float x = glm::length(glm::vec3(model[0]));
	float y = glm::length(glm::vec3(model[1]));
	float z = glm::length(glm::vec3(model[2]));
	return glm::vec3(x, y, z);
}

glm::vec3 Transform::LocalPosition() {
	return position;
	if (parent == nullptr) {
		return position;
	}
	return parent->InverseTransformPosition(position);
}

glm::quat Transform::LocalRotation() {
	return rotation;
}

glm::vec3 Transform::LocalScale() {
	return scale;
}

glm::vec3 Transform::Forward() {
	return glm::rotate(Rotation(), glm::vec3(0, 0, -1));
}

glm::vec3 Transform::Right() {
	return glm::rotate(Rotation(), glm::vec3(1, 0, 0));
}

glm::vec3 Transform::Up() {
	return glm::rotate(Rotation(), glm::vec3(0, 1, 0));
}

glm::mat4 Transform::Model() {
	return model;
}
