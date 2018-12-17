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

		//Flag Register Operations

		void setFlagBit(bool b, Byte mask){
			Byte & flagReg = this->AF.second();
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
			return this->AF.second() & 0x80;
		}
		
		void setNFlag(bool z){
			setFlagBit(z, 0x40);
		}
		bool getNFlag(){
			return this->AF.second() & 0x40;
		}
		
		void setHFlag(bool z){			
			setFlagBit(z, 0x20);
		}
		bool getHFlag(){
			return this->AF.second() & 0x20;
		}

		void setCFlag(bool z){
			setFlagBit(z, 0x10);
		}

		bool getCFlag(){
			return this->AF.second() & 0x10;
		}

		//GENERIC OPERATIONS

		/***
		8BIT LOADS
		***/

			/*
			LD R,n register <- immediate 8bit data
			USED BY: 0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e
			*/
			void LD_r_n(Byte& reg){
				reg = this->getNextByte();
			}

			/*
			LD R,R register <- register
			USED BY: 	0x4(0-5), 	0x4(8-d), 	0x5(0-5),	0x5(8-d),	0x6(0-5),	0x6(8-d),	0x7(8-f), 
						B			C			D			E			H			L			A
			*/
			void LD_r_r(Byte& destination, Byte& source){
				destination = source;
			}
		/***
		16BIT LOADS
		***/

			/*
			LD R,nn register <- immediate 16bit data
			*/
			void LD_r_nn(Word& reg){
				reg = getNextWord();
			}

			void LD_rr_rr(Word& destination, Word source){
				destination = source;
			}

			void POP_rr(Word& reg){
				Word& stackPointer = this->SP.word();
				Byte low = this->memoryMap.getByte(stackPointer++);
				Byte high = this->memoryMap.getByte(stackPointer++);
				reg = composeWord(high, low);
			}

			void PUSH_rr(RegisterPair& reg){
				Word& stackPointer = this->SP.word();
				stackPointer--;
				this->memoryMap.setByte(stackPointer, reg.first());
				stackPointer--;
				this->memoryMap.setByte(stackPointer, reg.second());
			}
		/***
		8BIT ALU
		***/

			/* ADD A,s	A <- A+s
			*/
			void ADD_A_r(Byte& add){
				Byte& A = this->AF.first();
				
				this->setNFlag(false);
				this->setCFlag((A+add) > 0xFF);
				this->setHFlag((A&0x0F) + (add&0x0F) > 0x0f);
				A = (A+add);
				this->setZFlag(A == 0);
			}

			/* ADC A,s  A <- A+s+CY
			*/
			void ADC_A_r(Byte& add){
				Byte& A = this->AF.first();
				Byte carry = this->getCFlag() ? 1 : 0;
				this->setNFlag(false);
				this->setCFlag((A+add+carry) > 0xFF);
				this->setHFlag((A&0x0F) + (add&0x0F) + carry > 0x0f);
				A = (A+add+carry);
				this->setZFlag(A == 0);
			}

			/* SUB A,s	A <- A-s
			*/
			void SUB_A_r(Byte& sub){
				Byte& A = this->AF.first();
				
				this->setNFlag(true);
				this->setCFlag(A < sub);
				this->setHFlag((A&0x0F) < (sub&0x0F));
				A = (A-sub);
				this->setZFlag(A == 0);
			}

			/* SBC A,s  A <- A-s-CY
			*/
			void SBC_A_r(Byte& sub){
				Byte& A = this->AF.first();
				Byte carry = this->getCFlag() ? 1 : 0;

				this->setNFlag(true);
				this->setCFlag(A < sub + carry);
				this->setHFlag((A&0x0F) < ((sub+carry)&0x0F));
				A = (A-sub-carry);
				this->setZFlag(A == 0);
			}

			/* AND s
			*/
			void AND_A_r(Byte& byte){
				this->setNFlag(false);
				this->setCFlag(false);
				this->setHFlag(true);

				Byte& A = this->AF.first();
				A &= byte;
				this->setZFlag(A == 0);
			}

			/* OR s
			*/
			void OR_A_r(Byte& byte){
				this->setNFlag(false);
				this->setCFlag(false);
				this->setHFlag(false);

				Byte& A = this->AF.first();
				A |= byte;
				this->setZFlag(A == 0);
			}

			/* XOR s
			*/
			void XOR_A_r(Byte& byte){
				this->setNFlag(false);
				this->setCFlag(false);
				this->setHFlag(false);

				Byte& A = this->AF.first();
				A ^= byte;
				this->setZFlag(A == 0);
			}

			/* CP s - same as SUB but without update of A
			*/
			void CP_A_r(Byte& sub){
				Byte& A = this->AF.first();
				this->setNFlag(true);
				this->setCFlag(A < sub);
				this->setHFlag((A&0x0F) < (sub&0x0F));
				this->setZFlag((A-sub) == 0);
			}
			
			/* INC s
			*/
			void INC_r(Byte& byte){
				setNFlag(false);
				byte++;
				setHFlag((byte & 0x0F) == 0);
				setZFlag(byte == 0);
			}
			
			/*  DEC s
			*/
			void DEC_r(Byte& byte){
				setNFlag(true);
				byte--;
				setHFlag((byte & 0x0F) == 0);
				setZFlag(byte == 0);
			}

		/***
		16BIT ALU
		***/

			/* ADD HL,rr
			*/
			void ADD_HL_rr(Word& word){
				Word & HLval = HL.word();
				setNFlag(false);
				setCFlag((HLval+word) > 0xffff);
				setHFlag((HLval & 0x0fff) + (word & 0x0fff) > 0x0fff);
				HLval += word;
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
			void ADD_SP_s(){
				SP.word() = ADD_SP_s_result();
			}

			/* INC s
			*/
			void INC_rr(Word& word){
				word++;
			}
			/*  DEC s
			*/
			void DEC_rr(Word& word){
				word--;
			}

		/***
		MISC
		***/

			void EI(){
				IE = true;
			}
			void DI(){
				IE = false;
			}
			/* CPL - invert A
			*/
			void CPL(){
				setNFlag(true);
				setHFlag(true);
				AF.first() = ~AF.first(); 
			}
			
			/* CCF - toggle carry flag
			*/
			void CCF(){
				setNFlag(false);
				setHFlag(false);
				setCFlag(!getCFlag());
			}
			/* SCF - set carry flag to 1
			*/
			void SCF(){
				setNFlag(false);
				setHFlag(false);
				setCFlag(true);
			}

			/*DAA - This may not work at all
			*/
			void DAA(){
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
			}

		/***
		JUMPS
		***/

			/* RST f
			*/
			void RST(Byte f){
				PUSH_rr(this->PC);
				PC.word() = f;
			}

			void CALL_nn(){
				PUSH_rr(this->PC);
				PC.word() = getNextWord();
			}

			//TODO different cycles if false 
			void CALL_cc_nn(bool condition){
				if(condition){
					CALL_nn();
				}
			}

			//Only used for PC <- HL	 0xE9
			void JP_rr(Word newLoc){
				Word & oldLoc = PC.word();
				if (newLoc == oldLoc){
					spdlog::get("stderr")->error("Infinite JP instruction");
					exit(0);
				}
				oldLoc = newLoc;
			}

			/* JP nn - PC <- nn
			*/
			void JP_nn(){
				Word newLoc = getNextWord();
				Word & oldLoc = PC.word();
				if (newLoc == oldLoc){
					spdlog::get("stderr")->error("Infinite JP instruction");
					exit(0);
				}
				oldLoc = newLoc;
			}

			/* JP nn - PC <- nn
			*/
			void JP_cc_nn(bool condition){
				//TODO: This only takes 12 if condition is false
				if(condition){
					JP_nn();
				}
			}
			
			void JR_n() {
				SignedByte jumpSize = (SignedByte)getNextByte();

				if(jumpSize==-2){ 
					spdlog::get("stderr")->error("Infinite JR instruction");
					exit(0); 
				}

				this->PC.word() += jumpSize;
			}

			void JR_cc_n(bool condition) {
				//TODO: This only takes 8  if condition is false
				if(condition){
					JR_n();
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
			void RLC(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				byte = (byte << 1) + carrybit;
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}

			/* RL
			*/
			void RL(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				Byte firstbit = getCFlag() ? 1 : 0;
				byte = (byte << 1) + firstbit;
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}

			/* RRC
			*/
			void RRC(Byte& byte){
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1) + (carrybit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}
			/* RR
			*/
			void RR(Byte& byte){
				Byte carrybit = getBit(byte, 0);
				Byte lastbit = getCFlag() ? 1 : 0;
				byte = (byte >> 1) + (lastbit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}

			//SLA, SRA, SWAP, SRL

			void SLA(Byte& byte){
				Byte carrybit =  getBit(byte, 7);
				byte = (byte << 1);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}
			
			void SRA(Byte& byte){
				Byte lastbit =  getBit(byte, 7);
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1)+ (lastbit << 7);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}

			void SRL(Byte& byte){
				Byte carrybit =  getBit(byte, 0);
				byte = (byte >> 1);
				setCFlag(carrybit);
				setZFlag(byte == 0);
				setNFlag(false);
				setHFlag(false);
			}

			void SWAP(Byte& byte){
				Byte smallNibble = byte & 0x0F;
				byte = (byte >> 4) + (smallNibble << 4);

				setZFlag(byte == 0);
				setCFlag(false);
				setNFlag(false);
				setHFlag(false);
			}
		/***
		RETURNS
		***/

			void RET(){
				POP_rr(this->PC.word());
			}

			void RET_cc(bool condition){
				//TODO: only 8 if not condition
				if (condition){
					POP_rr(this->PC.word());
				}
			}
		
			void RETI(){
				POP_rr(this->PC.word());
				EI();
			}

		/***
		BIT
		***/
			void BIT_b_r(Byte bit, Byte reg){
				setZFlag(!getBit(reg, bit));
    			setNFlag(false);
    			setHFlag(true);
			}

			//RES, SET
			void SET_b_r(Byte bit, Byte & reg){
				reg |= 1 << bit;
			}

			void RES_b_r(Byte bit, Byte & reg){
				reg &= ~(1 << bit);
			}
		//OPCODES

		const Operation instructionSet[0x100] = {
			/*0x00*/	{"NOP", 4, [this](){}},
			/*0x01*/	{"LD BC,nn", 12, [this](){ this->LD_r_nn(this->BC.word()); }},
			/*0x02*/	{"LD (BC),A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->BC.word()), this->AF.first()); }},
			/*0x03*/	{"INC BC", 8, [this](){ this->INC_rr(this->BC.word()); }},
			/*0x04*/	{"INC B", 4, [this](){ this->INC_r(this->BC.first()); }},
			/*0x05*/	{"DEC B", 4, [this](){ this->DEC_r(this->BC.first()); }},
			/*0x06*/	{"LD B,n", 8, [this](){ this->LD_r_n(this->BC.first()); }},
			/*0x07*/	{"RLCA", 4, [this](){ this->RLC(this->AF.first()); setZFlag(false); }},

			/*0x08*/	{"LD (a16),SP", 20, [this](){ 
							//Store low
							this->LD_r_r(this->memoryMap.byte(this->getNextByte()), this->SP.second());
							//Store high
							this->LD_r_r(this->memoryMap.byte(this->getNextByte()), this->SP.first());}},
			/*0x09*/	{"ADD HL,BC", 8, [this](){ this->ADD_HL_rr(this->BC.word()); }},
			/*0x0A*/	{"LD A, (BC)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->BC.word())); }},
			/*0x0B*/	{"DEC BC", 8, [this](){ this->DEC_rr(this->BC.word()); }},
			/*0x0C*/	{"INC C", 4, [this](){ this->INC_r(this->BC.second()); }},
			/*0x0D*/	{"DEC C", 4, [this](){ this->DEC_r(this->BC.second()); }},
			/*0x0E*/	{"LD C, n", 8, [this](){ this->LD_r_n(this->BC.second()); }},
			/*0x0F*/	{"RRCA", 4, [this](){ this->RRC(this->AF.first()); setZFlag(false); }},
			
			/*0x10*/	{"STOP 0", 4, [this](){ /*TODO: Stop processor & screen until button press*/ }},
			/*0x11*/	{"LD DE,nn", 12, [this](){ this->LD_r_nn(this->DE.word()); }},
			/*0x12*/	{"LD (DE),A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->DE.word()), this->AF.first()); }},
			/*0x13*/	{"INC DE", 8, [this](){ this->INC_rr(this->DE.word()); }},
			/*0x14*/	{"INC D", 4, [this](){ this->INC_r(this->DE.first()); }},
			/*0x15*/	{"DEC D", 4, [this](){ this->DEC_r(this->DE.first()); }},
			/*0x16*/	{"LD D, n", 8, [this](){ this->LD_r_n(this->DE.first()); }},
			/*0x17*/	{"RLA", 4, [this](){ this->RL(this->AF.first()); setZFlag(false); }},

			/*0x18*/	{"JR n", 12, [this](){ this->JR_n(); }},
			/*0x19*/	{"ADD HL,DE", 8, [this](){ this->ADD_HL_rr(this->DE.word()); }},
			/*0x1A*/	{"LD A,(DE)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->DE.word())); }},
			/*0x1B*/	{"DEC DE", 8, [this](){ this->DEC_rr(this->DE.word()); }},
			/*0x1C*/	{"INC E", 4, [this](){ this->INC_r(this->DE.second()); }},
			/*0x1D*/	{"DEC E", 4, [this](){ this->DEC_r(this->DE.second()); }},
			/*0x1E*/	{"LD E, n", 8, [this](){ this->LD_r_n(this->DE.second()); }},
			/*0x1F*/	{"RRA", 4, [this](){ this->RR(this->AF.first()); setZFlag(false); }},

			/*0x20*/	{"JR NZ,n", 12, [this](){ this->JR_cc_n(!this->getZFlag()); }},
			/*0x21*/	{"LD HL,nn", 12, [this](){ this->LD_r_nn(this->HL.word()); }},
			/*0x22*/	{"LD (HL+), A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->AF.first()); this->INC_rr(this->HL.word());}},
			/*0x23*/	{"INC HL", 8, [this](){ this->INC_rr(this->HL.word()); }},
			/*0x24*/	{"INC H", 4, [this](){ this->INC_r(this->HL.first()); }},
			/*0x25*/	{"DEC H", 4, [this](){ this->DEC_r(this->HL.first()); }},
			/*0x26*/	{"LD H, n", 8, [this](){ this->LD_r_n(this->HL.first()); }},
			/*0x27*/	{"DAA", 4, [this](){ this->DAA(); }}, 

			/*0x28*/	{"JR Z,n", 12, [this](){ this->JR_cc_n(this->getZFlag()); }},
			/*0x29*/	{"ADD HL,HL", 8, [this](){ this->ADD_HL_rr(this->HL.word()); }},
			/*0x2A*/	{"LD A,(HL+)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->HL.word())); this->INC_rr(this->HL.word());}},
			/*0x2B*/	{"DEC HL", 8, [this](){ this->DEC_rr(this->HL.word()); }},
			/*0x2C*/	{"INC L", 4, [this](){ this->INC_r(this->HL.second()); }},
			/*0x2D*/	{"DEC L", 4, [this](){ this->DEC_r(this->HL.second()); }},
			/*0x2E*/	{"LD L, n", 8, [this](){ this->LD_r_n(this->HL.second()); }},
			/*0x2F*/	{"CPL", 4, [this](){ this->CPL(); }},

			/*0x38*/	{"JR NC,n", 12, [this](){ this->JR_cc_n(!this->getCFlag()); }},
			/*0x31*/	{"LD SP,nn", 12, [this](){ this->LD_r_nn(this->SP.word()); }},
			/*0x32*/	{"LD (HL-), A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->AF.first()); this->DEC_rr(this->HL.word());}},
			/*0x33*/	{"INC SP", 8, [this](){ this->INC_rr(this->SP.word()); }},
			/*0x34*/	{"INC (HL)", 12, [this](){ this->INC_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x35*/	{"DEC (HL)", 12, [this](){ this->DEC_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x36*/	{"LD (HL), n", 12, [this](){ this->LD_r_n(this->memoryMap.byte(this->HL.word())); }},
			/*0x37*/	{"SCF", 4, [this](){ this->SCF(); }},

			/*0x38*/	{"JR C,n", 12, [this](){ this->JR_cc_n(this->getCFlag()); }},
			/*0x39*/	{"ADD HL,SP", 8, [this](){ this->ADD_HL_rr(this->SP.word()); }},
			/*0x3A*/	{"LD A,(HL-)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->HL.word())); this->DEC_rr(this->HL.word());}},
			/*0x3B*/	{"DEC SP", 8, [this](){ this->DEC_rr(this->SP.word()); }},
			/*0x3C*/	{"INC A", 4, [this](){ this->INC_r(this->AF.first()); }},
			/*0x3D*/	{"DEC A", 4, [this](){ this->DEC_r(this->AF.first()); }},
			/*0x3E*/	{"LD A, n", 8, [this](){ this->LD_r_n(this->AF.first()); }},
			/*0x3F*/	{"CCF", 4, [this](){ this->CCF(); }},
// COMPLETED
			/*0x40*/	{"LD B,B", 4, [this](){ this->LD_r_r(this->BC.first(), this->BC.first());}},
			/*0x41*/	{"LD B,C", 4, [this](){ this->LD_r_r(this->BC.first(), this->BC.second());}},
			/*0x42*/	{"LD B,D", 4, [this](){ this->LD_r_r(this->BC.first(), this->DE.first());}},
			/*0x43*/	{"LD B,E", 4, [this](){ this->LD_r_r(this->BC.first(), this->DE.second());}},
			/*0x44*/	{"LD B,H", 4, [this](){ this->LD_r_r(this->BC.first(), this->HL.first());}},
			/*0x45*/	{"LD B,L", 4, [this](){ this->LD_r_r(this->BC.first(), this->HL.second());}},
			/*0x46*/	{"LD B,(HL)", 8, [this](){ this->LD_r_r(this->BC.first(), this->memoryMap.byte(this->HL.word())); }},
			/*0x47*/	{"LD B,A", 4, [this](){ this->LD_r_r(this->BC.first(), this->AF.first());}},

			/*0x48*/	{"LD C,B", 4, [this](){ this->LD_r_r(this->BC.second(), this->BC.first());}},
			/*0x49*/	{"LD C,C", 4, [this](){ this->LD_r_r(this->BC.second(), this->BC.second());}},
			/*0x4A*/	{"LD C,D", 4, [this](){ this->LD_r_r(this->BC.second(), this->DE.first());}},
			/*0x4B*/	{"LD C,E", 4, [this](){ this->LD_r_r(this->BC.second(), this->DE.second());}},
			/*0x4C*/	{"LD C,H", 4, [this](){ this->LD_r_r(this->BC.second(), this->HL.first());}},
			/*0x4D*/	{"LD C,L", 4, [this](){ this->LD_r_r(this->BC.second(), this->HL.second());}},
			/*0x4E*/	{"LD C,(HL)", 8, [this](){ this->LD_r_r(this->BC.second(), this->memoryMap.byte(this->HL.word())); }},
			/*0x4F*/	{"LD C,A", 4, [this](){ this->LD_r_r(this->BC.second(), this->AF.first());}},

			/*0x50*/	{"LD D,B", 4, [this](){ this->LD_r_r(this->DE.first(), this->BC.first());}},
			/*0x51*/	{"LD D,C", 4, [this](){ this->LD_r_r(this->DE.first(), this->BC.second());}},
			/*0x52*/	{"LD D,D", 4, [this](){ this->LD_r_r(this->DE.first(), this->DE.first());}},
			/*0x53*/	{"LD D,E", 4, [this](){ this->LD_r_r(this->DE.first(), this->DE.second());}},
			/*0x54*/	{"LD D,H", 4, [this](){ this->LD_r_r(this->DE.first(), this->HL.first());}},
			/*0x55*/	{"LD D,L", 4, [this](){ this->LD_r_r(this->DE.first(), this->HL.second());}},
			/*0x56*/	{"LD D,(HL)", 8, [this](){ this->LD_r_r(this->DE.first(), this->memoryMap.byte(this->HL.word())); }},
			/*0x57*/	{"LD D,A", 4, [this](){ this->LD_r_r(this->DE.first(), this->AF.first());}},

			/*0x58*/	{"LD E,B", 4, [this](){ this->LD_r_r(this->DE.second(), this->BC.first());}},
			/*0x59*/	{"LD E,C", 4, [this](){ this->LD_r_r(this->DE.second(), this->BC.second());}},
			/*0x5A*/	{"LD E,D", 4, [this](){ this->LD_r_r(this->DE.second(), this->DE.first());}},
			/*0x5B*/	{"LD E,E", 4, [this](){ this->LD_r_r(this->DE.second(), this->DE.second());}},
			/*0x5C*/	{"LD E,H", 4, [this](){ this->LD_r_r(this->DE.second(), this->HL.first());}},
			/*0x5D*/	{"LD E,L", 4, [this](){ this->LD_r_r(this->DE.second(), this->HL.second());}},
			/*0x5E*/	{"LD E,(HL)", 8, [this](){ this->LD_r_r(this->DE.second(), this->memoryMap.byte(this->HL.word())); }},
			/*0x5F*/	{"LD E,A", 4, [this](){ this->LD_r_r(this->DE.second(), this->AF.first());}},

			/*0x60*/	{"LD H,B", 4, [this](){ this->LD_r_r(this->HL.first(), this->BC.first());}},
			/*0x61*/	{"LD H,C", 4, [this](){ this->LD_r_r(this->HL.first(), this->BC.second());}},
			/*0x62*/	{"LD H,D", 4, [this](){ this->LD_r_r(this->HL.first(), this->DE.first());}},
			/*0x63*/	{"LD H,E", 4, [this](){ this->LD_r_r(this->HL.first(), this->DE.second());}},
			/*0x64*/	{"LD H,H", 4, [this](){ this->LD_r_r(this->HL.first(), this->HL.first());}},
			/*0x65*/	{"LD H,L", 4, [this](){ this->LD_r_r(this->HL.first(), this->HL.second());}},
			/*0x66*/	{"LD H,(HL)", 8, [this](){ this->LD_r_r(this->HL.first(), this->memoryMap.byte(this->HL.word())); }},
			/*0x67*/	{"LD H,A", 4, [this](){ this->LD_r_r(this->HL.first(), this->AF.first());}},

			/*0x68*/	{"LD L,B", 4, [this](){ this->LD_r_r(this->HL.second(), this->BC.first());}},
			/*0x69*/	{"LD L,C", 4, [this](){ this->LD_r_r(this->HL.second(), this->BC.second());}},
			/*0x6A*/	{"LD L,D", 4, [this](){ this->LD_r_r(this->HL.second(), this->DE.first());}},
			/*0x6B*/	{"LD L,E", 4, [this](){ this->LD_r_r(this->HL.second(), this->DE.second());}},
			/*0x6C*/	{"LD L,H", 4, [this](){ this->LD_r_r(this->HL.second(), this->HL.first());}},
			/*0x6D*/	{"LD L,L", 4, [this](){ this->LD_r_r(this->HL.second(), this->HL.second());}},
			/*0x6E*/	{"LD L,(HL)", 8, [this](){ this->LD_r_r(this->HL.second(), this->memoryMap.byte(this->HL.word())); }},
			/*0x6F*/	{"LD L,A", 4, [this](){ this->LD_r_r(this->HL.second(), this->AF.first());}},

			/*0x70*/	{"LD (HL), B", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->BC.first());}},
			/*0x71*/	{"LD (HL), C", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->BC.second());}},
			/*0x72*/	{"LD (HL), D", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->DE.first());}},
			/*0x73*/	{"LD (HL), E", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->DE.second());}},
			/*0x74*/	{"LD (HL), H", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->HL.first());}},
			/*0x75*/	{"LD (HL), L", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->HL.second());}},
			/*0x76*/	{"HALT", 4, [this](){halt=true;}},
			/*0x77*/	{"LD (HL), A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->HL.word()), this->AF.first());}},

			/*0x78*/	{"LD A, B", 4, [this](){ this->LD_r_r(this->AF.first(), this->BC.first());}},
			/*0x79*/	{"LD A, C", 4, [this](){ this->LD_r_r(this->AF.first(), this->BC.second());}},
			/*0x7A*/	{"LD A, D", 4, [this](){ this->LD_r_r(this->AF.first(), this->DE.first());}},
			/*0x7B*/	{"LD A, E", 4, [this](){ this->LD_r_r(this->AF.first(), this->DE.second());}},
			/*0x7C*/	{"LD A, H", 4, [this](){ this->LD_r_r(this->AF.first(), this->HL.first());}},
			/*0x7D*/	{"LD A, L", 4, [this](){ this->LD_r_r(this->AF.first(), this->HL.second());}},
			/*0x7E*/	{"LD A,(HL)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->HL.word())); }},
			/*0x7F*/	{"LD A, A", 4, [this](){ this->LD_r_r(this->AF.first(), this->AF.first());}},

			/*0x80*/	{"ADD A,B", 4, [this](){ this->ADD_A_r(this->BC.first()); }},
			/*0x81*/	{"ADD A,C", 4, [this](){ this->ADD_A_r(this->BC.second()); }},
			/*0x82*/	{"ADD A,D", 4, [this](){ this->ADD_A_r(this->DE.first()); }},
			/*0x83*/	{"ADD A,E", 4, [this](){ this->ADD_A_r(this->DE.second()); }},
			/*0x84*/	{"ADD A,H", 4, [this](){ this->ADD_A_r(this->HL.first()); }},
			/*0x85*/	{"ADD A,L", 4, [this](){ this->ADD_A_r(this->HL.second()); }},
			/*0x86*/	{"ADD A,(HL)", 8, [this](){ this->ADD_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x87*/	{"ADD A,A", 4, [this](){ this->ADD_A_r(this->AF.first()); }},

			/*0x88*/	{"ADC A,B", 4, [this](){ this->ADC_A_r(this->BC.first()); }},
			/*0x89*/	{"ADC A,C", 4, [this](){ this->ADC_A_r(this->BC.second()); }},
			/*0x8A*/	{"ADC A,D", 4, [this](){ this->ADC_A_r(this->DE.first()); }},
			/*0x8B*/	{"ADC A,E", 4, [this](){ this->ADC_A_r(this->DE.second()); }},
			/*0x8C*/	{"ADC A,H", 4, [this](){ this->ADC_A_r(this->HL.first()); }},
			/*0x8D*/	{"ADC A,L", 4, [this](){ this->ADC_A_r(this->HL.second()); }},
			/*0x8E*/	{"ADC A,(HL)", 8, [this](){ this->ADC_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x8F*/	{"ADC A,A", 4, [this](){ this->ADC_A_r(this->AF.first()); }},

			/*0x90*/	{"SUB A,B", 4, [this](){ this->SUB_A_r(this->BC.first()); }},
			/*0x91*/	{"SUB A,C", 4, [this](){ this->SUB_A_r(this->BC.second()); }},
			/*0x92*/	{"SUB A,D", 4, [this](){ this->SUB_A_r(this->DE.first()); }},
			/*0x93*/	{"SUB A,E", 4, [this](){ this->SUB_A_r(this->DE.second()); }},
			/*0x94*/	{"SUB A,H", 4, [this](){ this->SUB_A_r(this->HL.first()); }},
			/*0x95*/	{"SUB A,L", 4, [this](){ this->SUB_A_r(this->HL.second()); }},
			/*0x96*/	{"SUB A,(HL)", 8, [this](){ this->SUB_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x97*/	{"SUB A,A", 4, [this](){ this->SUB_A_r(this->AF.first()); }},

			/*0x98*/	{"SBC A,B", 4, [this](){ this->SBC_A_r(this->BC.first()); }},
			/*0x99*/	{"SBC A,C", 4, [this](){ this->SBC_A_r(this->BC.second()); }},
			/*0x9A*/	{"SBC A,D", 4, [this](){ this->SBC_A_r(this->DE.first()); }},
			/*0x9B*/	{"SBC A,E", 4, [this](){ this->SBC_A_r(this->DE.second()); }},
			/*0x9C*/	{"SBC A,H", 4, [this](){ this->SBC_A_r(this->HL.first()); }},
			/*0x9D*/	{"SBC A,L", 4, [this](){ this->SBC_A_r(this->HL.second()); }},
			/*0x9E*/	{"SBC A,(HL)", 8, [this](){ this->SBC_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0x9F*/	{"SBC A,A", 4, [this](){ this->SBC_A_r(this->AF.first()); }},

			/*0xA0*/	{"AND A,B", 4, [this](){ this->AND_A_r(this->BC.first()); }},
			/*0xA1*/	{"AND A,C", 4, [this](){ this->AND_A_r(this->BC.second()); }},
			/*0xA2*/	{"AND A,D", 4, [this](){ this->AND_A_r(this->DE.first()); }},
			/*0xA3*/	{"AND A,E", 4, [this](){ this->AND_A_r(this->DE.second()); }},
			/*0xA4*/	{"AND A,H", 4, [this](){ this->AND_A_r(this->HL.first()); }},
			/*0xA5*/	{"AND A,L", 4, [this](){ this->AND_A_r(this->HL.second()); }},
			/*0xA6*/	{"AND A,(HL)", 8, [this](){ this->AND_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0xA7*/	{"AND A,A", 4, [this](){ this->AND_A_r(this->AF.first()); }},
			

			/*0xA8*/	{"XOR A,B", 4, [this](){ this->XOR_A_r(this->BC.first()); }},
			/*0xA9*/	{"XOR A,C", 4, [this](){ this->XOR_A_r(this->BC.second()); }},
			/*0xAA*/	{"XOR A,D", 4, [this](){ this->XOR_A_r(this->DE.first()); }},
			/*0xAB*/	{"XOR A,E", 4, [this](){ this->XOR_A_r(this->DE.second()); }},
			/*0xAC*/	{"XOR A,H", 4, [this](){ this->XOR_A_r(this->HL.first()); }},
			/*0xAD*/	{"XOR A,L", 4, [this](){ this->XOR_A_r(this->HL.second()); }},
			/*0xAE*/	{"XOR A,(HL)", 8, [this](){ this->XOR_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0xAF*/	{"XOR A,A", 4, [this](){ this->XOR_A_r(this->AF.first()); }},

			/*0xB0*/	{"OR A,B", 4, [this](){ this->OR_A_r(this->BC.first()); }},
			/*0xB1*/	{"OR A,C", 4, [this](){ this->OR_A_r(this->BC.second()); }},
			/*0xB2*/	{"OR A,D", 4, [this](){ this->OR_A_r(this->DE.first()); }},
			/*0xB3*/	{"OR A,E", 4, [this](){ this->OR_A_r(this->DE.second()); }},
			/*0xB4*/	{"OR A,H", 4, [this](){ this->OR_A_r(this->HL.first()); }},
			/*0xB5*/	{"OR A,L", 4, [this](){ this->OR_A_r(this->HL.second()); }},
			/*0xB6*/	{"OR A,(HL)", 8, [this](){ this->OR_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0xB7*/	{"OR A,A", 4, [this](){ this->OR_A_r(this->AF.first()); }},

			/*0xB8*/	{"CP A,B", 4, [this](){ this->CP_A_r(this->BC.first()); }},
			/*0xB9*/	{"CP A,C", 4, [this](){ this->CP_A_r(this->BC.second()); }},
			/*0xBA*/	{"CP A,D", 4, [this](){ this->CP_A_r(this->DE.first()); }},
			/*0xBB*/	{"CP A,E", 4, [this](){ this->CP_A_r(this->DE.second()); }},
			/*0xBC*/	{"CP A,H", 4, [this](){ this->CP_A_r(this->HL.first()); }},
			/*0xBD*/	{"CP A,L", 4, [this](){ this->CP_A_r(this->HL.second()); }},
			/*0xBE*/	{"CP A,(HL)", 8, [this](){ this->CP_A_r(this->memoryMap.byte(this->HL.word())); }},
			/*0xBF*/	{"CP A,A", 4, [this](){ this->CP_A_r(this->AF.first()); }},
//
			/*0xC0*/	{"RET NZ", 20, [this](){ this->RET_cc(!this->getZFlag()); }},
			/*0xC1*/	{"POP BC", 12, [this](){ this->POP_rr(this->BC.word()); }},
			/*0xC2*/	{"JP NZ,a16", 16, [this](){ this->JP_cc_nn(!this->getZFlag()); }},
			/*0xC3*/	{"JP a16", 16, [this](){ this->JP_nn(); }},
			/*0xC4*/	{"CALL NZ,a16", 24, [this](){ this->CALL_cc_nn(!this->getZFlag()); }},
			/*0xC5*/	{"PUSH BC", 16, [this](){ this->PUSH_rr(this->BC); }},
			/*0xC6*/	{"ADD A,d8", 8, [this](){ this->ADD_A_r(this->getNextByte()); }},
			/*0xC7*/	{"RST 00H", 16, [this](){ this->RST(0x00); }},
			/*0xC8*/	{"RET Z", 20, [this](){ this->RET_cc(this->getZFlag()); }},
			/*0xC9*/	{"RET", 16, [this](){ this->RET(); }},
			/*0xCA*/	{"JP Z,a16", 16, [this](){ this->JP_cc_nn(this->getZFlag()); }},
			/*0xCB*/	{"CB", 0, [this](){}},
			/*0xCC*/	{"CALL Z,a16", 24, [this](){ this->CALL_cc_nn(this->getZFlag()); }},
			/*0xCD*/	{"CALL a16", 24, [this](){ this->CALL_nn(); }},
			/*0xCE*/	{"ADC A,d8", 8, [this](){ this->ADC_A_r(this->getNextByte()); }},
			/*0xCF*/	{"RST 08H", 16, [this](){ this->RST(0x08); }},

			/*0xD0*/	{"RET NC", 20, [this](){ this->RET_cc(!this->getCFlag()); }},
			/*0xD1*/	{"POP DE", 12, [this](){ this->POP_rr(this->DE.word()); }},
			/*0xD2*/	{"JP NC,a16", 16, [this](){ this->JP_cc_nn(!this->getCFlag()); }},
			/*0xD3*/	{"NULL", 0, [this](){ }},
			/*0xD4*/	{"CALL NC,a16", 24, [this](){ this->CALL_cc_nn(!this->getCFlag()); }},
			/*0xD5*/	{"PUSH DE", 16, [this](){ this->PUSH_rr(this->DE); }},
			/*0xD6*/	{"SUB A,d8", 8, [this](){ this->SUB_A_r(this->getNextByte()); }},
			/*0xD7*/	{"RST 10H", 16, [this](){ this->RST(0x10); }},
			/*0xD8*/	{"RET C", 20, [this](){ this->RET_cc(this->getCFlag()); }},
			/*0xD9*/	{"RETI", 16, [this](){ this->RETI(); }},
			/*0xDA*/	{"JP C,a16", 16, [this](){ this->JP_cc_nn(this->getCFlag()); }},
			/*0xDB*/	{"NULL", 0, [this](){ }},
			/*0xD4*/	{"CALL C,a16", 24, [this](){ this->CALL_cc_nn(this->getCFlag()); }},
			/*0xDD*/	{"NULL", 0, [this](){ }},
			/*0xDE*/	{"SBC A,d8", 8, [this](){ this->SBC_A_r(this->getNextByte());}},
			/*0xDF*/	{"RST 18H", 16, [this](){ this->RST(0x18); }},

			/*0xE0*/	{"LDH (a8),A", 12, [this](){ this->LD_r_r(this->memoryMap.byte(this->getNextByte()+0xFF00), this->AF.first()); }},
			/*0xE1*/	{"POP HL", 12, [this](){ this->POP_rr(this->HL.word()); }},
			/*0xE2*/	{"LD (C),A", 8, [this](){ this->LD_r_r(this->memoryMap.byte(this->BC.second()+0xFF00), this->AF.first()); }},
			/*0xE3*/	{"NULL", 0, [this](){ }},
			/*0xE4*/	{"NULL", 0, [this](){ }},
			/*0xE5*/	{"PUSH HL", 16, [this](){ this->PUSH_rr(this->HL); }},
			/*0xE6*/	{"AND A,d8", 8, [this](){ this->AND_A_r(this->getNextByte()); }},
			/*0xE7*/	{"RST 20H", 16, [this](){ this->RST(0x20); }},
			/*0xE8*/	{"ADD SP,r8", 16, [this](){ this->ADD_SP_s(); }},
			/*0xE9*/	{"JP HL", 4, [this](){ this->JP_rr(this->HL.word()); }},
			/*0xEA*/	{"LD (a16),A", 16, [this](){ this->LD_r_r(this->memoryMap.byte(this->getNextWord()), this->AF.first()); }},
			/*0xEB*/	{"NULL", 0, [this](){ }},
			/*0xEC*/	{"NULL", 0, [this](){ }},
			/*0xED*/	{"NULL", 0, [this](){ }},
			/*0xEE*/	{"XOR A,d8", 8, [this](){ this->XOR_A_r(this->getNextByte()); }},
			/*0xEF*/	{"RST 28H", 16, [this](){ this->RST(0x28); }},

			/*0xF0*/	{"LDH A,(a8)", 12, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->getNextByte()+0xFF00)); }},
			/*0xF1*/	{"POP AF", 12, [this](){ this->POP_rr(this->AF.word());}},
			/*0xF2*/	{"LD A,(C)", 8, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->BC.second()+0xFF00)); }},
			/*0xF3*/	{"DI", 4, [this](){ this->DI(); }},
			/*0xF4*/	{"NULL", 0, [this](){ }},
			/*0xF5*/	{"PUSH AF", 16, [this](){ this->PUSH_rr(this->AF); }},
			/*0xF6*/	{"OR A,d8", 8, [this](){ this->OR_A_r(this->getNextByte()); }},
			/*0xF7*/	{"RST 30H", 16, [this](){ this->RST(0x30); }},
			/*0xF8*/	{"LD HL,SP+r8", 12, [this](){ this->LD_rr_rr(this->HL.word(), ADD_SP_s_result()); }},
			/*0xF9*/	{"LD SP,HL", 8, [this](){ this->LD_rr_rr(this->SP.word(), this->HL.word()); }},
			/*0xFA*/	{"LD A,(a16)", 16, [this](){ this->LD_r_r(this->AF.first(), this->memoryMap.byte(this->getNextWord())); }},
			/*0xFB*/	{"EI", 4, [this](){ this->EI(); }},
			/*0xFC*/	{"NULL", 0, [this](){ }},
			/*0xFD*/	{"NULL", 0, [this](){ }},
			/*0xFE*/	{"CP A,d8", 8, [this](){ this->CP_A_r(this->getNextByte()); }},
			/*0xFF*/	{"RST 38H", 16, [this](){ this->RST(0x38); }},
		};

		const Operation cbInstructionSet[0x100] = {
			/*0x00*/	{"RLC B", 8, [this](){ this->RLC(this->BC.first()); }},
			/*0x01*/	{"RLC C", 8, [this](){ this->RLC(this->BC.second()); }},
			/*0x02*/	{"RLC D", 8, [this](){ this->RLC(this->DE.first()); }},
			/*0x03*/	{"RLC E", 8, [this](){ this->RLC(this->DE.second()); }},
			/*0x04*/	{"RLC H", 8, [this](){ this->RLC(this->HL.first()); }},
			/*0x05*/	{"RLC L", 8, [this](){ this->RLC(this->HL.second()); }},
			/*0x06*/	{"RLC (HL)", 16, [this](){ this->RLC(this->memoryMap.byte(this->HL.word())); }},
			/*0x07*/	{"RRC A", 8, [this](){ this->RRC(this->AF.first()); }},
			/*0x08*/	{"RRC B", 8, [this](){ this->RRC(this->BC.first()); }},
			/*0x09*/	{"RRC C", 8, [this](){ this->RRC(this->BC.second()); }},
			/*0x0A*/	{"RRC D", 8, [this](){ this->RRC(this->DE.first()); }},
			/*0x0B*/	{"RRC E", 8, [this](){ this->RRC(this->DE.second()); }},
			/*0x0C*/	{"RRC H", 8, [this](){ this->RRC(this->HL.first()); }},
			/*0x0D*/	{"RRC L", 8, [this](){ this->RRC(this->HL.second()); }},
			/*0x0E*/	{"RRC (HL)", 16, [this](){ this->RRC(this->memoryMap.byte(this->HL.word())); }},
			/*0x0F*/	{"RRC A", 8, [this](){ this->RRC(this->AF.first()); }},
		
			/*0x10*/	{"RL B", 8, [this](){ this->RL(this->BC.first()); }},
			/*0x11*/	{"RL C", 8, [this](){ this->RL(this->BC.second()); }},
			/*0x12*/	{"RL D", 8, [this](){ this->RL(this->DE.first()); }},
			/*0x13*/	{"RL E", 8, [this](){ this->RL(this->DE.second()); }},
			/*0x14*/	{"RL H", 8, [this](){ this->RL(this->HL.first()); }},
			/*0x15*/	{"RL L", 8, [this](){ this->RL(this->HL.second()); }},
			/*0x16*/	{"RL (HL)", 16, [this](){ this->RLC(this->memoryMap.byte(this->HL.word())); }},
			/*0x17*/	{"RR A", 8, [this](){ this->RR(this->AF.first()); }},
			/*0x18*/	{"RR B", 8, [this](){ this->RR(this->BC.first()); }},
			/*0x19*/	{"RR C", 8, [this](){ this->RR(this->BC.second()); }},
			/*0x1A*/	{"RR D", 8, [this](){ this->RR(this->DE.first()); }},
			/*0x1B*/	{"RR E", 8, [this](){ this->RR(this->DE.second()); }},
			/*0x1C*/	{"RR H", 8, [this](){ this->RR(this->HL.first()); }},
			/*0x1D*/	{"RR L", 8, [this](){ this->RR(this->HL.second()); }},
			/*0x1E*/	{"RR (HL)", 16, [this](){ this->RRC(this->memoryMap.byte(this->HL.word())); }},
			/*0x1F*/	{"RR A", 8, [this](){ this->RR(this->AF.first()); }},

			/*0x20*/	{"SLA B", 8, [this](){ this->SLA(this->BC.first()); }},
			/*0x21*/	{"SLA C", 8, [this](){ this->SLA(this->BC.second()); }},
			/*0x22*/	{"SLA D", 8, [this](){ this->SLA(this->DE.first()); }},
			/*0x23*/	{"SLA E", 8, [this](){ this->SLA(this->DE.second()); }},
			/*0x24*/	{"SLA H", 8, [this](){ this->SLA(this->HL.first()); }},
			/*0x25*/	{"SLA L", 8, [this](){ this->SLA(this->HL.second()); }},
			/*0x26*/	{"SLA (HL)", 16, [this](){ this->SLA(this->memoryMap.byte(this->HL.word())); }},
			/*0x27*/	{"SLA A", 8, [this](){ this->SLA(this->AF.first()); }},
			/*0x28*/	{"SRA B", 8, [this](){ this->SRA(this->BC.first()); }},
			/*0x29*/	{"SRA C", 8, [this](){ this->SRA(this->BC.second()); }},
			/*0x2A*/	{"SRA D", 8, [this](){ this->SRA(this->DE.first()); }},
			/*0x2B*/	{"SRA E", 8, [this](){ this->SRA(this->DE.second()); }},
			/*0x2C*/	{"SRA H", 8, [this](){ this->SRA(this->HL.first()); }},
			/*0x2D*/	{"SRA L", 8, [this](){ this->SRA(this->HL.second()); }},
			/*0x2E*/	{"SRA (HL)", 16, [this](){ this->SRA(this->memoryMap.byte(this->HL.word())); }},
			/*0x2F*/	{"SRA A", 8, [this](){ this->SRA(this->AF.first()); }},

			/*0x30*/	{"SWAP B", 8, [this](){ this->SWAP(this->BC.first()); }},
			/*0x31*/	{"SWAP C", 8, [this](){ this->SWAP(this->BC.second()); }},
			/*0x32*/	{"SWAP D", 8, [this](){ this->SWAP(this->DE.first()); }},
			/*0x33*/	{"SWAP E", 8, [this](){ this->SWAP(this->DE.second()); }},
			/*0x34*/	{"SWAP H", 8, [this](){ this->SWAP(this->HL.first()); }},
			/*0x35*/	{"SWAP L", 8, [this](){ this->SWAP(this->HL.second()); }},
			/*0x36*/	{"SWAP (HL)", 16, [this](){ this->SWAP(this->memoryMap.byte(this->HL.word())); }},
			/*0x37*/	{"SWAP A", 8, [this](){ this->SWAP(this->AF.first()); }},
			/*0x38*/	{"SRL B", 8, [this](){ this->SRL(this->BC.first()); }},
			/*0x39*/	{"SRL C", 8, [this](){ this->SRL(this->BC.second()); }},
			/*0x3A*/	{"SRL D", 8, [this](){ this->SRL(this->DE.first()); }},
			/*0x3B*/	{"SRL E", 8, [this](){ this->SRL(this->DE.second()); }},
			/*0x3C*/	{"SRL H", 8, [this](){ this->SRL(this->HL.first()); }},
			/*0x3D*/	{"SRL L", 8, [this](){ this->SRL(this->HL.second()); }},
			/*0x3E*/	{"SRL (HL)", 16, [this](){ this->SRL(this->memoryMap.byte(this->HL.word())); }},
			/*0x3F*/	{"SRL A", 8, [this](){ this->SRL(this->AF.first()); }},

			/*0x40*/	{"BIT 0,B", 8, [this](){ this->BIT_b_r(0,this->BC.first()); }},
			/*0x41*/	{"BIT 0,C", 8, [this](){ this->BIT_b_r(0,this->BC.second()); }},
			/*0x42*/	{"BIT 0,D", 8, [this](){ this->BIT_b_r(0,this->DE.first()); }},
			/*0x43*/	{"BIT 0,E", 8, [this](){ this->BIT_b_r(0,this->DE.second()); }},
			/*0x44*/	{"BIT 0,H", 8, [this](){ this->BIT_b_r(0,this->HL.first()); }},
			/*0x45*/	{"BIT 0,L", 8, [this](){ this->BIT_b_r(0,this->HL.second()); }},
			/*0x46*/	{"BIT 0,(HL)", 16, [this](){ this->BIT_b_r(0,this->memoryMap.byte(this->HL.word())); }},
			/*0x47*/	{"BIT 0,A", 8, [this](){ this->BIT_b_r(0,this->AF.first()); }},
			/*0x48*/	{"BIT 1,B", 8, [this](){ this->BIT_b_r(1,this->BC.first()); }},
			/*0x49*/	{"BIT 1,C", 8, [this](){ this->BIT_b_r(1,this->BC.second()); }},
			/*0x4A*/	{"BIT 1,D", 8, [this](){ this->BIT_b_r(1,this->DE.first()); }},
			/*0x4B*/	{"BIT 1,E", 8, [this](){ this->BIT_b_r(1,this->DE.second()); }},
			/*0x4C*/	{"BIT 1,H", 8, [this](){ this->BIT_b_r(1,this->HL.first()); }},
			/*0x4D*/	{"BIT 1,L", 8, [this](){ this->BIT_b_r(1,this->HL.second()); }},
			/*0x4E*/	{"BIT 1,(HL)", 16, [this](){ this->BIT_b_r(1,this->memoryMap.byte(this->HL.word())); }},
			/*0x4F*/	{"BIT 1,A", 8, [this](){ this->BIT_b_r(1,this->AF.first()); }},

			/*0x50*/	{"BIT 2,B", 8, [this](){ this->BIT_b_r(2,this->BC.first()); }},
			/*0x51*/	{"BIT 2,C", 8, [this](){ this->BIT_b_r(2,this->BC.second()); }},
			/*0x52*/	{"BIT 2,D", 8, [this](){ this->BIT_b_r(2,this->DE.first()); }},
			/*0x53*/	{"BIT 2,E", 8, [this](){ this->BIT_b_r(2,this->DE.second()); }},
			/*0x54*/	{"BIT 2,H", 8, [this](){ this->BIT_b_r(2,this->HL.first()); }},
			/*0x55*/	{"BIT 2,L", 8, [this](){ this->BIT_b_r(2,this->HL.second()); }},
			/*0x56*/	{"BIT 2,(HL)", 16, [this](){ this->BIT_b_r(2,this->memoryMap.byte(this->HL.word())); }},
			/*0x57*/	{"BIT 2,A", 8, [this](){ this->BIT_b_r(2,this->AF.first()); }},
			/*0x58*/	{"BIT 3,B", 8, [this](){ this->BIT_b_r(3,this->BC.first()); }},
			/*0x59*/	{"BIT 3,C", 8, [this](){ this->BIT_b_r(3,this->BC.second()); }},
			/*0x5A*/	{"BIT 3,D", 8, [this](){ this->BIT_b_r(3,this->DE.first()); }},
			/*0x5B*/	{"BIT 3,E", 8, [this](){ this->BIT_b_r(3,this->DE.second()); }},
			/*0x5C*/	{"BIT 3,H", 8, [this](){ this->BIT_b_r(3,this->HL.first()); }},
			/*0x5D*/	{"BIT 3,L", 8, [this](){ this->BIT_b_r(3,this->HL.second()); }},
			/*0x5E*/	{"BIT 3,(HL)", 16, [this](){ this->BIT_b_r(3,this->memoryMap.byte(this->HL.word())); }},
			/*0x5F*/	{"BIT 3,A", 8, [this](){ this->BIT_b_r(3,this->AF.first()); }},

			/*0x60*/	{"BIT 4,B", 8, [this](){ this->BIT_b_r(4,this->BC.first()); }},
			/*0x61*/	{"BIT 4,C", 8, [this](){ this->BIT_b_r(4,this->BC.second()); }},
			/*0x62*/	{"BIT 4,D", 8, [this](){ this->BIT_b_r(4,this->DE.first()); }},
			/*0x63*/	{"BIT 4,E", 8, [this](){ this->BIT_b_r(4,this->DE.second()); }},
			/*0x64*/	{"BIT 4,H", 8, [this](){ this->BIT_b_r(4,this->HL.first()); }},
			/*0x65*/	{"BIT 4,L", 8, [this](){ this->BIT_b_r(4,this->HL.second()); }},
			/*0x66*/	{"BIT 4,(HL)", 16, [this](){ this->BIT_b_r(4,this->memoryMap.byte(this->HL.word())); }},
			/*0x67*/	{"BIT 4,A", 8, [this](){ this->BIT_b_r(4,this->AF.first()); }},
			/*0x68*/	{"BIT 5,B", 8, [this](){ this->BIT_b_r(5,this->BC.first()); }},
			/*0x69*/	{"BIT 5,C", 8, [this](){ this->BIT_b_r(5,this->BC.second()); }},
			/*0x6A*/	{"BIT 5,D", 8, [this](){ this->BIT_b_r(5,this->DE.first()); }},
			/*0x6B*/	{"BIT 5,E", 8, [this](){ this->BIT_b_r(5,this->DE.second()); }},
			/*0x6C*/	{"BIT 5,H", 8, [this](){ this->BIT_b_r(5,this->HL.first()); }},
			/*0x6D*/	{"BIT 5,L", 8, [this](){ this->BIT_b_r(5,this->HL.second()); }},
			/*0x6E*/	{"BIT 5,(HL)", 16, [this](){ this->BIT_b_r(5,this->memoryMap.byte(this->HL.word())); }},
			/*0x6F*/	{"BIT 5,A", 8, [this](){ this->BIT_b_r(5,this->AF.first()); }},

			/*0x70*/	{"BIT 6,B", 8, [this](){ this->BIT_b_r(6,this->BC.first()); }},
			/*0x71*/	{"BIT 6,C", 8, [this](){ this->BIT_b_r(6,this->BC.second()); }},
			/*0x72*/	{"BIT 6,D", 8, [this](){ this->BIT_b_r(6,this->DE.first()); }},
			/*0x73*/	{"BIT 6,E", 8, [this](){ this->BIT_b_r(6,this->DE.second()); }},
			/*0x74*/	{"BIT 6,H", 8, [this](){ this->BIT_b_r(6,this->HL.first()); }},
			/*0x75*/	{"BIT 6,L", 8, [this](){ this->BIT_b_r(6,this->HL.second()); }},
			/*0x76*/	{"BIT 6,(HL)", 16, [this](){ this->BIT_b_r(6,this->memoryMap.byte(this->HL.word())); }},
			/*0x77*/	{"BIT 6,A", 8, [this](){ this->BIT_b_r(6,this->AF.first()); }},
			/*0x78*/	{"BIT 7,B", 8, [this](){ this->BIT_b_r(7,this->BC.first()); }},
			/*0x79*/	{"BIT 7,C", 8, [this](){ this->BIT_b_r(7,this->BC.second()); }},
			/*0x7A*/	{"BIT 7,D", 8, [this](){ this->BIT_b_r(7,this->DE.first()); }},
			/*0x7B*/	{"BIT 7,E", 8, [this](){ this->BIT_b_r(7,this->DE.second()); }},
			/*0x7C*/	{"BIT 7,H", 8, [this](){ this->BIT_b_r(7,this->HL.first()); }},
			/*0x7D*/	{"BIT 7,L", 8, [this](){ this->BIT_b_r(7,this->HL.second()); }},
			/*0x7E*/	{"BIT 7,(HL)", 16, [this](){ this->BIT_b_r(7,this->memoryMap.byte(this->HL.word())); }},
			/*0x7F*/	{"BIT 7,A", 8, [this](){ this->BIT_b_r(7,this->AF.first()); }},

			/*0x80*/	{"RES 0,B", 8, [this](){ this->RES_b_r(0,this->BC.first()); }},
			/*0x81*/	{"RES 0,C", 8, [this](){ this->RES_b_r(0,this->BC.second()); }},
			/*0x82*/	{"RES 0,D", 8, [this](){ this->RES_b_r(0,this->DE.first()); }},
			/*0x83*/	{"RES 0,E", 8, [this](){ this->RES_b_r(0,this->DE.second()); }},
			/*0x84*/	{"RES 0,H", 8, [this](){ this->RES_b_r(0,this->HL.first()); }},
			/*0x85*/	{"RES 0,L", 8, [this](){ this->RES_b_r(0,this->HL.second()); }},
			/*0x86*/	{"RES 0,(HL)", 16, [this](){ this->RES_b_r(0,this->memoryMap.byte(this->HL.word())); }},
			/*0x87*/	{"RES 0,A", 8, [this](){ this->RES_b_r(0,this->AF.first()); }},
			/*0x88*/	{"RES 1,B", 8, [this](){ this->RES_b_r(1,this->BC.first()); }},
			/*0x89*/	{"RES 1,C", 8, [this](){ this->RES_b_r(1,this->BC.second()); }},
			/*0x8A*/	{"RES 1,D", 8, [this](){ this->RES_b_r(1,this->DE.first()); }},
			/*0x8B*/	{"RES 1,E", 8, [this](){ this->RES_b_r(1,this->DE.second()); }},
			/*0x8C*/	{"RES 1,H", 8, [this](){ this->RES_b_r(1,this->HL.first()); }},
			/*0x8D*/	{"RES 1,L", 8, [this](){ this->RES_b_r(1,this->HL.second()); }},
			/*0x8E*/	{"RES 1,(HL)", 16, [this](){ this->RES_b_r(1,this->memoryMap.byte(this->HL.word())); }},
			/*0x8F*/	{"RES 1,A", 8, [this](){ this->RES_b_r(1,this->AF.first()); }},

			/*0x90*/	{"RES 2,B", 8, [this](){ this->RES_b_r(2,this->BC.first()); }},
			/*0x91*/	{"RES 2,C", 8, [this](){ this->RES_b_r(2,this->BC.second()); }},
			/*0x92*/	{"RES 2,D", 8, [this](){ this->RES_b_r(2,this->DE.first()); }},
			/*0x93*/	{"RES 2,E", 8, [this](){ this->RES_b_r(2,this->DE.second()); }},
			/*0x94*/	{"RES 2,H", 8, [this](){ this->RES_b_r(2,this->HL.first()); }},
			/*0x95*/	{"RES 2,L", 8, [this](){ this->RES_b_r(2,this->HL.second()); }},
			/*0x96*/	{"RES 2,(HL)", 16, [this](){ this->RES_b_r(2,this->memoryMap.byte(this->HL.word())); }},
			/*0x97*/	{"RES 2,A", 8, [this](){ this->RES_b_r(2,this->AF.first()); }},
			/*0x98*/	{"RES 3,B", 8, [this](){ this->RES_b_r(3,this->BC.first()); }},
			/*0x99*/	{"RES 3,C", 8, [this](){ this->RES_b_r(3,this->BC.second()); }},
			/*0x9A*/	{"RES 3,D", 8, [this](){ this->RES_b_r(3,this->DE.first()); }},
			/*0x9B*/	{"RES 3,E", 8, [this](){ this->RES_b_r(3,this->DE.second()); }},
			/*0x9C*/	{"RES 3,H", 8, [this](){ this->RES_b_r(3,this->HL.first()); }},
			/*0x9D*/	{"RES 3,L", 8, [this](){ this->RES_b_r(3,this->HL.second()); }},
			/*0x9E*/	{"RES 3,(HL)", 16, [this](){ this->RES_b_r(3,this->memoryMap.byte(this->HL.word())); }},
			/*0x9F*/	{"RES 3,A", 8, [this](){ this->RES_b_r(3,this->AF.first()); }},

			/*0xA0*/	{"RES 4,B", 8, [this](){ this->RES_b_r(4,this->BC.first()); }},
			/*0xA1*/	{"RES 4,C", 8, [this](){ this->RES_b_r(4,this->BC.second()); }},
			/*0xA2*/	{"RES 4,D", 8, [this](){ this->RES_b_r(4,this->DE.first()); }},
			/*0xA3*/	{"RES 4,E", 8, [this](){ this->RES_b_r(4,this->DE.second()); }},
			/*0xA4*/	{"RES 4,H", 8, [this](){ this->RES_b_r(4,this->HL.first()); }},
			/*0xA5*/	{"RES 4,L", 8, [this](){ this->RES_b_r(4,this->HL.second()); }},
			/*0xA6*/	{"RES 4,(HL)", 16, [this](){ this->RES_b_r(4,this->memoryMap.byte(this->HL.word())); }},
			/*0xA7*/	{"RES 4,A", 8, [this](){ this->RES_b_r(4,this->AF.first()); }},
			/*0xA8*/	{"RES 5,B", 8, [this](){ this->RES_b_r(5,this->BC.first()); }},
			/*0xA9*/	{"RES 5,C", 8, [this](){ this->RES_b_r(5,this->BC.second()); }},
			/*0xAA*/	{"RES 5,D", 8, [this](){ this->RES_b_r(5,this->DE.first()); }},
			/*0xAB*/	{"RES 5,E", 8, [this](){ this->RES_b_r(5,this->DE.second()); }},
			/*0xAC*/	{"RES 5,H", 8, [this](){ this->RES_b_r(5,this->HL.first()); }},
			/*0xAD*/	{"RES 5,L", 8, [this](){ this->RES_b_r(5,this->HL.second()); }},
			/*0xAE*/	{"RES 5,(HL)", 16, [this](){ this->RES_b_r(5,this->memoryMap.byte(this->HL.word())); }},
			/*0xAF*/	{"RES 5,A", 8, [this](){ this->RES_b_r(5,this->AF.first()); }},

			/*0xB0*/	{"RES 6,B", 8, [this](){ this->RES_b_r(6,this->BC.first()); }},
			/*0xB1*/	{"RES 6,C", 8, [this](){ this->RES_b_r(6,this->BC.second()); }},
			/*0xB2*/	{"RES 6,D", 8, [this](){ this->RES_b_r(6,this->DE.first()); }},
			/*0xB3*/	{"RES 6,E", 8, [this](){ this->RES_b_r(6,this->DE.second()); }},
			/*0xB4*/	{"RES 6,H", 8, [this](){ this->RES_b_r(6,this->HL.first()); }},
			/*0xB5*/	{"RES 6,L", 8, [this](){ this->RES_b_r(6,this->HL.second()); }},
			/*0xB6*/	{"RES 6,(HL)", 16, [this](){ this->RES_b_r(6,this->memoryMap.byte(this->HL.word())); }},
			/*0xB7*/	{"RES 6,A", 8, [this](){ this->RES_b_r(6,this->AF.first()); }},
			/*0xB8*/	{"RES 7,B", 8, [this](){ this->RES_b_r(7,this->BC.first()); }},
			/*0xB9*/	{"RES 7,C", 8, [this](){ this->RES_b_r(7,this->BC.second()); }},
			/*0xBA*/	{"RES 7,D", 8, [this](){ this->RES_b_r(7,this->DE.first()); }},
			/*0xBB*/	{"RES 7,E", 8, [this](){ this->RES_b_r(7,this->DE.second()); }},
			/*0xBC*/	{"RES 7,H", 8, [this](){ this->RES_b_r(7,this->HL.first()); }},
			/*0xBD*/	{"RES 7,L", 8, [this](){ this->RES_b_r(7,this->HL.second()); }},
			/*0xBE*/	{"RES 7,(HL)", 16, [this](){ this->RES_b_r(7,this->memoryMap.byte(this->HL.word())); }},
			/*0xBF*/	{"RES 7,A", 8, [this](){ this->RES_b_r(7,this->AF.first()); }},

			/*0xC0*/	{"SET 0,B", 8, [this](){ this->SET_b_r(0,this->BC.first()); }},
			/*0xC1*/	{"SET 0,C", 8, [this](){ this->SET_b_r(0,this->BC.second()); }},
			/*0xC2*/	{"SET 0,D", 8, [this](){ this->SET_b_r(0,this->DE.first()); }},
			/*0xC3*/	{"SET 0,E", 8, [this](){ this->SET_b_r(0,this->DE.second()); }},
			/*0xC4*/	{"SET 0,H", 8, [this](){ this->SET_b_r(0,this->HL.first()); }},
			/*0xC5*/	{"SET 0,L", 8, [this](){ this->SET_b_r(0,this->HL.second()); }},
			/*0xC6*/	{"SET 0,(HL)", 16, [this](){ this->SET_b_r(0,this->memoryMap.byte(this->HL.word())); }},
			/*0xC7*/	{"SET 0,A", 8, [this](){ this->SET_b_r(0,this->AF.first()); }},
			/*0xC8*/	{"SET 1,B", 8, [this](){ this->SET_b_r(1,this->BC.first()); }},
			/*0xC9*/	{"SET 1,C", 8, [this](){ this->SET_b_r(1,this->BC.second()); }},
			/*0xCA*/	{"SET 1,D", 8, [this](){ this->SET_b_r(1,this->DE.first()); }},
			/*0xCB*/	{"SET 1,E", 8, [this](){ this->SET_b_r(1,this->DE.second()); }},
			/*0xCC*/	{"SET 1,H", 8, [this](){ this->SET_b_r(1,this->HL.first()); }},
			/*0xCD*/	{"SET 1,L", 8, [this](){ this->SET_b_r(1,this->HL.second()); }},
			/*0xCE*/	{"SET 1,(HL)", 16, [this](){ this->SET_b_r(1,this->memoryMap.byte(this->HL.word())); }},
			/*0xCF*/	{"SET 1,A", 8, [this](){ this->SET_b_r(1,this->AF.first()); }},

			/*0xD0*/	{"SET 2,B", 8, [this](){ this->SET_b_r(2,this->BC.first()); }},
			/*0xD1*/	{"SET 2,C", 8, [this](){ this->SET_b_r(2,this->BC.second()); }},
			/*0xD2*/	{"SET 2,D", 8, [this](){ this->SET_b_r(2,this->DE.first()); }},
			/*0xD3*/	{"SET 2,E", 8, [this](){ this->SET_b_r(2,this->DE.second()); }},
			/*0xD4*/	{"SET 2,H", 8, [this](){ this->SET_b_r(2,this->HL.first()); }},
			/*0xD5*/	{"SET 2,L", 8, [this](){ this->SET_b_r(2,this->HL.second()); }},
			/*0xD6*/	{"SET 2,(HL)", 16, [this](){ this->SET_b_r(2,this->memoryMap.byte(this->HL.word())); }},
			/*0xD7*/	{"SET 2,A", 8, [this](){ this->SET_b_r(2,this->AF.first()); }},
			/*0xD8*/	{"SET 3,B", 8, [this](){ this->SET_b_r(3,this->BC.first()); }},
			/*0xD9*/	{"SET 3,C", 8, [this](){ this->SET_b_r(3,this->BC.second()); }},
			/*0xDA*/	{"SET 3,D", 8, [this](){ this->SET_b_r(3,this->DE.first()); }},
			/*0xDB*/	{"SET 3,E", 8, [this](){ this->SET_b_r(3,this->DE.second()); }},
			/*0xDC*/	{"SET 3,H", 8, [this](){ this->SET_b_r(3,this->HL.first()); }},
			/*0xDD*/	{"SET 3,L", 8, [this](){ this->SET_b_r(3,this->HL.second()); }},
			/*0xDE*/	{"SET 3,(HL)", 16, [this](){ this->SET_b_r(3,this->memoryMap.byte(this->HL.word())); }},
			/*0xDF*/	{"SET 3,A", 8, [this](){ this->SET_b_r(3,this->AF.first()); }},

			/*0xE0*/	{"SET 4,B", 8, [this](){ this->SET_b_r(4,this->BC.first()); }},
			/*0xE1*/	{"SET 4,C", 8, [this](){ this->SET_b_r(4,this->BC.second()); }},
			/*0xE2*/	{"SET 4,D", 8, [this](){ this->SET_b_r(4,this->DE.first()); }},
			/*0xE3*/	{"SET 4,E", 8, [this](){ this->SET_b_r(4,this->DE.second()); }},
			/*0xE4*/	{"SET 4,H", 8, [this](){ this->SET_b_r(4,this->HL.first()); }},
			/*0xE5*/	{"SET 4,L", 8, [this](){ this->SET_b_r(4,this->HL.second()); }},
			/*0xE6*/	{"SET 4,(HL)", 16, [this](){ this->SET_b_r(4,this->memoryMap.byte(this->HL.word())); }},
			/*0xE7*/	{"SET 4,A", 8, [this](){ this->SET_b_r(4,this->AF.first()); }},
			/*0xE8*/	{"SET 5,B", 8, [this](){ this->SET_b_r(5,this->BC.first()); }},
			/*0xE9*/	{"SET 5,C", 8, [this](){ this->SET_b_r(5,this->BC.second()); }},
			/*0xEA*/	{"SET 5,D", 8, [this](){ this->SET_b_r(5,this->DE.first()); }},
			/*0xEB*/	{"SET 5,E", 8, [this](){ this->SET_b_r(5,this->DE.second()); }},
			/*0xEC*/	{"SET 5,H", 8, [this](){ this->SET_b_r(5,this->HL.first()); }},
			/*0xED*/	{"SET 5,L", 8, [this](){ this->SET_b_r(5,this->HL.second()); }},
			/*0xEE*/	{"SET 5,(HL)", 16, [this](){ this->SET_b_r(5,this->memoryMap.byte(this->HL.word())); }},
			/*0xEF*/	{"SET 5,A", 8, [this](){ this->SET_b_r(5,this->AF.first()); }},

			/*0xF0*/	{"SET 6,B", 8, [this](){ this->SET_b_r(6,this->BC.first()); }},
			/*0xF1*/	{"SET 6,C", 8, [this](){ this->SET_b_r(6,this->BC.second()); }},
			/*0xF2*/	{"SET 6,D", 8, [this](){ this->SET_b_r(6,this->DE.first()); }},
			/*0xF3*/	{"SET 6,E", 8, [this](){ this->SET_b_r(6,this->DE.second()); }},
			/*0xF4*/	{"SET 6,H", 8, [this](){ this->SET_b_r(6,this->HL.first()); }},
			/*0xF5*/	{"SET 6,L", 8, [this](){ this->SET_b_r(6,this->HL.second()); }},
			/*0xF6*/	{"SET 6,(HL)", 16, [this](){ this->SET_b_r(6,this->memoryMap.byte(this->HL.word())); }},
			/*0xF7*/	{"SET 6,A", 8, [this](){ this->SET_b_r(6,this->AF.first()); }},
			/*0xF8*/	{"SET 7,B", 8, [this](){ this->SET_b_r(7,this->BC.first()); }},
			/*0xF9*/	{"SET 7,C", 8, [this](){ this->SET_b_r(7,this->BC.second()); }},
			/*0xFA*/	{"SET 7,D", 8, [this](){ this->SET_b_r(7,this->DE.first()); }},
			/*0xFB*/	{"SET 7,E", 8, [this](){ this->SET_b_r(7,this->DE.second()); }},
			/*0xFC*/	{"SET 7,H", 8, [this](){ this->SET_b_r(7,this->HL.first()); }},
			/*0xFD*/	{"SET 7,L", 8, [this](){ this->SET_b_r(7,this->HL.second()); }},
			/*0xFE*/	{"SET 7,(HL)", 16, [this](){ this->SET_b_r(7,this->memoryMap.byte(this->HL.word())); }},
			/*0xFF*/	{"SET 7,A", 8, [this](){ this->SET_b_r(7,this->AF.first()); }},
		};
};