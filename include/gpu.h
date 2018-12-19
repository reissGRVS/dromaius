#pragma once
#include "forwarddecls.h"
#include "memorymap.h"

class GPU{
	public:
		GPU(MemoryMap& memoryMap);
		void process();

	private:
		MemoryMap& memoryMap;
};