#include "cpu.h"
/***
FLAG REGISTER OPERATIONS
***/

	void CPU::setFlagBit(bool b, unsigned char mask){
		Byte & flagReg = AF.second();
		if (b){
			flagReg |= mask;
		}
		else {
			flagReg &= ~mask;
		}
	}
	
	void CPU::setZFlag(bool z){
		setFlagBit(z, 0x80);
	}
	bool CPU::getZFlag(){
		return AF.second() & 0x80;
	}
	
	void CPU::setNFlag(bool z){
		setFlagBit(z, 0x40);
	}
	bool CPU::getNFlag(){
		return AF.second() & 0x40;
	}
	
	void CPU::setHFlag(bool z){			
		setFlagBit(z, 0x20);
	}
	bool CPU::getHFlag(){
		return AF.second() & 0x20;
	}

	void CPU::setCFlag(bool z){
		setFlagBit(z, 0x10);
	}

	bool CPU::getCFlag(){
		return AF.second() & 0x10;
	}


/***
8BIT LOADS
***/

	/*
	LD R,n register <- immediate 8bit data
	USED BY: 0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e
	*/
	Ticks CPU::LD_r_n(Byte reg){
		reg = getNextByte();
		return 8;
	}

	/*
	LD R,R register <- register
	USED BY: 	0x4(0-5), 	0x4(8-d), 	0x5(0-5),	0x5(8-d),	0x6(0-5),	0x6(8-d),	0x7(8-f), 
				B			C			D			E			H			L			A
	*/
	Ticks CPU::LD_r_r(Byte destination, Byte source, bool fetchAddress/* = false*/){
		destination = source;
		return fetchAddress? 8 : 4;
	}
/***
16BIT LOADS
***/

	/*
	LD R,nn register <- immediate 16bit data
	*/
	Ticks CPU::LD_r_nn(Word& reg){
		reg = getNextWord();
		return 12;
	}

	//Only two opcodes use cpu and they both have different ticks
	void CPU::LD_rr_rr(Word& destination, unsigned int source){
		destination = source;
	}

	Ticks CPU::POP_rr(Word& reg){
		Word& stackPointer = SP.word();
		Byte low = memoryMap.byte(stackPointer++);
		Byte high = memoryMap.byte(stackPointer++);
		reg = composeWord(high, low);
		return 12;
	}

	Ticks CPU::PUSH_rr(RegisterPair& reg){
		Word& stackPointer = SP.word();
		stackPointer--;
		memoryMap.byte(stackPointer) = reg.first();
		stackPointer--;
		memoryMap.byte(stackPointer) = reg.second();
		return 16;
	}
/***
8BIT ALU
***/

	/* ADD A,s	A <- A+s
	*/
	Ticks CPU::ADD_A_r(Byte add){
		Byte& A = AF.first();
		
		setNFlag(false);
		setCFlag((A+add) > 0xFF);
		setHFlag((A&0x0F) + (add&0x0F) > 0x0f);
		A = (A+add);
		setZFlag(A == 0);
		return 4;
	}

	/* ADC A,s  A <- A+s+CY
	*/
	Ticks CPU::ADC_A_r(Byte add){
		Byte& A = AF.first();
		unsigned char carry = getCFlag() ? 1 : 0;
		setNFlag(false);
		setCFlag((A+add+carry) > 0xFF);
		setHFlag((A&0x0F) + (add&0x0F) + carry > 0x0f);
		A = (A+add+carry);
		setZFlag(A == 0);
		return 4;
	}

	/* SUB A,s	A <- A-s
	*/
	Ticks CPU::SUB_A_r(Byte sub){
		Byte& A = AF.first();
		
		setNFlag(true);
		setCFlag(A < sub);
		setHFlag((A&0x0F) < (sub&0x0F));
		A = (A-sub);
		setZFlag(A == 0);
		return 4;
	}

	/* SBC A,s  A <- A-s-CY
	*/
	Ticks CPU::SBC_A_r(Byte sub){
		Byte& A = AF.first();
		unsigned char carry = getCFlag() ? 1 : 0;

		setNFlag(true);
		setCFlag(A < (sub + carry));
		setHFlag((A&0x0F) < ((sub+carry)&0x0F));
		A = (A-sub-carry);
		setZFlag(A == 0);
		return 4;
	}

	/* AND s
	*/
	Ticks CPU::AND_A_r(Byte byte){
		setNFlag(false);
		setCFlag(false);
		setHFlag(true);

		Byte& A = AF.first();
		A &= byte;
		setZFlag(A == 0);
		return 4;
	}

	/* OR s
	*/
	Ticks CPU::OR_A_r(Byte byte){
		setNFlag(false);
		setCFlag(false);
		setHFlag(false);

		Byte& A = AF.first();
		A |= byte;
		setZFlag(A == 0);
		return 4;
	}

	/* XOR s
	*/
	Ticks CPU::XOR_A_r(Byte byte){
		setNFlag(false);
		setCFlag(false);
		setHFlag(false);

		Byte& A = AF.first();
		A ^= byte;
		setZFlag(A == 0);
		return 4;
	}

	/* CP s - same as SUB but without update of A
	*/
	Ticks CPU::CP_A_r(Byte sub){
		Byte& A = AF.first();
		setNFlag(true);
		setCFlag(A < sub);
		setHFlag((A&0x0F) < (sub&0x0F));
		setZFlag((A-sub) == 0);
		return 4;
	}
	
	/* INC s
	*/
	Ticks CPU::INC_r(Byte byte){
		setNFlag(false);
		byte++;
		setHFlag((byte & 0x0F) == 0);
		setZFlag(byte == 0);
		return 4;
	}
	
	/*  DEC s
	*/
	Ticks CPU::DEC_r(Byte byte){
		setNFlag(true);
		byte--;
		setHFlag((byte & 0x0F) == 0);
		setZFlag(byte == 0);
		return 4;
	}

/***
16BIT ALU
***/

	/* ADD HL,rr
	*/
	Ticks CPU::ADD_HL_rr(Word& word){
		Word & HLval = HL.word();
		setNFlag(false);
		setCFlag((HLval+word) > 0xffff);
		setHFlag((HLval & 0x0fff) + (word & 0x0fff) > 0x0fff);
		HLval += word;
		return 8;
	}

	unsigned int CPU::ADD_SP_s_result(){
		char byte = (char)getNextByte().val();
		Word & stackPointer = SP.word();
		unsigned int result = stackPointer.val() + byte;

		setZFlag(false);
		setNFlag(false);

		if (byte >= 0) {
			setCFlag(((result & 0xFF) + byte) > 0xFF);
			setHFlag(((result & 0x0F) + (byte & 0x0F)) > 0xF);
		}
		else {
			setCFlag((stackPointer & 0xFF) <= (result & 0xFF));
			setHFlag((stackPointer & 0xF) <= (result & 0xF));
		}
		return result;
	}

	/* ADD SP,s
	*/
	Ticks CPU::ADD_SP_s(){
		SP.word() = ADD_SP_s_result();
		return 16;
	}

	/* INC s
	*/
	Ticks CPU::INC_rr(Word& word){
		word++;
		return 8;
	}
	/*  DEC s
	*/
	Ticks CPU::DEC_rr(Word& word){
		word--;
		return 8;
	}

/***
MISC
***/

	Ticks CPU::EI(){
		memoryMap.byte(IE) = 1;
		return 4;
	}
	Ticks CPU::DI(){
		memoryMap.byte(IE) = 0;
		return 4;
	}
	/* CPL - invert A
	*/
	Ticks CPU::CPL(){
		setNFlag(true);
		setHFlag(true);
		AF.first() = ~AF.first();
		return 4;
	}
	
	/* CCF - toggle carry flag
	*/
	Ticks CPU::CCF(){
		setNFlag(false);
		setHFlag(false);
		setCFlag(!getCFlag());
		return 4;
	}
	/* SCF - set carry flag to 1
	*/
	Ticks CPU::SCF(){
		setNFlag(false);
		setHFlag(false);
		setCFlag(true);
		return 4;
	}

	/*DAA - This may not work at all
	*/
	Ticks CPU::DAA(){
		//spdlog::get("console")->error("DAA has been called");
		Byte & A = AF.first();
		if ((A & 0x0f) > 9 || getHFlag()) {
			A += 0x06;
		}
		if ((A & 0xf0) > 0x90 || getCFlag()) {
			A += 0x60;
			setCFlag(true);
		}

		setZFlag(A == 0);
		setHFlag(false);
		return 4;
	}

/***
JUMPS
***/

	/* RST f
	*/
	Ticks CPU::RST(unsigned char f){
		PUSH_rr(PC);
		PC.word() = f;
		return 16;
	}

	Ticks CPU::CALL_nn(){
		Word location = getNextWord();
		PUSH_rr(PC);
		PC.word() = location;

		return 24;
	}


	Ticks CPU::CALL_cc_nn(bool condition){
		if(condition){
			return CALL_nn();
		}
		else{
			//Skip this calls inputs
			getNextWord();
			return 12;
		}
	}

	//Only used for PC <- HL	 0xE9
	Ticks CPU::JP_rr(Word newLoc){
		Word & oldLoc = PC.word();
		if (newLoc == oldLoc){
			spdlog::get("stderr")->error("Infinite JP instruction");
			exit(0);
		}
		oldLoc = newLoc;
		return 4;
	}

	/* JP nn - PC <- nn
	*/
	Ticks CPU::JP_nn(){
		Word newLoc = getNextWord();
		Word & oldLoc = PC.word();
		if (newLoc == oldLoc){
			spdlog::get("stderr")->error("Infinite JP instruction");
			exit(0);
		}
		oldLoc = newLoc;
		return 16;
	}

	/* JP nn - PC <- nn
	*/
	Ticks CPU::JP_cc_nn(bool condition){
		if(condition){
			return JP_nn();
		}
		else{
			//Skip this calls inputs
			getNextWord();
			return 12;
		}
	}
	
	Ticks CPU::JR_n() {
		SignedByte jumpSize = (SignedByte)getNextByte();

		if(jumpSize==-2){ 
			spdlog::get("stderr")->error("Infinite JR instruction");
			exit(0); 
		}

		PC.word() += jumpSize;

		return 12;
	}

	Ticks CPU::JR_cc_n(bool condition) {
		if(condition){
			return JR_n();
		}
		else{
			//Skip this calls inputs
			getNextByte();
			return 8;
		}
	}

/***
ROTATES & SHIFTS
***/

	unsigned char CPU::getBit(Byte byte, unsigned char bit){
		return (byte.val() >> bit) & 1;
	}

	/* RLC
	*/
	Ticks CPU::RLC(Byte byte){
		unsigned char carrybit =  getBit(byte, 7);
		byte = (byte << 1) + carrybit;
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	/* RL
	*/
	Ticks CPU::RL(Byte byte){
		unsigned char carrybit =  getBit(byte, 7);
		unsigned char firstbit = getCFlag() ? 1 : 0;
		byte = (byte << 1) + firstbit;
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	/* RRC
	*/
	Ticks CPU::RRC(Byte byte){
		unsigned char carrybit =  getBit(byte, 0);
		byte = (byte >> 1) + (carrybit << 7);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}
	/* RR
	*/
	Ticks CPU::RR(Byte byte){
		unsigned char carrybit = getBit(byte, 0);
		unsigned char lastbit = getCFlag() ? 1 : 0;
		byte = (byte >> 1) + (lastbit << 7);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	//SLA, SRA, SWAP, SRL

	Ticks CPU::SLA(Byte byte){
		unsigned char carrybit =  getBit(byte, 7);
		byte = (byte << 1);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}
	
	Ticks CPU::SRA(Byte byte){
		unsigned char lastbit =  getBit(byte, 7);
		unsigned char carrybit =  getBit(byte, 0);
		byte = (byte >> 1)+ (lastbit << 7);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	Ticks CPU::SRL(Byte byte){
		unsigned char carrybit =  getBit(byte, 0);
		byte = (byte >> 1);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	Ticks CPU::SWAP(Byte byte){
		unsigned char smallNibble = byte.val() & 0x0F;
		byte = (byte >> 4) + (smallNibble << 4);

		setZFlag(byte == 0);
		setCFlag(false);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}
/***
RETURNS
***/

	Ticks CPU::RET(){
		POP_rr(PC.word());
		return 16;
	}

	Ticks CPU::RET_cc(bool condition){
		if (condition){
			POP_rr(PC.word());
			return 20;
		}
		else{
			return 8;
		}
	}

	Ticks CPU::RETI(){
		POP_rr(PC.word());
		EI();
		return 16;
	}

/***
BIT
***/
	Ticks CPU::BIT_b_r(unsigned char bit, Byte reg){
		setZFlag(!getBit(reg, bit));
		setNFlag(false);
		setHFlag(true);
		return 8;
	}

	//RES, SET
	Ticks CPU::SET_b_r(unsigned char bit, Byte reg){
		reg |= 1 << bit;
		return 8;
	}

	Ticks CPU::RES_b_r(unsigned char bit, Byte reg){
		reg &= ~(1 << bit);
		return 8;
	}