#include "cpu.h"
#include <cstdlib>
#include "memorylocs.h"
#include "interruptbits.h"

CPU::CPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{}

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

Word CPU::getNextWord(){

	auto low = getNextByte();
	auto high = getNextByte();
	return composeWord(high, low);
}

Word CPU::composeWord(Byte high, Byte low){
	return Word(high, low);
}

Ticks CPU::process(){

	//If interrupt is enabled and one is serviced return 20 Ticks
	
	if (handleInterruptRequests()){
		return 20;
	}

	if(halt){
		return 4;
	}

	uint16_t location = PC.word().val();
	uint8_t opcode = getNextByte();
	const Operation * op = &instructionSet[opcode];
	//If CB Prefix instruction
	if (opcode == 0xCB) {
		//get next opcode
		opcode = getNextByte();
		//get extended op
		op = &cbInstructionSet[opcode];
	}

	spdlog::get("console")->debug("At loc {:x} Opcode {:x} {}", location, opcode, op->mnemonic);
	return op->action(this);;
}


void CPU::handleInterrupt(uint8_t toHandle){
	//Reset IF flag for handled interrupt and disable interrupts
	memoryMap.byte(IF).setBit(toHandle, false);
	IME = false;

	//Jump to starting address of interrupt
	switch(toHandle){
		case VBLANK:
			CPU::RST(0x40);
			break;
		case LCDSTAT:
			CPU::RST(0x48);
			break;
		case TIMER:
			CPU::RST(0x50);
			break;
		case SERIAL:
			CPU::RST(0x58);
			break;
		case JOYPAD:
			CPU::RST(0x60);
			break;
		default:
			spdlog::get("stderr")->error("Handle interrupt called with invalid bit");
			exit(0);
	}
}

bool CPU::handleInterruptRequests() {
	
	//Check set IF flags
	auto interruptsRequested = memoryMap.byte(IF);
	auto interruptsEnabled = memoryMap.byte(IE);
	uint8_t interrupts = interruptsEnabled & interruptsRequested;

	if(interrupts == 0){
		return false;
	}
	
	if (!IME){
		halt = false;
		return false;
	}
	
	//Find lowest set bit
	uint8_t lowestBit = 0;
	while(~interrupts & 0x01){
		lowestBit++;
		interrupts >>= 1;
	};
	if (lowestBit > 4){
		return false;
	}

	handleInterrupt(lowestBit);
	return true;
}
