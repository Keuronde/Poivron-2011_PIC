#include "PaP.h"
#include <timers.h>
#include <delays.h>
#include <p18cxxx.h>


volatile int pas_restant;
volatile int pos_actuelle;
volatile int pos_but;
volatile char depl_en_cours;
volatile int T_PAS = 200;

void set_vitesse(int t_pas){
    T_PAS = t_pas;
}
int get_pos(void){
    return pos_actuelle;
}
void setHalf(char _half){
    if(_half){
        HALF = 1;
    }else{
        HALF = 0;
    }
        
    
}

void mPaP_int(){
    if(INTCONbits.TMR0IF){
        // Réinitialisation de l'interruption
        INTCONbits.TMR0IF = 0;
        
        // On avance d'un pas
        Pas();
        
        // On ajuste nos positions
        pas_restant--;
        if(CW ==1){
            pos_actuelle++;
        }else{
            pos_actuelle--;
        }
        if(!HALF){         // Si on marche par pas complet, on passe un deuxieme demi-pas
            pas_restant--;
            if(CW ==1){
                pos_actuelle++;
            }else{
                pos_actuelle--;
            }
        }
        
        // Modulo 1 tour (on reste entre -400 et 400).
        if(pos_actuelle < (int)-400){
            pos_actuelle += (int)800;
        }else if(pos_actuelle > (int)400){
            pos_actuelle -= (int)800;
        }
        
        // On planifie le pas suivant
        if(pas_restant > 0){
            WriteTimer0(0xFFFF - T_PAS);
        }else{
            CloseTimer0();
            depl_en_cours=0;
        }
    }
}

void set_consigne(int consigne){

	ENABLE = 1;
// Consigne entre 400 et -400;
    while(consigne <= (int)-400){
        consigne += (int)800;
    }
    while(consigne > (int)400){
        consigne -= (int)800;
    }

    if(!depl_en_cours){
    // S'il n'y a pas de déplacement en cours
        
        pos_but = consigne;
        pas_restant = pos_but - pos_actuelle;
        depl_en_cours = 1;
        
        // Chemin le plus court
        while(pas_restant <= (int)-400){
            pas_restant += (int)800;
        }
        while(pas_restant > (int)400){
            pas_restant -= (int)800;
        }
            
        if(pas_restant > 0){
            CW =1;
        }else{
            CW =0;
            pas_restant = -pas_restant;
        }
        if(pas_restant !=0){
	        // On active le timer...
		    OpenTimer0(TIMER_INT_ON &
		           T0_16BIT &
		           T0_SOURCE_INT &
		           T0_PS_1_256);
            WriteTimer0(0xFFFF - T_PAS);
        }else{
            depl_en_cours=0;
        }

    }else{
        char sens;
    // Si le moteur était déjà en train de tourner, on lui laisse un peu de temps pour qu'il s'arrête avant de repartir dans l'autre sens
    
        if(pos_but - pos_actuelle > 0){
            sens =1;
        }else{
            sens = 0;
        }

        pos_but = consigne;
        pas_restant = pos_but - pos_actuelle;
        
        // Chemin le plus court
        while(pas_restant <= (int)-400){
            pas_restant += (int)800;
        }
        while(pas_restant > (int)400){
            pas_restant -= (int)800;
        }
        if( (pas_restant > 0 && sens ==1) || (pas_restant < 0 && sens ==0)){
            sens = 1; // On continue dans le même sens
//            if(pas_restant != 0)
//                WriteTimer0(0xFFFF -  T_PAS);

        }else{
            sens =0; // On change de sens
            if(pas_restant != 0)
                WriteTimer0(0xFFFF - (2 * T_PAS));
        }
           
        if(pas_restant > 0){
            CW =1;
        }else{
            CW =0;
            pas_restant = -pas_restant;
        }

        
    }

}

void MoteurPInit(){

	// Les pattes de contrôle en sortie
	TRIS_RESET = 0;
	TRIS_HALF = 0;
	TRIS_CLOCK = 0;
	TRIS_CW = 0;
	TRIS_CONTROLE = 0;
	TRIS_ENABLE = 0;

	RESET = 1;
	HALF = 0;
	CLOCK = 1;
	CW = 1; // On tourne dans le sens des aiquilles d'une montre.
	CONTROLE =0; // Parce que !
	// Le mettre à 0 permet de couper l'alimentation avec les pattes enable du L298. Ce sera plus simple à déboguer
	ENABLE = 0; // On active le composant.
	
	// Initialisation des variables d'état :
	depl_en_cours = 0;
	pos_actuelle = 0;
	pas_restant = 0;
	pos_but = 0;
	
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
}

void Pas(){
	CLOCK =0;
	// On attend 1 µs, 12 cycles
	Delay1TCY();
	Delay1TCY();
	Delay10TCYx(1);
	// On repasse à 1
	CLOCK =1;
}
