#pragma once

#include "forwarddecls.h"

class MemoryMap{

	public:
		MemoryMap() {};
		Byte getByte(Word address) const {
			return memory[address];
		};

	private:
		Byte memory[0x10000] = {0};
};