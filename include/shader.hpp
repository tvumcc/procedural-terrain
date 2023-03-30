// Thomas McConkey (piigle) 10/15/2022
// This file makes dealing with shaders 10 times easier
// Inspired by learnopengl.com
// Usage: Shader shader_name = Shader(vertex_shader_source_path, fragment_shader_source_path);

#pragma once
#include <glm/glm.hpp>

#include <string>

enum class ShaderType {
    Vertex,
    Fragment,
    ShaderProgram,
};

class Shader {
public:
    unsigned int ID;
    
    Shader(const char* vertex_shader_path, const char* fragment_shader_path);

    /*Uniform setting methods*/

    void set_int(const std::string& variable_name, int value);
    void set_float(const std::string& variable_name, float value);
    void set_bool(const std::string& varibale_name, bool value);
    void set_mat4x4(const std::string& variable_name, glm::mat4 value);
    void set_vec3(const std::string& variable_name, glm::vec3 value);
    void set_vec2(const std::string& variable_name, glm::vec2 value);

    /*Binding methods*/

    void bind();
    void unbind();
private:
    const char* vertex_shader_file_name;
    const char* fragment_shader_file_name;
    void checkErrors(unsigned int ID, ShaderType type);
};