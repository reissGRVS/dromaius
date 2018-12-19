#include "cpu.h"
#include <cstdlib>

CPU::CPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{
}

void CPU::dump(){
	spdlog::get("console")->info("AF {}", AF.word());
	spdlog::get("console")->info("BC {}", BC.word());
	spdlog::get("console")->info("DE {}", DE.word());
	spdlog::get("console")->info("HL {}", HL.word());
	spdlog::get("console")->info("PC {}", PC.word());
	spdlog::get("console")->info("SP {}", SP.word());
}

void CPU::ping(){
	spdlog::get("console")->info("CPU Ping");
}

Byte CPU::getNextByte(){
	Word & pc = PC.word();
	return memoryMap.byte(pc++);
}

//TODO: Make it clear the that getNextByte and getNextWord are not similar
Word CPU::getNextWord(){

	Byte low = this->getNextByte();
	Byte high = this->getNextByte();
	return composeWord(high, low);
}

Word CPU::composeWord(Byte high, Byte low){
	return Word(high, low);
}

Ticks CPU::process(){

	if (interruptsEnabled()){
		handleInterruptRequest();
	}
	unsigned int location = PC.word().val();
	unsigned char opcode = getNextByte();
	const Operation * op = &instructionSet[opcode];
	//If CB Prefix instruction
	if (opcode == 0xCB) {
		//get next opcode
		opcode = getNextByte();
		//get extended op
		op = &cbInstructionSet[opcode];
	}

	spdlog::get("console")->info("At loc {:x} Opcode {:x} {}", location, opcode, op->mnemonic);
	return op->action(this);;
}

//TODO
bool CPU::interruptsEnabled() const{
	return IE;
}

//TODO
void CPU::handleInterruptRequest() {
	return;
}
