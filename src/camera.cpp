#include "camera.hpp"

Camera::Camera(glm::vec3 position) {
	this->position = position;
	
	this->aspect_ratio = 1.0f;
	this->fov = 45.0f;
	this->yaw = -90.0f; 
	this->pitch = 0.0f;

	this->movement_speed = 5.0f;
	this->mouse_sensitivity = 0.1f;

	this->first_mouse = true;
	update_camera_vectors();
}	


void Camera::move(Direction direction, float deltaTime) {
	if (direction == Direction::Forward)  xz_pos -= glm::vec2(glm::cross(right, glm::vec3(0.0f, 1.0f, 0.0f)).x, glm::cross(right, glm::vec3(0.0f, 1.0f, 0.0f)).z) * movement_speed * deltaTime;
	if (direction == Direction::Backward) xz_pos += glm::vec2(glm::cross(right, glm::vec3(0.0f, 1.0f, 0.0f)).x, glm::cross(right , glm::vec3(0.0f, 1.0f, 0.0f)).z) * movement_speed * deltaTime;
	if (direction == Direction::Left)     xz_pos -= glm::vec2(right.x, right.z) * movement_speed * deltaTime;
	if (direction == Direction::Right)    xz_pos += glm::vec2(right.x, right.z) * movement_speed * deltaTime;
	if (direction == Direction::Up)       position += glm::vec3(0.0f, 1.0f, 0.0f) * movement_speed * deltaTime;
	if (direction == Direction::Down)     position -= glm::vec3(0.0f, 1.0f, 0.0f) * movement_speed * deltaTime;
}

void Camera::rotate(float x_offset, float y_offset) { 
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	yaw += x_offset;
	pitch += y_offset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	update_camera_vectors();
}

glm::mat4 Camera::vp_matrix() {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f) * glm::lookAt(position, position + front, up);
}

bool Camera::get_first_mouse() {return this->first_mouse;}
void Camera::unset_first_mouse() {this->first_mouse = false;}

glm::vec3 Camera::get_position() {return this->position;}
void Camera::set_position(glm::vec3 pos) {this->position = pos;}
glm::vec3 Camera::get_front() {return this->front;}
glm::vec3 Camera::get_right() {return this->right;}

void Camera::set_aspect_ratio(int width, int height) {this->aspect_ratio = (float)width/(float)height;}

void Camera::reset_settings() {
	this->movement_speed = 5.0f;
}

void Camera::update_camera_vectors() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	direction.y = sin(glm::radians(this->pitch));
	direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	front = glm::normalize(direction);
	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));
}
