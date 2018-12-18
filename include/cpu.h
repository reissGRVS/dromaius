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
		Byte& flag = AF.second();
		RegisterPair BC;
		RegisterPair DE;
		RegisterPair HL;
		//These should probably be a seperate thing, as should only be accessed as a word
		RegisterPair SP;
		RegisterPair PC;

		bool halt = false;
		//Interrupts enabled
		bool IE = false;
		
		MemoryMap& memoryMap;
		
		Byte & getNextByte();
		Word getNextWord();
		Word composeWord(Byte high, Byte low);

		bool interruptsEnabled() const;
		void handleInterruptRequest();
		
		void setHalt(bool h){
			halt = h;
		}
		//Flag Register Operations

		void setFlagBit(bool b, Byte mask){
			Byte & flagReg = AF.second();
			if (b){
				flagReg |= mask;
			}
			else {
				flagReg &= ~mask;
			}
		}
		
		void setZFlag(bool z){
			setFlagBit(z, 0x80);
		}
		bool getZFlag(){
			return AF.second() & 0x80;
		}
		
		void setNFlag(bool z){
			setFlagBit(z, 0x40);
		}
		bool getNFlag(){
			return AF.second() & 0x40;
		}
		
		void setHFlag(bool z){			
			setFlagBit(z, 0x20);
		}
		bool getHFlag(){
			return AF.second() & 0x20;
		}

		void setCFlag(bool z){
			setFlagBit(z, 0x10);
		}

		bool getCFlag(){
			return AF.second() & 0x10;
		}

		//GENERIC OPERATIONS

		/***
		8BIT LOADS
		***/

			/*
			LD R,n register <- immediate 8bit data
			USED BY: 0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e
			*/
			Ticks LD_r_n(Byte& reg){
				reg = getNextByte();
				return 8;
			}

			/*
			LD R,R register <- register
			USED BY: 	0x4(0-5), 	0x4(8-d), 	0x5(0-5),	0x5(8-d),	0x6(0-5),	0x6(8-d),	0x7(8-f), 
						B			C			D			E			H			L			A
			*/
			Ticks LD_r_r(Byte& destination, Byte& source, bool fetchAddress = false){
				destination = source;
				return fetchAddress? 8 : 4;
			}
		/***
		16BIT LOADS
		***/

			/*
			LD R,nn register <- immediate 16bit data
			*/
			Ticks LD_r_nn(Word& reg){
				reg = getNextWord();
				return 12;
			}

			//Only two opcodes use cpu and they both have different ticks
			void LD_rr_rr(Word& destination, Word source){
				destination = source;
			}

			Ticks POP_rr(Word& reg){
				Word& stackPointer = SP.word();
				Byte low = memoryMap.getByte(stackPointer++);
				Byte high = memoryMap.getByte(stackPointer++);
				reg = composeWord(high, low);
				return 12;
			}

			Ticks PUSH_rr(RegisterPair& reg){
				Word& stackPointer = SP.word();
				stackPointer--;
				memoryMap.setByte(stackPointer, reg.first());
				stackPointer--;
				memoryMap.setByte(stackPointer, reg.second());
				return 16;
			}
		/***
		8BIT ALU
		***/

			/* ADD A,s	A <- A+s
			*/
			Ticks ADD_A_r(Byte& add){
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
			Ticks ADC_A_r(Byte& add){
				Byte& A = AF.first();
				Byte carry = getCFlag() ? 1 : 0;
				setNFlag(false);
				setCFlag((A+add+carry) > 0xFF);
				setHFlag((A&0x0F) + (add&0x0F) + carry > 0x0f);
				A = (A+add+carry);
				setZFlag(A == 0);
				return 4;
			}

			/* SUB A,s	A <- A-s
			*/
			Ticks SUB_A_r(Byte& sub){
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
			Ticks SBC_A_r(Byte& sub){
				Byte& A = AF.first();
				Byte carry = getCFlag() ? 1 : 0;

				setNFlag(true);
				setCFlag(A < sub + carry);
				setHFlag((A&0x0F) < ((sub+carry)&0x0F));
				A = (A-sub-carry);
				setZFlag(A == 0);
				return 4;
			}

			/* AND s
			*/
			Ticks AND_A_r(Byte& byte){
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
			Ticks OR_A_r(Byte& byte){
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
			Ticks XOR_A_r(Byte& byte){
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
			Ticks CP_A_r(Byte& sub){
				Byte& A = AF.first();
				setNFlag(true);
				setCFlag(A < sub);
				setHFlag((A&0x0F) < (sub&0x0F));
				setZFlag((A-sub) == 0);
				return 4;
			}
			
			/* INC s
			*/
			Ticks INC_r(Byte& byte){
				setNFlag(false);
				byte++;
				setHFlag((byte & 0x0F) == 0);
				setZFlag(byte == 0);
				return 4;
			}
			
			/*  DEC s
			*/
			Ticks DEC_r(Byte& byte){
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
			Ticks ADD_HL_rr(Word& word){
				Word & HLval = HL.word();
				setNFlag(false);
				setCFlag((HLval+word) > 0xffff);
				setHFlag((HLval & 0x0fff) + (word & 0x0fff) > 0x0fff);
				HLval += word;
				return 8;
			}

			Word ADD_SP_s_result(){
				SignedByte& byte = (SignedByte&)getNextByte();
				Word & stackPointer = SP.word();
				Word result = static_cast<Word>(stackPointer + byte);

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
			Ticks ADD_SP_s(){
				SP.word() = ADD_SP_s_result();
				return 16;
			}

			/* INC s
			*/
			Ticks INC_rr(Word& word){
				word++;
				return 8;
			}
			/*  DEC s
			*/
			Ticks DEC_rr(Word& word){
				word--;
				return 8;
			}

		/***
		MISC
		***/

			Ticks EI(){
				IE = true;
				return 4;
			}
			Ticks DI(){
				IE = false;
				return 4;
			}
			/* CPL - invert A
			*/
			Ticks CPL(){
				setNFlag(true);
				setHFlag(true);
				AF.first() = ~AF.first();
				return 4;
			}
			
			/* CCF - toggle carry flag
			*/
			Ticks CCF(){
				setNFlag(false);
				setHFlag(false);
				setCFlag(!getCFlag());
				return 4;
			}
			/* SCF - set carry flag to 1
			*/
			Ticks SCF(){
				setNFlag(false);
				setHFlag(false);
				setCFlag(true);
				return 4;
			}

			/*DAA - This may not work at all
			*/
			Ticks DAA(){
				spdlog::get("console")->error("DAA has been called");
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
			Ticks RST(Byte f){
				PUSH_rr(PC);
				PC.word() = f;
				return 16;
			}

			Ticks CALL_nn(){
				PUSH_rr(PC);
				PC.word() = getNextWord();
				return 24;
			}


			Ticks CALL_cc_nn(bool condition){
				if(condition){
					return CALL_nn();
				}
				else{
					return 12;
				}
			}

			//Only used for PC <- HL	 0xE9
			Ticks JP_rr(Word newLoc){
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
			Ticks JP_nn(){
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
			Ticks JP_cc_nn(bool condition){
				if(condition){
					return JP_nn();
				}
				else{
					return 12;
				}
			}
			
			Ticks JR_n() {
				SignedByte jumpSize = (SignedByte)getNextByte();

				if(jumpSize==-2){ 
					spdlog::get("stderr")->error("Infinite JR instruction");
					exit(0); 
				}

				PC.word() += jumpSize;

				return 12;
			}

			Ticks JR_cc_n(bool condition) {
				//TODO: This only takes 8  if condition is false
				if(condition){
					return JR_n();
				}
				else{
					return 8;
				}
			}

		/***
		ROTATES & SHIFTS
		***/

			Byte getBit(Byte byte, Byte bit){
				return (byte >> bit) & 1;
			}

			/* RLC
			*/
			Ticks RLC(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				byte = (byte << 1) + carrybit;
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}

			/* RL
			*/
			Ticks RL(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				Byte firstbit = getCFlag() ? 1 : 0;
				byte = (byte << 1) + firstbit;
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}

			/* RRC
			*/
			Ticks RRC(Byte& byte){
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1) + (carrybit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}
			/* RR
			*/
			Ticks RR(Byte& byte){
				Byte carrybit = getBit(byte, 0);
				Byte lastbit = getCFlag() ? 1 : 0;
				byte = (byte >> 1) + (lastbit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}

			//SLA, SRA, SWAP, SRL

			Ticks SLA(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				byte = (byte << 1);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}
			
			Ticks SRA(Byte& byte){
				Byte lastbit =  getBit(byte, 7);
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1)+ (lastbit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}

			Ticks SRL(Byte& byte){
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
				return 8;
			}

			Ticks SWAP(Byte& byte){
				Byte smallNibble = byte & 0x0F;
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

			Ticks RET(){
				POP_rr(PC.word());
				return 16;
			}

			Ticks RET_cc(bool condition){
				//TODO: only 8 if not condition
				if (condition){
					POP_rr(PC.word());
					return 20;
				}
				else{
					return 8;
				}
			}
		
			Ticks RETI(){
				POP_rr(PC.word());
				EI();
				return 16;
			}

		/***
		BIT
		***/
			Ticks BIT_b_r(Byte bit, Byte reg){
				setZFlag(!getBit(reg, bit));
    			setNFlag(false);
    			setHFlag(true);
				return 8;
			}

			//RES, SET
			Ticks SET_b_r(Byte bit, Byte & reg){
				reg |= 1 << bit;
				return 8;
			}

			Ticks RES_b_r(Byte bit, Byte & reg){
				reg &= ~(1 << bit);
				return 8;
			}
		
		//OPCODES
		static const Operation instructionSet[0x100];
		static const Operation cbInstructionSet[0x100];
};