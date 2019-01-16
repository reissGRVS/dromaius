#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include "memorylocs.h"
const uint8_t SCALE = 3;

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap), window(sf::VideoMode(WIDTH*SCALE,HEIGHT*SCALE), "Dromaius", sf::Style::Titlebar)
{
	memoryMap.setJoypadCallback(joypad.byteUpdate);

	//Window set up
	window.setFramerateLimit(60);
	
	background.create(WIDTH, HEIGHT);
	b.setTexture(background);
	b.scale(SCALE,SCALE);
	//This updates joypad, draws buffer to screen and helps with timing frames
	gpu.setDrawCallback(
		[this](){
			joypad.keyUpdate();
			background.update(gpu.framebufferSF.data());
			window.clear();
			window.draw(b);
			window.display();
		}
	);


	spdlog::get("console")->info("Powering up Gameboy");
    while (1)
    {
		auto ticks = cpu.process();
		gpu.process(ticks);
		timer.process(ticks);
    }  
	
}