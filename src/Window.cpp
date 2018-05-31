#include "PCH.hpp"
#include "Emulator.hpp"
#include "Window.hpp"

void processEvent(xcb_generic_event_t *event);

/*
	@brief	Create a window surface for drawing to screen
*/
void Window::create(WindowCreateInfo * c)
{
	connection = c->connection;
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
	
	screen = iter.data;
	window = xcb_generate_id(connection);
 	uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  	uint32_t valueList[] = {screen->black_pixel, 	XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
													XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
													XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
													XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE};
													
	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, c->width, c->height,
						0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, eventMask, valueList);
	
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(c->title), c->title);
	
	//Sets window destroy
	xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
	xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
	xcb_intern_atom_reply_t *wmDeleteReply = xcb_intern_atom_reply(connection, wmDeleteCookie, NULL);
	xcb_intern_atom_reply_t *wmProtocolsReply = xcb_intern_atom_reply(connection, wmProtocolsCookie, NULL);
	wmDeleteWin = wmDeleteReply->atom;
	wmProtocols = wmProtocolsReply->atom;

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
						wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
	xcb_map_window(connection, window);
	xcb_flush(connection);

}

/*
	@brief	Destroy the window
*/
void Window::destroy()
{
	xcb_destroy_window(connection, window);
}

/*
	@brief	Process window related messages. 
	@note	This function should detect user input (keyboard, mouse)
*/
bool Window::processMessages()
{
	xcb_generic_event_t *event;
	while( event = xcb_poll_for_event(connection)){
		processEvent(event);
		free(event);
	}

	return false;

}

Event Window::constructKeyEvent(u8 keyCode, Event::Type eventType){
	auto check = [](u8 pKey) -> bool { return Keyboard::isKeyPressed(pKey); };
	Event newEvent(eventType); 
	newEvent.constructKey(keyCode, 
		check(Key::KC_RIGHT_SHIFT) || check(Key::KC_LEFT_SHIFT), 
		check(Key::KC_LEFT_ALT) || check(Key::KC_RIGHT_ALT), 
		check(Key::KC_RIGHT_SUPER) || check(Key::KC_LEFT_SUPER), 
		check(Key::KC_CAPS), 
		check(Key::KC_RIGHT_CTRL) || check(Key::KC_LEFT_CTRL));

	return newEvent;
}

void processEvent(xcb_generic_event_t *event){

	switch (event->response_type & ~0x80) {
		
		case XCB_CLIENT_MESSAGE: {
			if (Engine::window->isWmDeleteWin(((xcb_client_message_event_t *)event)->data.data32[0]))
				Engine::engineRunning = false;
			break;
		}
		
		case XCB_BUTTON_PRESS: {
			DBG_INFO("Button pressed");
			xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;

			switch (bp->detail) {
			case 4:
				//Wheel Button up in window
				break;
			case 5:
				//Wheel Button down in window
				break;
			default:
				//Button pressed in window
				break;
			}
			break;
		}
		case XCB_BUTTON_RELEASE: {
			DBG_INFO("Button released");
			//Button released in window
			xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;

			break;
		}
		case XCB_MOTION_NOTIFY: {
			//Mouse moved in window
			xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;

			break;
		}
		case XCB_ENTER_NOTIFY: {
			//Mouse entered window
			xcb_enter_notify_event_t *enter = (xcb_enter_notify_event_t *)event;

			break;
		}
		case XCB_LEAVE_NOTIFY: {
			//Mouse left window
			xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;

			break;
		}
		case XCB_KEY_PRESS: {
			DBG_INFO("Key pressed");
			
			//Key pressed in window
			/*xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
			Keyboard::keyState[kp->detail] = 1;
			Event keyEvent = Engine::window->constructKeyEvent(kp->detail, Event::KeyDown);
			Engine::window->eventQ.pushEvent(keyEvent);
			*/
			break;
		}
		case XCB_KEY_RELEASE: {
			DBG_INFO("Key released");
			//Key released in window
			/*
			xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
			Keyboard::keyState[kr->detail] = 0;
			Event keyEvent = Engine::window->constructKeyEvent(kr->detail, Event::KeyUp);
			Engine::window->eventQ.pushEvent(keyEvent);
			*/
			break;
		}
		default:
			break;
		}
}

bool Window::isWmDeleteWin(xcb_atom_t message){
	return wmDeleteWin == message;
}
