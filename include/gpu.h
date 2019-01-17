#pragma once
#include "forwarddecls.h"
#include "memorymap.h"
#include <stddef.h>
#include <array>
#include <SFML/Graphics.hpp>

template <class T, size_t ROW, size_t COL>
using Matrix = std::array<std::array<T, COL>, ROW>;

//Number of tiles represented from 0x8000-0x97FF
const uint16_t NO_TILES = 384;
const uint8_t WIDTH = 160;
const uint8_t HEIGHT = 144;
const uint8_t TILEMAP_WIDTH = 32;
const uint8_t TILEMAP_HEIGHT = 32;

class Tile {
	public:
		static const uint8_t HEIGHT = 8;
		static const uint8_t WIDTH = 8;
		Matrix<uint8_t, HEIGHT, WIDTH> img;
	
};

enum class Mode{
	OAM_SEARCH,
	PIXEL_TRANSFER,
	H_BLANK,
	V_BLANK
};

class GPU{
	public:
		GPU(MemoryMap& memoryMap);
		void process(Ticks ticks);
		void setDrawCallback(std::function<void()> drawCallback);
		
		std::array<sf::Uint8, 4* WIDTH*HEIGHT> framebufferSF;
		
		void exportTileMap();
	private:
		MemoryMap& memoryMap;

		//Pixel processing
		void initialiseTileMapData();
		void renderBackground(uint8_t yOffset);
		void renderWindow(uint8_t yOffset);
		void renderSprites();
		void renderSpriteTile(uint8_t tileID, uint8_t xStart, uint8_t yStart, Byte attributes);
		
		uint8_t getTilePixel(uint8_t tileID, uint8_t x, uint8_t y, bool sprite);
		void drawPixel(uint8_t pixel, uint8_t x, uint8_t y);

		std::array<uint8_t, NO_TILES*Tile::HEIGHT*Tile::WIDTH> tileMapData;
		std::array<uint8_t, WIDTH*HEIGHT> framebuffer;

		//Mode management
		void setMode(Mode m);

		Mode mode = Mode::OAM_SEARCH;
		Ticks tickCount = 0;
		const Ticks ticksPerOamSearch = 80;
		const Ticks ticksPerPixelTransfer = 172;
		const Ticks ticksPerHBlank = 204;
		const Ticks ticksPerLine = ticksPerHBlank + ticksPerOamSearch + ticksPerPixelTransfer;
		
		//Callback function for when buffer is ready to draw
		std::function<void()> draw;

		//Used for exportTileMap
		std::array<sf::Uint8, 4* NO_TILES*Tile::HEIGHT*Tile::WIDTH> tileMapDataSF;
};


