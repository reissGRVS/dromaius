#pragma once
#include "registers.h"
#include "forwarddecls.h"
#include "memorymap.h"
#include "operation.h"

class CPU{
	public:
		CPU(MemoryMap& memoryMap);
		Ticks process();
		void ping();
	private:
		RegisterPair AF;
		RegisterPair BC;
		RegisterPair DE;
		RegisterPair HL;
		//These should probably be a seperate thing, as should only be accessed as a word
		RegisterPair SP;
		RegisterPair PC;

		MemoryMap& memoryMap;

		bool interruptsEnabled() const;
		void handleInterruptRequest();

		const Operation instructionSet[0x100] = {

			/*0x00*/	{"NOP", 4, [this](){ this->ping(); }},
		};
};