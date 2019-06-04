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
	t+=ticks;
	if (t >= TIMER_TICKS){
		t-=TIMER_TICKS;
		tock();
	}

}

void Timer::tock(){
	//increment clock for DIV and B
	d++;
	b++;

	if (d == (DIV_TICKS/TIMER_TICKS)){
		//increment DIV and reset DIV clock
		div++;
		d=0;
	}

	//If Timer is started
	if(tac.getBit(2)){
		auto timerTickPerClock = TIMER_CLOCK/TAC_CLOCK[tac & 0x3];
		//while base clock is past ticks required for timer tick
		while (b >= timerTickPerClock){
			b -= timerTickPerClock;

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