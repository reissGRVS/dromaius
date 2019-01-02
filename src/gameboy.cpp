#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include "memorylocs.h"
#include <iostream>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap)
{
	memoryMap.setJoypadCallback(joypad.byteUpdate);


	sf::RenderWindow window(sf::VideoMode(WIDTH,HEIGHT), "Dromaius");
	sf::Texture background;
	background.create(WIDTH, HEIGHT);

	spdlog::get("console")->info("Powering up Gameboy");
	Ticks tickTotal = 0;
	//auto begin = std::chrono::high_resolution_clock::now();
    while (1)
    {
		auto ticks = cpu.process();
		tickTotal += ticks;
		gpu.process(ticks);
		timer.process(ticks);
		
		//TODO: Change the location of this, maybe make a callback function for gpu to call
		//This is each frame (~60Hz) 
		if (tickTotal > (456*154)){
			joypad.keyUpdate();
			tickTotal = 0;
			gpu.initialiseTileMapData();
			gpu.renderBackground();
			gpu.renderSprites();
			//gpu.exportTileMap();
			
			background.update(gpu.framebufferSF.data());
			sf::Sprite b;
			b.setTexture(background);
			window.clear();
			window.draw(b);
			window.display();

		}
    }  
	
}