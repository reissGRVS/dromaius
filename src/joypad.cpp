#include "joypad.h"
#include <SFML/Window/Keyboard.hpp>

void Joypad::keyUpdate(){
		
	right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	up = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	a = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
	b = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
	select = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
	start = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
}

bool Joypad::right;
bool Joypad::left;
bool Joypad::up;
bool Joypad::down;
bool Joypad::a;
bool Joypad::b;
bool Joypad::select;
bool Joypad::start;