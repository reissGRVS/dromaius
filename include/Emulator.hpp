

class Emulator {
	public:
		Emulator() {}
		~Emulator() {}

		static void start(char* romFilename);
		static void reset();

	private:
		static void createWindow();
		static void loadROM(char* romFilename);
	
		static CPU cpu;
		static GPU gpu;
		static ROM rom;
		static Memory mem;
		static Display display;
		static Window * window;
		static xcb_connection_t * connection;

		static bool emuRunning;
		static Time emuStartTime;


}