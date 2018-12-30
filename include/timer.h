#pragma once
#include "forwarddecls.h"
#include "memorymap.h"
#include "bytes/byte.h"


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
		Byte tima;
		Byte tma;
		Byte tac;
		Byte div;
};