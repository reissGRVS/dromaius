#pragma once
#include <string>
#include "cpu.h"
#include "memorymap.h"

class Gameboy{
	public:
		Gameboy(std::string rom_name);
	
	private:
		MemoryMap memoryMap;
		CPU cpu;
		//Cartridge
};