#include "gpu.h"
#include "memorylocs.h"

GPU::GPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{
}

void GPU::process(){
	//This gets us past the boot sequence waiting for interrupt
	auto byteLY = memoryMap.byte(LY);
	byteLY++;
	if (byteLY > 154){
		byteLY = 0;
	}
}