#pragma once
#include <string>
#include "cpu.h"
#include "gpu.h"
#include "memorymap.h"
#include "timer.h"
#include "joypad.h"
#include <chrono>

class Gameboy{
	public:
		Gameboy(std::string cartridgeName);
	
	private:
		MemoryMap memoryMap;
		CPU cpu;
		GPU gpu;
		Timer timer;
		Joypad joypad;
		sf::Sprite b;
		sf::RenderWindow window;
		sf::Texture background;

		int64_t FRAME_US = 16666;
		std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();
};