#include "cpu.h"
/***
FLAG REGISTER OPERATIONS
***/

	void CPU::setZFlag(bool z){
		F.setBit(7, z);
	}
	bool CPU::getZFlag(){
		return F.getBit(7);
	}
	
	void CPU::setNFlag(bool z){
		F.setBit(6, z);
	}
	bool CPU::getNFlag(){
		return F.getBit(6);
	}
	
	void CPU::setHFlag(bool z){			
		F.setBit(5, z);
	}
	bool CPU::getHFlag(){
		return F.getBit(5);
	}

	void CPU::setCFlag(bool z){
		F.setBit(4, z);
	}
	bool CPU::getCFlag(){
		return F.getBit(4);
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
	void CPU::LD_rr_rr(Word& destination, uint16_t source){
		destination = source;
	}

	Ticks CPU::LD_aa_SP(){
		auto address = getNextWord();
		//Store low
		LD_r_r(memoryMap.byte(address), SP.second());
		//Store high
		LD_r_r(memoryMap.byte(address+1), SP.first()); 
		return 20;
	}

	Ticks CPU::POP_rr(Word& reg){
		Word & stackPointer = SP.word();
		Byte low = memoryMap.byte(stackPointer++);
		Byte high = memoryMap.byte(stackPointer++);
		reg = composeWord(high, low);
		return 12;
	}

	Ticks CPU::PUSH_rr(RegisterPair& reg){
		Word & stackPointer = SP.word();
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
		uint8_t carry = getCFlag() ? 1 : 0;
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
		uint8_t carry = getCFlag() ? 1 : 0;

		setNFlag(true);
		setCFlag(A < (sub + carry));
		setHFlag((A&0x0F) - (sub&0x0F) - carry < 0);
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

		A ^= byte;
		setZFlag(A == 0);
		return 4;
	}

	/* CP s - same as SUB but without update of A
	*/
	Ticks CPU::CP_A_r(Byte sub){
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
		setHFlag((byte & 0x0F) == 0x0F);
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

	uint16_t CPU::ADD_SP_s_result(){
		int8_t byte = (int8_t)getNextByte().val();
		Word & stackPointer = SP.word();
		uint16_t result = stackPointer.val() + byte;

		setZFlag(false);
		setNFlag(false);
		setHFlag(((stackPointer ^ byte ^ result) & 0x10));
    	setCFlag(((stackPointer ^ byte ^ result) & 0x100));
		
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
		IME = true;
		return 4;
	}
	Ticks CPU::DI(){
		IME = false;
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

	/*DAA - https://ehaskins.com/2018-01-30%20Z80%20DAA/
	*/
	Ticks CPU::DAA(){
		uint8_t correction = 0x00;
		//if the last addition caused the first nibble to be greater than 9 then correct by 6
		//the half carry flag covers the cases where it is greater than 15
		if ((!getNFlag() && ((A & 0x0F) > 9)) || getHFlag()) {
			correction += 0x06;
		}

		//Same as above but with full carry
		if ((!getNFlag() && (A > 0x99)) || getCFlag()) {
			correction += 0x60;
			setCFlag(true);
		}

		if (getNFlag()) {
			A -= correction;
		} else {
			A += correction;
		}

		setHFlag(false);
		setZFlag(A == 0);
		return 4;
	}

/***
JUMPS
***/

	/* RST f
	*/
	Ticks CPU::RST(uint8_t f){
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
		PC.word() = newLoc;
		return 4;
	}

	/* JP nn - PC <- nn
	*/
	Ticks CPU::JP_nn(){
		PC.word() = getNextWord();
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
		int8_t jumpSize = (int8_t)getNextByte();

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

	uint8_t CPU::getBit(Byte byte, uint8_t bit){
		return (byte.val() >> bit) & 1;
	}

	/* RLC
	*/
	Ticks CPU::RLC(Byte byte){
		uint8_t carrybit = getBit(byte, 7);
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
		uint8_t carrybit = getBit(byte, 7);
		uint8_t firstbit = getCFlag() ? 1 : 0;
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
		uint8_t carrybit = getBit(byte, 0);
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
		uint8_t carrybit = getBit(byte, 0);
		uint8_t lastbit = getCFlag() ? 1 : 0;
		byte = (byte >> 1) + (lastbit << 7);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	//SLA, SRA, SWAP, SRL

	Ticks CPU::SLA(Byte byte){
		uint8_t carrybit = getBit(byte, 7);
		byte = (byte << 1);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}
	
	Ticks CPU::SRA(Byte byte){
		uint8_t lastbit = getBit(byte, 7);
		uint8_t carrybit = getBit(byte, 0);
		byte = (byte >> 1)+ (lastbit << 7);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	Ticks CPU::SRL(Byte byte){
		uint8_t carrybit = getBit(byte, 0);
		byte = (byte >> 1);
		setCFlag(carrybit);
		setZFlag(byte == 0);
		setNFlag(false);
		setHFlag(false);
		return 8;
	}

	Ticks CPU::SWAP(Byte byte){
		uint8_t smallNibble = byte.val() & 0x0F;
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
	Ticks CPU::BIT_b_r(uint8_t bit, Byte reg){
		setZFlag(!getBit(reg, bit));
		setNFlag(false);
		setHFlag(true);
		return 8;
	}

	//RES, SET
	Ticks CPU::SET_b_r(uint8_t bit, Byte reg){
		reg |= 1 << bit;
		return 8;
	}

	Ticks CPU::RES_b_r(uint8_t bit, Byte reg){
		reg &= ~(1 << bit);
		return 8;
	}