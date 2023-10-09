#include "Components.h"
#include <iostream>

Camera::Camera() {
    yaw = 0.0;
    pitch = 0.0;
    fov = 60.0f;
    aspect = 1920.0f / 1080.0f;
    near = 0.01f;
    far = 500.0f;
    view = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

glm::mat4 Camera::ViewProjection() const {
    return projection * view;
}

static bool cameras_are_equal(const Camera& cam1, const Camera& cam2) {
    return (
        cam1.fov == cam2.fov &&
        cam1.aspect == cam2.aspect &&
        cam1.near == cam2.near &&
        cam1.far == cam2.far &&
        cam1.view == cam2.view &&
        cam1.projection == cam2.projection
    );
}

bool Camera::operator!=(const Camera& other) const {
    return !cameras_are_equal(*this, other);
}

bool Camera::operator==(const Camera& other) const {
    return cameras_are_equal(*this, other);
}

glm::vec3 Transform::Forward() const {
	return glm::rotate(rotation, glm::vec3(0, 0, -1));
}

glm::vec3 Transform::Right() const {
	return glm::rotate(rotation, glm::vec3(1, 0, 0));
}

glm::vec3 Transform::Up() const {
	return glm::rotate(rotation, glm::vec3(0, 1, 0));
}
