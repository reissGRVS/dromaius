#pragma once
#include "forwarddecls.h"
#include "bytes/byte.h"


//Interface class between computer input and emulator input
class Joypad{
	public:
		void keyUpdate();

		static void byteUpdate(Byte P1){
			auto directionSwitch = !P1.getBit(4);
			auto buttonSwitch = !P1.getBit(5);

			auto p10 = (directionSwitch && right) 	|| (buttonSwitch && a);
			auto p11 = (directionSwitch && left) 	|| (buttonSwitch && b);
			auto p12 = (directionSwitch && up) 		|| (buttonSwitch && select);
			auto p13 = (directionSwitch && down) 	|| (buttonSwitch && start);
			
			P1.setBit(0,	!p10);
			P1.setBit(1,	!p11);
			P1.setBit(2,	!p12);
			P1.setBit(3,	!p13);

			// 			P14 		P15
			//  		| 			|
			//  P10-----O-Right-----O-A
			//  		| 			|
			//  P11-----O-Left------O-B
			//  		| 			|
			//  P12-----O-Up--------O-Select
			//  		| 			|
			//  P13-----O-Down------O-Start
			//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
		}

	private:

		//Key state
		static bool right;
		static bool left;
		static bool up;
		static bool down;

		static bool a;
		static bool b;
		static bool select;
		static bool start;
};