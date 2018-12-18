#pragma once

#include "forwarddecls.h"
#include "memorylocs.h"
#include <array>
#include <fstream>
#include <iostream>

class MemoryMap{
	
	public:
		MemoryMap(std::string cartridgeName) {
			//Load bootRom
			std::ifstream bootRomStream {"roms/dmg_boot.bin"};
			if (bootRomStream.good()){
				bootRomStream.seekg(0, bootRomStream.end);
				bootRomSize = bootRomStream.tellg();
				spdlog::get("console")->info("LOADING Boot Rom - Size: {}",bootRomSize);
				bootRomStream.seekg(0, bootRomStream.beg);
				bootRomStream.read(reinterpret_cast<char*>(bootRom.data()), bootRomSize);
			}
			else {
				spdlog::get("stderr")->error("Couldnt open boot rom");
			}

			//Load cartridge
			std::ifstream cartridgeStream {cartridgeName};
			if (cartridgeStream.good()){
				cartridgeStream.seekg(0, cartridgeStream.end);
				cartridgeSize = cartridgeStream.tellg();
				spdlog::get("console")->info("LOADING Cartridge - Size: {}",cartridgeSize);
				cartridgeStream.seekg(0, cartridgeStream.beg);
				cartridgeStream.read(reinterpret_cast<char*>(cartridge.data()), cartridgeSize);
			}
			else {
				spdlog::get("stderr")->error("Couldnt open cartridge");
			}		

		};

		void printRegion(Word address){
			for (Word i = 0; i < 16; i++){
				Word loc = address + i;
				spdlog::get("console")->info("{:x} @ loc {:x}", cartridge[loc], loc);
			}
		}
		//Once disabled it cannot be re-enabled
		bool bootRomEnabled(){
			_bootRomEnabled = _bootRomEnabled && cartridge[BOOT_ROM_DISABLE] != 1;
			if(_bootRomEnabled == false){

				spdlog::get("console")->info("BootRom Disabled");
				printRegion(BOOT_ROM_DISABLE-5);
				exit(0);
			}
			return _bootRomEnabled;
		}

		Byte & byte(Word address) {
			if (bootRomEnabled() && address < bootRomSize){
				return bootRom[address];
			}
			else{
				return cartridge[address];
			}
		}

		Byte getByte(Word address) {
			if (bootRomEnabled() && address < bootRomSize){
				return bootRom[address];
			}
			else{
				return cartridge[address];
			}
		};

		void setByte(const Word address, const Byte value) {
			
			if (address >= bootRomSize) {
				cartridge[address] = value;
			}
			else {
				spdlog::get("stderr")->error("Tried to write set read only byte {0:x}", address);
				exit(1);
			}
		}


	private:
		std::array<Byte, 0x100> bootRom = {};
		std::array<Byte, 0x10000> cartridge = {};
		bool _bootRomEnabled = true;
		unsigned int cartridgeSize;
		unsigned int bootRomSize;
};