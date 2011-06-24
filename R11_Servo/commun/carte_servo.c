#include <p18cxxx.h>
#include "carte_servo.h"

void init_io(){
// AN0 à AN3 en analogique,
// Le reste en numérique
    ADCON1 |= 0x0B;
// Config entrée ou sortie
    TRIS_SERVO1 = 0; // sortie
    TRIS_SERVO2 = 0; // sortie
    TRIS_SERVO3 = 0; // sortie
    TRIS_SERVO4 = 0; // sortie
    TRIS_SERVO5 = 0; // sortie
    TRIS_SERVO_SUP1 = 0; // sortie
    TRIS_SERVO_SUP2 = 0; // sortie
    TRIS_CRE_SENS = 0; // sortie
    TRIS_CRE_ENABLE = 0; // sortie
    TRIS_BOOT = 1; // Entrée
    
    TRIS_CDE_LED_BLEUES = 0; // sortie
    TRIS_CDE_LED_ROUGES = 0; // sortie
    

// config des valeurs des sorties
    CRE_SENS = 1;
    CRE_ENABLE = 1;
    LED_OK = 0;

}
