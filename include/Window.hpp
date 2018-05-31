#pragma once
#include "PCH.hpp"
#include "Event.hpp"

/*
	@brief	Data required to create a window
*/
struct WindowCreateInfo
{
	WindowCreateInfo() : posX(0), posY(0) {}
	xcb_connection_t * connection;
	int width;
	int height;
	const char* title;

	int posX;
	int posY;
};

/*
	@brief	On screen window and its data. Owns Vulkan surface object. Owns event queue.
*/
class Window
{
public:
	Window() {}
	~Window() {}

	void create(WindowCreateInfo* c);
	void destroy();
	// Returns true if all messages have been processed
	bool processMessages();
	bool isWmDeleteWin(xcb_atom_t message);
	Event constructKeyEvent(u8 keyCode, Event::Type eventType);

	u32 resX;
	u32 resY;
	std::string windowName;
	EventQ eventQ;
	xcb_connection_t * connection;
	xcb_window_t window;
	xcb_screen_t * screen;
	xcb_atom_t wmProtocols;
	xcb_atom_t wmDeleteWin;
};