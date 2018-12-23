#include "gpu.h"
#include "memorylocs.h"
#include "binOps.h"

GPU::GPU(MemoryMap& gameboyMemory) :
	memoryMap(gameboyMemory)
{
}

void GPU::process(){
	//This gets us past the boot sequence waiting for interrupt
	auto byteLY = memoryMap.byte(LY);
	byteLY++;
	if (byteLY > 154){
		byteLY = 0;
	}
}

const unsigned int VRAM_TILE_START = 0x8000;

void GPU::initialiseTileMapData(){
	auto address = VRAM_TILE_START; 
	for (auto tile = 0; tile < NO_TILES; tile++){
		unsigned int tileStart = tile*Tile::WIDTH*4;
		for (auto row = 0; row < Tile::HEIGHT; row++){
			auto b1 = memoryMap.byte(address++).val();
			auto b2 = memoryMap.byte(address++).val();
			for (auto column = 0; column < Tile::WIDTH; column++){
				//Figure out colour
				auto b1Bit = getBitValue(b1, Tile::WIDTH - column -1);
				auto b2Bit = getBitValue(b2, Tile::WIDTH - column -1);
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
	tileMap.create(NO_TILES*8, 8, tileMapData.data());
	tileMap.saveToFile("tilemap.png");
}