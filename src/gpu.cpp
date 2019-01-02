#include "gpu.h"
#include "memorylocs.h"
#include "bytes/byte.h"

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
	auto statLCDC = memoryMap.byte(STAT);
	switch(mode){
		case Mode::H_BLANK:
			//H-Blank interrupt
			if (statLCDC.getBit(3)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCDC.setBit(0,0);
			statLCDC.setBit(1,0);
			break;
		case Mode::V_BLANK:
			//V-Blank interrupt
			if (statLCDC.getBit(4)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCDC.setBit(0,1);
			statLCDC.setBit(1,0);
			break;
		case Mode::OAM_SEARCH:
			//OAM interrupt
			if (statLCDC.getBit(5)){
				memoryMap.byte(IF).setBit(1, 1);
			}
			statLCDC.setBit(0,0);
			statLCDC.setBit(1,1);
			break;
		case Mode::PIXEL_TRANSFER:
			statLCDC.setBit(0,1);
			statLCDC.setBit(1,1);
			break;

	}
}


void GPU::initialiseTileMapData(){
	auto address = VRAM_TILE_START;
	const unsigned int rowSize = NO_TILES*Tile::WIDTH;
	for (auto tile = 0; tile < NO_TILES; tile++){
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
	for (auto x = 0; x < NO_TILES *Tile::WIDTH; x++){
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
	return tileMapData[tileID*Tile::WIDTH + x + y*NO_TILES*Tile::WIDTH];
}


const unsigned int BACKGROUND_TILE_MAP = 0x9800;
void GPU::renderBackground(){
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
			unsigned int address = BACKGROUND_TILE_MAP+xTile+yTile*(32);

			auto tileID = memoryMap.byte(address).val();

			auto pixel = getTilePixel(tileID, x%Tile::WIDTH, y%Tile::HEIGHT);
			auto pos = xOffset+yOffset*WIDTH;
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
	}
}

const unsigned int MAX_SPRITES = 40;

void GPU::renderSprites(){
	auto address = SPRITE_OAM;
	for (unsigned int sprite = 0; sprite < MAX_SPRITES; sprite++){
		unsigned char y = memoryMap.byte(address++).val()-16;
		// unsigned char x = memoryMap.byte(address++).val()-8;
		// auto tileID = memoryMap.byte(address++).val();
		// //TODO: implement all of these 
		// auto attributes = memoryMap.byte(address++);
		// //address++;

		// sprites[sprite].setTexture(tileMap);
		// sprites[sprite].setTextureRect(sf::IntRect(tileID*8,0, 8, 8));
		// sprites[sprite].setPosition(sf::Vector2f(scx+x, scy+y));
		// float xflip = attributes.getBit(5) ? -1 : 1; 
		// float yflip = attributes.getBit(6) ? -1 : 1;
		// sprites[sprite].setScale(xflip, yflip);
	}

}
