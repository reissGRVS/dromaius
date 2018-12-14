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

		MemoryMap& memoryMap;
		
		Byte getNextByte();
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
				byte++;
			}

			void DEC_r(Byte& byte){
				byte--;
			}

			/*  DEC s
			*/

		//OPCODES

		const Operation instructionSet[0x100] = {
			/*0x00*/	{"NOP", 4, [this](){}},
			/*0x01*/	{"???", 0, [this](){ this->ping(); }},
			/*0x02*/	{"???", 0, [this](){ this->ping(); }},
			/*0x03*/	{"???", 0, [this](){ this->ping(); }},
			/*0x04*/	{"INC B", 4, [this](){ this->INC_r(this->BC.first()); }},
			/*0x05*/	{"DEC B", 4, [this](){ this->DEC_r(this->BC.first()); }},
			/*0x06*/	{"LD B, n", 8, [this](){ this->LD_r_n(this->BC.first()); }},
			/*0x07*/	{"???", 0, [this](){ this->ping(); }},

			/*0x08*/	{"???", 0, [this](){ this->ping(); }},
			/*0x09*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0C*/	{"INC C", 4, [this](){ this->INC_r(this->BC.second()); }},
			/*0x0D*/	{"DEC C", 4, [this](){ this->DEC_r(this->BC.second()); }},
			/*0x0E*/	{"LD C, n", 8, [this](){ this->LD_r_n(this->BC.second()); }},
			/*0x0F*/	{"???", 0, [this](){ this->ping(); }},
			
			/*0x10*/	{"???", 0, [this](){ this->ping(); }},
			/*0x11*/	{"???", 0, [this](){ this->ping(); }},
			/*0x12*/	{"???", 0, [this](){ this->ping(); }},
			/*0x13*/	{"???", 0, [this](){ this->ping(); }},
			/*0x14*/	{"INC D", 4, [this](){ this->INC_r(this->DE.first()); }},
			/*0x15*/	{"DEC D", 4, [this](){ this->DEC_r(this->DE.first()); }},
			/*0x16*/	{"LD D, n", 8, [this](){ this->LD_r_n(this->DE.first()); }},
			/*0x17*/	{"???", 0, [this](){ this->ping(); }},

			/*0x18*/	{"???", 0, [this](){ this->ping(); }},
			/*0x19*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1C*/	{"INC E", 4, [this](){ this->INC_r(this->DE.second()); }},
			/*0x1D*/	{"DEC E", 4, [this](){ this->DEC_r(this->DE.second()); }},
			/*0x1E*/	{"LD E, n", 8, [this](){ this->LD_r_n(this->DE.second()); }},
			/*0x1F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x20*/	{"???", 0, [this](){ this->ping(); }},
			/*0x21*/	{"???", 0, [this](){ this->ping(); }},
			/*0x22*/	{"???", 0, [this](){ this->ping(); }},
			/*0x23*/	{"???", 0, [this](){ this->ping(); }},
			/*0x24*/	{"INC H", 4, [this](){ this->INC_r(this->HL.first()); }},
			/*0x25*/	{"DEC H", 4, [this](){ this->DEC_r(this->HL.first()); }},
			/*0x26*/	{"LD H, n", 8, [this](){ this->LD_r_n(this->HL.first()); }},
			/*0x27*/	{"???", 0, [this](){ this->ping(); }},

			/*0x28*/	{"???", 0, [this](){ this->ping(); }},
			/*0x29*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2C*/	{"INC L", 4, [this](){ this->INC_r(this->HL.second()); }},
			/*0x2D*/	{"DEC L", 4, [this](){ this->DEC_r(this->HL.second()); }},
			/*0x2E*/	{"LD L, n", 8, [this](){ this->LD_r_n(this->HL.second()); }},
			/*0x2F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x30*/	{"???", 0, [this](){ this->ping(); }},
			/*0x31*/	{"???", 0, [this](){ this->ping(); }},
			/*0x32*/	{"???", 0, [this](){ this->ping(); }},
			/*0x33*/	{"???", 0, [this](){ this->ping(); }},
			/*0x34*/	{"???", 0, [this](){ this->ping(); }},
			/*0x35*/	{"???", 0, [this](){ this->ping(); }},
			/*0x36*/	{"???", 0, [this](){ this->ping(); }},
			/*0x37*/	{"???", 0, [this](){ this->ping(); }},

			/*0x38*/	{"???", 0, [this](){ this->ping(); }},
			/*0x39*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3C*/	{"INC A", 4, [this](){ this->INC_r(this->AF.first()); }},
			/*0x3D*/	{"DEC A", 4, [this](){ this->DEC_r(this->AF.first()); }},
			/*0x3E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x40*/	{"LD B,B", 4, [this](){ this->LD_r_r(this->BC.first(), this->BC.first());}},
			/*0x41*/	{"LD B,C", 4, [this](){ this->LD_r_r(this->BC.first(), this->BC.second());}},
			/*0x42*/	{"LD B,D", 4, [this](){ this->LD_r_r(this->BC.first(), this->DE.first());}},
			/*0x43*/	{"LD B,E", 4, [this](){ this->LD_r_r(this->BC.first(), this->DE.second());}},
			/*0x44*/	{"LD B,H", 4, [this](){ this->LD_r_r(this->BC.first(), this->HL.first());}},
			/*0x45*/	{"LD B,L", 4, [this](){ this->LD_r_r(this->BC.first(), this->HL.second());}},
			/*0x46*/	{"LD B,(HL)", 0, [this](){ this->ping(); }},
			/*0x47*/	{"???", 0, [this](){ this->ping(); }},

			/*0x48*/	{"LD C,B", 4, [this](){ this->LD_r_r(this->BC.second(), this->BC.first());}},
			/*0x49*/	{"LD C,C", 4, [this](){ this->LD_r_r(this->BC.second(), this->BC.second());}},
			/*0x4A*/	{"LD C,D", 4, [this](){ this->LD_r_r(this->BC.second(), this->DE.first());}},
			/*0x4B*/	{"LD C,E", 4, [this](){ this->LD_r_r(this->BC.second(), this->DE.second());}},
			/*0x4C*/	{"LD C,H", 4, [this](){ this->LD_r_r(this->BC.second(), this->HL.first());}},
			/*0x4D*/	{"LD C,L", 4, [this](){ this->LD_r_r(this->BC.second(), this->HL.second());}},
			/*0x4E*/	{"LD C,(HL)", 0, [this](){ this->ping(); }},
			/*0x4F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x50*/	{"LD D,B", 4, [this](){ this->LD_r_r(this->DE.first(), this->BC.first());}},
			/*0x51*/	{"LD D,C", 4, [this](){ this->LD_r_r(this->DE.first(), this->BC.second());}},
			/*0x52*/	{"LD D,D", 4, [this](){ this->LD_r_r(this->DE.first(), this->DE.first());}},
			/*0x53*/	{"LD D,E", 4, [this](){ this->LD_r_r(this->DE.first(), this->DE.second());}},
			/*0x54*/	{"LD D,H", 4, [this](){ this->LD_r_r(this->DE.first(), this->HL.first());}},
			/*0x55*/	{"LD D,L", 4, [this](){ this->LD_r_r(this->DE.first(), this->HL.second());}},
			/*0x56*/	{"LD D,(HL)", 0, [this](){ this->ping(); }},
			/*0x57*/	{"???", 0, [this](){ this->ping(); }},

			/*0x58*/	{"LD E,B", 4, [this](){ this->LD_r_r(this->DE.second(), this->BC.first());}},
			/*0x59*/	{"LD E,C", 4, [this](){ this->LD_r_r(this->DE.second(), this->BC.second());}},
			/*0x5A*/	{"LD E,D", 4, [this](){ this->LD_r_r(this->DE.second(), this->DE.first());}},
			/*0x5B*/	{"LD E,E", 4, [this](){ this->LD_r_r(this->DE.second(), this->DE.second());}},
			/*0x5C*/	{"LD E,H", 4, [this](){ this->LD_r_r(this->DE.second(), this->HL.first());}},
			/*0x5D*/	{"LD E,L", 4, [this](){ this->LD_r_r(this->DE.second(), this->HL.second());}},
			/*0x5E*/	{"LD E,(HL)", 0, [this](){ this->ping(); }},
			/*0x5F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x60*/	{"LD H,B", 4, [this](){ this->LD_r_r(this->HL.first(), this->BC.first());}},
			/*0x61*/	{"LD H,C", 4, [this](){ this->LD_r_r(this->HL.first(), this->BC.second());}},
			/*0x62*/	{"LD H,D", 4, [this](){ this->LD_r_r(this->HL.first(), this->DE.first());}},
			/*0x63*/	{"LD H,E", 4, [this](){ this->LD_r_r(this->HL.first(), this->DE.second());}},
			/*0x64*/	{"LD H,H", 4, [this](){ this->LD_r_r(this->HL.first(), this->HL.first());}},
			/*0x65*/	{"LD H,L", 4, [this](){ this->LD_r_r(this->HL.first(), this->HL.second());}},
			/*0x66*/	{"LD H,(HL)", 0, [this](){ this->ping(); }},
			/*0x67*/	{"???", 0, [this](){ this->ping(); }},

			/*0x68*/	{"LD L,B", 4, [this](){ this->LD_r_r(this->HL.second(), this->BC.first());}},
			/*0x69*/	{"LD L,C", 4, [this](){ this->LD_r_r(this->HL.second(), this->BC.second());}},
			/*0x6A*/	{"LD L,D", 4, [this](){ this->LD_r_r(this->HL.second(), this->DE.first());}},
			/*0x6B*/	{"LD L,E", 4, [this](){ this->LD_r_r(this->HL.second(), this->DE.second());}},
			/*0x6C*/	{"LD L,H", 4, [this](){ this->LD_r_r(this->HL.second(), this->HL.first());}},
			/*0x6D*/	{"LD L,L", 4, [this](){ this->LD_r_r(this->HL.second(), this->HL.second());}},
			/*0x6E*/	{"LD L,(HL)", 0, [this](){ this->ping(); }},
			/*0x6F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x70*/	{"???", 0, [this](){ this->ping(); }},
			/*0x71*/	{"???", 0, [this](){ this->ping(); }},
			/*0x72*/	{"???", 0, [this](){ this->ping(); }},
			/*0x73*/	{"???", 0, [this](){ this->ping(); }},
			/*0x74*/	{"???", 0, [this](){ this->ping(); }},
			/*0x75*/	{"???", 0, [this](){ this->ping(); }},
			/*0x76*/	{"???", 0, [this](){ this->ping(); }},
			/*0x77*/	{"???", 0, [this](){ this->ping(); }},

			/*0x78*/	{"LD A, B", 4, [this](){ this->LD_r_r(this->AF.first(), this->BC.first());}},
			/*0x79*/	{"LD A, C", 4, [this](){ this->LD_r_r(this->AF.first(), this->BC.second());}},
			/*0x7A*/	{"LD A, D", 4, [this](){ this->LD_r_r(this->AF.first(), this->DE.first());}},
			/*0x7B*/	{"LD A, E", 4, [this](){ this->LD_r_r(this->AF.first(), this->DE.second());}},
			/*0x7C*/	{"LD A, H", 4, [this](){ this->LD_r_r(this->AF.first(), this->HL.first());}},
			/*0x7D*/	{"LD A, L", 4, [this](){ this->LD_r_r(this->AF.first(), this->HL.second());}},
			/*0x7E*/	{"LD A, (HL) ?", 0, [this](){ this->ping(); }},
			/*0x7F*/	{"LD A, A", 4, [this](){ this->LD_r_r(this->AF.first(), this->AF.first());}},

			/*0x80*/	{"ADD A,B", 4, [this](){ this->ADD_A_r(this->BC.first()); }},
			/*0x81*/	{"ADD A,C", 4, [this](){ this->ADD_A_r(this->BC.second()); }},
			/*0x82*/	{"ADD A,D", 4, [this](){ this->ADD_A_r(this->DE.first()); }},
			/*0x83*/	{"ADD A,E", 4, [this](){ this->ADD_A_r(this->DE.second()); }},
			/*0x84*/	{"ADD A,H", 4, [this](){ this->ADD_A_r(this->HL.first()); }},
			/*0x85*/	{"ADD A,L", 4, [this](){ this->ADD_A_r(this->HL.second()); }},
			/*0x86*/	{"???", 0, [this](){ this->ping(); }},
			/*0x87*/	{"ADD A,A", 4, [this](){ this->ADD_A_r(this->AF.first()); }},

			/*0x88*/	{"ADC A,B", 4, [this](){ this->ADC_A_r(this->BC.first()); }},
			/*0x89*/	{"ADC A,C", 4, [this](){ this->ADC_A_r(this->BC.second()); }},
			/*0x8A*/	{"ADC A,D", 4, [this](){ this->ADC_A_r(this->DE.first()); }},
			/*0x8B*/	{"ADC A,E", 4, [this](){ this->ADC_A_r(this->DE.second()); }},
			/*0x8C*/	{"ADC A,H", 4, [this](){ this->ADC_A_r(this->HL.first()); }},
			/*0x8D*/	{"ADC A,L", 4, [this](){ this->ADC_A_r(this->HL.second()); }},
			/*0x8E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8F*/	{"ADC A,A", 4, [this](){ this->ADC_A_r(this->AF.first()); }},

			/*0x80*/	{"SUB A,B", 4, [this](){ this->SUB_A_r(this->BC.first()); }},
			/*0x81*/	{"SUB A,C", 4, [this](){ this->SUB_A_r(this->BC.second()); }},
			/*0x82*/	{"SUB A,D", 4, [this](){ this->SUB_A_r(this->DE.first()); }},
			/*0x83*/	{"SUB A,E", 4, [this](){ this->SUB_A_r(this->DE.second()); }},
			/*0x84*/	{"SUB A,H", 4, [this](){ this->SUB_A_r(this->HL.first()); }},
			/*0x85*/	{"SUB A,L", 4, [this](){ this->SUB_A_r(this->HL.second()); }},
			/*0x86*/	{"???", 0, [this](){ this->ping(); }},
			/*0x87*/	{"SUB A,A", 4, [this](){ this->SUB_A_r(this->AF.first()); }},

			/*0x88*/	{"SBC A,B", 4, [this](){ this->SBC_A_r(this->BC.first()); }},
			/*0x89*/	{"SBC A,C", 4, [this](){ this->SBC_A_r(this->BC.second()); }},
			/*0x8A*/	{"SBC A,D", 4, [this](){ this->SBC_A_r(this->DE.first()); }},
			/*0x8B*/	{"SBC A,E", 4, [this](){ this->SBC_A_r(this->DE.second()); }},
			/*0x8C*/	{"SBC A,H", 4, [this](){ this->SBC_A_r(this->HL.first()); }},
			/*0x8D*/	{"SBC A,L", 4, [this](){ this->SBC_A_r(this->HL.second()); }},
			/*0x8E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8F*/	{"SBC A,A", 4, [this](){ this->SBC_A_r(this->AF.first()); }},

			/*0xA0*/	{"AND A,B", 4, [this](){ this->AND_A_r(this->BC.first()); }},
			/*0xA1*/	{"AND A,C", 4, [this](){ this->AND_A_r(this->BC.second()); }},
			/*0xA2*/	{"AND A,D", 4, [this](){ this->AND_A_r(this->DE.first()); }},
			/*0xA3*/	{"AND A,E", 4, [this](){ this->AND_A_r(this->DE.second()); }},
			/*0xA4*/	{"AND A,H", 4, [this](){ this->AND_A_r(this->HL.first()); }},
			/*0xA5*/	{"AND A,L", 4, [this](){ this->AND_A_r(this->HL.second()); }},
			/*0xA6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA7*/	{"AND A,A", 4, [this](){ this->AND_A_r(this->AF.first()); }},
			

			/*0xA8*/	{"XOR A,B", 4, [this](){ this->XOR_A_r(this->BC.first()); }},
			/*0xA9*/	{"XOR A,C", 4, [this](){ this->XOR_A_r(this->BC.second()); }},
			/*0xAA*/	{"XOR A,D", 4, [this](){ this->XOR_A_r(this->DE.first()); }},
			/*0xAB*/	{"XOR A,E", 4, [this](){ this->XOR_A_r(this->DE.second()); }},
			/*0xAC*/	{"XOR A,H", 4, [this](){ this->XOR_A_r(this->HL.first()); }},
			/*0xAD*/	{"XOR A,L", 4, [this](){ this->XOR_A_r(this->HL.second()); }},
			/*0xAE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAF*/	{"XOR A,A", 4, [this](){ this->XOR_A_r(this->AF.first()); }},

			/*0xB0*/	{"OR A,B", 4, [this](){ this->OR_A_r(this->BC.first()); }},
			/*0xB1*/	{"OR A,C", 4, [this](){ this->OR_A_r(this->BC.second()); }},
			/*0xB2*/	{"OR A,D", 4, [this](){ this->OR_A_r(this->DE.first()); }},
			/*0xB3*/	{"OR A,E", 4, [this](){ this->OR_A_r(this->DE.second()); }},
			/*0xB4*/	{"OR A,H", 4, [this](){ this->OR_A_r(this->HL.first()); }},
			/*0xB5*/	{"OR A,L", 4, [this](){ this->OR_A_r(this->HL.second()); }},
			/*0xB6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB7*/	{"OR A,A", 4, [this](){ this->OR_A_r(this->AF.first()); }},

			/*0xB8*/	{"CP A,B", 4, [this](){ this->CP_A_r(this->BC.first()); }},
			/*0xB9*/	{"CP A,C", 4, [this](){ this->CP_A_r(this->BC.second()); }},
			/*0xBA*/	{"CP A,D", 4, [this](){ this->CP_A_r(this->DE.first()); }},
			/*0xBB*/	{"CP A,E", 4, [this](){ this->CP_A_r(this->DE.second()); }},
			/*0xBC*/	{"CP A,H", 4, [this](){ this->CP_A_r(this->HL.first()); }},
			/*0xBD*/	{"CP A,L", 4, [this](){ this->CP_A_r(this->HL.second()); }},
			/*0xBE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBF*/	{"CP A,A", 4, [this](){ this->CP_A_r(this->AF.first()); }},

			/*0xC0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xD0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xE0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xED*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xF0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFF*/	{"???", 0, [this](){ this->ping(); }},
		};

		const Operation cbInstructionSet[0x100] = {
			/*0x00*/	{"???", 0, [this](){ this->ping(); }},
			/*0x01*/	{"???", 0, [this](){ this->ping(); }},
			/*0x02*/	{"???", 0, [this](){ this->ping(); }},
			/*0x03*/	{"???", 0, [this](){ this->ping(); }},
			/*0x04*/	{"???", 0, [this](){ this->ping(); }},
			/*0x05*/	{"???", 0, [this](){ this->ping(); }},
			/*0x06*/	{"???", 0, [this](){ this->ping(); }},
			/*0x07*/	{"???", 0, [this](){ this->ping(); }},
			/*0x08*/	{"???", 0, [this](){ this->ping(); }},
			/*0x09*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x0F*/	{"???", 0, [this](){ this->ping(); }},
			
			/*0x10*/	{"???", 0, [this](){ this->ping(); }},
			/*0x11*/	{"???", 0, [this](){ this->ping(); }},
			/*0x12*/	{"???", 0, [this](){ this->ping(); }},
			/*0x13*/	{"???", 0, [this](){ this->ping(); }},
			/*0x14*/	{"???", 0, [this](){ this->ping(); }},
			/*0x15*/	{"???", 0, [this](){ this->ping(); }},
			/*0x16*/	{"???", 0, [this](){ this->ping(); }},
			/*0x17*/	{"???", 0, [this](){ this->ping(); }},
			/*0x18*/	{"???", 0, [this](){ this->ping(); }},
			/*0x19*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x1F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x20*/	{"???", 0, [this](){ this->ping(); }},
			/*0x21*/	{"???", 0, [this](){ this->ping(); }},
			/*0x22*/	{"???", 0, [this](){ this->ping(); }},
			/*0x23*/	{"???", 0, [this](){ this->ping(); }},
			/*0x24*/	{"???", 0, [this](){ this->ping(); }},
			/*0x25*/	{"???", 0, [this](){ this->ping(); }},
			/*0x26*/	{"???", 0, [this](){ this->ping(); }},
			/*0x27*/	{"???", 0, [this](){ this->ping(); }},
			/*0x28*/	{"???", 0, [this](){ this->ping(); }},
			/*0x29*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x2F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x30*/	{"???", 0, [this](){ this->ping(); }},
			/*0x31*/	{"???", 0, [this](){ this->ping(); }},
			/*0x32*/	{"???", 0, [this](){ this->ping(); }},
			/*0x33*/	{"???", 0, [this](){ this->ping(); }},
			/*0x34*/	{"???", 0, [this](){ this->ping(); }},
			/*0x35*/	{"???", 0, [this](){ this->ping(); }},
			/*0x36*/	{"???", 0, [this](){ this->ping(); }},
			/*0x37*/	{"???", 0, [this](){ this->ping(); }},
			/*0x38*/	{"???", 0, [this](){ this->ping(); }},
			/*0x39*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x3F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x40*/	{"???", 0, [this](){ this->ping(); }},
			/*0x41*/	{"???", 0, [this](){ this->ping(); }},
			/*0x42*/	{"???", 0, [this](){ this->ping(); }},
			/*0x43*/	{"???", 0, [this](){ this->ping(); }},
			/*0x44*/	{"???", 0, [this](){ this->ping(); }},
			/*0x45*/	{"???", 0, [this](){ this->ping(); }},
			/*0x46*/	{"???", 0, [this](){ this->ping(); }},
			/*0x47*/	{"???", 0, [this](){ this->ping(); }},
			/*0x48*/	{"???", 0, [this](){ this->ping(); }},
			/*0x49*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x4F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x50*/	{"???", 0, [this](){ this->ping(); }},
			/*0x51*/	{"???", 0, [this](){ this->ping(); }},
			/*0x52*/	{"???", 0, [this](){ this->ping(); }},
			/*0x53*/	{"???", 0, [this](){ this->ping(); }},
			/*0x54*/	{"???", 0, [this](){ this->ping(); }},
			/*0x55*/	{"???", 0, [this](){ this->ping(); }},
			/*0x56*/	{"???", 0, [this](){ this->ping(); }},
			/*0x57*/	{"???", 0, [this](){ this->ping(); }},
			/*0x58*/	{"???", 0, [this](){ this->ping(); }},
			/*0x59*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x5F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x60*/	{"???", 0, [this](){ this->ping(); }},
			/*0x61*/	{"???", 0, [this](){ this->ping(); }},
			/*0x62*/	{"???", 0, [this](){ this->ping(); }},
			/*0x63*/	{"???", 0, [this](){ this->ping(); }},
			/*0x64*/	{"???", 0, [this](){ this->ping(); }},
			/*0x65*/	{"???", 0, [this](){ this->ping(); }},
			/*0x66*/	{"???", 0, [this](){ this->ping(); }},
			/*0x67*/	{"???", 0, [this](){ this->ping(); }},
			/*0x68*/	{"???", 0, [this](){ this->ping(); }},
			/*0x69*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x6F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x70*/	{"???", 0, [this](){ this->ping(); }},
			/*0x71*/	{"???", 0, [this](){ this->ping(); }},
			/*0x72*/	{"???", 0, [this](){ this->ping(); }},
			/*0x73*/	{"???", 0, [this](){ this->ping(); }},
			/*0x74*/	{"???", 0, [this](){ this->ping(); }},
			/*0x75*/	{"???", 0, [this](){ this->ping(); }},
			/*0x76*/	{"???", 0, [this](){ this->ping(); }},
			/*0x77*/	{"???", 0, [this](){ this->ping(); }},
			/*0x78*/	{"???", 0, [this](){ this->ping(); }},
			/*0x79*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x7F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x80*/	{"???", 0, [this](){ this->ping(); }},
			/*0x81*/	{"???", 0, [this](){ this->ping(); }},
			/*0x82*/	{"???", 0, [this](){ this->ping(); }},
			/*0x83*/	{"???", 0, [this](){ this->ping(); }},
			/*0x84*/	{"???", 0, [this](){ this->ping(); }},
			/*0x85*/	{"???", 0, [this](){ this->ping(); }},
			/*0x86*/	{"???", 0, [this](){ this->ping(); }},
			/*0x87*/	{"???", 0, [this](){ this->ping(); }},
			/*0x88*/	{"???", 0, [this](){ this->ping(); }},
			/*0x89*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x8F*/	{"???", 0, [this](){ this->ping(); }},

			/*0x90*/	{"???", 0, [this](){ this->ping(); }},
			/*0x91*/	{"???", 0, [this](){ this->ping(); }},
			/*0x92*/	{"???", 0, [this](){ this->ping(); }},
			/*0x93*/	{"???", 0, [this](){ this->ping(); }},
			/*0x94*/	{"???", 0, [this](){ this->ping(); }},
			/*0x95*/	{"???", 0, [this](){ this->ping(); }},
			/*0x96*/	{"???", 0, [this](){ this->ping(); }},
			/*0x97*/	{"???", 0, [this](){ this->ping(); }},
			/*0x98*/	{"???", 0, [this](){ this->ping(); }},
			/*0x99*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9A*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9B*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9C*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9D*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9E*/	{"???", 0, [this](){ this->ping(); }},
			/*0x9F*/	{"???", 0, [this](){ this->ping(); }},

			/*0xA0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xA9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xAF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xB0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xB9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xBF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xC0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xC9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xCF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xD0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xD9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xDF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xE0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xE9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xED*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xEF*/	{"???", 0, [this](){ this->ping(); }},

			/*0xF0*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF1*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF2*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF3*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF4*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF5*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF6*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF7*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF8*/	{"???", 0, [this](){ this->ping(); }},
			/*0xF9*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFA*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFB*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFC*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFD*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFE*/	{"???", 0, [this](){ this->ping(); }},
			/*0xFF*/	{"???", 0, [this](){ this->ping(); }},
		};
};