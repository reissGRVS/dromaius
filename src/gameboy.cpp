#include "gameboy.h"
#include "spdlog/spdlog.h"

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap)
{
	spdlog::get("console")->info("Powering up Gameboy");
	Ticks sinceLY = 0;
	while(true){	
		sinceLY += cpu.process();
		//TODO: Remove this
		if (sinceLY > 450){
			sinceLY = 0;
			gpu.process();
		}
	}
}