#pragma once 

#include "PCH.hpp"
#include <GLFW/glfw3.h>
#include "glad/glad.h"

class Emulator {
	public:
		Emulator() {}
		~Emulator() {}

		static void start(char* romFilename);
		static void reset();
		static void quit();

	private:
		static void createWindow();
		static void loadROM(char* romFilename);
	
		//static CPU cpu;
		//static GPU gpu;
		//static ROM rom;
		//static Memory mem;
		static GLFWwindow* window;

		static bool emuRunning;
		//static Time emuStartTime;

};