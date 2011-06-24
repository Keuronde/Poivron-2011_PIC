#include <p18cxxx.h>
#include <timers.h>
#include "../commun/carte_servo.h"
#include "../servo/servo.h"
#include "../i2c_s/i2c_s.h"

/** D E F I N E D ********************************************************/
#define ROUGE 0
#define BLEU 1
#define VIDE 2

/** V A R I A B L E S ********************************************************/
#pragma udata
volatile char timer;
/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MyInterrupt(void);
void MyInterrupt_L(void);
void Init(void);


/** V E C T O R  R E M A P P I N G *******************************************/

extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000808
void _high_ISR (void)
{
    MyInterrupt();
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000818
void _low_ISR (void)
{
    MyInterrupt_L();
}
#pragma code


#pragma interrupt MyInterrupt 
void MyInterrupt(void){
	// code de "Rustre Corner"
	// Adapté et modifié par S. KAY
	unsigned char sauv1;
	unsigned char sauv2;

	sauv1 = PRODL;
	sauv2 = PRODH;
	
	Servo_Int();

	
	if(INTCONbits.TMR0IF == 1){
		INTCONbits.TMR0IF = 0;
		WriteTimer0(65535 - 48000); //pour un préscaler de 1 : 12000 = 1ms
                                    // On est à 4 ms.
		timer++;
	}

	PRODL = sauv1;
	PRODH = sauv2;		

}

#pragma interrupt MyInterrupt_L
void MyInterrupt_L(void){
	unsigned char sauv1;
	unsigned char sauv2;

	sauv1 = PRODL;
	sauv2 = PRODH;

	com_i2c();

	PRODL = sauv1;
	PRODH = sauv2;		

}


#pragma code




void main(void){
    unsigned char recu[2],etat,envoi;
    unsigned char gauche_old=0,droit_old=0,gauche_tmp=0,droit_tmp=0,avant_old=0,avant_tmp=0;
    unsigned int capteur_gauche[4],capteur_droit[4],capteur_avant[4];
    Init();
    LED_OK = 0;
    etat = 0;
    
    
    while(1){
        // Si on a reçu une commande de la carte de stratégie
        if(rec_i2c(recu)){
            LED_OK = 1;
        
            // Code normal
            // Bits :
            // 0 : Servo Poussoir arrière G
            // 1 : Servo Poussoir arrière D
            // 2 : Servo Poussoir avant
            // 3 : Servo CMUCAM
            // 4 : Servo (réserve)
            // 5 : Crémaillère
            if(recu[0] & 0x01){
                Servo_Set(POUSSOIR_AR_G_HAUT);
            }else{
                Servo_Set(POUSSOIR_AR_G_BAS);
            }
            if(recu[0] & 0x02){
                Servo_Set(POUSSOIR_AR_D_HAUT);
            }else{
                Servo_Set(POUSSOIR_AR_D_BAS);
            }
            if(recu[0] & 0x04){
                Servo_Set(POUSSOIR_AV_HAUT);
            }else{
                Servo_Set(POUSSOIR_AV_BAS);
            }
            if(recu[0] & 0x08){
                Servo_Set(CMUCAM_HAUT);
            }else{
                Servo_Set(CMUCAM_BAS);
            }
            
            if(recu[0] & 0x20){
                CRE_SENS = 0;
            }else{
                CRE_SENS = 1;
            }
        }
        // Si on doit faire quelque chose pour les capteurs de couleur
        // test sur un Timer, toute les 40 ms.
        if(timer>10){
            unsigned int critere;


            //Acquisition - Capteur AVANT
		    ADCON0 = 8; // Choix de AN2
		    ADCON2 = 0b10101110; //  finalement Fosc/64 et 12TAD, résultat justifié à droite (anciennement 2TAD et Fosc/4)
		    ADCON0 = ADCON0 | 3; // On lance l'acquisition
		    while(ADCON0bits.GO == 1);
		    // Acquisition terminée
		    
		    capteur_avant[etat] = ((unsigned int)ADRESH<<8) | (unsigned int)ADRESL;

		    //Acquisition - Capteur DROIT
		    ADCON0 = 4; // Choix de AN1
		    ADCON2 = 0b10101110; //  finalement Fosc/64 et 12TAD, résultat justifié à droite (anciennement 2TAD et Fosc/4)
		    ADCON0 = ADCON0 | 3; // On lance l'acquisition
		    while(ADCON0bits.GO == 1);
		    // Acquisition terminée
		    
		    capteur_droit[etat] = ((unsigned int)ADRESH<<8) | (unsigned int)ADRESL;
		    
			
		    //Acquisition - Capteur Gauche
		    ADCON0 = 0; // Choix de AN0
		    ADCON2 = 0b10101110; //  finalement Fosc/64 et 12TAD, résultat justifié à droite (anciennement 2TAD et Fosc/4)
		    ADCON0 = ADCON0 | 3; // On lance l'acquisition
		    while(ADCON0bits.GO == 1);
		    // Acquisition terminée
		    
		    capteur_gauche[etat] = ((unsigned int)ADRESH<<8) | (unsigned int)ADRESL;
		    
		    switch(etat){
			    case 0 : // Rouge
				    CDE_LED_ROUGES = 0; // Rouge
				    CDE_LED_BLEUES = 1; // Bleue
				    etat++;
				    break;
			    case 1 : // Bleu
				    CDE_LED_ROUGES = 0; // Rouge
				    CDE_LED_BLEUES = 0; // Bleue
				    etat++;
				    break;
			    case 2 : // Vide
				    CDE_LED_ROUGES = 1; // Rouge
				    CDE_LED_BLEUES = 0; // Bleue
				    etat=0;
				    break;
		    }

		    if(etat == 0){
		        // Capteur gauche
		        if( (capteur_gauche[BLEU] - capteur_gauche[VIDE]) != 0){
		            critere = ((capteur_gauche[ROUGE]-capteur_gauche[VIDE])<<4)/(capteur_gauche[BLEU]-capteur_gauche[VIDE]);
		        }else{
		            critere = 3<<4;
		        }
		        
		        if(critere < 20){ // (anciennement 12 => critère < 0,75)
				    capteur_gauche[3] = 1; // Bleu
			    }else if(critere > 16){ // critère > 1,75 (26)
				    capteur_gauche[3] = 3; // Rouge
			    }else{
				    capteur_gauche[3] = 2; // Très probablement noir
			    }

			    /*
			    if(gauche_old != capteur_gauche[3]){
			        if(gauche_tmp == capteur_gauche[3]){
			            gauche_old = capteur_gauche[3];
			        }else{
    			        gauche_tmp = capteur_gauche[3];
    			        capteur_gauche[3] = gauche_old;
			        }
			    }*/
			    
			    
			    
			    // Capteur droit
		        if(capteur_droit[BLEU] - capteur_droit[VIDE] != 0){
		            critere = ((capteur_droit[ROUGE]-capteur_droit[VIDE])<<4)/(capteur_droit[BLEU]-capteur_droit[VIDE]);
		        }else{
		            critere = 3<<4;
		        }
		        
		        if(critere < 20){ // (anciennement 12 => critère < 0,75)
				    capteur_droit[3] = 1; // Bleu
			    }else if(critere > 16){ // critère > 1,7
				    capteur_droit[3] = 3; // Rouge
			    }else{
				    capteur_droit[3] = 2; // Très probablement noir
			    }
			    /*
			    if(droit_old != capteur_droit[3]){
			        if(droit_tmp == capteur_droit[3]){
			            droit_old = capteur_droit[3];
			        }else{
    			        droit_tmp = capteur_droit[3];
    			        capteur_droit[3] = droit_old;
			        }
			    }*/


			    // Capteur avant
		        if(capteur_avant[BLEU] - capteur_avant[VIDE] != 0){
		            critere = ((capteur_avant[ROUGE]-capteur_avant[VIDE])<<4)/(capteur_avant[BLEU]-capteur_avant[VIDE]);
		        }else{
		            critere = 3<<4;
		        }
		        
		        if(critere < 20){ // (anciennement 12 => critère < 0,75)
				    capteur_avant[3] = 1; // Bleu
			    }else if(critere > 16){ // critère > 1,7
				    capteur_avant[3] = 3; // Rouge
			    }else{
				    capteur_avant[3] = 2; // Très probablement noir
			    }
				
			    
			    // On prépare l'envoi
			    envoi = 0x00;
			    envoi |= (unsigned char)(capteur_gauche[3] & 0x0003) | ((unsigned char) (capteur_droit[3] & 0x0003) << 2) | ((unsigned char) (capteur_avant[3] & 0x0003) << 4);
			    envoi_i2c(&envoi);
		    }
		    timer = 0;
	    }

    }

}

void Init(){
    init_io();
    init_i2c(0x33);
    IPR1bits.SSPIP=0; // Priorité basse pour l'I2C
    Servo_Init();
   	INTCONbits.GIEH = 1; // Activation interruptions hautes
  	INTCONbits.GIEL = 1; // Activation interruptions basses
  	RCONbits.IPEN=1; // Activation des niveau d'interruptions
    // Initialisation du Timer 0 pour la base de temps
	OpenTimer0(	TIMER_INT_ON &  // interruption ON
				T0_16BIT &		// Timer 0 en 16 bits
				T0_SOURCE_INT & // Source interne (Quartz + PLL)
				T0_PS_1_1);		// 1 cycle, 1 incrémentation
    timer=0;
    Servo_Set(POUSSOIR_AR_D_BAS);
    Servo_Set(POUSSOIR_AR_G_BAS);
    Servo_Set(POUSSOIR_AV_HAUT);
}













