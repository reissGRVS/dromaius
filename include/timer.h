#pragma once
#include "forwarddecls.h"
#include "memorymap.h"
#include "bytes/byte.h"


class Timer{
	public:
		Timer(MemoryMap& mem);
		void process(Ticks ticks);
	
	private:
		void tock();
		//Machine clock counter
		uint32_t t;

		//DIV clock
		uint32_t d;
		//Base clock
		uint32_t b;

		MemoryMap& memoryMap;
		Byte tima;
		Byte tma;
		Byte tac;
		Byte div;
};