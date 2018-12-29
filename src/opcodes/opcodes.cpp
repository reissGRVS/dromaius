#include "cpu.h"
const Ticks ADDRESS_FETCH = 4;
const Operation CPU::instructionSet[0x100] = {
			/*0x00*/	{"NOP", [](CPU*){ return 4;}},
			/*0x01*/	{"LD BC,nn", [](CPU* cpu){ return cpu->LD_r_nn(cpu->BC.word()); }},
			/*0x02*/	{"LD (BC),A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->BC.word()), cpu->AF.first(), true); }},
			/*0x03*/	{"INC BC", [](CPU* cpu){ return cpu->INC_rr(cpu->BC.word()); }},
			/*0x04*/	{"INC B", [](CPU* cpu){ return cpu->INC_r(cpu->BC.first()); }},
			/*0x05*/	{"DEC B", [](CPU* cpu){ return cpu->DEC_r(cpu->BC.first()); }},
			/*0x06*/	{"LD B,n", [](CPU* cpu){ return cpu->LD_r_n(cpu->BC.first()); }},
			/*0x07*/	{"RLCA", [](CPU* cpu){ Ticks ticks = cpu->RLC(cpu->AF.first()); cpu->setZFlag(false); return ticks;}},

			/*0x08*/	{"LD (a16),SP", [](CPU* cpu){
							//Store low
							cpu->LD_r_r(cpu->memoryMap.byte(cpu->getNextByte()), cpu->SP.second());
							//Store high
							cpu->LD_r_r(cpu->memoryMap.byte(cpu->getNextByte()), cpu->SP.first()); return 20;}},
			/*0x09*/	{"ADD HL,BC", [](CPU* cpu){ return cpu->ADD_HL_rr(cpu->BC.word()); }},
			/*0x0A*/	{"LD A, (BC)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->BC.word())); }},
			/*0x0B*/	{"DEC BC", [](CPU* cpu){ return cpu->DEC_rr(cpu->BC.word()); }},
			/*0x0C*/	{"INC C", [](CPU* cpu){ return cpu->INC_r(cpu->BC.second()); }},
			/*0x0D*/	{"DEC C", [](CPU* cpu){ return cpu->DEC_r(cpu->BC.second()); }},
			/*0x0E*/	{"LD C, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->BC.second()); }},
			/*0x0F*/	{"RRCA", [](CPU* cpu){ Ticks ticks = cpu->RRC(cpu->AF.first()); cpu->setZFlag(false); return ticks;}},
			
			/*0x10*/	{"STOP 0", [](CPU*){ return 4; /*TODO: Stop processor & screen until button press*/ }},
			/*0x11*/	{"LD DE,nn", [](CPU* cpu){ return cpu->LD_r_nn(cpu->DE.word()); }},
			/*0x12*/	{"LD (DE),A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->DE.word()), cpu->AF.first(), true); }},
			/*0x13*/	{"INC DE", [](CPU* cpu){ return cpu->INC_rr(cpu->DE.word()); }},
			/*0x14*/	{"INC D", [](CPU* cpu){ return cpu->INC_r(cpu->DE.first()); }},
			/*0x15*/	{"DEC D", [](CPU* cpu){ return cpu->DEC_r(cpu->DE.first()); }},
			/*0x16*/	{"LD D, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->DE.first()); }},
			/*0x17*/	{"RLA", [](CPU* cpu){ Ticks ticks = cpu->RL(cpu->AF.first()); cpu->setZFlag(false); return ticks;}},

			/*0x18*/	{"JR n", [](CPU* cpu){ return cpu->JR_n(); }},
			/*0x19*/	{"ADD HL,DE", [](CPU* cpu){ return cpu->ADD_HL_rr(cpu->DE.word()); }},
			/*0x1A*/	{"LD A,(DE)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->DE.word()), true); }},
			/*0x1B*/	{"DEC DE", [](CPU* cpu){ return cpu->DEC_rr(cpu->DE.word()); }},
			/*0x1C*/	{"INC E", [](CPU* cpu){ return cpu->INC_r(cpu->DE.second()); }},
			/*0x1D*/	{"DEC E", [](CPU* cpu){ return cpu->DEC_r(cpu->DE.second()); }},
			/*0x1E*/	{"LD E, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->DE.second()); }},
			/*0x1F*/	{"RRA", [](CPU* cpu){ Ticks ticks = cpu->RR(cpu->AF.first()); cpu->setZFlag(false); return ticks;}},

			/*0x20*/	{"JR NZ,n", [](CPU* cpu){ return cpu->JR_cc_n(!cpu->getZFlag()); }},
			/*0x21*/	{"LD HL,nn", [](CPU* cpu){ return cpu->LD_r_nn(cpu->HL.word()); }},
			/*0x22*/	{"LD (HL+), A", [](CPU* cpu){ Ticks ticks = cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->AF.first(), true); cpu->INC_rr(cpu->HL.word()); return ticks;}},
			/*0x23*/	{"INC HL", [](CPU* cpu){ return cpu->INC_rr(cpu->HL.word()); }},
			/*0x24*/	{"INC H", [](CPU* cpu){ return cpu->INC_r(cpu->HL.first()); }},
			/*0x25*/	{"DEC H", [](CPU* cpu){ return cpu->DEC_r(cpu->HL.first()); }},
			/*0x26*/	{"LD H, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->HL.first()); }},
			/*0x27*/	{"DAA", [](CPU* cpu){ return cpu->DAA(); }}, 

			/*0x28*/	{"JR Z,n", [](CPU* cpu){ return cpu->JR_cc_n(cpu->getZFlag()); }},
			/*0x29*/	{"ADD HL,HL", [](CPU* cpu){ return cpu->ADD_HL_rr(cpu->HL.word()); }},
			/*0x2A*/	{"LD A,(HL+)", [](CPU* cpu){ Ticks ticks = cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->HL.word()), true); cpu->INC_rr(cpu->HL.word()); return ticks;}},
			/*0x2B*/	{"DEC HL", [](CPU* cpu){ return cpu->DEC_rr(cpu->HL.word()); }},
			/*0x2C*/	{"INC L", [](CPU* cpu){ return cpu->INC_r(cpu->HL.second()); }},
			/*0x2D*/	{"DEC L", [](CPU* cpu){ return cpu->DEC_r(cpu->HL.second()); }},
			/*0x2E*/	{"LD L, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->HL.second()); }},
			/*0x2F*/	{"CPL", [](CPU* cpu){ return cpu->CPL(); }},

			/*0x38*/	{"JR NC,n", [](CPU* cpu){ return cpu->JR_cc_n(!cpu->getCFlag()); }},
			/*0x31*/	{"LD SP,nn", [](CPU* cpu){ return cpu->LD_r_nn(cpu->SP.word()); }},
			/*0x32*/	{"LD (HL-), A", [](CPU* cpu){ Ticks ticks = cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->AF.first(), true); cpu->DEC_rr(cpu->HL.word()); return ticks;}},
			/*0x33*/	{"INC SP", [](CPU* cpu){ return cpu->INC_rr(cpu->SP.word()); }},
			/*0x34*/	{"INC (HL)", [](CPU* cpu){ return 8 + cpu->INC_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x35*/	{"DEC (HL)", [](CPU* cpu){ return 8 + cpu->DEC_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x36*/	{"LD (HL), n", [](CPU* cpu){ return ADDRESS_FETCH + cpu->LD_r_n(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x37*/	{"SCF", [](CPU* cpu){ return cpu->SCF(); }},

			/*0x38*/	{"JR C,n", [](CPU* cpu){ return cpu->JR_cc_n(cpu->getCFlag()); }},
			/*0x39*/	{"ADD HL,SP", [](CPU* cpu){ return cpu->ADD_HL_rr(cpu->SP.word()); }},
			/*0x3A*/	{"LD A,(HL-)", [](CPU* cpu){ Ticks ticks = cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->HL.word()), true); cpu->DEC_rr(cpu->HL.word()); return ticks;}},
			/*0x3B*/	{"DEC SP", [](CPU* cpu){ return cpu->DEC_rr(cpu->SP.word()); }},
			/*0x3C*/	{"INC A", [](CPU* cpu){ return cpu->INC_r(cpu->AF.first()); }},
			/*0x3D*/	{"DEC A", [](CPU* cpu){ return cpu->DEC_r(cpu->AF.first()); }},
			/*0x3E*/	{"LD A, n", [](CPU* cpu){ return cpu->LD_r_n(cpu->AF.first()); }},
			/*0x3F*/	{"CCF", [](CPU* cpu){ return cpu->CCF(); }},
// COMPLETED
			/*0x40*/	{"LD B,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->BC.first());}},
			/*0x41*/	{"LD B,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->BC.second());}},
			/*0x42*/	{"LD B,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->DE.first());}},
			/*0x43*/	{"LD B,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->DE.second());}},
			/*0x44*/	{"LD B,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->HL.first());}},
			/*0x45*/	{"LD B,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->HL.second());}},
			/*0x46*/	{"LD B,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x47*/	{"LD B,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.first(), cpu->AF.first());}},

			/*0x48*/	{"LD C,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->BC.first());}},
			/*0x49*/	{"LD C,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->BC.second());}},
			/*0x4A*/	{"LD C,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->DE.first());}},
			/*0x4B*/	{"LD C,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->DE.second());}},
			/*0x4C*/	{"LD C,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->HL.first());}},
			/*0x4D*/	{"LD C,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->HL.second());}},
			/*0x4E*/	{"LD C,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x4F*/	{"LD C,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->BC.second(), cpu->AF.first());}},

			/*0x50*/	{"LD D,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->BC.first());}},
			/*0x51*/	{"LD D,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->BC.second());}},
			/*0x52*/	{"LD D,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->DE.first());}},
			/*0x53*/	{"LD D,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->DE.second());}},
			/*0x54*/	{"LD D,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->HL.first());}},
			/*0x55*/	{"LD D,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->HL.second());}},
			/*0x56*/	{"LD D,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x57*/	{"LD D,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.first(), cpu->AF.first());}},

			/*0x58*/	{"LD E,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->BC.first());}},
			/*0x59*/	{"LD E,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->BC.second());}},
			/*0x5A*/	{"LD E,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->DE.first());}},
			/*0x5B*/	{"LD E,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->DE.second());}},
			/*0x5C*/	{"LD E,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->HL.first());}},
			/*0x5D*/	{"LD E,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->HL.second());}},
			/*0x5E*/	{"LD E,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x5F*/	{"LD E,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->DE.second(), cpu->AF.first());}},

			/*0x60*/	{"LD H,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->BC.first());}},
			/*0x61*/	{"LD H,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->BC.second());}},
			/*0x62*/	{"LD H,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->DE.first());}},
			/*0x63*/	{"LD H,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->DE.second());}},
			/*0x64*/	{"LD H,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->HL.first());}},
			/*0x65*/	{"LD H,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->HL.second());}},
			/*0x66*/	{"LD H,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x67*/	{"LD H,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.first(), cpu->AF.first());}},

			/*0x68*/	{"LD L,B", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->BC.first());}},
			/*0x69*/	{"LD L,C", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->BC.second());}},
			/*0x6A*/	{"LD L,D", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->DE.first());}},
			/*0x6B*/	{"LD L,E", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->DE.second());}},
			/*0x6C*/	{"LD L,H", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->HL.first());}},
			/*0x6D*/	{"LD L,L", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->HL.second());}},
			/*0x6E*/	{"LD L,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x6F*/	{"LD L,A", [](CPU* cpu){ return cpu->LD_r_r(cpu->HL.second(), cpu->AF.first());}},

			/*0x70*/	{"LD (HL), B", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->BC.first(), true);}},
			/*0x71*/	{"LD (HL), C", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->BC.second(), true);}},
			/*0x72*/	{"LD (HL), D", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->DE.first(), true);}},
			/*0x73*/	{"LD (HL), E", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->DE.second(), true);}},
			/*0x74*/	{"LD (HL), H", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->HL.first(), true);}},
			/*0x75*/	{"LD (HL), L", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->HL.second(), true);}},
			/*0x76*/	{"HALT", [](CPU* cpu){ cpu->setHalt(true); return 4;}},
			/*0x77*/	{"LD (HL), A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->HL.word()), cpu->AF.first(), true);}},

			/*0x78*/	{"LD A, B", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->BC.first());}},
			/*0x79*/	{"LD A, C", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->BC.second());}},
			/*0x7A*/	{"LD A, D", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->DE.first());}},
			/*0x7B*/	{"LD A, E", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->DE.second());}},
			/*0x7C*/	{"LD A, H", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->HL.first());}},
			/*0x7D*/	{"LD A, L", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->HL.second());}},
			/*0x7E*/	{"LD A,(HL)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->HL.word()), true); }},
			/*0x7F*/	{"LD A, A", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->AF.first());}},

			/*0x80*/	{"ADD A,B", [](CPU* cpu){ return cpu->ADD_A_r(cpu->BC.first()); }},
			/*0x81*/	{"ADD A,C", [](CPU* cpu){ return cpu->ADD_A_r(cpu->BC.second()); }},
			/*0x82*/	{"ADD A,D", [](CPU* cpu){ return cpu->ADD_A_r(cpu->DE.first()); }},
			/*0x83*/	{"ADD A,E", [](CPU* cpu){ return cpu->ADD_A_r(cpu->DE.second()); }},
			/*0x84*/	{"ADD A,H", [](CPU* cpu){ return cpu->ADD_A_r(cpu->HL.first()); }},
			/*0x85*/	{"ADD A,L", [](CPU* cpu){ return cpu->ADD_A_r(cpu->HL.second()); }},
			/*0x86*/	{"ADD A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->ADD_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x87*/	{"ADD A,A", [](CPU* cpu){ return cpu->ADD_A_r(cpu->AF.first()); }},

			/*0x88*/	{"ADC A,B", [](CPU* cpu){ return cpu->ADC_A_r(cpu->BC.first()); }},
			/*0x89*/	{"ADC A,C", [](CPU* cpu){ return cpu->ADC_A_r(cpu->BC.second()); }},
			/*0x8A*/	{"ADC A,D", [](CPU* cpu){ return cpu->ADC_A_r(cpu->DE.first()); }},
			/*0x8B*/	{"ADC A,E", [](CPU* cpu){ return cpu->ADC_A_r(cpu->DE.second()); }},
			/*0x8C*/	{"ADC A,H", [](CPU* cpu){ return cpu->ADC_A_r(cpu->HL.first()); }},
			/*0x8D*/	{"ADC A,L", [](CPU* cpu){ return cpu->ADC_A_r(cpu->HL.second()); }},
			/*0x8E*/	{"ADC A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->ADC_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x8F*/	{"ADC A,A", [](CPU* cpu){ return cpu->ADC_A_r(cpu->AF.first()); }},

			/*0x90*/	{"SUB A,B", [](CPU* cpu){ return cpu->SUB_A_r(cpu->BC.first()); }},
			/*0x91*/	{"SUB A,C", [](CPU* cpu){ return cpu->SUB_A_r(cpu->BC.second()); }},
			/*0x92*/	{"SUB A,D", [](CPU* cpu){ return cpu->SUB_A_r(cpu->DE.first()); }},
			/*0x93*/	{"SUB A,E", [](CPU* cpu){ return cpu->SUB_A_r(cpu->DE.second()); }},
			/*0x94*/	{"SUB A,H", [](CPU* cpu){ return cpu->SUB_A_r(cpu->HL.first()); }},
			/*0x95*/	{"SUB A,L", [](CPU* cpu){ return cpu->SUB_A_r(cpu->HL.second()); }},
			/*0x96*/	{"SUB A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->SUB_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x97*/	{"SUB A,A", [](CPU* cpu){ return cpu->SUB_A_r(cpu->AF.first()); }},

			/*0x98*/	{"SBC A,B", [](CPU* cpu){ return cpu->SBC_A_r(cpu->BC.first()); }},
			/*0x99*/	{"SBC A,C", [](CPU* cpu){ return cpu->SBC_A_r(cpu->BC.second()); }},
			/*0x9A*/	{"SBC A,D", [](CPU* cpu){ return cpu->SBC_A_r(cpu->DE.first()); }},
			/*0x9B*/	{"SBC A,E", [](CPU* cpu){ return cpu->SBC_A_r(cpu->DE.second()); }},
			/*0x9C*/	{"SBC A,H", [](CPU* cpu){ return cpu->SBC_A_r(cpu->HL.first()); }},
			/*0x9D*/	{"SBC A,L", [](CPU* cpu){ return cpu->SBC_A_r(cpu->HL.second()); }},
			/*0x9E*/	{"SBC A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->SBC_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x9F*/	{"SBC A,A", [](CPU* cpu){ return cpu->SBC_A_r(cpu->AF.first()); }},

			/*0xA0*/	{"AND A,B", [](CPU* cpu){ return cpu->AND_A_r(cpu->BC.first()); }},
			/*0xA1*/	{"AND A,C", [](CPU* cpu){ return cpu->AND_A_r(cpu->BC.second()); }},
			/*0xA2*/	{"AND A,D", [](CPU* cpu){ return cpu->AND_A_r(cpu->DE.first()); }},
			/*0xA3*/	{"AND A,E", [](CPU* cpu){ return cpu->AND_A_r(cpu->DE.second()); }},
			/*0xA4*/	{"AND A,H", [](CPU* cpu){ return cpu->AND_A_r(cpu->HL.first()); }},
			/*0xA5*/	{"AND A,L", [](CPU* cpu){ return cpu->AND_A_r(cpu->HL.second()); }},
			/*0xA6*/	{"AND A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->AND_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xA7*/	{"AND A,A", [](CPU* cpu){ return cpu->AND_A_r(cpu->AF.first()); }},
			

			/*0xA8*/	{"XOR A,B", [](CPU* cpu){ return cpu->XOR_A_r(cpu->BC.first()); }},
			/*0xA9*/	{"XOR A,C", [](CPU* cpu){ return cpu->XOR_A_r(cpu->BC.second()); }},
			/*0xAA*/	{"XOR A,D", [](CPU* cpu){ return cpu->XOR_A_r(cpu->DE.first()); }},
			/*0xAB*/	{"XOR A,E", [](CPU* cpu){ return cpu->XOR_A_r(cpu->DE.second()); }},
			/*0xAC*/	{"XOR A,H", [](CPU* cpu){ return cpu->XOR_A_r(cpu->HL.first()); }},
			/*0xAD*/	{"XOR A,L", [](CPU* cpu){ return cpu->XOR_A_r(cpu->HL.second()); }},
			/*0xAE*/	{"XOR A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->XOR_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xAF*/	{"XOR A,A", [](CPU* cpu){ return cpu->XOR_A_r(cpu->AF.first()); }},

			/*0xB0*/	{"OR A,B", [](CPU* cpu){ return cpu->OR_A_r(cpu->BC.first()); }},
			/*0xB1*/	{"OR A,C", [](CPU* cpu){ return cpu->OR_A_r(cpu->BC.second()); }},
			/*0xB2*/	{"OR A,D", [](CPU* cpu){ return cpu->OR_A_r(cpu->DE.first()); }},
			/*0xB3*/	{"OR A,E", [](CPU* cpu){ return cpu->OR_A_r(cpu->DE.second()); }},
			/*0xB4*/	{"OR A,H", [](CPU* cpu){ return cpu->OR_A_r(cpu->HL.first()); }},
			/*0xB5*/	{"OR A,L", [](CPU* cpu){ return cpu->OR_A_r(cpu->HL.second()); }},
			/*0xB6*/	{"OR A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->OR_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xB7*/	{"OR A,A", [](CPU* cpu){ return cpu->OR_A_r(cpu->AF.first()); }},

			/*0xB8*/	{"CP A,B", [](CPU* cpu){ return cpu->CP_A_r(cpu->BC.first()); }},
			/*0xB9*/	{"CP A,C", [](CPU* cpu){ return cpu->CP_A_r(cpu->BC.second()); }},
			/*0xBA*/	{"CP A,D", [](CPU* cpu){ return cpu->CP_A_r(cpu->DE.first()); }},
			/*0xBB*/	{"CP A,E", [](CPU* cpu){ return cpu->CP_A_r(cpu->DE.second()); }},
			/*0xBC*/	{"CP A,H", [](CPU* cpu){ return cpu->CP_A_r(cpu->HL.first()); }},
			/*0xBD*/	{"CP A,L", [](CPU* cpu){ return cpu->CP_A_r(cpu->HL.second()); }},
			/*0xBE*/	{"CP A,(HL)", [](CPU* cpu){ return ADDRESS_FETCH + cpu->CP_A_r(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xBF*/	{"CP A,A", [](CPU* cpu){ return cpu->CP_A_r(cpu->AF.first()); }},
//
			/*0xC0*/	{"RET NZ", [](CPU* cpu){ return cpu->RET_cc(!cpu->getZFlag()); }},
			/*0xC1*/	{"POP BC", [](CPU* cpu){ return cpu->POP_rr(cpu->BC.word()); }},
			/*0xC2*/	{"JP NZ,a16", [](CPU* cpu){ return cpu->JP_cc_nn(!cpu->getZFlag()); }},
			/*0xC3*/	{"JP a16", [](CPU* cpu){ return cpu->JP_nn(); }},
			/*0xC4*/	{"CALL NZ,a16", [](CPU* cpu){ return cpu->CALL_cc_nn(!cpu->getZFlag()); }},
			/*0xC5*/	{"PUSH BC", [](CPU* cpu){ return cpu->PUSH_rr(cpu->BC); }},
			/*0xC6*/	{"ADD A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->ADD_A_r(cpu->getNextByte()); }},
			/*0xC7*/	{"RST 00H", [](CPU* cpu){ return cpu->RST(0x00); }},
			/*0xC8*/	{"RET Z", [](CPU* cpu){ return cpu->RET_cc(cpu->getZFlag()); }},
			/*0xC9*/	{"RET", [](CPU* cpu){ return cpu->RET(); }},
			/*0xCA*/	{"JP Z,a16", [](CPU* cpu){ return cpu->JP_cc_nn(cpu->getZFlag()); }},
			/*0xCB*/	{"CB", [](CPU*){ return 4; }},
			/*0xCC*/	{"CALL Z,a16", [](CPU* cpu){ return cpu->CALL_cc_nn(cpu->getZFlag()); }},
			/*0xCD*/	{"CALL a16", [](CPU* cpu){ return cpu->CALL_nn(); }},
			/*0xCE*/	{"ADC A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->ADC_A_r(cpu->getNextByte()); }},
			/*0xCF*/	{"RST 08H", [](CPU* cpu){ return cpu->RST(0x08); }},

			/*0xD0*/	{"RET NC", [](CPU* cpu){ return cpu->RET_cc(!cpu->getCFlag()); }},
			/*0xD1*/	{"POP DE", [](CPU* cpu){ return cpu->POP_rr(cpu->DE.word()); }},
			/*0xD2*/	{"JP NC,a16", [](CPU* cpu){ return cpu->JP_cc_nn(!cpu->getCFlag()); }},
			/*0xD3*/	{"NULL", [](CPU*){ return 0; }},
			/*0xD4*/	{"CALL NC,a16", [](CPU* cpu){ return cpu->CALL_cc_nn(!cpu->getCFlag()); }},
			/*0xD5*/	{"PUSH DE", [](CPU* cpu){ return cpu->PUSH_rr(cpu->DE); }},
			/*0xD6*/	{"SUB A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->SUB_A_r(cpu->getNextByte()); }},
			/*0xD7*/	{"RST 10H", [](CPU* cpu){ return cpu->RST(0x10); }},
			/*0xD8*/	{"RET C", [](CPU* cpu){ return cpu->RET_cc(cpu->getCFlag()); }},
			/*0xD9*/	{"RETI", [](CPU* cpu){ return cpu->RETI(); }},
			/*0xDA*/	{"JP C,a16", [](CPU* cpu){ return cpu->JP_cc_nn(cpu->getCFlag()); }},
			/*0xDB*/	{"NULL", [](CPU*){ return 0; }},
			/*0xD4*/	{"CALL C,a16", [](CPU* cpu){ return cpu->CALL_cc_nn(cpu->getCFlag()); }},
			/*0xDD*/	{"NULL", [](CPU*){ return 0; }},
			/*0xDE*/	{"SBC A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->SBC_A_r(cpu->getNextByte());}},
			/*0xDF*/	{"RST 18H", [](CPU* cpu){ return cpu->RST(0x18); }},

			/*0xE0*/	{"LDH (a8),A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->getNextByte()+0xFF00), cpu->AF.first()); }},
			/*0xE1*/	{"POP HL", [](CPU* cpu){ return cpu->POP_rr(cpu->HL.word()); }},
			/*0xE2*/	{"LD (C),A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->BC.second()+0xFF00), cpu->AF.first()); }},
			/*0xE3*/	{"NULL", [](CPU*){ return 0; }},
			/*0xE4*/	{"NULL", [](CPU*){ return 0; }},
			/*0xE5*/	{"PUSH HL", [](CPU* cpu){ return cpu->PUSH_rr(cpu->HL); }},
			/*0xE6*/	{"AND A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->AND_A_r(cpu->getNextByte()); }},
			/*0xE7*/	{"RST 20H", [](CPU* cpu){ return cpu->RST(0x20); }},
			/*0xE8*/	{"ADD SP,r8", [](CPU* cpu){ return cpu->ADD_SP_s(); }},
			/*0xE9*/	{"JP HL", [](CPU* cpu){ return cpu->JP_rr(cpu->HL.word()); }},
			/*0xEA*/	{"LD (a16),A", [](CPU* cpu){ return cpu->LD_r_r(cpu->memoryMap.byte(cpu->getNextWord()), cpu->AF.first()); }},
			/*0xEB*/	{"NULL", [](CPU*){ return 0; }},
			/*0xEC*/	{"NULL", [](CPU*){ return 0; }},
			/*0xED*/	{"NULL", [](CPU*){ return 0; }},
			/*0xEE*/	{"XOR A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->XOR_A_r(cpu->getNextByte()); }},
			/*0xEF*/	{"RST 28H", [](CPU* cpu){ return cpu->RST(0x28); }},

			/*0xF0*/	{"LDH A,(a8)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->getNextByte()+0xFF00)); }},
			/*0xF1*/	{"POP AF", [](CPU* cpu){ Ticks ticks = cpu->POP_rr(cpu->AF.word()); cpu->AF.second() &= 0xF0;return ticks;}},
			/*0xF2*/	{"LD A,(C)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->BC.second()+0xFF00)); }},
			/*0xF3*/	{"DI", [](CPU* cpu){ return cpu->DI(); }},
			/*0xF4*/	{"NULL", [](CPU*){ return 0; }},
			/*0xF5*/	{"PUSH AF", [](CPU* cpu){ return cpu->PUSH_rr(cpu->AF); }},
			/*0xF6*/	{"OR A,d8", [](CPU* cpu){ return ADDRESS_FETCH + cpu->OR_A_r(cpu->getNextByte()); }},
			/*0xF7*/	{"RST 30H", [](CPU* cpu){ return cpu->RST(0x30); }},
			/*0xF8*/	{"LD HL,SP+r8", [](CPU* cpu){ cpu->LD_rr_rr(cpu->HL.word(), cpu->ADD_SP_s_result()); return 12; }},
			/*0xF9*/	{"LD SP,HL", [](CPU* cpu){ cpu->LD_rr_rr(cpu->SP.word(), cpu->HL.word()); return 8; }},
			/*0xFA*/	{"LD A,(a16)", [](CPU* cpu){ return cpu->LD_r_r(cpu->AF.first(), cpu->memoryMap.byte(cpu->getNextWord())); }},
			/*0xFB*/	{"EI", [](CPU* cpu){ return cpu->EI(); }},
			/*0xFC*/	{"NULL", [](CPU*){ return 0; }},
			/*0xFD*/	{"NULL", [](CPU*){ return 0; }},
			/*0xFE*/	{"CP A,d8", [](CPU* cpu){return ADDRESS_FETCH + cpu->CP_A_r(cpu->getNextByte()); }},
			/*0xFF*/	{"RST 38H", [](CPU* cpu){ return cpu->RST(0x38); }},
		};