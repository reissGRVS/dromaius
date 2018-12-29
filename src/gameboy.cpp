#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include <chrono>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap)
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