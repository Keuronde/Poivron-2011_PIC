#include <p18cxxx.h>
#include "carte_moteurs.h"

void init_io(){
// Toutes les pattes en digital (pas d'analogique)
    ADCON1 |= 0x0F;
// Config entrée ou sortie
    TRIS_P_RESET = 0; // sortie
    TRIS_P_HALF = 0;
    TRIS_P_CLOCK = 0;
    TRIS_P_SENS = 0;
    TRIS_P_CONTROL = 0;
    TRIS_P_ENABLE = 0;
    
    TRIS_M_SENS_1 = 0;
    TRIS_M_SENS_2 = 0;
    TRIS_M_ENABLE = 0;
    
    TRIS_CT1 = 0;
    TRIS_CT2 = 1;
    TRIS_CT3 = 1;
    TRIS_CT4 = 1;
    TRIS_CT5 = 1;
    TRIS_CT6 = 1;

// config des valeurs des sorties

    LED_OK = 0;

}
