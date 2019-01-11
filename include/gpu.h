#pragma once
#include "forwarddecls.h"
#include "memorymap.h"
#include <stddef.h>
#include <array>
#include <SFML/Graphics.hpp>

template <class T, size_t ROW, size_t COL>
using Matrix = std::array<std::array<T, COL>, ROW>;

//Number of tiles represented from 0x8000-0x9000 7FF
const unsigned int NO_TILES = 256;
const unsigned char WIDTH = 160;
const unsigned char HEIGHT = 144;
class Tile {
	public:
		static const unsigned char HEIGHT = 8;
		static const unsigned char WIDTH = 8;
		Matrix<unsigned char, HEIGHT, WIDTH> img;
	
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
		void setMode(Mode m);
		void initialiseTileMapData();
		
		std::array<sf::Uint8, 4* NO_TILES*Tile::HEIGHT*Tile::WIDTH> tileMapDataSF;
		std::array<sf::Uint8, 4* WIDTH*HEIGHT> framebufferSF;
		void renderBackground();
		void renderWindow();
		void renderSprites();
		void exportTileMap();
	private:
		MemoryMap& memoryMap;
		
		unsigned char getTilePixel(unsigned char tileID, unsigned char x, unsigned char y);
		void drawPixel(unsigned char pixel, unsigned char x, unsigned char y);
		std::array<unsigned char, NO_TILES*Tile::HEIGHT*Tile::WIDTH> tileMapData;
		std::array<unsigned char, WIDTH*HEIGHT> framebuffer;

		Mode mode = Mode::OAM_SEARCH;
		Ticks tickCount = 0;
		const Ticks ticksPerOamSearch = 80;
		const Ticks ticksPerPixelTransfer = 172;
		const Ticks ticksPerHBlank = 204;
		const Ticks ticksPerLine = ticksPerHBlank + ticksPerOamSearch + ticksPerPixelTransfer;
};


