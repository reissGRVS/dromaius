#pragma once
using LOC = const uint16_t;


LOC ROM_BANK_00 = 0x0000;
LOC ROM_BANK_NN = 0x4000;
LOC VIDEO_RAM = 0x8000;
LOC WORK_RAM_0 = 0xC000;
LOC WORK_RAM_1 = 0xD000;
LOC ECHO_RAM = 0xE000;
LOC SPRITE_OAM = 0xFE00;

  /////////////
 //REGISTERS//
/////////////

LOC P1 		= 0xFF00; //Reading joy pad info and determining system type. (R/W)
LOC SB 		= 0xFF01; //Serial transfer data (R/W)
LOC SC 		= 0xFF02; //SIO control (R/W)
LOC DIV 	= 0xFF04; //Divider Register (R/W)
LOC TIMA 	= 0xFF05; //Timer counter (R/W)
LOC TMA 	= 0xFF06; //Timer Modulo (R/W)
LOC TAC		= 0xFF07; //Timer Control (R/W)
LOC IF		= 0xFF0F; //Interrupt Flag (R/W)

LOC NR_10	= 0xFF10; //Sound Mode 1 register, Sweep register (R/W)
LOC NR_11	= 0xFF11; //Sound Mode 1 register, Sound length/Wave pattern duty (R/W)
LOC NR_12	= 0xFF12; //Sound Mode 1 register, Envelope (R/W)
LOC NR_13	= 0xFF13; //Sound Mode 1 register, Frequency lo (R/W)
LOC NR_14	= 0xFF14; //Sound Mode 1 register, Frequency hi (R/W)

LOC NR_21	= 0xFF16; //Sound Mode 2 register, Sound length/Wave pattern duty (R/W)
LOC NR_22	= 0xFF17; //Sound Mode 2 register, Envelope (R/W)
LOC NR_23	= 0xFF18; //Sound Mode 2 register, Frequency lo (R/W)
LOC NR_24	= 0xFF19; //Sound Mode 2 register, Frequency hi (R/W)

LOC NR_30	= 0xFF1A; //Sound Mode 3 register, Sound on/off (R/W)
LOC NR_31	= 0xFF1B; //Sound Mode 3 register, Sound length (R/W)
LOC NR_32	= 0xFF1C; //Sound Mode 3 register, Select output level (R/W)
LOC NR_33	= 0xFF1D; //Sound Mode 3 register, Frequency's lower data (W)
LOC NR_34	= 0xFF1E; //Sound Mode 3 register, Frequency's higher data (R/W)

LOC NR_41	= 0xFF20; //Sound Mode 4 register, Sound length (R/W)
LOC NR_42	= 0xFF21; //Sound Mode 4 register, Envelope (R/W)
LOC NR_43	= 0xFF22; //Sound Mode 4 register, Polynomial counter (R/W)
LOC NR_44	= 0xFF23; //Sound Mode 4 register, Counter/consecutive; initial (R/W)

LOC NR_50	= 0xFF24; //Channel control / ON-OFF / Volume (R/W)
LOC NR_51	= 0xFF25; //Selection of Sound output terminal (R/W)
LOC NR_52	= 0xFF26; //Sound on/off (R/W)

//FF30-FF3F (Wave Pattern RAM)


LOC LCDC	= 0xFF40; //LCD Control (R/W)
LOC STAT	= 0xFF41; //LCD Status (R/W)
LOC SCY		= 0xFF42; //Scroll Y (R/W)
LOC SCX		= 0xFF43; //Scroll X (R/W)
LOC LY		= 0xFF44; //LCDC Y-Coordinate (R)
LOC LYC		= 0xFF45; //LY Compare (R/W)
LOC DMA		= 0xFF46; //DMA Transfer and Start Address (W)
LOC BGP		= 0xFF47; //BG & Window Palette Data (R/W)
LOC OBP0	= 0xFF48; //Object Palette 0 Data (R/W)
LOC OBP1	= 0xFF49; //Object Palette 1 Data (R/W)
LOC WY		= 0xFF4A; //Window Y Position (R/W)
LOC WX		= 0xFF4B; //Window X Position (R/W)

LOC BRD		= 0xFF50; //Boot Rom Disable

LOC HIGH_RAM = 0xFF90;

LOC IE 		= 0xFFFF; //Interrupt Enable (R/W)