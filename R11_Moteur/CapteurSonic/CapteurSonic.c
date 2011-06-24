/** I N C L U D E S **********************************************************/ 
#include <delays.h>

#include <p18cxxx.h>
#include "CapteurSonic.h"
#include "../Temps/temps.h"

/** V A R I A B L E S ********************************************************/
unsigned int distance_cs, distance_micro_s;

unsigned int getDistance(){
	// On emet une impulsion 
	TRIS_SONIC = 0; // Patte en sortie
	SONIC = 1;    // Etat haut

	Delay10TCYx(12);
	// On arrete l'impulsion
	SONIC = 0;    // Etat bas
	// On écoute le port
	TRIS_SONIC = 1; // Patte en entrée
	Delay1TCY();
	// On attend que le capteur commence son echelon
	while(SONIC == 0);

	// On compte le temps pendant lequel le capteur attend
	distance_cs = getTemps_cs();
	distance_micro_s = getTemps_micro_s();

	// On attends que l'echellon se termine
	while(SONIC == 1);

	distance_micro_s = getTemps_micro_s() - distance_micro_s ;
	if(distance_cs != getTemps_cs()){
		distance_micro_s = 10000 * (getTemps_cs()-distance_cs)+ distance_micro_s;
	}
	return (unsigned int) (distance_micro_s);
}

unsigned int getDistance_mm(){
	// On emet une impulsion 
	TRIS_SONIC = 0; // Patte en sortie
	SONIC = 1;    // Etat haut

	Delay10TCYx(12);
	// On arrete l'impulsion
	SONIC = 0;    // Etat bas
	// On écoute le port
	TRIS_SONIC = 1; // Patte en entrée
	Delay1TCY();
	// On attend que le capteur commence son echelon
	while(SONIC == 0);

	// On compte le temps pendant lequel le capteur attend
	distance_cs = getTemps_cs();
	distance_micro_s = getTemps_micro_s();

	// On attends que l'echellon se termine
	while(SONIC == 1);

	distance_micro_s = getTemps_micro_s() - distance_micro_s ;
	if(distance_cs != getTemps_cs()){
		distance_micro_s = 10000 * (getTemps_cs()-distance_cs)+ distance_micro_s;
	}
	return (unsigned int) (distance_micro_s / 6.4);
}
