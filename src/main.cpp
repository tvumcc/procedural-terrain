#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_image/stb_image.h>

#include "shader.hpp"
#include "camera.hpp"
#include "point_light.hpp"
#include "water.hpp"
#include "terrain.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

static float delta_time = 0.0f;
static float prev_time = 0.0f;
static float prev = 0.0f;
static float diff = 0.0f;
static unsigned int counter = 0;
static float FPS = 0.0f;
static bool wireframe = false;
static bool fps_counter = false;
static bool camera_mode = true;

static int window_width = 1000;
static int window_height = 1000;
static Camera cam(glm::vec3(0.0f, 0.0f, 0.0f));

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Procedural Terrain Generation by Thomas McConkey", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetWindowSize(window, &window_width, &window_height);
	framebuffer_size_callback(window, window_width, window_height);

	// Load the icon
	GLFWimage icon; 
	icon.pixels = stbi_load("assets/Earth.png", &icon.width, &icon.height, 0, 4);
	if (icon.pixels != NULL) {
		glfwSetWindowIcon(window, 1, &icon); 
	} else {
		std::cout << "ERROR: Could not find Earth.png" << std::endl;
	}
	stbi_image_free(icon.pixels);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);
	glfwPollEvents();

	Terrain terrain = Terrain(15.0f, 15.0f, 1200);
	cam.set_position(glm::vec3(terrain.get_width() / 2.0f, 1.5f, terrain.get_height() / 2.0f));
	PointLight light = PointLight();
	Water water;

	Shader terrain_shader("shaders/terrain/terrain.vert", "shaders/terrain/terrain.frag", "shaders/terrain/terrain.geom");
	Shader water_shader("shaders/water/water.vert", "shaders/water/water.frag");

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
		diff = time - prev;

		counter++;
		if (diff >= 1.0f / 30.0f) {
			FPS = (1.0f / diff) * counter;
			counter = 0;
			prev = time;
		}
		prev_time = time;
		
		process_input(window);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		terrain.draw(terrain_shader, cam, light);
		water.draw(water_shader, cam, light, terrain.get_width(), terrain.get_height());

		// ImGui stuff
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(350, 550), ImGuiCond_FirstUseEver);
		ImGui::Begin("Settings");
		if(ImGui::Button("Reset to Default Settings")) {
			terrain.reset_settings();
			water.reset_settings();
			light.reset_settings();
		}
		ImGui::Text("\nTerrain");
		if (ImGui::Button("Generate New Random Seed")) { 
			srand((int)(glfwGetTime() * 10000));
			terrain.seed = rand();
		}
		ImGui::Text(("Seed: " + std::to_string(terrain.seed)).c_str());
		ImGui::SliderFloat("Frequency", &terrain.frequency, 0.1f, 2.0f);
		ImGui::SliderFloat("Gain", &terrain.gain, 0.1f, 2.0f);
		ImGui::SliderFloat("Lacunarity", &terrain.lacunarity, 0.1f, 2.0f);
		ImGui::SliderFloat("Contrast", &terrain.contrast, 0.1f, 2.0f);
		ImGui::SliderFloat("Water Level", &water.water_level, -0.5f, 0.090f);

		ImGui::Text("\nLighting");
		ImGui::SliderFloat3("Light Position", light.position, -terrain.get_width(), terrain.get_width());
		ImGui::ColorEdit3("Light Color", light.color);

		ImGui::Text("\nMiscellaneous");
		ImGui::SliderFloat("Camera Speed", &cam.movement_speed, 3.0f, 10.0f);
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::Checkbox("FPS Counter", &fps_counter);
		
		ImGui::Text("\nControls");
		ImGui::Text("F - Toggles the mouse");
		ImGui::Text("WASD - Camera Movement");
		ImGui::Text("Space - Move camera up");
		ImGui::Text("Shift - Move camera down");
		ImGui::Text("Esc - Exit the program");
		ImGui::End();
		
		if (fps_counter) {
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 80, 10), ImGuiCond_FirstUseEver);
			ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
			ImGui::Text((std::string("FPS: ") + std::to_string((int)FPS)).c_str());
			ImGui::End();
		}

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
	if (camera_mode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.move(Direction::Forward, delta_time);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.move(Direction::Backward, delta_time);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.move(Direction::Left, delta_time);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.move(Direction::Right, delta_time);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.move(Direction::Up, delta_time);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cam.move(Direction::Down, delta_time);
	}
}

// Processes single click input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Toggles Camera Movement Mode
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		camera_mode = !camera_mode;
		if (camera_mode) {
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
	
	if (camera_mode)
		cam.rotate(x_offset, y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	cam.set_aspect_ratio(width, height);
	window_width = width;
	window_height = height;
}
