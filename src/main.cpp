#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

Camera cam(glm::vec3(0.0f, 1.0f, 2.0f));
int main() {
	std::vector<int> p_table(256);
	std::mt19937 rng(456789654);
	for (int i = 0; i < p_table.size(); i++) {
		p_table[i] = i;
	}
	std::shuffle(p_table.begin(), p_table.end(), rng);
	for (int i = 0; i < p_table.size(); i++) {
		std::cout << p_table[i] << std::endl;	
	}

	unsigned int width = 1600;
	unsigned int height = 1600;
	unsigned int subdivisions = 40;
	std::vector<float> terrain_mesh_vertices;

	for (float z = 0.0f; z < (float)(height / 100.0f); z += (float)(height / 100.0f) / (float)subdivisions) {
		for (float x = 0.0f; x < (float)(width / 100.0f); x += (float)(width / 100.0f) / (float)subdivisions) {
			// First triangle
			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z+(float)(height / 100.0f) / (float)subdivisions);

			terrain_mesh_vertices.push_back(x+(float)(width / 100.0f) / (float)subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			// Second triangle
			terrain_mesh_vertices.push_back(x+(float)(width / 100.0f) / (float)subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z);

			terrain_mesh_vertices.push_back(x);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z+(float)(height / 100.0f) / (float)subdivisions);

			terrain_mesh_vertices.push_back(x+(float)(width / 100.0f) / (float)subdivisions);
			terrain_mesh_vertices.push_back(0.0f);
			terrain_mesh_vertices.push_back(z+(float)(height / 100.0f) / (float)subdivisions);
		}
	}	


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Using version 3.3 of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(900, 900, "Procedural Terrain Generation", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // Initialize GLAD
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, terrain_mesh_vertices.size() * sizeof(float), terrain_mesh_vertices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

    // Create a shader
	Shader shader("shaders/main.vert", "shaders/main.frag");
	cam.set_aspect_ratio(900, 900);

    // Setting a uniform example
    shader.bind();
    glm::vec3 triangle_color = glm::vec3(36.0f, 140.0f, 64.0f);
	triangle_color /= 255.0f;
	glm::vec3 triangle_pos = glm::vec3(-2.0f, 0.0f, -1.0f);
    shader.set_vec3("color", triangle_color);
	glUniform1iv(glGetUniformLocation(shader.ID, "permutation_table"), 256, p_table.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		// Calculate deltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

        // Clear the screen with this color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, triangle_pos);
		shader.set_mat4x4("model", model); 
		shader.set_mat4x4("vp", cam.vp_matrix());
        // Draw the vertices
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.move(Direction::Forward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.move(Direction::Backward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.move(Direction::Left, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.move(Direction::Right, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.move(Direction::Up, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cam.move(Direction::Down, deltaTime);
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
