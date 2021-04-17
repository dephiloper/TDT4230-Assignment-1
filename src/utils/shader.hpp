#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // the program ID
    unsigned int ID;
    
    // constructor reads and builds the shader
    int load(const std::string& vertexPath, const std::string& fragmentPath);

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec3(const std::string &name, glm::vec3 value);
    void setVec4(const std::string &name, glm::vec4 value);
    void setMat4(const std::string &name, glm::mat4 value);

private:
    static std::string readFile(const std::string& filepath);
    static unsigned int createShader(const char* shaderSource, GLint shaderType, const std::string& shaderName);
};

#endif //SHADER_HPP