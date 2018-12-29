#pragma once
#include "forwarddecls.h"
#include "memorymap.h"

class Timer{
	public:
		Timer(MemoryMap& mem);
		void process(Ticks ticks);
	
	private:
		unsigned int clock();
		//Timer ticks
		Ticks t;
		//DIV ticks
		Ticks d;
		MemoryMap& memoryMap;
		Byte tima = memoryMap.byte(TIMA);
		Byte tma = memoryMap.byte(TMA);
		Byte tac = memoryMap.byte(TAC);
		Byte div = memoryMap.byte(DIV);
};