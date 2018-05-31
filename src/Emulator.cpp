#include "Emulator.hpp"
#include "PCH.hpp"
#include "Window.hpp"

void Emulator::start(char* filename) {
	DBG_INFO("Emulator starting");
	
	connection = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(connection))
		DBG_SEVERE("Failed to connect to X server using XCB");
	
	createWindow();


}



void Emulator::createWindow() {
	DBG_INFO("Creating window");
	WindowCreateInfo wci;
	wci.connection = connection;
	wci.height = 144;
	wci.width = 160;
	wci.posX = 100;
	wci.posY = 100;
	wci.title = "dromaius";

	window = new Window;
	window->create(&wci);
}

void Emulator::loadROM(char* romFilename) {

}

void Emulator::reset() {

}

void Emulator::quit() {

	window->destroy();

	return 0;
}

CPU Emulator::cpu;
GPU Emulator::gpu;
ROM Emulator::rom;
Memory Emulator::mem;
Display Emulator::display;
Window * Emulator::window;
xcb_connection_t * Engine::connection;

bool Emulator::emuRunning;
Time Emulator::emuStartTime;