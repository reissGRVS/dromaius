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

		//Should maybe change this to be graphics platform agnostic
		void initialiseTileMapData();
		void frameBuffer();
		void renderBackground();
		std::array<sf::Sprite, 32 * 32> backgroundTiles;
	private:
		sf::Uint8 * getTile(unsigned char tileID);
		MemoryMap& memoryMap;
		//Image of height 8 width NO_TILES*8 (3072 atm)
		
		std::array<sf::Uint8, NO_TILES * Tile::HEIGHT * Tile::WIDTH * 4> tileMapData;
		sf::Texture tileMap;


		Mode mode = Mode::OAM_SEARCH;

		Ticks tickCount = 0;
		const Ticks ticksPerOamSearch = 80;
		const Ticks ticksPerPixelTransfer = 172;
		const Ticks ticksPerHBlank = 204;
		const Ticks ticksPerLine = ticksPerHBlank + ticksPerOamSearch + ticksPerPixelTransfer;
};


