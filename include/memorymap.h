#pragma once

#include "forwarddecls.h"
#include <array>

class MemoryMap{
	
	public:
		MemoryMap(std::string cartridgeName);
		void setJoypadCallback(std::function<void(Byte)> joypadUpdate);
		bool bootRomEnabled();
		Byte byte(unsigned int address);
		void startDMA();
		void setVramAccess(bool value);
		void setOamAccess(bool value);
	private:
		std::array<unsigned char, 0x100> bootRom = {};
		std::array<unsigned char, 0x10000> cartridge = {};
		bool _bootRomEnabled = true;
		bool vramEnabled = true;
		bool oamEnabled = true;
		unsigned int cartridgeSize;
		unsigned int bootRomSize;

		std::function<void(Byte)> joypadUpdateByte;
};