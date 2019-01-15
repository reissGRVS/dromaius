#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include "memorylocs.h"
#include <iostream>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap), window(sf::VideoMode(WIDTH,HEIGHT), "Dromaius")
{
	memoryMap.setJoypadCallback(joypad.byteUpdate);

	//Window set up
	background.create(WIDTH, HEIGHT);
	b.setTexture(background);

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
	Ticks tickTotal = 0;
	//auto begin = std::chrono::high_resolution_clock::now();
    while (1)
    {
		auto ticks = cpu.process();
		tickTotal += ticks;
		gpu.process(ticks);
		timer.process(ticks);
    }  
	
}