#include "Emulator.hpp"

/*
	@brief	Entry point
*/
int main(int argc, char* argv[])
{
	char *filename = NULL;
	printf("argc = %d\n", argc);
	int i;
	if (argc) {
		filename = argv[argc-1];
	}

	if(filename != NULL){
		Emulator::start(filename);
	}
	else
	{
		DBG_SEVERE("No ROM filename")
		return 1;
	}
	
}