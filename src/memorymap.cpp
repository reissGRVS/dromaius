#include "memorymap.h"
#include "bytes/byte.h"

#include <fstream>
#include <iostream>
#include <iomanip>

MemoryMap::MemoryMap(std::string cartridgeName) {
	//Load bootRom
	std::ifstream bootRomStream {"roms/dmg_boot.bin"};
	if (bootRomStream.good()){
		spdlog::get("console")->info("LOADING Boot Rom");
		bootRomStream.read(reinterpret_cast<char*>(bootRom.data()), BOOT_ROM_SIZE);
	}
	else {
		spdlog::get("stderr")->error("Couldnt open boot rom");
	}

	//Load cartridge
	std::ifstream cartridgeStream(cartridgeName, std::ios::binary);
	cartridgeStream.unsetf(std::ios::skipws);
	if (cartridgeStream.good()){
		//find cartridge size
		cartridgeStream.seekg(0, cartridgeStream.end);
		cartridgeSize = cartridgeStream.tellg();
		spdlog::get("console")->info("LOADING Cartridge - Size: {}",cartridgeSize);
		cartridgeStream.seekg(0, cartridgeStream.beg);
		cartridge.resize(cartridgeSize);
		cartridge.insert(cartridge.begin(), std::istream_iterator<uint8_t>(cartridgeStream), 
											std::istream_iterator<uint8_t>());	
	}
	else {
		spdlog::get("stderr")->error("Couldnt open cartridge");
	}

	//Cartridge info
	//Title
	for (auto address = 0x134; address < 0x143; address++){
		std::cout << byte(address).val();
	}
	std::cout << std::endl;
	std::cout << "Cartridge type: " << (int)byte(0x147).val() << std::endl;
	std::cout << "ROM size: " << (int)byte(0x148).val() << std::endl;
	std::cout << "RAM size: " << (int)byte(0x149).val() << std::endl;
}

//Set the function that sets the P1 byte used for joypad checks
void MemoryMap::setJoypadCallback(std::function<void(Byte)> joypadUpdate){
	joypadUpdateByte = joypadUpdate;
}

//Once disabled it cannot be re-enabled
bool MemoryMap::bootRomEnabled(){
	_bootRomEnabled = _bootRomEnabled && byte(BRD).val() != 1;
	return _bootRomEnabled;
}

void MemoryMap::startDMA(){
	spdlog::get("console")->debug("DMA");
	uint16_t sourceOffset = byte(DMA).val(); //Source: XX00-XX9F XX in range from 00-F1h;
	sourceOffset = sourceOffset<<8;
	for (int i = 0; i < 0xA0; i++){
		oam[i] = byte(sourceOffset+i).val();
	}
	//TODO: proper timing and write protection during this
}

void MemoryMap::setVramAccess(bool value){
	vramEnabled = value;
}

void MemoryMap::setOamAccess(bool value){
	oamEnabled = value;
}


Byte MemoryMap::byte(uint16_t address) {

	//ROM
	if 		(address < VIDEO_RAM){
		if (bootRomEnabled() && address < BOOT_ROM_SIZE){
			return Byte(&bootRom[address], ByteType::NO_WRITE);
		}
		return Byte(&cartridge[address], ByteType::NO_WRITE);
	}
	//VIDEO RAM
	else if (address < EXTERNAL_RAM){
		address -= VIDEO_RAM;
		if (address >= vram.size()){ spdlog::get("stderr")->error("VRAM BAD ADDRESS");}
		return Byte(&vram[address]);
	}
	//EXTERNAL RAM
	else if (address < WORK_RAM_0){
		address -= EXTERNAL_RAM;
		if (address >= extram.size()){ spdlog::get("stderr")->error("EXTRAM BAD ADDRESS");}
		return Byte(&extram[address]);
	}
	//WORK RAM
	else if (address < ECHO_RAM){
		address -= WORK_RAM_0;
		if (address >= wram.size()){ spdlog::get("stderr")->error("WRAM BAD ADDRESS");}
		return Byte(&wram[address]);
	}
	//ECHO RAM
	else if (address < SPRITE_OAM){
		address -= ECHO_RAM;
		if (address >= wram.size()){ spdlog::get("stderr")->error("ECHO BAD ADDRESS");}
		return Byte(&wram[address]);
	}
	//SPRITE OAM
	else if (address < NOT_USABLE){
		address -= SPRITE_OAM;
		if (address >= oam.size()){ spdlog::get("stderr")->error("SPRITE OAM BAD ADDRESS");}
		return Byte(&oam[address]);
	}
	//NOT USABLE
	else if (address < IO_PORTS){
		address-=NOT_USABLE;
		if (address >= notUsable.size()){ spdlog::get("stderr")->error("NOT USABLE BAD ADDRESS");}
		//Returns bootRom as by the time a program tries to access this section it should be disabled and wont be used
		return Byte(&notUsable[address]);
	}
	//IO PORTS
	else if (address < HIGH_RAM){
		address -= IO_PORTS;
		if (address >= wram.size()){ spdlog::get("stderr")->error("ECHO BAD ADDRESS");}
		switch (address+IO_PORTS){
			case P1:
				//Update joypad input byte with current key inputs
				joypadUpdateByte(Byte(&ioPorts[address]));
				break;
			case SC:
				//Link cable debugging
				std::cout << ioPorts[SB];
				break;
			case DMA:
				return Byte(&ioPorts[address], ByteType::DMA_SIGNAL, this);
			case DIV:
				return Byte(&ioPorts[address], ByteType::WRITE_RESET);
		}
		return Byte(&ioPorts[address]);
	}
	//HRAM
	else if (address < IE){
		address -= HIGH_RAM;
		return Byte(&hram[address]);
	}
	//IE
	else if (address == IE){
		return Byte(&interruptEnable);
	}

}