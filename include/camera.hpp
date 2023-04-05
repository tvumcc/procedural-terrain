#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction {
	Up, Down, Left, Right, Forward, Backward
};

class Camera {
public:
	Camera(glm::vec3 position);
	void move(Direction direction, float deltaTime);
	void rotate(float x_offset, float y_offset);

	glm::mat4 view_matrix();
	glm::mat4 proj_matrix();
	glm::mat4 vp_matrix();

	bool get_first_mouse();
	void unset_first_mouse();

	glm::vec3 get_position();
	glm::vec3 get_front();
	glm::vec3 get_right();

	void set_fov(float fov);
	void set_aspect_ratio(int width, int height);
	void set_speed(float speed);
	void set_sensitivity(float sensitivity);
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	float yaw;
	float pitch;
	float fov;
	float aspect_ratio;

	float movement_speed;
	float mouse_sensitivity;
	bool first_mouse;

	void update_camera_vectors();
};
