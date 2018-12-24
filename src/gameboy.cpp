#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap)
{

	
	spdlog::get("console")->info("Powering up Gameboy");
	Ticks sinceLY = 0;
    while (1)
    {
		sinceLY += cpu.process();
		//TODO: Remove this
		if (sinceLY > 450){
			sinceLY = 0;
			gpu.process();
		}

		if(memoryMap.bootRomEnabled() == false){

			
			gpu.initialiseTileMapData();
			gpu.renderBackground();
			sf::RenderTexture texture;
			texture.create(32*8,32*8);
			texture.clear();
			for (auto tile : gpu.backgroundTiles){
				texture.draw(tile);
				texture.display();
				texture.getTexture().copyToImage().saveToFile("background.png");
			}
			exit(0);
		}

    }  
	
}