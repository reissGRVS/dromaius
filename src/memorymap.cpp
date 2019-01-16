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
	//TODO: Make this more accurate
	romBankCount = 2<<(byte(0x148).val());
	std::cout << "ROM size (banks): " << (int)romBankCount << std::endl;
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
void MemoryMap::setRomBank(uint8_t bank){
	if (bank <= romBankCount){
		if (bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60 ){
			bank += 1;
		}
		romBankNumber = bank;
	}
	else{
		spdlog::get("stderr")->error("Trying to set rombank to invalid value {:x}", bank);
	}
}


Byte MemoryMap::byte(uint16_t address) {

	//ROM BANK 00
	if 		(address < ROM_BANK_NN){
		if (bootRomEnabled() && address < BOOT_ROM_SIZE){
			return Byte(&bootRom[address], ByteType::NO_WRITE);
		}

		if (address >= 0x2000){
			return Byte(&cartridge[address], ByteType::ROM_BANK_SELECT, this);
		}
		return Byte(&cartridge[address], ByteType::NO_WRITE);
	}
	//ROM BANK NN
	else if	(address < VIDEO_RAM){
		uint32_t cartAddress = address + ROM_BANK_NN * (romBankNumber-1);
		return Byte(&cartridge[cartAddress], ByteType::NO_WRITE);
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
			case DIV:
				return Byte(&ioPorts[address], ByteType::WRITE_RESET);
			case LY:
				return Byte(&ioPorts[address], ByteType::WRITE_RESET);
			case DMA:
				return Byte(&ioPorts[address], ByteType::DMA_SIGNAL, this);
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

	exit(2);
}