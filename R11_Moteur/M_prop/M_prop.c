#include <p18cxxx.h>
#include <pwm.h>
#include "M_prop.h"

// Fonctions privées
void _sens(char);

char vitesse;
char sens;

// Initialisation du Moteur
void M_prop_init(void){
    TRIS_M_SENS_1 = 0; // Sortie
    TRIS_M_SENS_2 = 0; // Sortie
    TRIS_M_ENABLE = 0; // Sortie
    // Ouverture du PWM
   	OpenPWM1(255);
	SetDCPWM1(0);
	// Stop 
	Stop();
}
void V_lent(){
    SetDCPWM1(750);
}
void V_rapide(){
    SetDCPWM1(0x03FF);
}
void V_stop(){
    SetDCPWM1(0x0000);
}
void Stop(){
    M_SENS_1 = 0;
    M_SENS_2 = 0;
    sens = AUCUN;
}
void Avance(){
    _sens(0);
}
    
void Recule(){
    _sens(1);
}

void _sens(char s){
    if(s == 0){
        M_SENS_1 = 0;
        M_SENS_2 = 1;
        sens = AVANT;
    }else{
        M_SENS_1 = 1;
        M_SENS_2 = 0;
        sens = ARRIERE;
    }
}

char get_Sens(void){
    return sens;
}

