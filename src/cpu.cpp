#include "cpu.h"

CPU::CPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{
	// TODO: Add this as a test
	// AF.setFirst(0x01);
	// AF.setSecond(0x02);
	// spdlog::get("console")->info("AF {}", AF.getWord());

}

void CPU::ping(){
	spdlog::get("console")->info("CPU Ping");
}

Byte & CPU::getNextByte(){
	Word pc = PC.word();
	return memoryMap.byte(pc++);
}

//TODO: Make it clear the that getNextByte and getNextWord are not similar
Word CPU::getNextWord(){
	Byte low = this->getNextByte();
	Byte high = this->getNextByte();
	return ((high << 8) + low);
}

Ticks CPU::process(){

	if (interruptsEnabled()){
		handleInterruptRequest();
	}

	Byte opcode = getNextByte();
	const Operation * op = &instructionSet[opcode];
	//If CB Prefix instruction
	if (opcode == 0xCB) {
		//get next opcode
		opcode = getNextByte();
		//get extended op
		op = &cbInstructionSet[opcode];
	}

	if (op->ticks == 0){
		spdlog::get("stderr")->error("Not implemented: {0} - opCode {1:x}", op->mnemonic, opcode);
		exit(0);
	}

	op->action();
	
	return op->ticks;
}

//TODO
bool CPU::interruptsEnabled() const{
	return false;
}

//TODO
void CPU::handleInterruptRequest() {
	return;
}