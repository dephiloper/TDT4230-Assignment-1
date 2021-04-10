#include "shader.hpp"

/**
* https://stackoverflow.com/a/2602258
*/
std::string Shader::readFile(const std::string &filepath) {
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream buffer;
    try {
        stream.open(filepath);
        buffer << stream.rdbuf();
        stream.close();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath << std::endl;
    }
    return buffer.str();
}

int Shader::load(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);
    const char *vShaderSource = vertexSource.c_str();  // https://stackoverflow.com/a/30804288
    const char *fShaderSource = fragmentSource.c_str();

    // compile the shader
    unsigned int vertex, fragment;

    // shader creation and compilation
    vertex = createShader(vShaderSource, GL_VERTEX_SHADER, "VERTEX");
    fragment = createShader(fShaderSource, GL_FRAGMENT_SHADER, "FRAGMENT");

    // shader program creation and linking
    ID = glCreateProgram();

    if (ID == 0) {
        std::cout << "Error occurred while creating shader program." << std::endl;
        return -1;
    }

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    int success;
    char infoLog[512];

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return 0;
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 value) {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, glm::vec4 value) {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

unsigned int Shader::createShader(const char *shaderSource, GLint shaderType, const std::string &shaderName) {
    int success;
    char infoLog[512];

    unsigned int shaderId;

    // shader creation and compilation
    shaderId = glCreateShader(shaderType);

    glShaderSource(shaderId, 1, &shaderSource, nullptr);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::" << shaderName << "::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shaderId;
}