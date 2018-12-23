#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap)
{

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
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
			exit(0);
		}
        // sf::Event event;
        // while (window.pollEvent(event))
        // {
        //     if (event.type == sf::Event::Closed)
        //         window.close();
        // }

        // window.clear();
        // window.display();
    }  
	
}