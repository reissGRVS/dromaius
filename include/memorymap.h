#pragma once

#include "forwarddecls.h"
#include <array>

class MemoryMap{
	
	public:
		MemoryMap(std::string cartridgeName);
		void setJoypadCallback(std::function<void(Byte)> joypadUpdate);
		bool bootRomEnabled();
		Byte byte(uint16_t address);
		void startDMA();
		void setVramAccess(bool value);
		void setOamAccess(bool value);
	private:
		std::array<uint8_t, 0x100> bootRom = {};
		std::array<uint8_t, 0x10000> cartridge = {};
		bool _bootRomEnabled = true;
		//Currently unused
		bool vramEnabled = true;
		bool oamEnabled = true;
		
		uint16_t cartridgeSize;
		uint16_t bootRomSize;

		std::function<void(Byte)> joypadUpdateByte;
};