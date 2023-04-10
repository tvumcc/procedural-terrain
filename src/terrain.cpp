#include <glad/glad.h>
#include <iostream>

#include "terrain.hpp"

Terrain::Terrain(float width, float height, int subdivisions) {
    this->seed = 314159265;
    this->width = width;
    this->height = height;
    this->subdivisions = subdivisions;
    reset_settings();
    generate_mesh();
}

void Terrain::generate_mesh() {
	for (float z = 0.0f; z < height; z += height / subdivisions) {
		for (float x = 0.0f; x < width; x += width / subdivisions) {
			// First triangle
			mesh_vertices.push_back(x);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z);

			mesh_vertices.push_back(x);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z + height / subdivisions);

			mesh_vertices.push_back(x + width / subdivisions);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z);

			// Second triangle
			mesh_vertices.push_back(x + width / subdivisions);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z);

			mesh_vertices.push_back(x);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z + height / subdivisions);

			mesh_vertices.push_back(x + width / subdivisions);
			mesh_vertices.push_back(0.0f);
			mesh_vertices.push_back(z + height / subdivisions);
		}
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size() * sizeof(float), mesh_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Terrain::draw(Shader& shader, Camera& camera, PointLight& light) {
    glm::mat4 model = glm::mat4(1.0f);

    shader.bind();
    shader.set_mat4x4("model", model); 
    shader.set_mat4x4("vp", camera.vp_matrix());
    shader.set_float("x_pos", camera.xz_pos.x);
    shader.set_float("z_pos", camera.xz_pos.y);

    shader.set_float("amp", this->amplitude);
    shader.set_float("freq", this->frequency);
    shader.set_float("gain", this->gain);
    shader.set_float("lacunarity", this->lacunarity);
    shader.set_float("fudge", this->contrast);
    shader.set_int("seed", this->seed);

    shader.set_vec3("light_pos", glm::vec3(light.position[0], light.position[1], light.position[2]));
    shader.set_vec3("light_color", glm::vec3(light.color[0], light.color[1], light.color[2]));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh_vertices.size() / 3);
    glBindVertexArray(0);
}

void Terrain::reset_settings() {
    // Set defaults
    this->amplitude = 1.0f;
    this->frequency = 1.0f;
    this->gain = 0.5f;
    this->lacunarity = 2.0f;
    this->contrast = 1.2f;
}