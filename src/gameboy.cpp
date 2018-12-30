#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include "memorylocs.h"


Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap)
{

	sf::RenderWindow window(sf::VideoMode(256,256), "Dromaius");
	sf::View view;
	view.setSize(sf::Vector2f(160, 144));

	spdlog::get("console")->info("Powering up Gameboy");
	Ticks tickTotal = 0;
    while (1)
    {
		auto ticks = cpu.process();
		tickTotal += ticks;
		gpu.process(ticks);
		timer.process(ticks);
		
		//TODO: Change the location of this, maybe make a callback function for gpu to call
		//This is each frame (~60Hz) 
		if (tickTotal > (456*154)){
			tickTotal = 0;
			gpu.initialiseTileMapData();
			gpu.renderBackground();
			auto scx = memoryMap.byte(SCX).val();
			auto scy = memoryMap.byte(SCY).val();
			view.setCenter(scx+80,scy+72);
			window.clear();
			window.setView(view);
			for (auto tile : gpu.backgroundTiles){
				window.draw(tile);
			}
			window.display();

		}
    }  
	
}