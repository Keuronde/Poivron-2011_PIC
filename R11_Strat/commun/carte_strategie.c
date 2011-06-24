#include <p18cxxx.h>
#include "carte_strategie.h"

void init_io(){
// Toutes les pattes en digital (pas d'analogique)
    ADCON1 |= 0x0F;
// Config entrée ou sortie
    TRIS_RELAIS = 0; // sortie
    TRIS_LED_OK = 0; // sortie
	TRIS_LED_OK1 = 0; // sortie
    TRIS_TIRETTE = 1; // entrée
    TRIS_COULEUR = 1; // entrée
    TRIS_LED_CMUCAM = 0; // sortie
    TRIS_LED_ROUGE = 0; // sortie
    TRIS_LED_BLEUE = 0; // sortie
    TRIS_RESERVE1 = 1; // entrée
    TRIS_ABSENCE_PION = 1; // entrée
    TRIS_BOOT = 1; // entrée
    

// config des valeurs des sorties
    RELAIS = 0;
    LED_OK = 0;
	LED_OK1 = 0;
    LED_CMUCAM = 0;
    LED_ROUGE = 0;
    LED_BLEUE = 0;

}
