#pragma once

#include "forwarddecls.h"
#include <array>
#include <fstream>

class MemoryMap{
	
	public:
		MemoryMap() {
			std::ifstream bootRom {"roms/dmg_boot.bin"};
			if (bootRom.good()){
				
				spdlog::get("console")->info("LOADING Boot Rom - Size: {}", BOOT_ROM_SIZE);
				bootRom.read(reinterpret_cast<char*>(memory.data()), BOOT_ROM_SIZE);
			}
			else {
				spdlog::get("stderr")->error("Couldnt open boot rom");
			}

		};

		Byte & byte(Word address) {
			return memory[address];
		}

		Byte getByte(Word address) const {
			return memory[address];
		};

		void setByte(const Word address, const Byte value) {
			
			if (address >= BOOT_ROM_SIZE) {
				memory[address] = value;
			}
			else {
				spdlog::get("stderr")->error("Tried to write set read only byte {0:x}", address);
				exit(1);
			}
		}

	private:
		std::array<Byte, 0x10000> memory = {};
		const unsigned int BOOT_ROM_SIZE = 0x100;
};