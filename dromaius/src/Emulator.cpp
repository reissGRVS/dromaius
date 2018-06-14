#include "PCH.hpp"

#include "Emulator.hpp"


void Emulator::start(GLFWwindow* context) {
	window = context;
	DBG_INFO("Emulator starting");

	// Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Rendering Loop
    while (glfwWindowShouldClose(window) == false) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            DBG_INFO("Window set to close");
        }

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glfwTerminate();
	return;
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