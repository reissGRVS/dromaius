#include "PCH.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Emulator.hpp"


void Emulator::start(char* filename) {
	DBG_INFO("Emulator starting");

	createWindow();

	while(!glfwWindowShouldClose(window))
	{
	    glfwSwapBuffers(window);
	    glfwPollEvents();    
	}


	glfwTerminate();
	return;
}

void Emulator::createWindow() {
 	

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
	    DBG_SEVERE("Failed to create GLFW Window");
	    glfwTerminate();
	    return;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
	    std::cout << "Failed to initialize GLAD" << std::endl;
	    return;
	}

	glViewport(0, 0, 800, 600);

}

void Emulator::loadROM(char* romFilename) {

}

void Emulator::reset() {

}

void Emulator::quit() {

	return;
}

//CPU Emulator::cpu;
//GPU Emulator::gpu;
//ROM Emulator::rom;
//Memory Emulator::mem;
GLFWwindow* Emulator::window;

bool Emulator::emuRunning;
//Time Emulator::emuStartTime;