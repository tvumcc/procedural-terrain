#include "camera.hpp"

Camera::Camera(glm::vec3 position) {
	this->position = position;
	
	this->aspect_ratio = 800.0f/600.0f;
	this->fov = 45.0f;
	this->yaw = -90.0f; 
	this->pitch = 0.0f;

	this->movement_speed = 2.5f;
	this->mouse_sensitivity = 0.1f;

	this->first_mouse = true;
	update_camera_vectors();
}	


void Camera::move(Direction direction, float deltaTime) {
	if (direction == Direction::Forward) position += front * movement_speed * deltaTime;
	if (direction == Direction::Backward) position -= front * movement_speed * deltaTime;
	if (direction == Direction::Right) position += right * movement_speed * deltaTime;
	if (direction == Direction::Left) position -= right * movement_speed * deltaTime;
	if (direction == Direction::Up) position += up * movement_speed * deltaTime;
	if (direction == Direction::Down) position -= up * movement_speed * deltaTime;
}

void Camera::rotate(float x_offset, float y_offset) { x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	yaw += x_offset;
	pitch += y_offset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = 89.0f;

	update_camera_vectors();
}

glm::mat4 Camera::view_matrix() {
	glm::mat4 view = glm::mat4(1.0f);
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::proj_matrix() {
	glm::mat4 proj = glm::mat4(1.0f);
	return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
}

glm::mat4 Camera::vp_matrix() {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f) * glm::lookAt(position, position + front, up);
}

bool Camera::get_first_mouse() {return this->first_mouse;}
void Camera::unset_first_mouse() {this->first_mouse = false;}

void Camera::set_fov(float fov) {this->fov = fov;}
void Camera::set_aspect_ratio(int width, int height) {this->aspect_ratio = (float)width/(float)height;}
void Camera::set_speed(float speed) {this->movement_speed = speed;}
void Camera::set_sensitivity(float sensitivity) {this->mouse_sensitivity = sensitivity;}

void Camera::update_camera_vectors() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	direction.y = sin(glm::radians(this->pitch));
	direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	front = glm::normalize(direction);
	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));
}
