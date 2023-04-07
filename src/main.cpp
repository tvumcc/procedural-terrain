#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "settings.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

static float delta_time = 0.0f;
static float prev_time = 0.0f;
static float width = 10;
static float height = 10;
static unsigned int subdivisions = 800;
static Settings settings = Settings();
static glm::vec2 xz_pos(0.0f, 0.0f);
static glm::vec2 last_xz_pos(0.05f, 0.05f);
static Camera cam(glm::vec3(5.0f, 1.5f, 5.0f));

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

int main() {
	std::vector<int> p_table(256);
	std::mt19937 rng(settings.seed);
	for (int i = 0; i < p_table.size(); i++) {
		p_table[i] = i;
	}
	std::shuffle(p_table.begin(), p_table.end(), rng);
	for (int i = 0; i < 256; i++) {
		p_table.push_back(i);
	}

	std::vector<float> terrain_mesh_vertices;

	for (float z = 0.0f; z < height; z += height / subdivisions) {
		for (float x = 0.0f; x < width; x += width / subdivisions) {
			// First triangle
			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z + height / subdivisions);

			terrain_mesh_vertices.push_back(x + width / subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			// Second triangle
			terrain_mesh_vertices.push_back(x + width / subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z + height / subdivisions);

			terrain_mesh_vertices.push_back(x + width / subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z + height / subdivisions);
		}
	}	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(900, 900, "Procedural Terrain Generation", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, terrain_mesh_vertices.size() * sizeof(float), terrain_mesh_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader shader("shaders/terrain/terrain.vert", "shaders/terrain/terrain.frag", "shaders/terrain/terrain.geom");
    shader.bind();
    glm::vec3 terrain_color = glm::vec3(36.0f, 140.0f, 64.0f) / 255.0f;
	glm::vec3 terrain_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    shader.set_vec3("color", terrain_color);
	glUniform1iv(glGetUniformLocation(shader.ID, "permutation_table"), 512, p_table.data());

	Shader water("shaders/water/water.vert", "shaders/water/water.frag");
	water.bind();
	glm::vec3 water_color = glm::vec3(27.0f, 191.0f, 255.0f) / 255.0f;
	glm::vec3 water_pos = glm::vec3(0.0f, -0.1f, 0.0f);
	water.set_vec3("color", water_color);

	cam.set_aspect_ratio(900, 900);
	while (!glfwWindowShouldClose(window)) {
		float time = (float)glfwGetTime();
		delta_time = time - prev_time;
		prev_time = time;
		
		process_input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, terrain_pos);
		
		shader.bind();
		shader.set_mat4x4("model", model); 
		shader.set_mat4x4("vp", cam.vp_matrix());
		shader.set_float("x_pos", xz_pos.x);
		shader.set_float("z_pos", xz_pos.y);
		shader.set_float("last_x_pos", last_xz_pos.x);
		shader.set_float("last_z_pos", last_xz_pos.y);
		glDrawArrays(GL_TRIANGLES, 0, terrain_mesh_vertices.size() / 3);

		water.bind();
		glm::mat4 waterModel = glm::mat4(1.0f);
		waterModel = glm::translate(model, water_pos);
		water.set_mat4x4("model", waterModel);
		water.set_mat4x4("vp", cam.vp_matrix());
		water.set_float("time", time);
		glDrawArrays(GL_TRIANGLES, 0, terrain_mesh_vertices.size() / 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		last_xz_pos = xz_pos;
		xz_pos -= glm::vec2(glm::cross(cam.get_right(), glm::vec3(0.0f, 1.0f, 0.0f)).x, glm::cross(cam.get_right(), glm::vec3(0.0f, 1.0f, 0.0f)).z) * settings.camera_speed * delta_time;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		last_xz_pos = xz_pos;
		xz_pos += glm::vec2(glm::cross(cam.get_right(), glm::vec3(0.0f, 1.0f, 0.0f)).x, glm::cross(cam.get_right(), glm::vec3(0.0f, 1.0f, 0.0f)).z) * settings.camera_speed * delta_time;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		last_xz_pos = xz_pos;
		xz_pos -= glm::vec2(cam.get_right().x, cam.get_right().z) * settings.camera_speed * delta_time;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		last_xz_pos = xz_pos;
		xz_pos += glm::vec2(cam.get_right().x, cam.get_right().z) * settings.camera_speed * delta_time;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.move(Direction::Up, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cam.move(Direction::Down, delta_time);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {

	static float last_x = 900.0f / 2;
	static float last_y = 900.0f / 2;

	if (cam.get_first_mouse() == true) {
		last_x = (float)x_pos;
		last_y = (float)y_pos;
		cam.unset_first_mouse();
	}

	float x_offset = (float)x_pos - last_x;
	float y_offset = last_y - (float)y_pos;
	last_x = (float)x_pos;
	last_y = (float)y_pos;

	cam.rotate(x_offset, y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
