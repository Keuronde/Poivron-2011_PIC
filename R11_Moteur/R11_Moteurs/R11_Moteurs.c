#include <p18cxxx.h>
#include <delays.h>
#include "../commun/carte_moteurs.h"
#include "../PaP/PaP.h"
#include "../i2c_s/i2c_s.h"
#include "../M_prop/M_prop.h"
#include "../Temps/temps.h"
#include "../CapteurSonic/CapteurSonic.h"

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MyInterrupt(void);
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
    ;
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
	
    com_i2c();
    mPaP_int();
    Temps_Int();

	PRODL = sauv1;
	PRODH = sauv2;		

}


#pragma code

struct {
  unsigned ACQ_CT1:1;
  unsigned ACQ_CT2:1;
  unsigned ACQ_CT3:1;
  unsigned ACQ_CT4:1;
  unsigned ACQ_CT5:1;
  unsigned ACQ_CT6:1;
  unsigned SONIC_PROCHE:1;
  unsigned SONIC_LOIN:1;
} ACQ_CT;

struct {
  unsigned _CT1:1;
  unsigned _CT2:1;
  unsigned _CT3:1;
  unsigned _CT4:1;
  unsigned _CT5:1;
  unsigned _CT6:1;
  unsigned SONIC_PROCHE:1;
  unsigned SONIC_LOIN:1;
} CAPTEURS, A_ENVOYER,CAPTEURS_ENVOYES;


void main(void){
    unsigned char recu[2];
    unsigned char envoi;
    char valeur_sonic_loin[10];
    char valeur_sonic_proche[10];
    char CT_acquittement;
    char index_sonic=0;
    int consigne_recu=0;
    int consigne_recu_old=0;
    char vitesse=0;
    char sens;
    char vitesse_cde;
    char acquittement;
    char T_old;
    int  t_v0;
    
    Init();
    for(index_sonic = 0;index_sonic< 10; index_sonic++){
        valeur_sonic_loin[index_sonic] = 0;
        valeur_sonic_proche[index_sonic] = 0;
    }
    T_old = getTemps_cs();
    t_v0 = getTemps_cs();
// On allume les LEDs


    while(1){
        int i;
        char total_sonic_proche,total_sonic_loin;
        unsigned int distance;
        
        
        
        // Reception d'ordre
        if(rec_i2c(recu)){
//        	LED_OK = 1;

            // Consigne PaP
            consigne_recu = (int)recu[0] + (((int) recu[1] & 3) << 8);
            //if(consigne_recu != consigne_recu_old){
	            set_consigne(consigne_recu);
            //}
            consigne_recu_old = consigne_recu;
            // Sens M Propulsion
            sens = (recu[1] >> 2) & 0x03;
            
            
            if(sens == 0){
                Stop();
            }else{
                if(sens == 1){
                    Avance();
                }else{
                    Recule();
                }
            }
            
            
            // Vitess M_Propulsion
            vitesse_cde = (recu[1] >> 4) & 0x01;
            
            
            
            // Acquitement contacteur
            acquittement = (recu[1] >> 5) & 0x01;
            if(acquittement){
                if(CAPTEURS_ENVOYES._CT1)
                    ACQ_CT.ACQ_CT1 = 1;
                if(CAPTEURS_ENVOYES._CT2)
                    ACQ_CT.ACQ_CT2 = 1;
                if(CAPTEURS_ENVOYES._CT3)
                    ACQ_CT.ACQ_CT3 = 1;    
                if(CAPTEURS_ENVOYES._CT4)
                    ACQ_CT.ACQ_CT4 = 1;
                if(CAPTEURS_ENVOYES._CT5)
                    ACQ_CT.ACQ_CT5 = 1;
                if(CAPTEURS_ENVOYES._CT6)
                    ACQ_CT.ACQ_CT6 = 1;
                if(CAPTEURS_ENVOYES.SONIC_PROCHE)
                    ACQ_CT.SONIC_PROCHE = 1;
                  
            }
            // Si les état capteurs ont bien été envoyés
            if(envoi_ok()){
                CAPTEURS_ENVOYES = A_ENVOYER;
            }
            
        }
        
        // Lecture 
        if(!CT1)
            ACQ_CT.ACQ_CT1 = 0;
        if(!CT2)
            ACQ_CT.ACQ_CT2 = 0;
        if(!CT3)
            ACQ_CT.ACQ_CT3 = 0;    
        if(!CT4)
            ACQ_CT.ACQ_CT4 = 0;
        if(!CT5)
            ACQ_CT.ACQ_CT5 = 0;
        if(!CT6)
            ACQ_CT.ACQ_CT6 = 0;
        if(!CAPTEURS.SONIC_PROCHE)
            ACQ_CT.SONIC_PROCHE =0;
        
        // Gestion de la vitesse  
        // Vitesse commandées
        if(vitesse_cde){
				vitesse = 2;
        }else{
            vitesse = 1;
        }
        
        if( get_Sens() == AVANT){
            // Ralentissement du au capteur sonique
            if(CAPTEURS.SONIC_LOIN){
                vitesse = 1;
            }
            // Arrêt du au capteur sonique
            if(CAPTEURS.SONIC_PROCHE == 1 && ACQ_CT.SONIC_PROCHE == 0){
                vitesse = 0;
            }
            // Test des contacteurs avant
            if(CT1 == 1 && ACQ_CT.ACQ_CT1 == 0){
                vitesse = 0;
            }
            if(CT2 == 1 && ACQ_CT.ACQ_CT2 == 0){
                vitesse = 0;
            }
        }else if(get_Sens() == ARRIERE){
            // Test des contacteurs arriere
            if(CT3 == 1 && ACQ_CT.ACQ_CT3 == 0){
                vitesse = 0;
            }
            if(CT4 == 1 && ACQ_CT.ACQ_CT4 == 0){
                vitesse = 0;
            }
            if(CT5 == 1 && ACQ_CT.ACQ_CT5 == 0){
                vitesse = 0;
            }
        }
        
        // Si le robot est sensé avancer, on ajuste la vitesse
        switch(vitesse){
        case 2:
            V_rapide();
            break;
            
        case 1:
            V_lent();
            break;
            
        case 0:
        default :
            V_stop();
            break;
        }
        
        // Si le robot est à l'arrêt
        // Soit à cause d'un ordre, soit à cause d'un obstacle
        // On note l'heure afin de connaître le temps écoulé depuis le dernier arrêt
        if(get_Sens() == AUCUN || vitesse == 0){
			t_v0=getTemps_cs();
		}
        
        
        // Lecture du capteur sonique
        // Ne pas lire le capteur sonique si la denière lecture date de moins de 50ms.
        // XXX Rajouter une condition ici
        // Quelque chose comme :
        //if(getTemps_cs() > T_old + 5){
            //T_old = getTemps_cs();
            
            distance = getDistance();
            
            if(index_sonic == 10){
                index_sonic = 0;
            }

            if(distance < _45_CM ){
                valeur_sonic_loin[index_sonic]=1;
            }else{
                valeur_sonic_loin[index_sonic]=0;
            }
            
            if(distance < _25_CM ){
                valeur_sonic_proche[index_sonic]=1;
            }else{
                valeur_sonic_proche[index_sonic]=0;
            }
            
            
            total_sonic_proche = 0;
            total_sonic_loin = 0;
            for(i=0;i<10;i++){
                total_sonic_proche += valeur_sonic_proche[i];
                total_sonic_loin += valeur_sonic_loin[i];
            }
            if(total_sonic_proche > 2){
                CAPTEURS.SONIC_PROCHE = 1;
//                LED_OK =1;
            }else{
                CAPTEURS.SONIC_PROCHE = 0;
//                LED_OK =0;
            }
            
            if(total_sonic_loin > 2){
                CAPTEURS.SONIC_LOIN = 1;
                // LED_OK =1;
            }else{
                CAPTEURS.SONIC_LOIN = 0;
                // LED_OK =0;
            }
            
            index_sonic++;
        //}
        // Fin Capteur Sonique
        
		// CapteurSonique désactivé
        //CAPTEURS.SONIC_LOIN = 0;
        //CAPTEURS.SONIC_PROCHE = 0;
        // Envoi des valeurs 
        CAPTEURS._CT1 =CT1;
        CAPTEURS._CT2 =CT2;
        CAPTEURS._CT3 =CT3;
        CAPTEURS._CT4 =CT4;
        CAPTEURS._CT5 =CT5;
        CAPTEURS._CT6 =CT6;
        if(envoi_i2c((char *) &CAPTEURS)){
            A_ENVOYER = CAPTEURS;
        }
        
    }

}


void Init(){
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
    init_io();
    init_i2c(0x31); // Les interruptions doivent être activées pour que l'I2C foncitonne	
    MoteurPInit();
    M_prop_init();
    Temps_Init();

    A_ENVOYER._CT1=0;
    A_ENVOYER._CT2=0;
    A_ENVOYER._CT3=0;
    A_ENVOYER._CT4=0;
    A_ENVOYER._CT5=0;
    A_ENVOYER._CT6=0;
    A_ENVOYER.SONIC_PROCHE=0;
    A_ENVOYER.SONIC_LOIN=0;
    
    CAPTEURS_ENVOYES._CT1=0;
    CAPTEURS_ENVOYES._CT2=0;
    CAPTEURS_ENVOYES._CT3=0;
    CAPTEURS_ENVOYES._CT4=0;
    CAPTEURS_ENVOYES._CT5=0;
    CAPTEURS_ENVOYES._CT6=0;
    CAPTEURS_ENVOYES.SONIC_PROCHE=0;
    CAPTEURS_ENVOYES.SONIC_LOIN=0;

    LED_OK = 1;
    Delay10KTCYx(0);
    LED_OK = 0;
    Delay10KTCYx(0);
    LED_OK = 1;
    Delay10KTCYx(0);
    LED_OK = 0;
    Delay10KTCYx(0);
    LED_OK = 1;
    Delay10KTCYx(0);
    LED_OK = 0;
    Delay10KTCYx(0);
    TRIS_CT1 = 1; // entrée
    


}












