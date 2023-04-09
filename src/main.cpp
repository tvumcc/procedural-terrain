#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_image/stb_image.h>

#include "shader.hpp"
#include "camera.hpp"
#include "settings.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

static float delta_time = 0.0f;
static float prev_time = 0.0f;
static float width = 15;
static float height = 15;
static unsigned int subdivisions = 1200;
static glm::vec2 xz_pos(0.0f, 0.0f);
static glm::vec2 last_xz_pos(0.05f, 0.05f);
static Settings settings = Settings();

static int window_width = 1000;
static int window_height = 800;
static Camera cam(glm::vec3(width / 2.0f, 1.5f, height / 2.0f));

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
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

	float water_mesh[] {
		 7.5f,  0.0f, 7.5f,   256.0f, 256.0f,// top right
		 7.5f,  0.0f, -7.5f,  256.0f, 0.0f,// bottom right
		-7.5f,  0.0f, -7.5f,  0.0f, 0.0f,// bottom left
		-7.5f,  0.0f, 7.5f,   0.0f, 256.0f,// top left 	
	};

	unsigned int water_indices[] {
		0, 1, 2,
		0, 2, 3,
	};

	glfwInit();
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Procedural Terrain Generation", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetWindowSize(window, &window_width, &window_height);
	framebuffer_size_callback(window, window_width, window_height);
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


	unsigned int waterVAO, waterVBO, waterEBO;
	glGenVertexArrays(1, &waterVAO);
	glBindVertexArray(waterVAO);

	glGenBuffers(1, &waterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(water_mesh), water_mesh, GL_STATIC_DRAW);

	glGenBuffers(1, &waterEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(water_indices), water_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // The texture will be repeated (tiled) on the s axis (x) when the texture coordinates exceed 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // The texture will be repeated (tiled) on the t axis (x) when the texture coordinates exceed 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Will linearly interpolate between the two closest mipmaps and then linear interpolate the texel color when the texture is minified
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Uses linear interpolation to sample the color of a texel when the texture is magnified

	int texture_width, texture_height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("assets/water.jpg", &texture_width, &texture_height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	Shader shader("shaders/terrain/terrain.vert", "shaders/terrain/terrain.frag", "shaders/terrain/terrain.geom");
    shader.bind();
	
    glm::vec3 terrain_color = glm::vec3(36.0f, 140.0f, 64.0f) / 255.0f;
	glm::vec3 terrain_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    // shader.set_vec3("color", terrain_color);

	Shader water("shaders/water/water.vert", "shaders/water/water.frag");
	water.bind();
	glm::vec3 water_color = glm::vec3(0.0f, 187.0f, 255.0f) / 255.0f;
	glm::vec3 water_pos = glm::vec3(0.0f, -0.1f, 0.0f);
	water.set_vec3("color", water_color);
	water.set_int("water", 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	while (!glfwWindowShouldClose(window)) {
		float time = (float)glfwGetTime();
		delta_time = time - prev_time;
		prev_time = time;
		
		process_input(window);
		glPolygonMode(GL_FRONT_AND_BACK, settings.wireframe_rendering ? GL_LINE : GL_FILL);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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

		shader.set_float("amp", settings.amplitude);
		shader.set_float("freq", settings.frequency);
		shader.set_float("gain", settings.gain);
		shader.set_float("lacunarity", settings.lacunarity);
		shader.set_float("fudge", settings.fudge);
		shader.set_int("seed", settings.seed);

		shader.set_vec3("light_pos", glm::vec3(settings.light_position[0], settings.light_position[1], settings.light_position[2]));
		shader.set_vec3("light_color", glm::vec3(settings.light_color[0], settings.light_color[1], settings.light_color[2]));

		glDrawArrays(GL_TRIANGLES, 0, terrain_mesh_vertices.size() / 3);
		glBindVertexArray(0);

		water.bind();
		glBindVertexArray(waterVAO);
		glm::mat4 waterModel = glm::mat4(1.0f);
		waterModel = glm::translate(waterModel, glm::vec3(width / 2.0f, -0.1f, height / 2.0f));
		waterModel = glm::scale(waterModel, glm::vec3(1.0f, 1.0f, 1.0f));
		water.set_mat4x4("model", waterModel);
		water.set_mat4x4("vp", cam.vp_matrix());
		water.set_float("water_level", settings.water_level);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
		ImGui::Begin("Settings");
		if(ImGui::Button("Reset to Default Settings")) {
			settings.reset();
		}
		ImGui::Text("Terrain");
		if (ImGui::Button("Generate New Random Seed")) { 
			srand((int)(glfwGetTime() * 10000));
			settings.seed = rand();
		}
		ImGui::Text(("Seed: " + std::to_string(settings.seed)).c_str());
		ImGui::SliderFloat("Frequency", &settings.frequency, 0.1f, 2.0f);
		ImGui::SliderFloat("Gain", &settings.gain, 0.1f, 2.0f);
		ImGui::SliderFloat("Lacunarity", &settings.lacunarity, 0.1f, 2.0f);
		ImGui::SliderFloat("Contrast", &settings.fudge, 0.1f, 2.0f);
		ImGui::SliderFloat("Water Level", &settings.water_level, -0.5f, 0.090f);

		ImGui::Text("Lighting");
		ImGui::SliderFloat3("Light Position", settings.light_position, (float)-width, (float)width);
		ImGui::ColorEdit3("Light Color", settings.light_color);

		ImGui::Text("Miscellaneous");
		ImGui::SliderFloat("Camera Speed", &settings.camera_speed, 3.0f, 10.0f);
		ImGui::Checkbox("Wireframe", &settings.wireframe_rendering);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

// Processes continuous input
void process_input(GLFWwindow* window) {
	// Movement Controls
	if (settings.camera_mode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			last_xz_pos = xz_pos;
			// Ensures that movement speed doesn't slow down when looking directly up or directly down
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
}

// Processes single click input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Toggles Camera Movement Mode
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		settings.camera_mode = !settings.camera_mode;
		if (settings.camera_mode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	// Closes the program
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
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
	
	if (settings.camera_mode)
		cam.rotate(x_offset, y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	cam.set_aspect_ratio(width, height);
	window_width = width;
	window_height = height;
}
