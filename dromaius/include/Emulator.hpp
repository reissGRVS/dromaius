#pragma once 

#include "PCH.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Emulator {
	public:
		Emulator() {}
		~Emulator() {}

		static void start(GLFWwindow* context);
		static void reset();
		static void quit();

	private:
		static void loadROM(char* romFilename);
	
		//static CPU cpu;
		//static GPU gpu;
		//static ROM rom;
		//static Memory mem;

		static GLFWwindow* window;
		static bool emuRunning;
		//static Time emuStartTime;

};