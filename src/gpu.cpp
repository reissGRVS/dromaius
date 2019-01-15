#include "gpu.h"
#include "memorylocs.h"
#include "bytes/byte.h"
#include <iostream>

GPU::GPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{
}

void GPU::process(Ticks ticks){
	tickCount += ticks;
	
	switch(mode){
		case Mode::OAM_SEARCH:
			if (tickCount > ticksPerOamSearch){
				tickCount %= ticksPerOamSearch;
				setMode(Mode::PIXEL_TRANSFER);
			}
			break;
		case Mode::PIXEL_TRANSFER:
			if (tickCount > ticksPerPixelTransfer){
				tickCount %= ticksPerPixelTransfer;
				setMode(Mode::H_BLANK);

				//LY Coincidence interrupt	TODO: I believe this should be possible in other transitions as well			
				auto lyMatch = (memoryMap.byte(LYC).val() == memoryMap.byte(LY).val());
				auto stat = memoryMap.byte(STAT);
				//set coincedence flag
				stat.setBit(2,lyMatch);
				//if enabled
				if(lyMatch && stat.getBit(6)){
					memoryMap.byte(IF).setBit(1, 1);
				}

			}
			break;
		case Mode::H_BLANK:
			if (tickCount > ticksPerHBlank){
				tickCount %= ticksPerHBlank;

				//Process line
				auto byteLY = memoryMap.byte(LY);
				//TODO: Draw this line
				byteLY++;

				//144 Gameboy screen height TODO magic number
				if (byteLY == 144){
					//VBlank interrupt
					memoryMap.byte(IF).setBit(0, 1);
					setMode(Mode::V_BLANK);
				}
				else{
					setMode(Mode::OAM_SEARCH);
				}

			}
			break;
		case Mode::V_BLANK:
			if (tickCount > ticksPerLine){
				tickCount %= ticksPerLine;
				auto byteLY = memoryMap.byte(LY);
				byteLY++;

				if (byteLY == 154){
					//TODO: last LY is meant to be shorter http://gameboy.mongenel.com/dmg/istat98.txt
					//TODO: push current buffer to screen
					byteLY = 0;
					setMode(Mode::OAM_SEARCH);
				}
			}
			break;
		default:
			spdlog::get("stderr")->info("invalid GPU mode");
			exit(0);
	}
}

void GPU::setMode(Mode m){
	mode = m;
	auto statLCD = memoryMap.byte(STAT);
	switch(mode){
		case Mode::H_BLANK:
			memoryMap.setVramAccess(true);
			memoryMap.setOamAccess(true);
			//H-Blank interrupt
			if (statLCD.getBit(3)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCD.setBit(0,0);
			statLCD.setBit(1,0);
			break;
		case Mode::V_BLANK:
			//V-Blank interrupt
			if (statLCD.getBit(4)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCD.setBit(0,1);
			statLCD.setBit(1,0);
			break;
		case Mode::OAM_SEARCH:
			memoryMap.setOamAccess(false);
			//OAM interrupt
			if (statLCD.getBit(5)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCD.setBit(0,0);
			statLCD.setBit(1,1);
			break;
		case Mode::PIXEL_TRANSFER:
			statLCD.setBit(0,1);
			statLCD.setBit(1,1);
			memoryMap.setVramAccess(false);
			break;

	}
}


void GPU::initialiseTileMapData(){
	auto address = VRAM_TILE_START;
	const unsigned int rowSize = NO_TILES*Tile::WIDTH;
	for (unsigned int tile = 0; tile < NO_TILES; tile++){
		for (auto row = 0; row < Tile::HEIGHT; row++){
			auto b1 = memoryMap.byte(address++);
			auto b2 = memoryMap.byte(address++);

			const unsigned int tileStart = tile*Tile::WIDTH;
			for (auto column = 0; column < Tile::WIDTH; column++){
				//Figure out colour
				auto b1Bit = b1.getBit(Tile::WIDTH - column -1);
				auto b2Bit = b2.getBit(Tile::WIDTH - column -1);
				auto colourCode = (b2Bit << 1) + b1Bit;
				//Add to the tileMapData
				auto pos = tileStart + column + row*(rowSize);
				tileMapData[pos] = colourCode;
			}
		}
	}

}

void GPU::exportTileMap(){
	for (unsigned int x = 0; x < NO_TILES *Tile::WIDTH; x++){
		for (auto y = 0; y < Tile::HEIGHT; y++){
				
			auto pos = x + y * NO_TILES *Tile::WIDTH;
			auto firstPos = 4*pos;
			auto colour = tileMapData[pos];
			switch(colour){
				case 3:
					/*R*/tileMapDataSF[firstPos] = 0;
					/*G*/tileMapDataSF[firstPos+1] = 0;
					/*B*/tileMapDataSF[firstPos+2] = 0;
					/*A*/tileMapDataSF[firstPos+3] = 255;
					break;
				case 2:
					/*R*/tileMapDataSF[firstPos] = 80;
					/*G*/tileMapDataSF[firstPos+1] = 80;
					/*B*/tileMapDataSF[firstPos+2] = 80;
					/*A*/tileMapDataSF[firstPos+3] = 255;
					break;
				case 1:
					/*R*/tileMapDataSF[firstPos] = 180;
					/*G*/tileMapDataSF[firstPos+1] = 180;
					/*B*/tileMapDataSF[firstPos+2] = 180;
					/*A*/tileMapDataSF[firstPos+3] = 255;
					break;
				case 0:
					/*R*/tileMapDataSF[firstPos] = 250;
					/*G*/tileMapDataSF[firstPos+1] = 255;
					/*B*/tileMapDataSF[firstPos+2] = 255;
					/*A*/tileMapDataSF[firstPos+3] = 255;
					break;
			} 
		}
	}

	sf::Image tileMap;
	tileMap.create(NO_TILES *Tile::WIDTH, Tile::HEIGHT, tileMapDataSF.data());
	tileMap.saveToFile("tilemap.png");
}

unsigned char GPU::getTilePixel(unsigned char tileID, unsigned char x, unsigned char y){
	if (memoryMap.byte(LCDC).getBit(4) == 0){
		//8800-97FF 9000 is tile 0, 8800 is -128
		//Change "signed" byte counting from 9000 to unsigned counting from 8800
		tileID += 128;
		return tileMapData[(128+tileID)*Tile::WIDTH + x + y*NO_TILES*Tile::WIDTH];
	}
	else{
		return tileMapData[tileID*Tile::WIDTH + x + y*NO_TILES*Tile::WIDTH];
	}
}


void GPU::renderBackground(){
	auto lcdc = memoryMap.byte(LCDC);
	//if background display disabled dont draw
	if (lcdc.getBit(0) == 0){
		return;
	}
	const unsigned int backgroundTileMap = lcdc.getBit(3) ? 0x9C00 : 0x9800;
	//get background map
	auto scx = memoryMap.byte(SCX).val();
	auto scy = memoryMap.byte(SCY).val();
	
	for (unsigned char yOffset = 0; yOffset < HEIGHT; yOffset++ ) { //32x32tilemap
		for (unsigned char xOffset = 0; xOffset < WIDTH; xOffset++ ){

			unsigned char x = xOffset + scx;
			unsigned char y = yOffset + scy;

			unsigned char xTile = x/Tile::WIDTH;
			unsigned char yTile = y/Tile::HEIGHT;
			//TODO Magic Number
			unsigned int address = backgroundTileMap+xTile+yTile*(32);

			auto tileID = memoryMap.byte(address).val();
			auto pixel = getTilePixel(tileID, x%Tile::WIDTH, y%Tile::HEIGHT);
			drawPixel(pixel, xOffset, yOffset);
		}
	}
}

void GPU::renderWindow(){
	auto lcdc = memoryMap.byte(LCDC);
	//if window not enabled dont draw
	if (lcdc.getBit(5) == 0){
		return;
	}

	const unsigned int windowTileMap = lcdc.getBit(6) ? 0x9C00 : 0x9800;
	auto wx = memoryMap.byte(WX).val();
	auto wy = memoryMap.byte(WY).val();
	
	for (unsigned char yOffset = 0; yOffset < HEIGHT; yOffset++ ) { //32x32tilemap
		for (unsigned char xOffset = 0; xOffset < WIDTH; xOffset++ ){

			unsigned char x = xOffset;
			unsigned char y = yOffset;

			unsigned char xTile = x/Tile::WIDTH;
			unsigned char yTile = y/Tile::HEIGHT;
			//TODO Magic Number
			unsigned int address = windowTileMap+xTile+yTile*(32);

			auto tileID = memoryMap.byte(address).val();
			auto pixel = getTilePixel(tileID, x%Tile::WIDTH, y%Tile::HEIGHT);
			
			auto xTotal = wx+x;
			auto yTotal = wy+y;
			if (pixel && xTotal < WIDTH && xTotal >= 0 && yTotal < HEIGHT && yTotal >= 0){
					drawPixel(pixel, xTotal, yTotal);
			}
		}
	}
}

const unsigned int MAX_SPRITES = 40;

void GPU::renderSprites(){
	auto lcdc = memoryMap.byte(LCDC);
	//if sprite not enabled dont draw
	if (lcdc.getBit(1) == 0){
		return;
	}
	auto address = SPRITE_OAM;
	for (unsigned int sprite = 0; sprite < MAX_SPRITES; sprite++){
		unsigned char yStart = memoryMap.byte(address++).val()-16;
		unsigned char xStart = memoryMap.byte(address++).val()-8;
		auto tileID = memoryMap.byte(address++).val();
		//TODO: implement all of these 
		auto attributes = memoryMap.byte(address++);

		bool xflip = attributes.getBit(5); 
		bool yflip = attributes.getBit(6);

		for (auto y = 0; y < Tile::HEIGHT; y++){
			for (auto x = 0; x < Tile::WIDTH; x++){

				auto pixel = getTilePixel(	tileID, 
											xflip ? (Tile::WIDTH - x - 1) : x, 
											yflip ? (Tile::HEIGHT - y - 1) : y );
				auto xTotal = xStart+x;
				auto yTotal = yStart+y;
				//Checks if pixel is not 0 (transparent) and that it is in range of screen
				if (pixel && xTotal < WIDTH && xTotal >= 0 && yTotal < HEIGHT && yTotal >= 0){
					drawPixel(pixel, xTotal, yTotal);
				}
			}
		}
		
		
	}

}

//TODO: Move to Window class
void GPU::drawPixel(unsigned char pixel, unsigned char x, unsigned char y){
	
	auto pos = x+y*WIDTH;
	framebuffer[pos] = pixel;

	auto firstPos = 4*pos;
	switch(pixel){
		case 3:
			/*R*/framebufferSF[firstPos] = 0;
			/*G*/framebufferSF[firstPos+1] = 0;
			/*B*/framebufferSF[firstPos+2] = 0;
			/*A*/framebufferSF[firstPos+3] = 255;
			break;
		case 2:
			/*R*/framebufferSF[firstPos] = 80;
			/*G*/framebufferSF[firstPos+1] = 80;
			/*B*/framebufferSF[firstPos+2] = 80;
			/*A*/framebufferSF[firstPos+3] = 255;
			break;
		case 1:
			/*R*/framebufferSF[firstPos] = 180;
			/*G*/framebufferSF[firstPos+1] = 180;
			/*B*/framebufferSF[firstPos+2] = 180;
			/*A*/framebufferSF[firstPos+3] = 255;
			break;
		case 0:
			/*R*/framebufferSF[firstPos] = 250;
			/*G*/framebufferSF[firstPos+1] = 255;
			/*B*/framebufferSF[firstPos+2] = 255;
			/*A*/framebufferSF[firstPos+3] = 255;
			break;
	} 
}