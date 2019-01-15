#pragma once

#include "forwarddecls.h"
#include "memorylocs.h"
#include <array>
#include <vector>

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
		std::vector<uint8_t> cartridge = {};
		
		std::array<uint8_t, (EXTERNAL_RAM - VIDEO_RAM)> vram = {};
		std::array<uint8_t, (WORK_RAM_0 - EXTERNAL_RAM)> extram = {};
		std::array<uint8_t, (ECHO_RAM-WORK_RAM_0)> wram = {};
		std::array<uint8_t, (NOT_USABLE-SPRITE_OAM)> oam = {};
		std::array<uint8_t, (IO_PORTS-NOT_USABLE)> notUsable = {};
		std::array<uint8_t, (HIGH_RAM-IO_PORTS)> ioPorts = {};
		std::array<uint8_t, (IE-HIGH_RAM)> hram = {};
		uint8_t interruptEnable = 0;

		bool _bootRomEnabled = true;
		//Currently unused
		bool vramEnabled = true;
		bool oamEnabled = true;
		
		uint32_t cartridgeSize;
		const uint16_t BOOT_ROM_SIZE = 0x100;

		std::function<void(Byte)> joypadUpdateByte;
};