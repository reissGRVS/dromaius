#include "cpu.h"
#include "spdlog/spdlog.h"

CPU::CPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{}

void CPU::ping(){
	spdlog::get("console")->info("CPU Ping");
}

Ticks CPU::process(){
	//TESTING TODO REMOVE
	
	Operation testOp = instructionSet[0];
	spdlog::get("console")->info("{} - Ticks {}", testOp.mnemonic, testOp.ticks);
	testOp.action();
	return testOp.ticks;

	if (interruptsEnabled()){
		handleInterruptRequest();
	}

	Word pc = PC.getWord();
	Byte opcode = memoryMap.getByte(pc);
	pc++;
	//If CB Prefix instruction
	Operation op = instructionSet[opcode];
	if (opcode == 0xCB) {
		//get next opcode
		Byte opcode = memoryMap.getByte(pc);
		pc++;
		//get extended op

	}
	else{
		//get normal op

	}


}

//TODO
bool CPU::interruptsEnabled() const{
	return false;
}

//TODO
void CPU::handleInterruptRequest() {
	return;
}