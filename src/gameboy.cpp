#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include "memorylocs.h"
const uint8_t SCALE = 3;

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap), window(sf::VideoMode(WIDTH*SCALE,HEIGHT*SCALE), "Dromaius", sf::Style::Resize)
{
	memoryMap.setJoypadCallback(joypad.byteUpdate);

	//Window set up
	background.create(WIDTH, HEIGHT);
	b.setTexture(background);
	b.scale(SCALE,SCALE);
	gpu.setDrawCallback(
		[this](){
			joypad.keyUpdate();
			gpu.initialiseTileMapData();
			gpu.renderBackground();
			gpu.renderWindow();
			gpu.renderSprites();
		
		
			background.update(gpu.framebufferSF.data());
			window.clear();
			window.draw(b);
			window.display();}
	);


	spdlog::get("console")->info("Powering up Gameboy");
    while (1)
    {
		auto ticks = cpu.process();
		gpu.process(ticks);
		timer.process(ticks);
    }  
	
}