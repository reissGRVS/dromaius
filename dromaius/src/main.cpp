// Local Headers
#include "glitter.hpp"
#include "PCH.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Emulator.hpp"

// Standard Headers
#include <cstdio>
#include <cstdlib>

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    if (!glfwInit()){
        DBG_SEVERE("Failed to init GLFW");
        return EXIT_FAILURE;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);
    // Check for Valid Context
    if (window == nullptr) {
        DBG_SEVERE("Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }


    Emulator::start(window);
    
    return EXIT_SUCCESS;
}
