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

		void printRom(){
			for (unsigned int i = 0; i < 16; i++){
				for (unsigned int j = 0; j < 16; j++){
					std::cout << std::hex << (int)bootRom[i*16+j] << " ";
				}
				std::cout << std::endl;
			}
		}
		void printRegion(unsigned int address){
			for (unsigned int i = 0; i < 16; i++){
				unsigned int loc = address + i;
				spdlog::get("console")->info("{:x} @ loc {:x}", bootRom[loc], loc);
			}
		}
		//Once disabled it cannot be re-enabled
		bool bootRomEnabled(){
			_bootRomEnabled = _bootRomEnabled && cartridge[BRD] != 1;
			if(_bootRomEnabled == false){
				spdlog::get("console")->info("BootRom Disabled");
				exit(0);
			}
			return _bootRomEnabled;
		}

		Byte byte(unsigned int address) {
			if(address == 0x29 || address == 0x2A){
				spdlog::get("console")->info("fetch");
			}
			if (bootRomEnabled() && address < bootRomSize){
				return Byte(&bootRom[address], true);
			}
			else{
				return Byte(&cartridge[address]);
			}
		}

		void setByte(const unsigned int address, const Byte value) {
			
			if (address >= bootRomSize) {
				cartridge[address] = value;
			}
			else {
				spdlog::get("stderr")->error("Tried to write set read only byte {0:x}", address);
				exit(1);
			}
		}


	private:
		std::array<unsigned char, 0x100> bootRom = {};
		std::array<unsigned char, 0x10000> cartridge = {};
		bool _bootRomEnabled = true;
		unsigned int cartridgeSize;
		unsigned int bootRomSize;
};