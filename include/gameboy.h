#pragma once
#include <string>
#include "cpu.h"
#include "gpu.h"
#include "memorymap.h"
#include "timer.h"
#include "joypad.h"

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
};