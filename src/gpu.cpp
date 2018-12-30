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

const unsigned int VRAM_TILE_START = 0x8000;

void GPU::initialiseTileMapData(){
	auto address = VRAM_TILE_START; 
	for (auto tile = 0; tile < NO_TILES; tile++){
		unsigned int tileStart = tile*Tile::WIDTH*4;
		for (auto row = 0; row < Tile::HEIGHT; row++){
			auto b1 = memoryMap.byte(address++);
			auto b2 = memoryMap.byte(address++);
			for (auto column = 0; column < Tile::WIDTH; column++){
				//Figure out colour
				auto b1Bit = b1.getBit(Tile::WIDTH - column -1);
				auto b2Bit = b2.getBit(Tile::WIDTH - column -1);
				auto colourCode = (b2Bit << 1) + b1Bit;
				//Add to the tileMapData
				auto firstPos = tileStart + (row*(NO_TILES*Tile::WIDTH) + column)*4;
				switch(colourCode){
					case 0:
						/*R*/tileMapData[firstPos] = 0;
						/*G*/tileMapData[firstPos+1] = 0;
						/*B*/tileMapData[firstPos+2] = 0;
						/*A*/tileMapData[firstPos+3] = 255;
						break;
					case 1:
						/*R*/tileMapData[firstPos] = 255;
						/*G*/tileMapData[firstPos+1] = 255;
						/*B*/tileMapData[firstPos+2] = 255;
						/*A*/tileMapData[firstPos+3] = 255;
						break;
					case 2:
						/*R*/tileMapData[firstPos] = 255;
						/*G*/tileMapData[firstPos+1] = 255;
						/*B*/tileMapData[firstPos+2] = 255;
						/*A*/tileMapData[firstPos+3] = 255;
						break;
					case 3:
						/*R*/tileMapData[firstPos] = 250;
						/*G*/tileMapData[firstPos+1] = 255;
						/*B*/tileMapData[firstPos+2] = 255;
						/*A*/tileMapData[firstPos+3] = 255;
						break;
				} 
			}

		}
	}
	tileMap.create(NO_TILES*8, 8);
	tileMap.update(tileMapData.data());
	tileMap.copyToImage().saveToFile("tilemap.png");
}

sf::Uint8 * GPU::getTile(unsigned char tileID){
	return tileMapData.data() + (tileID*4*Tile::WIDTH);
}

const unsigned int BACKGROUND_TILE_MAP = 0x9800;
void GPU::renderBackground(){
	//get background map
	unsigned int address = BACKGROUND_TILE_MAP;
	for (int y = 0; y < 32; y++ ) { //32x32tilemap
		for (int x = 0; x < 32; x++ ){
			auto tileID = memoryMap.byte(address++).val();
			backgroundTiles[x+y*32].setTexture(tileMap);
			backgroundTiles[x+y*32].setTextureRect(sf::IntRect(tileID*8,0, 8, 8));
			backgroundTiles[x+y*32].setPosition(sf::Vector2f(x*8.f, y*8.f));
		}
	}
	
}

void GPU::frameBuffer(){
	//Add background
	renderBackground();
	//Add window

	//Add sprites
}