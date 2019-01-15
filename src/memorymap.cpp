#include "memorymap.h"
#include "memorylocs.h"
#include "bytes/byte.h"

#include <fstream>
#include <iostream>

MemoryMap::MemoryMap(std::string cartridgeName) {
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
}

//Set the function that sets the P1 byte used for joypad checks
void MemoryMap::setJoypadCallback(std::function<void(Byte)> joypadUpdate){
	joypadUpdateByte = joypadUpdate;
}

//Once disabled it cannot be re-enabled
bool MemoryMap::bootRomEnabled(){
	_bootRomEnabled = _bootRomEnabled && cartridge[BRD] != 1;
	return _bootRomEnabled;
}

void MemoryMap::startDMA(){
	spdlog::get("console")->info("DMA");
	uint16_t sourceOffset = cartridge[DMA]; //Source: XX00-XX9F XX in range from 00-F1h;
	sourceOffset = sourceOffset<<8;
	for (int i = 0; i < 0xA0; i++){
		cartridge[SPRITE_OAM+i] = cartridge[sourceOffset+i];
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


	if (address == P1){
		//Update joypad input byte with current key inputs
		joypadUpdateByte(Byte(&cartridge[address]));
	}

	//Echo memory
	if (address >= ECHO_RAM && address < SPRITE_OAM ){
		address -= (ECHO_RAM-WORK_RAM_0);
	}

	// if (!oamEnabled && address >= SPRITE_OAM && address < (SPRITE_OAM+0x100)){
	// 	return Byte(&cartridge[address], ByteType::NO_WRITE);
	// };

	// if (!vramEnabled && address >= VIDEO_RAM && address < (VIDEO_RAM+0x2000)){
	// 	return Byte(&cartridge[address], ByteType::NO_WRITE);
	// };

	//Link cable debugging
	if (address == SC) {
		std::cout << cartridge[SB];
	}

	if (address == DMA){
		return Byte(&cartridge[address], ByteType::DMA_SIGNAL, this);
	}

	if (address == DIV){
		return Byte(&cartridge[address], ByteType::WRITE_RESET);
	}

	if (bootRomEnabled() && address < bootRomSize){
		return Byte(&bootRom[address], ByteType::NO_WRITE);
	}
	else{
		//if ROM
		if (address < VIDEO_RAM){
			return Byte(&cartridge[address], ByteType::NO_WRITE);
		}
		return Byte(&cartridge[address]);
	}
}