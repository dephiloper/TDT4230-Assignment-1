#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils/shader.h"
#include "utils/renderer.h"

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

Shader gameShader{};
Renderer renderer{};

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouseCallback(GLFWwindow *window, double xPos, double yPos) {
    // todo process mouse movement input
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    // todo process mouse press input
}

void mainLoop(GLFWwindow *window) {
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(window);
        
        // rendering
        glClearColor(0.18f, 0.4f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Marching Cubes", NULL, NULL);
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // // callback for window resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    renderer.init();
    mainLoop(window);

    glfwTerminate();
    return 0;
}