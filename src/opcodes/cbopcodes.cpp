#include "cpu.h"
const Ticks HL_RESOLVE = 8;

const Operation CPU::cbInstructionSet[0x100] = {
			/*0x00*/	{"RLC B", [](CPU* cpu){ return cpu->RLC(cpu->BC.first()); }},
			/*0x01*/	{"RLC C", [](CPU* cpu){ return cpu->RLC(cpu->BC.second()); }},
			/*0x02*/	{"RLC D", [](CPU* cpu){ return cpu->RLC(cpu->DE.first()); }},
			/*0x03*/	{"RLC E", [](CPU* cpu){ return cpu->RLC(cpu->DE.second()); }},
			/*0x04*/	{"RLC H", [](CPU* cpu){ return cpu->RLC(cpu->HL.first()); }},
			/*0x05*/	{"RLC L", [](CPU* cpu){ return cpu->RLC(cpu->HL.second()); }},
			/*0x06*/	{"RLC (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RLC(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x07*/	{"RRC A", [](CPU* cpu){ return cpu->RLC(cpu->AF.first()); }},
			/*0x08*/	{"RRC B", [](CPU* cpu){ return cpu->RRC(cpu->BC.first()); }},
			/*0x09*/	{"RRC C", [](CPU* cpu){ return cpu->RRC(cpu->BC.second()); }},
			/*0x0A*/	{"RRC D", [](CPU* cpu){ return cpu->RRC(cpu->DE.first()); }},
			/*0x0B*/	{"RRC E", [](CPU* cpu){ return cpu->RRC(cpu->DE.second()); }},
			/*0x0C*/	{"RRC H", [](CPU* cpu){ return cpu->RRC(cpu->HL.first()); }},
			/*0x0D*/	{"RRC L", [](CPU* cpu){ return cpu->RRC(cpu->HL.second()); }},
			/*0x0E*/	{"RRC (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RRC(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x0F*/	{"RRC A", [](CPU* cpu){ return cpu->RRC(cpu->AF.first()); }},
		
			/*0x10*/	{"RL B", [](CPU* cpu){ return cpu->RL(cpu->BC.first()); }},
			/*0x11*/	{"RL C", [](CPU* cpu){ return cpu->RL(cpu->BC.second()); }},
			/*0x12*/	{"RL D", [](CPU* cpu){ return cpu->RL(cpu->DE.first()); }},
			/*0x13*/	{"RL E", [](CPU* cpu){ return cpu->RL(cpu->DE.second()); }},
			/*0x14*/	{"RL H", [](CPU* cpu){ return cpu->RL(cpu->HL.first()); }},
			/*0x15*/	{"RL L", [](CPU* cpu){ return cpu->RL(cpu->HL.second()); }},
			/*0x16*/	{"RL (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RL(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x17*/	{"RR A", [](CPU* cpu){ return cpu->RL(cpu->AF.first()); }},
			/*0x18*/	{"RR B", [](CPU* cpu){ return cpu->RR(cpu->BC.first()); }},
			/*0x19*/	{"RR C", [](CPU* cpu){ return cpu->RR(cpu->BC.second()); }},
			/*0x1A*/	{"RR D", [](CPU* cpu){ return cpu->RR(cpu->DE.first()); }},
			/*0x1B*/	{"RR E", [](CPU* cpu){ return cpu->RR(cpu->DE.second()); }},
			/*0x1C*/	{"RR H", [](CPU* cpu){ return cpu->RR(cpu->HL.first()); }},
			/*0x1D*/	{"RR L", [](CPU* cpu){ return cpu->RR(cpu->HL.second()); }},
			/*0x1E*/	{"RR (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RR(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x1F*/	{"RR A", [](CPU* cpu){ return cpu->RR(cpu->AF.first()); }},

			/*0x20*/	{"SLA B", [](CPU* cpu){ return cpu->SLA(cpu->BC.first()); }},
			/*0x21*/	{"SLA C", [](CPU* cpu){ return cpu->SLA(cpu->BC.second()); }},
			/*0x22*/	{"SLA D", [](CPU* cpu){ return cpu->SLA(cpu->DE.first()); }},
			/*0x23*/	{"SLA E", [](CPU* cpu){ return cpu->SLA(cpu->DE.second()); }},
			/*0x24*/	{"SLA H", [](CPU* cpu){ return cpu->SLA(cpu->HL.first()); }},
			/*0x25*/	{"SLA L", [](CPU* cpu){ return cpu->SLA(cpu->HL.second()); }},
			/*0x26*/	{"SLA (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SLA(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x27*/	{"SLA A", [](CPU* cpu){ return cpu->SLA(cpu->AF.first()); }},
			/*0x28*/	{"SRA B", [](CPU* cpu){ return cpu->SRA(cpu->BC.first()); }},
			/*0x29*/	{"SRA C", [](CPU* cpu){ return cpu->SRA(cpu->BC.second()); }},
			/*0x2A*/	{"SRA D", [](CPU* cpu){ return cpu->SRA(cpu->DE.first()); }},
			/*0x2B*/	{"SRA E", [](CPU* cpu){ return cpu->SRA(cpu->DE.second()); }},
			/*0x2C*/	{"SRA H", [](CPU* cpu){ return cpu->SRA(cpu->HL.first()); }},
			/*0x2D*/	{"SRA L", [](CPU* cpu){ return cpu->SRA(cpu->HL.second()); }},
			/*0x2E*/	{"SRA (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SRA(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x2F*/	{"SRA A", [](CPU* cpu){ return cpu->SRA(cpu->AF.first()); }},

			/*0x30*/	{"SWAP B", [](CPU* cpu){ return cpu->SWAP(cpu->BC.first()); }},
			/*0x31*/	{"SWAP C", [](CPU* cpu){ return cpu->SWAP(cpu->BC.second()); }},
			/*0x32*/	{"SWAP D", [](CPU* cpu){ return cpu->SWAP(cpu->DE.first()); }},
			/*0x33*/	{"SWAP E", [](CPU* cpu){ return cpu->SWAP(cpu->DE.second()); }},
			/*0x34*/	{"SWAP H", [](CPU* cpu){ return cpu->SWAP(cpu->HL.first()); }},
			/*0x35*/	{"SWAP L", [](CPU* cpu){ return cpu->SWAP(cpu->HL.second()); }},
			/*0x36*/	{"SWAP (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SWAP(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x37*/	{"SWAP A", [](CPU* cpu){ return cpu->SWAP(cpu->AF.first()); }},
			/*0x38*/	{"SRL B", [](CPU* cpu){ return cpu->SRL(cpu->BC.first()); }},
			/*0x39*/	{"SRL C", [](CPU* cpu){ return cpu->SRL(cpu->BC.second()); }},
			/*0x3A*/	{"SRL D", [](CPU* cpu){ return cpu->SRL(cpu->DE.first()); }},
			/*0x3B*/	{"SRL E", [](CPU* cpu){ return cpu->SRL(cpu->DE.second()); }},
			/*0x3C*/	{"SRL H", [](CPU* cpu){ return cpu->SRL(cpu->HL.first()); }},
			/*0x3D*/	{"SRL L", [](CPU* cpu){ return cpu->SRL(cpu->HL.second()); }},
			/*0x3E*/	{"SRL (HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SRL(cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x3F*/	{"SRL A", [](CPU* cpu){ return cpu->SRL(cpu->AF.first()); }},

			/*0x40*/	{"BIT 0,B", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->BC.first()); }},
			/*0x41*/	{"BIT 0,C", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->BC.second()); }},
			/*0x42*/	{"BIT 0,D", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->DE.first()); }},
			/*0x43*/	{"BIT 0,E", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->DE.second()); }},
			/*0x44*/	{"BIT 0,H", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->HL.first()); }},
			/*0x45*/	{"BIT 0,L", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->HL.second()); }},
			/*0x46*/	{"BIT 0,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(0,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x47*/	{"BIT 0,A", [](CPU* cpu){ return cpu->BIT_b_r(0,cpu->AF.first()); }},
			/*0x48*/	{"BIT 1,B", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->BC.first()); }},
			/*0x49*/	{"BIT 1,C", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->BC.second()); }},
			/*0x4A*/	{"BIT 1,D", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->DE.first()); }},
			/*0x4B*/	{"BIT 1,E", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->DE.second()); }},
			/*0x4C*/	{"BIT 1,H", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->HL.first()); }},
			/*0x4D*/	{"BIT 1,L", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->HL.second()); }},
			/*0x4E*/	{"BIT 1,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(1,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x4F*/	{"BIT 1,A", [](CPU* cpu){ return cpu->BIT_b_r(1,cpu->AF.first()); }},

			/*0x50*/	{"BIT 2,B", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->BC.first()); }},
			/*0x51*/	{"BIT 2,C", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->BC.second()); }},
			/*0x52*/	{"BIT 2,D", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->DE.first()); }},
			/*0x53*/	{"BIT 2,E", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->DE.second()); }},
			/*0x54*/	{"BIT 2,H", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->HL.first()); }},
			/*0x55*/	{"BIT 2,L", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->HL.second()); }},
			/*0x56*/	{"BIT 2,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(2,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x57*/	{"BIT 2,A", [](CPU* cpu){ return cpu->BIT_b_r(2,cpu->AF.first()); }},
			/*0x58*/	{"BIT 3,B", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->BC.first()); }},
			/*0x59*/	{"BIT 3,C", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->BC.second()); }},
			/*0x5A*/	{"BIT 3,D", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->DE.first()); }},
			/*0x5B*/	{"BIT 3,E", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->DE.second()); }},
			/*0x5C*/	{"BIT 3,H", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->HL.first()); }},
			/*0x5D*/	{"BIT 3,L", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->HL.second()); }},
			/*0x5E*/	{"BIT 3,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(3,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x5F*/	{"BIT 3,A", [](CPU* cpu){ return cpu->BIT_b_r(3,cpu->AF.first()); }},

			/*0x60*/	{"BIT 4,B", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->BC.first()); }},
			/*0x61*/	{"BIT 4,C", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->BC.second()); }},
			/*0x62*/	{"BIT 4,D", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->DE.first()); }},
			/*0x63*/	{"BIT 4,E", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->DE.second()); }},
			/*0x64*/	{"BIT 4,H", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->HL.first()); }},
			/*0x65*/	{"BIT 4,L", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->HL.second()); }},
			/*0x66*/	{"BIT 4,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(4,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x67*/	{"BIT 4,A", [](CPU* cpu){ return cpu->BIT_b_r(4,cpu->AF.first()); }},
			/*0x68*/	{"BIT 5,B", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->BC.first()); }},
			/*0x69*/	{"BIT 5,C", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->BC.second()); }},
			/*0x6A*/	{"BIT 5,D", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->DE.first()); }},
			/*0x6B*/	{"BIT 5,E", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->DE.second()); }},
			/*0x6C*/	{"BIT 5,H", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->HL.first()); }},
			/*0x6D*/	{"BIT 5,L", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->HL.second()); }},
			/*0x6E*/	{"BIT 5,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(5,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x6F*/	{"BIT 5,A", [](CPU* cpu){ return cpu->BIT_b_r(5,cpu->AF.first()); }},

			/*0x70*/	{"BIT 6,B", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->BC.first()); }},
			/*0x71*/	{"BIT 6,C", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->BC.second()); }},
			/*0x72*/	{"BIT 6,D", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->DE.first()); }},
			/*0x73*/	{"BIT 6,E", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->DE.second()); }},
			/*0x74*/	{"BIT 6,H", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->HL.first()); }},
			/*0x75*/	{"BIT 6,L", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->HL.second()); }},
			/*0x76*/	{"BIT 6,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(6,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x77*/	{"BIT 6,A", [](CPU* cpu){ return cpu->BIT_b_r(6,cpu->AF.first()); }},
			/*0x78*/	{"BIT 7,B", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->BC.first()); }},
			/*0x79*/	{"BIT 7,C", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->BC.second()); }},
			/*0x7A*/	{"BIT 7,D", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->DE.first()); }},
			/*0x7B*/	{"BIT 7,E", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->DE.second()); }},
			/*0x7C*/	{"BIT 7,H", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->HL.first()); }},
			/*0x7D*/	{"BIT 7,L", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->HL.second()); }},
			/*0x7E*/	{"BIT 7,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->BIT_b_r(7,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x7F*/	{"BIT 7,A", [](CPU* cpu){ return cpu->BIT_b_r(7,cpu->AF.first()); }},

			/*0x80*/	{"RES 0,B", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->BC.first()); }},
			/*0x81*/	{"RES 0,C", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->BC.second()); }},
			/*0x82*/	{"RES 0,D", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->DE.first()); }},
			/*0x83*/	{"RES 0,E", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->DE.second()); }},
			/*0x84*/	{"RES 0,H", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->HL.first()); }},
			/*0x85*/	{"RES 0,L", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->HL.second()); }},
			/*0x86*/	{"RES 0,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(0,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x87*/	{"RES 0,A", [](CPU* cpu){ return cpu->RES_b_r(0,cpu->AF.first()); }},
			/*0x88*/	{"RES 1,B", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->BC.first()); }},
			/*0x89*/	{"RES 1,C", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->BC.second()); }},
			/*0x8A*/	{"RES 1,D", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->DE.first()); }},
			/*0x8B*/	{"RES 1,E", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->DE.second()); }},
			/*0x8C*/	{"RES 1,H", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->HL.first()); }},
			/*0x8D*/	{"RES 1,L", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->HL.second()); }},
			/*0x8E*/	{"RES 1,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(1,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x8F*/	{"RES 1,A", [](CPU* cpu){ return cpu->RES_b_r(1,cpu->AF.first()); }},

			/*0x90*/	{"RES 2,B", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->BC.first()); }},
			/*0x91*/	{"RES 2,C", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->BC.second()); }},
			/*0x92*/	{"RES 2,D", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->DE.first()); }},
			/*0x93*/	{"RES 2,E", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->DE.second()); }},
			/*0x94*/	{"RES 2,H", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->HL.first()); }},
			/*0x95*/	{"RES 2,L", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->HL.second()); }},
			/*0x96*/	{"RES 2,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(2,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x97*/	{"RES 2,A", [](CPU* cpu){ return cpu->RES_b_r(2,cpu->AF.first()); }},
			/*0x98*/	{"RES 3,B", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->BC.first()); }},
			/*0x99*/	{"RES 3,C", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->BC.second()); }},
			/*0x9A*/	{"RES 3,D", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->DE.first()); }},
			/*0x9B*/	{"RES 3,E", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->DE.second()); }},
			/*0x9C*/	{"RES 3,H", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->HL.first()); }},
			/*0x9D*/	{"RES 3,L", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->HL.second()); }},
			/*0x9E*/	{"RES 3,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(3,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0x9F*/	{"RES 3,A", [](CPU* cpu){ return cpu->RES_b_r(3,cpu->AF.first()); }},

			/*0xA0*/	{"RES 4,B", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->BC.first()); }},
			/*0xA1*/	{"RES 4,C", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->BC.second()); }},
			/*0xA2*/	{"RES 4,D", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->DE.first()); }},
			/*0xA3*/	{"RES 4,E", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->DE.second()); }},
			/*0xA4*/	{"RES 4,H", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->HL.first()); }},
			/*0xA5*/	{"RES 4,L", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->HL.second()); }},
			/*0xA6*/	{"RES 4,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(4,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xA7*/	{"RES 4,A", [](CPU* cpu){ return cpu->RES_b_r(4,cpu->AF.first()); }},
			/*0xA8*/	{"RES 5,B", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->BC.first()); }},
			/*0xA9*/	{"RES 5,C", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->BC.second()); }},
			/*0xAA*/	{"RES 5,D", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->DE.first()); }},
			/*0xAB*/	{"RES 5,E", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->DE.second()); }},
			/*0xAC*/	{"RES 5,H", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->HL.first()); }},
			/*0xAD*/	{"RES 5,L", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->HL.second()); }},
			/*0xAE*/	{"RES 5,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(5,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xAF*/	{"RES 5,A", [](CPU* cpu){ return cpu->RES_b_r(5,cpu->AF.first()); }},

			/*0xB0*/	{"RES 6,B", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->BC.first()); }},
			/*0xB1*/	{"RES 6,C", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->BC.second()); }},
			/*0xB2*/	{"RES 6,D", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->DE.first()); }},
			/*0xB3*/	{"RES 6,E", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->DE.second()); }},
			/*0xB4*/	{"RES 6,H", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->HL.first()); }},
			/*0xB5*/	{"RES 6,L", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->HL.second()); }},
			/*0xB6*/	{"RES 6,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(6,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xB7*/	{"RES 6,A", [](CPU* cpu){ return cpu->RES_b_r(6,cpu->AF.first()); }},
			/*0xB8*/	{"RES 7,B", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->BC.first()); }},
			/*0xB9*/	{"RES 7,C", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->BC.second()); }},
			/*0xBA*/	{"RES 7,D", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->DE.first()); }},
			/*0xBB*/	{"RES 7,E", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->DE.second()); }},
			/*0xBC*/	{"RES 7,H", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->HL.first()); }},
			/*0xBD*/	{"RES 7,L", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->HL.second()); }},
			/*0xBE*/	{"RES 7,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->RES_b_r(7,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xBF*/	{"RES 7,A", [](CPU* cpu){ return cpu->RES_b_r(7,cpu->AF.first()); }},

			/*0xC0*/	{"SET 0,B", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->BC.first()); }},
			/*0xC1*/	{"SET 0,C", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->BC.second()); }},
			/*0xC2*/	{"SET 0,D", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->DE.first()); }},
			/*0xC3*/	{"SET 0,E", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->DE.second()); }},
			/*0xC4*/	{"SET 0,H", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->HL.first()); }},
			/*0xC5*/	{"SET 0,L", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->HL.second()); }},
			/*0xC6*/	{"SET 0,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(0,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xC7*/	{"SET 0,A", [](CPU* cpu){ return cpu->SET_b_r(0,cpu->AF.first()); }},
			/*0xC8*/	{"SET 1,B", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->BC.first()); }},
			/*0xC9*/	{"SET 1,C", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->BC.second()); }},
			/*0xCA*/	{"SET 1,D", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->DE.first()); }},
			/*0xCB*/	{"SET 1,E", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->DE.second()); }},
			/*0xCC*/	{"SET 1,H", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->HL.first()); }},
			/*0xCD*/	{"SET 1,L", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->HL.second()); }},
			/*0xCE*/	{"SET 1,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(1,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xCF*/	{"SET 1,A", [](CPU* cpu){ return cpu->SET_b_r(1,cpu->AF.first()); }},

			/*0xD0*/	{"SET 2,B", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->BC.first()); }},
			/*0xD1*/	{"SET 2,C", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->BC.second()); }},
			/*0xD2*/	{"SET 2,D", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->DE.first()); }},
			/*0xD3*/	{"SET 2,E", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->DE.second()); }},
			/*0xD4*/	{"SET 2,H", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->HL.first()); }},
			/*0xD5*/	{"SET 2,L", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->HL.second()); }},
			/*0xD6*/	{"SET 2,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(2,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xD7*/	{"SET 2,A", [](CPU* cpu){ return cpu->SET_b_r(2,cpu->AF.first()); }},
			/*0xD8*/	{"SET 3,B", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->BC.first()); }},
			/*0xD9*/	{"SET 3,C", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->BC.second()); }},
			/*0xDA*/	{"SET 3,D", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->DE.first()); }},
			/*0xDB*/	{"SET 3,E", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->DE.second()); }},
			/*0xDC*/	{"SET 3,H", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->HL.first()); }},
			/*0xDD*/	{"SET 3,L", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->HL.second()); }},
			/*0xDE*/	{"SET 3,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(3,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xDF*/	{"SET 3,A", [](CPU* cpu){ return cpu->SET_b_r(3,cpu->AF.first()); }},

			/*0xE0*/	{"SET 4,B", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->BC.first()); }},
			/*0xE1*/	{"SET 4,C", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->BC.second()); }},
			/*0xE2*/	{"SET 4,D", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->DE.first()); }},
			/*0xE3*/	{"SET 4,E", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->DE.second()); }},
			/*0xE4*/	{"SET 4,H", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->HL.first()); }},
			/*0xE5*/	{"SET 4,L", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->HL.second()); }},
			/*0xE6*/	{"SET 4,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(4,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xE7*/	{"SET 4,A", [](CPU* cpu){ return cpu->SET_b_r(4,cpu->AF.first()); }},
			/*0xE8*/	{"SET 5,B", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->BC.first()); }},
			/*0xE9*/	{"SET 5,C", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->BC.second()); }},
			/*0xEA*/	{"SET 5,D", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->DE.first()); }},
			/*0xEB*/	{"SET 5,E", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->DE.second()); }},
			/*0xEC*/	{"SET 5,H", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->HL.first()); }},
			/*0xED*/	{"SET 5,L", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->HL.second()); }},
			/*0xEE*/	{"SET 5,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(5,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xEF*/	{"SET 5,A", [](CPU* cpu){ return cpu->SET_b_r(5,cpu->AF.first()); }},

			/*0xF0*/	{"SET 6,B", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->BC.first()); }},
			/*0xF1*/	{"SET 6,C", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->BC.second()); }},
			/*0xF2*/	{"SET 6,D", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->DE.first()); }},
			/*0xF3*/	{"SET 6,E", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->DE.second()); }},
			/*0xF4*/	{"SET 6,H", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->HL.first()); }},
			/*0xF5*/	{"SET 6,L", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->HL.second()); }},
			/*0xF6*/	{"SET 6,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(6,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xF7*/	{"SET 6,A", [](CPU* cpu){ return cpu->SET_b_r(6,cpu->AF.first()); }},
			/*0xF8*/	{"SET 7,B", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->BC.first()); }},
			/*0xF9*/	{"SET 7,C", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->BC.second()); }},
			/*0xFA*/	{"SET 7,D", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->DE.first()); }},
			/*0xFB*/	{"SET 7,E", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->DE.second()); }},
			/*0xFC*/	{"SET 7,H", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->HL.first()); }},
			/*0xFD*/	{"SET 7,L", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->HL.second()); }},
			/*0xFE*/	{"SET 7,(HL)", [](CPU* cpu){ return HL_RESOLVE + cpu->SET_b_r(7,cpu->memoryMap.byte(cpu->HL.word())); }},
			/*0xFF*/	{"SET 7,A", [](CPU* cpu){ return cpu->SET_b_r(7,cpu->AF.first()); }},
		};