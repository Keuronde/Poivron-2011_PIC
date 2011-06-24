#include <timers.h>
#include "temps.h"
#include <p18cxxx.h>

#define TIMER_INIT 50536

static unsigned int centisecondes;



void Temps_Init(){
	OpenTimer3( TIMER_INT_ON &
		T3_16BIT_RW &
		T3_SOURCE_INT &
		T3_PS_1_8 &
		T3_OSC1EN_OFF &
		T3_SYNC_EXT_OFF );
	centisecondes = 0;
}

void Temps_Int(){
	if(PIR2bits.TMR3IF){
		WriteTimer3(TIMER_INIT);
		PIR2bits.TMR3IF = 0; // On réarme le Timer3
		centisecondes++;
	}
}

unsigned int getTemps_cs(){
	return centisecondes;
}
unsigned char getTemps_s(){
	return (unsigned char) centisecondes/100;
}
unsigned int getTemps_micro_s(){
	return  (ReadTimer3() - TIMER_INIT) / 1.5;
}
