#include "gameboy.h"
#include "spdlog/spdlog.h"

Gameboy::Gameboy(std::string rom_name) :
	cpu(memoryMap)
{
	spdlog::get("console")->info("Powering up Gameboy");
	//TODO: Load rom with rom_name
	while(true){	
		cpu.process();
		
	}
}