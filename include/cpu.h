#pragma once
#include "registers.h"
#include "forwarddecls.h"
#include "memorymap.h"
#include "operation.h"
#include "bytes/byte.h"
#include "bytes/word.h"

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


		bool IME = false;
		bool halt = false;
		
		MemoryMap& memoryMap;
		
		Byte getNextByte();

		Word getNextWord();
		Word composeWord(Byte high, Byte low);
		void dump();

		//INTERRUPTS
		bool handleInterruptRequests();
		void handleInterrupt(unsigned char toHandle);
		
		void setHalt(bool h){
			halt = h;
		}
		
		//FLAG REGISTER OPERATIONS

			void setFlagBit(bool b, unsigned char mask);
			void setZFlag(bool z);
			bool getZFlag();
			void setNFlag(bool z);
			bool getNFlag();
			void setHFlag(bool z);
			bool getHFlag();
			void setCFlag(bool z);
			bool getCFlag();

		//GENERIC OPERATIONS

			/***
			8BIT LOADS
			***/
				Ticks LD_r_n(Byte reg);
				Ticks LD_r_r(Byte destination, Byte source, bool fetchAddress = false);
			/***
			16BIT LOADS
			***/
				Ticks LD_r_nn(Word& reg);
				void LD_rr_rr(Word& destination, unsigned int source);
				Ticks POP_rr(Word& reg);
				Ticks PUSH_rr(RegisterPair& reg);
			/***
			8BIT ALU
			***/
				Ticks ADD_A_r(Byte add);
				Ticks ADC_A_r(Byte add);
				Ticks SUB_A_r(Byte sub);
				Ticks SBC_A_r(Byte sub);
				Ticks AND_A_r(Byte byte);
				Ticks OR_A_r(Byte byte);
				Ticks XOR_A_r(Byte byte);
				Ticks CP_A_r(Byte sub);
				Ticks INC_r(Byte byte);
				Ticks DEC_r(Byte byte);
			/***
			16BIT ALU
			***/
				Ticks ADD_HL_rr(Word& word);
				unsigned int ADD_SP_s_result();
				Ticks ADD_SP_s();
				Ticks INC_rr(Word& word);
				Ticks DEC_rr(Word& word);
			/***
			MISC
			***/
				Ticks EI();
				Ticks DI();
				Ticks CPL();
				Ticks CCF();
				Ticks SCF();
				Ticks DAA();
			/***
			JUMPS
			***/
				Ticks RST(unsigned char f);
				Ticks CALL_nn();
				Ticks CALL_cc_nn(bool condition);
				Ticks JP_rr(Word newLoc);
				Ticks JP_nn();
				Ticks JP_cc_nn(bool condition);
				Ticks JR_n();
				Ticks JR_cc_n(bool condition);
			/***
			ROTATES & SHIFTS
			***/
				unsigned char getBit(Byte byte, unsigned char bit);
				Ticks RLC(Byte byte);
				Ticks RL(Byte byte);
				Ticks RRC(Byte byte);
				Ticks RR(Byte byte);
				Ticks SLA(Byte byte);
				Ticks SRA(Byte byte);
				Ticks SRL(Byte byte);
				Ticks SWAP(Byte byte);
			/***
			RETURNS
			***/
				Ticks RET();
				Ticks RET_cc(bool condition);
				Ticks RETI();
			/***
			BIT
			***/
				Ticks BIT_b_r(unsigned char bit, Byte reg);
				Ticks SET_b_r(unsigned char bit, Byte reg);
				Ticks RES_b_r(unsigned char bit, Byte reg);
			
		//OPCODES
			static const Operation instructionSet[0x100];
			static const Operation cbInstructionSet[0x100];
};