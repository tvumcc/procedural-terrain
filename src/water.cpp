#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "water.hpp"

Water::Water() {
	reset_settings();

	float water_mesh[] {
		 0.5f,  0.0f, 0.5f,   1.0f, 1.0f,
         0.5f,  0.0f, -0.5f,  1.0f, 0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
		-0.5f,  0.0f, 0.5f,   0.0f, 1.0f,
	};

	unsigned int water_indices[] {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(water_mesh), water_mesh, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(water_indices), water_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Water::draw(Shader& shader, Camera& camera, PointLight& light, float width, float height) {
    shader.bind();
    shader.set_vec3("color", color);
    shader.set_vec3("light_color", glm::vec3(light.color[0], light.color[1], light.color[2]));

    glm::mat4 model = glm::mat4(1.0f);    
    model = glm::translate(model, glm::vec3(width / 2.0f, -0.1f, height / 2.0f));
	model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
    shader.set_mat4x4("model", model);
    shader.set_mat4x4("vp", camera.vp_matrix());
    shader.set_float("water_level", water_level);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Water::reset_settings() {
    this->water_level = 0.055f;
	this->color = glm::vec3(0.0f, 188.0f, 255.0f) / 255.0f;
}