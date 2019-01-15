#include "timer.h"
#include "speeds.h"
#include "memorylocs.h"

Timer::Timer(MemoryMap& mem) :
	memoryMap(mem),
	tima(memoryMap.byte(TIMA)),
	tma(memoryMap.byte(TMA)),
	tac(memoryMap.byte(TAC)),
	div(memoryMap.byte(DIV))
	{}


void Timer::process(Ticks ticks){
	d+=ticks;
	if (d > DIV_TICKS){
		div++;
		d%=DIV_TICKS;
	}

	//If Timer is started
	if(tac.getBit(2)){
		t+=ticks;
		auto ticksPerClock = clock();
		if (t > ticksPerClock){
			uint16_t clockInc = t/ticksPerClock;
			t %= ticksPerClock;

			while(clockInc){
				clockInc--;
				if (tima == 0xFF){
					tima = tma;
					//Request Timer interrupt
					memoryMap.byte(IF).setBit(2,1);
				}
				else{
					tima++;
				}
			}
			
		}
	}
}

uint16_t Timer::clock(){
	uint8_t clockSelect = (tac.getBit(1) << 1) + tac.getBit(0);

	switch(clockSelect){
		case 0: //00
			return CPU_CLOCK/TAC_00;
		case 1: //01
			return CPU_CLOCK/TAC_01;
		case 2: //10
			return CPU_CLOCK/TAC_10;
		case 3: //11
			return CPU_CLOCK/TAC_11;
		default:
			spdlog::get("stderr")->critical("This should not happen, Timer::clock");
			exit(0);
	}
}