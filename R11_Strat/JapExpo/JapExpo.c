#include <p18cxxx.h>
#include <delays.h>
#include <timers.h>
#include "../commun/carte_strategie.h"
#include "../i2c_m/i2c_m.h"
#include "../i2c_moteurs/i2c_moteurs.h"
#include "../i2c_servo/i2c_servo.h"
#include "../WMP/WMP.h"
#include "../CMUcam/cmucam.h"

/** T Y P E   P R I V E S ****************************************************/

enum etat_strategie_t {
    INIT=0,
    INIT_1,
    SORTIR_DEPART_INIT,
    SORTIR_DEPART,
    RECHERCHE_PION_INIT,
    RECHERCHE_PION_1_OU_2_INIT,
    RECHERCHE_PION_ATTENTE,
    AVANCE_CASE_1,
    AVANCE_CASE_2,
    AVANCE_CASE_3,
    POINT_FIXE_1,
    POINT_FIXE_2,
    POINT_FIXE_3,
    POINT_FIXE_4,
    POINT_FIXE_5,
    _45DEGRES,
    ATTENTE_45DEGRES,
    TEMPO_45,
    ALLER_VERS_PION,
    TOURNER,
    ALLER_VERS_CASE,
    ALLER_VERS_CASE_1,
    ALLER_VERS_CASE_2,
    ALLER_VERS_CASE_3,
    ALLER_VERS_CASE_4,
    ALLER_VERS_CASE_5,
    ALLER_VERS_CASE_6,
    PARTIR_CASE_1,
    PARTIR_CASE_2,
    PARTIR_CASE_3,
    PARTIR_CASE_4,
    PARTIR_CASE_5,
    PARTIR_CASE_6,
    PION2,
    VERS_HAUT_1,
    VERS_HAUT_2,
    SORTIR_CASE,
    EVITEMENT_RECULE,
    TEST_SERVO_1,
    TEST_SERVO_2_1,
    TEST_SERVO_2_2,
    TEST_SERVO_2_3,
    TEST_LECTURE_MOTEUR_1,
    TEST_ACQUITTEMENT_1,
    TEST_ACQUITTEMENT_2,
    TEST_ACQUITTEMENT_3,
    TEST_SERVO_CMUCAM_1,
    TEST_SERVO_CMUCAM_2,
    TEST_SERVO_CMUCAM_3,
    TEST_SERVO_CMUCAM_4,
    TEST_ABSENCE_PION,
    TEST_ABSENCE_PION_INIT,
    TEST_ABSENCE_PION_1,
    TEST_ABSENCE_PION_2,
    TEST_ABSENCE_PION_3,
    TEST_ABSENCE_PION_4,
    TEST_DROIT_1,
    TEST_DROIT_2,
    TEST_LIAISON,
	TEST_RELAIS_1,
	TEST_RELAIS_2,
	TEST_RELAIS_3,
	TEST_RELAIS_4,
	TEST_SONIC,
	TEST_PIED_1,
	TEST_PIED_2,
	TEST_PIED_3,
	TEST_PAS_1,
	TEST_PAS_2,
	TEST_PAS_3,
	TEST_PAS_4,
	TEST_PAS_5,
	TEST_PAS_6,
	TEST_PAS_7,
	TEST_PAS_8,
	TEST_CMUCAM_1,
	TEST_CMUCAM_2,
	TEST_ASSER_1,
	TEST_ASSER_2,
	TEST_ASSER_3,
	TEST_ALLERRETOUR_1,
	TEST_ALLERRETOUR_2,
	TEST_ALLERRETOUR_3,
	TEST_ALLERRETOUR_4
};

enum etat_asser_t {
	AVANCE_DROIT_INIT=0,
	AVANCE_DROIT=2,
	AVANCE_DROIT_TEMPO,
	TOURNE_INIT,
	TOURNE_TEMPO,
	TOURNE,
	FIN_TOURNE,
	TOURNE_VERS_AVANCE,
	TOURNE_VERS_AVANCE_2,
	AVANCE_VERS_TOURNE,
	FIN_ASSER
};



/** V A R I A B L E S ********************************************************/
// Non initialisées
#pragma udata
volatile unsigned char timer; 
volatile int nb3ms; 
unsigned char mTimer;
long consigne_angle; // Pour l'asservissement

// Initialisées
#pragma idata
char asser_actif=0; 					// Pour l'asservissement
enum etat_asser_t etat_asser=FIN_ASSER; // Pour l'asservissement



/** P R I V A T E  P R O T O T Y P E S ***************************************/
void MyInterrupt(void);
void Init(void);
char getTimer(void);
void active_asser(char avance_droit, long _angle);
void desactive_asser(void);
char fin_asser(void);





#define MOTEUR 1
#define SERVO 2
#define ROUGE 3
#define BLEU 1

#define DEBUG_SONIC 0
#define DEBUG_CC_GAUCHE 0
#define DEBUG_CC_DROIT 0
#define DEBUG_CC_AVANT 0
#define DEBUG_CC_DEUX 1
#define DEBUG_WMP 0


#define PION_1_XMIN (unsigned int)52
#define PION_1_XMAX (unsigned int)180
#define PION_1_YMIN (unsigned int)132
#define PION_1_YMAX (unsigned int)234
#define PION_1_XMILIEU 116
#define PION_1_YMILIEU 183

#define PION_2_XMIN (unsigned int)88
#define PION_2_XMAX (unsigned int)194
#define PION_2_YMIN (unsigned int)242
#define PION_2_YMAX (unsigned int)284

#define CMUCAM_MILIEU_X (int) 176
#define FACTEUR_CMUCAM_ANGLE (int) 2800

// 5 degrés
#define SEUIL_ANGLE_LENT  (long) 100000
// 1 degrés
#define SEUIL_ANGLE_ARRET (long)  20000

#define ASSER_TOURNE 0
#define ASSER_AVANCE 1


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
	
	// Compteur de temps : 1 incrément toutes les 3 ms
   	if(INTCONbits.TMR0IF == 1){
		INTCONbits.TMR0IF = 0;
//		WriteTimer0(65535 - 36000); //pour un préscaler de 1 : 12000 = 1ms
									//pour un préscaler de 32 : 375 = 1 ms
		WriteTimer0(65535 - 48000); //pour un préscaler de 1 : 12000 = 1ms
		timer++;
		
		if(timer == 255){
		    nb3ms--; 
		}
		
	}
	
	gestion_i2c();
	CMUcam_int();

	PRODL = sauv1;
	PRODH = sauv2;		

}


#pragma code



void main(void){
    char chaine[NB_DATA_IN]; // Reception CMUcam
    char etat =0;
    
    char tempo=0;
    char cmucam_active=0;
    
    char sens =1;
    unsigned char id_pion_1=255;
    unsigned char id_pion_2=255;
    long _5degres = 100000;
    long _3degres =  60000;
    long angle;
    
    int consigne_pap=0;
    int consigne_pap_I=0;
    int consigne_pap_P=0;
    char DernierEnvoi=0;
	char couleur;
	char essai_cmucam;
	char nb_essai_cmucam;
	char tempo_cmucam=0;
	char ignore_pion = 0;
	unsigned char id_forme;
	char cmucam_perdu=0;
	char a_lacher=0;
	char evitement =0;
	char tempo_e=20;

    figure_t mFigure;
    
	
    enum etat_cmucam_t etat_cmucam=INIT;
    enum etat_strategie_t etat_strategie=INIT, old_etat_strategie;
//    enum etat_strategie_t etat_strategie=TEST_ABSENCE_PION_INIT, old_etat_strategie;
    
    
    chaine[0]=0;
    chaine[1]=0;
    chaine[2]=0;
    chaine[3]=0;
    chaine[4]=0;
    chaine[5]=0;
    chaine[6]=0;
    chaine[7]=0;
    

    Init();

	while(nb3ms>0){
	    char timer;
	    int tempo_s;
	    char i,j;

	    /*********************************
	    *                                *
	    * Actualisation de l'orientation *
	    *                                *
	    *********************************/
	    
	    // Attente que les 4ms précédentes se soient bien écoulées. (A21)
        
        while(mTimer == getTimer());
        // Calculer et récupérer l'angle du gyroscope (A22)
        mTimer =getTimer(); // On récupère le numéro (0 à 255) du pas de temps pour le gyroscope
        
        WMP_calcul(mTimer); // On actualise l'angle
        angle = WMP_get_Angle(); // Récupérer l'angle du gyrosocpe
        
        /**************************
        *                         *
        * Gestion de la stratégie *
        *                         *
        **************************/
        GetDonneesServo();
        
        switch (etat_strategie){
        	case INIT :
        		GetDonneesServo();
        		if(get_CC_Gauche() == get_CC_Droit()){
        			if(get_CC_Gauche() == BLEU || get_CC_Gauche() == ROUGE){
        				couleur = get_CC_Gauche();
        				if(couleur==ROUGE){
        					LED_ROUGE=1;
        				}else{
        					LED_BLEUE=1;
        				}
        				etat_strategie=INIT_1;
        			}
        		}
        		break;
        	
            case INIT_1:
            	SetCremaillere(HAUT);
				SetServoPAv(HAUT);
				SetServoPArG(BAS);
				SetServoPArD(BAS);
            	RELAIS=1;
            	LED_OK=0;
            	LED_OK1=0;
            	etat_strategie = SORTIR_DEPART_INIT;
            	break;
        	case SORTIR_DEPART_INIT:
        		active_asser(ASSER_AVANCE,0);
        		tempo_s=0;
        		etat_strategie=SORTIR_DEPART;
        		break;
        		
        	case SORTIR_DEPART:
        		tempo_s++;
        		if(tempo_s == 300){ // 1200 ms
					desactive_asser();
        			prop_stop();
        			etat_strategie=RECHERCHE_PION_INIT;
        		}
        		break;

            case RECHERCHE_PION_INIT:
            	setCouleur('P');
				etat_cmucam=INIT;
            	cmucam_active=1;
            	tempo_s=0;
            	etat_strategie=RECHERCHE_PION_ATTENTE;
            	break;
            	
            case RECHERCHE_PION_ATTENTE:

// ### Ce bloque semble bizarre , le robot est à l'arrêt !
            	if(!ABSENCE_PION){
            		etat_strategie = ALLER_VERS_PION;
            		tempo_s=0;
            	}
// ### Fin bloque suspect

				tempo_s++;
				if(tempo_s > 250){
					etat_cmucam = CMUCAM_RESET;
            		etat_strategie=_45DEGRES;
            		tempo_s=0;
				}
            	if(etat_cmucam == TRACKING){ // La caméra a activé l'asservissement
            		etat_strategie = ALLER_VERS_PION;
            		tempo_s=0;
            	}
            	if(etat_cmucam == PERDU){
            		etat_cmucam = CMUCAM_RESET;
            		etat_strategie=_45DEGRES;
            		tempo_s=0;
            	}
            	break;
            	
            case ALLER_VERS_PION:
            	
				if(!ABSENCE_PION){
					tempo_s++;
					if(tempo_s > 60){  // 0.25s
						prop_stop();
						asser_actif=0;
						etat_cmucam=CMUCAM_RESET;
						etat_strategie=TOURNER;
						
						LED_OK=1;
						LED_OK1=1;
						LED_CMUCAM=1;

						tempo_s=0;
						SetServoPAv(BAS);
					}
				}else{
					if(etat_cmucam == PERDU){
						tempo_s=0;
						etat_strategie = TEMPO_45;
					}
				}
				break;
				
			case TEMPO_45:
				tempo_s++;
				if(tempo_s > 500){
					etat_strategie= _45DEGRES;
				}
				if(!ABSENCE_PION){
					etat_strategie = ALLER_VERS_PION;
					tempo_s=0;
				}
				break;
				
			case _45DEGRES:
				if(couleur == ROUGE){
					consigne_angle = 900000;
				}else{
					consigne_angle = -900000;
				}
				etat_asser=0;
				asser_actif=1;
				etat_strategie=ATTENTE_45DEGRES;
				tempo_s=0;
				break;

			case ATTENTE_45DEGRES:
				tempo_s++;
				if(tempo_s > 400){
					prop_stop();
					asser_actif =0;
					etat_asser = 0;
					etat_strategie = RECHERCHE_PION_INIT;
				}
				break;
				
			case TOURNER:
				if(tempo_s==0){
					if(couleur == ROUGE){
						consigne_angle=1800000;
					}else{
						consigne_angle=-1800000;
					}
					active_asser(ASSER_TOURNE,consigne_angle);
					tempo_s++;
				}
				
				if(fin_asser()){
					prop_stop();
					etat_strategie=ALLER_VERS_CASE_1;
					LED_OK=0;
					LED_OK1=0;
					LED_CMUCAM=0;
				}
				break;
			case ALLER_VERS_CASE_1:
				if(couleur == ROUGE){
					setCouleur('R');
				}else{
					setCouleur('B');
				}
				etat_cmucam=INIT;
				cmucam_active=1;
				etat_strategie=ALLER_VERS_CASE_2;

				break;
			case ALLER_VERS_CASE_2:
				if(etat_cmucam==TRACKING){
					active_asser(ASSER_TOURNE,consigne_angle);
					etat_strategie=ALLER_VERS_CASE_3;
					tempo_s=0;
				}
				break;
			case ALLER_VERS_CASE_3:
				if(tempo_s == 0){
					if(fin_asser()){
						active_asser(ASSER_AVANCE,consigne_angle);
						tempo_s++;
					}
				}
				if(tempo_s != 0){
					if(etat_cmucam == TRACKING_PROCHE){
						etat_strategie = ALLER_VERS_CASE_4;
					}
				}
				
				break;
			case ALLER_VERS_CASE_4:
				if(get_CC_Gauche() == couleur && get_CC_Droit() == couleur && get_CC_Avant() == couleur){
					SetServoPArG(HAUT);
					SetServoPArD(HAUT);
					SetServoPAv(HAUT);
					prop_stop();
					etat_cmucam=CMUCAM_RESET;
					etat_strategie=PARTIR_CASE_1;
				}
				break;

			case PARTIR_CASE_1:
				active_asser(ASSER_TOURNE,0);
				etat_strategie=PARTIR_CASE_2;
				break;

			case PARTIR_CASE_2:
				if(fin_asser()){
					etat_strategie=PARTIR_CASE_3;
					setCouleur('P');
					etat_cmucam=INIT;
					cmucam_active=1;
				}
				break;

			case PARTIR_CASE_3:
				if(etat_cmucam == TRACKING || etat_cmucam == TRACKING_PROCHE){
					active_asser(ASSER_AVANCE,consigne_angle);
					if(ABSENCE_PION){
						etat_strategie=PARTIR_CASE_4;
					}else{
						etat_strategie=PARTIR_CASE_5;
					}
				}
				break;
			case PARTIR_CASE_4:
				if(!ABSENCE_PION){
					etat_strategie = PARTIR_CASE_5;
				}
				break;
			case PARTIR_CASE_5:
				if(ABSENCE_PION){
					tempo_s = 0;
					etat_strategie = PARTIR_CASE_6;
				}
				break;

			case PARTIR_CASE_6:
				tempo_s++;
				if(tempo_s >125){
					SetServoPArG(BAS);
					SetServoPArD(BAS);
					etat_strategie = PION2;
				}
				break;
				
			case PION2:
				if(!ABSENCE_PION){
					tempo_s++;
					if(tempo_s > 60){  // 0.25s
						prop_stop();
						desactive_asser();
						etat_cmucam=CMUCAM_RESET;
						etat_strategie=VERS_HAUT_1;
						
						LED_OK=1;
						LED_OK1=1;
						LED_CMUCAM=1;

						tempo_s=0;
						SetServoPAv(BAS);
					}
				}else{
					if(etat_cmucam == PERDU){
						// Redémarrer la caméra et chercher un pion ?
						// Tourner un peu avant ?
						//tempo_s=0;
						//etat_strategie = TEMPO_45;
					}
				}
				break;
			
			case VERS_HAUT_1:
				GetDonneesMoteurs();
				if(tempo_s==0){
					// Adapter la consigne de l'angle en fonction de l'orientation
					// Pour trouver la rotation la plus courte
					if(couleur == ROUGE){
						consigne_angle=-1800000;
					}else{
						consigne_angle=1800000;
					}
					active_asser(ASSER_AVANCE,consigne_angle);
					tempo_s++;
				}
				if(get_CT_AV_G()){
					prop_stop();
					etat_strategie = VERS_HAUT_2;
				}
				break;
				
			case AVANCE_CASE_1:
				GetDonneesServo();
				ignore_pion = 1;
				if(get_CC_Droit() == get_CC_Gauche()){
					if(couleur == BLEU){
						if(get_CC_Droit() ==ROUGE){
							tempo_s++;
						}else{
							tempo_s=0;
						}
					}else{
						if(get_CC_Droit() ==BLEU){
							tempo_s++;
						}else{
							tempo_s=0;
						}
					}
				}else{
					tempo_s =0;
				}
				if(tempo_s > 100){ // 400 ms sur la bonne couleur
					tempo_s = 0;
					etat_strategie = AVANCE_CASE_2;
				}
				break;
			case AVANCE_CASE_2:
				GetDonneesServo();
				if(get_CC_Droit() == get_CC_Gauche()){
					if(couleur == get_CC_Droit()){
						tempo_s++;
					}else{
						tempo_s=0;
					}
				}else{
					tempo_s =0;
				}
				if(tempo_s > 100){ // 400 ms sur la bonne couleur
					tempo_s=0;
					etat_strategie = AVANCE_CASE_3;
				}
				break;
			case AVANCE_CASE_3:
				GetDonneesServo();
				if(get_CC_Droit() == get_CC_Gauche()){
					if(couleur == BLEU){
						if(get_CC_Droit() ==ROUGE){
							tempo_s++;
						}else{
							tempo_s=0;
						}
					}else{
						if(get_CC_Droit() ==BLEU){
							tempo_s++;
						}else{
							tempo_s=0;
						}
					}
				}else{
					tempo_s =0;
				}
				if(tempo_s > 1){ // avant, un délais de 400 ms sur la bonne couleur (> 100)
					tempo_s = 0;
					etat_strategie = POINT_FIXE_1;
					asser_actif=0;
					etat_asser=0;
					ignore_pion = 0;
					//prop_stop();
				}
				break;
			case POINT_FIXE_1:
				// Tenir compte de la couleur ### XXX
				GetDonneesServo();
				pap_set_pos(170);
				if(angle > 1200000){ // 60° 
					prop_stop();
					etat_strategie = POINT_FIXE_2;
					tempo_s =0;
				}/* 	
				if(cmucam_active==1){
					// Si on voit un pion, on va vers le pion
					if(etat_cmucam == TRACKING || etat_cmucam == TRACKING_PROCHE){
						etat_asser=0;
						asser_actif=1;
						etat_strategie = ALLER_VERS_PION;
					}
				}*/
				break;
			case POINT_FIXE_2:
				asser_actif=0;
				if(couleur==ROUGE){
					pap_set_pos(170);
				}else{
					pap_set_pos(-170);
				}
				tempo_s = 0;
				etat_strategie = POINT_FIXE_3;
				break;
			case POINT_FIXE_3:
				tempo_s++;
				if(tempo_s > 500){
					tempo_s=0;
					etat_strategie = POINT_FIXE_4;
				}
				break;
			case POINT_FIXE_4:
				pap_set_pos(0);
				etat_cmucam=INIT;
				cmucam_active=1;
				etat_strategie = POINT_FIXE_5;
				break;
			case POINT_FIXE_5:
				GetDonneesServo();
				if(etat_cmucam == TRACKING || etat_cmucam == TRACKING_PROCHE){
					etat_asser = 0;
					asser_actif=1;
					prop_set_vitesse(0);
					etat_strategie = ALLER_VERS_PION;
				}
				break;


			case EVITEMENT_RECULE:
				tempo_s++;
				Recule();
				if(tempo_s > 250){
					etat_strategie= old_etat_strategie;
				}
				
				break;
			
		
			
			
			
			
			case ALLER_VERS_CASE_5:
				Avance();
				if(angle> 1750000){
					prop_stop();
				}
				break;
				
			/*	
			case ALLER_VERS_CASE:
				GetDonneesServo();
				if(! cmucam_active){
					tempo_s++;
					if(tempo_s>200){
					LED_OK=1;
					etat_cmucam=INIT_CASE;
					cmucam_active=1;
					}
				}
				if(etat_cmucam==TRACKING){
					LED_OK1=1;
					etat_asser=0;
					asser_actif=1;
				}
				if(etat_cmucam==TRACKING_PROCHE){
					LED_CMUCAM=1;
					if(couleur == BLEU){
						if(get_CC_Droit() == BLEU && get_CC_Gauche() == BLEU ){
							prop_stop();
							asser_actif=0;
							etat_cmucam=CMUCAM_RESET;							
							etat_strategie=SORTIR_CASE;
						}
					}
					if(couleur == ROUGE){
						if(get_CC_Droit() == ROUGE && get_CC_Gauche() == ROUGE ){
							prop_stop();
							asser_actif=0;
							etat_cmucam=CMUCAM_RESET;
							etat_strategie=SORTIR_CASE;							
						}
					}
				}
				break;*/
			case SORTIR_CASE:
				GetDonneesServo();
				SetServoPArG(HAUT);
				SetServoPArD(HAUT);
				RELAIS=0;
				break;
            case TEST_SERVO_1:
                GetDonneesServo();
                SetServoPArG(BAS);
                SetServoPArD(BAS);
                if(get_CC_Droit() == 1){
                    LED_BLEUE=1;
                }else{
                    LED_BLEUE=0;
                }
                if(get_CC_Droit() == 3){
                    LED_ROUGE=1;
                }else{
                    LED_ROUGE=0;
                }
                break;
            case TEST_SERVO_2_1 :
                LED_ROUGE =1;
                SetServoPArG(HAUT);
                SetServoPArD(HAUT);
                SetServoPAv(HAUT);
				RELAIS=1;
                etat_strategie = TEST_SERVO_2_2;
                tempo_s = 0;
                break;
            case TEST_SERVO_2_2 :
                LED_BLEUE =1;
                tempo_s++;
                if(tempo_s > 1000){
                    SetServoPArG(BAS);
                    SetServoPArD(BAS);
                    SetServoPAv(BAS);
					LED_CMUCAM=0;
                    etat_strategie = TEST_SERVO_2_3;
                } 
                break;
            case TEST_SERVO_2_3 :
                LED_ROUGE =0;
                break;
            case TEST_LECTURE_MOTEUR_1:
                break;
            case TEST_ACQUITTEMENT_1:
                Avance();
                etat_asser =1;
                etat_strategie = TEST_ACQUITTEMENT_2;
                LED_OK1=1;
                break;    
            case TEST_ACQUITTEMENT_2:
                LED_OK1=0;
                LED_OK=1;
                GetDonneesMoteurs();
                if(get_CT_AV_G()){
                    ignore_contacteur();
                    etat_strategie = TEST_ACQUITTEMENT_3;
                }
                break;
            case TEST_ACQUITTEMENT_3:
                LED_OK1=1;
                LED_OK=1;
                break;
            case TEST_SERVO_CMUCAM_1:
                SetServoCMUcam(1);
                tempo_s=0;
                etat_strategie = TEST_SERVO_CMUCAM_2;
                break;
            case TEST_SERVO_CMUCAM_2:
                tempo_s++;
                if(tempo_s > 300){
                    etat_strategie = TEST_SERVO_CMUCAM_3;
                    tempo_s=0;
                }
                break;
            case TEST_SERVO_CMUCAM_3:
                SetServoCMUcam(0);
                tempo_s=0;
                etat_strategie = TEST_SERVO_CMUCAM_4;
                break;
            case TEST_SERVO_CMUCAM_4:
                tempo_s++;
                if(tempo_s > 300){
                    etat_strategie = TEST_SERVO_CMUCAM_1;
                    tempo_s=0;
                }
                break;
            case TEST_DROIT_1:
            	etat_asser = 0;
                asser_actif=1;
                consigne_angle=0;
                etat_strategie = TEST_DROIT_2;
                break;
            case TEST_DROIT_2:
                break;
            case TEST_LIAISON:
                GetDonneesServo();
                GetDonneesMoteurs();
                SetServoPAv(BAS);
                break;
			case TEST_SONIC:
				// Si on a recu qqch de la carte moteur
				if(get_capteur_sonique_proche()){
				    LED_OK =1;
				}else{
				    LED_OK =0;
				}
				
				if(get_capteur_sonique_loin()){
				    LED_OK1 =1;
				}else{
				    LED_OK1 =0;
				}
				etat_asser=0;
				break;
			case TEST_PIED_1:
				RELAIS=1;
				tempo_s++;
                if(tempo_s > 1000){
                    etat_strategie = TEST_PIED_2;
					SetCremaillere(BAS);
                    tempo_s=0;
                }
				break;
			case TEST_PIED_2:
				tempo_s++;
				if(tempo_s > 1000){
                    etat_strategie = TEST_PIED_1;
					SetCremaillere(HAUT);
                    tempo_s=0;
                }
				break;break;
			case TEST_PAS_1:
				asser_actif=0;
				pap_set_pos(180);
				tempo_s = 0;
				etat_strategie = TEST_PAS_2;
				break;
			case TEST_PAS_2:
				tempo_s++;
				pap_set_pos(180);
				if(tempo_s > 500){
					tempo_s=0;
					etat_strategie = TEST_PAS_3;
				}
				break;
			case TEST_PAS_3:
				pap_set_pos(0);
				GetDonneesServo();
				etat_strategie = TEST_PAS_4;
				tempo_s = 0;
				break;
			case TEST_PAS_4:
				tempo_s++;
				pap_set_pos(0);
				if(tempo_s > 500){
					tempo_s=0;
					etat_strategie = TEST_PAS_1;
				}
				break;
				
			case TEST_PAS_5:
				consigne_angle = 0;
				etat_asser = 0;
				asser_actif = 1;
				tempo_s=0;
				etat_strategie = TEST_PAS_6;
				break;
			case TEST_PAS_6:
				tempo_s++;
				if(tempo_s > 1000){
					tempo_s=0;
					asser_actif = 0;
					etat_strategie = TEST_PAS_7;
				}
				break;
			case TEST_PAS_7:
				pap_set_pos(115); // Rotation par rapport au centre
//				pap_set_pos((int)-285); // Rotation par rapport au centre
				etat_strategie = TEST_PAS_8;
				break;
				
			case TEST_CMUCAM_1:
				etat_cmucam=INIT;
				cmucam_active=1;
				etat_strategie = TEST_CMUCAM_2;
				break;
			case TEST_ABSENCE_PION:
				LED_OK=ABSENCE_PION;
				LED_OK1=ABSENCE_PION;
				LED_CMUCAM=ABSENCE_PION;
				break;
			case TEST_ABSENCE_PION_INIT:
				SetServoPArD(BAS);
				SetServoPArG(BAS);
				SetServoPAv(HAUT);
				etat_strategie = TEST_ABSENCE_PION_1;
				break;
			case TEST_ABSENCE_PION_1:
				if(! ABSENCE_PION){
					SetServoPAv(BAS);
					etat_strategie = TEST_ABSENCE_PION_2;
				}
				break;
			case TEST_ABSENCE_PION_2:
				if(! BOOT){
					SetServoPArD(HAUT);
					SetServoPArG(HAUT);
					SetServoPAv(HAUT);
					etat_strategie = TEST_ABSENCE_PION_3;
				}
				break;
			case TEST_ABSENCE_PION_3:
				if(ABSENCE_PION){
					tempo_s = 0;
					etat_strategie = TEST_ABSENCE_PION_4;
				}
				break;
			case TEST_ABSENCE_PION_4:
				tempo_s++;
				if(tempo_s > 1000){
					tempo_s = 0;
					etat_strategie = TEST_ABSENCE_PION_INIT;
				}
				break;
			case TEST_ASSER_1:
				// On donne une consigne de 90°
				active_asser(ASSER_TOURNE,1800000);
				/*consigne_angle = (long)-1800000; // -90 * 20000
				etat_asser = TOURNE_INIT;
				asser_actif = 1;*/
				etat_strategie = TEST_ASSER_2;
				break;
			case TEST_ASSER_2:
				if(fin_asser()){
					active_asser(ASSER_TOURNE,-1800000);
					etat_strategie = TEST_ASSER_3;
				}
				break;
			case TEST_ASSER_3:
				if(fin_asser()){
					active_asser(ASSER_TOURNE,1800000);
					etat_strategie = TEST_ASSER_2;
				}
				break;
			case TEST_ALLERRETOUR_1:
				if(fin_asser()){
					active_asser(ASSER_AVANCE,0);
					tempo_s = 0;
					etat_strategie = TEST_ALLERRETOUR_2;
				}
				break;
			case TEST_ALLERRETOUR_2:
				tempo_s++;
				if(tempo_s > 750){
					active_asser(ASSER_TOURNE,3600000);
					etat_strategie = TEST_ALLERRETOUR_3;
				}
				break;
			case TEST_ALLERRETOUR_3:
				if(fin_asser()){
					active_asser(ASSER_AVANCE,3600000);
					etat_strategie = TEST_ALLERRETOUR_4;
					tempo_s=0;
				}
				break;
            case TEST_ALLERRETOUR_4:
				tempo_s++;
				if(tempo_s>750){
					active_asser(ASSER_TOURNE,0);
					etat_strategie = TEST_ALLERRETOUR_1;
				}
				break;
            default:
                break;
        }

        
        /***********************
        *                      *
        *  Affichages / DEBUG  *
        *                      *
        ***********************/
		{
			if(DEBUG_SONIC){
				if(get_capteur_sonique_proche()){
					LED_OK =1;
				}else{
					LED_OK =0;
				}
				
				if(get_capteur_sonique_loin()){
					LED_OK1 =1;
				}else{
					LED_OK1 =0;
				}
			}

			if(DEBUG_CC_DROIT){
				if(get_CC_Droit() == 1){
					LED_BLEUE=1;
				}else{
					LED_BLEUE=0;
				}
				if(get_CC_Droit() == 3){
					LED_ROUGE=1;
				}else{
					LED_ROUGE=0;
				}
			}


			if(DEBUG_CC_GAUCHE){
				if(get_CC_Gauche() == 1){
					LED_BLEUE=1;
				}else{
					LED_BLEUE=0;
				}
				if(get_CC_Gauche() == 3){
					LED_ROUGE=1;
				}else{
					LED_ROUGE=0;
				}
			}
			if(DEBUG_CC_AVANT){
				if(get_CC_Avant() == 1){
					LED_BLEUE=1;
				}else{
					LED_BLEUE=0;
				}
				if(get_CC_Avant() == 3){
					LED_ROUGE=1;
				}else{
					LED_ROUGE=0;
				}
			}
			if(DEBUG_CC_DEUX){
				if(get_CC_Gauche() == BLEU){
					LED_BLEUE=1;
				}else{
					LED_BLEUE=0;
				}
				if(get_CC_Droit() == BLEU){
					LED_ROUGE=1;
				}else{
					LED_ROUGE=0;
				}
			}

			if(DEBUG_WMP){
				if((angle <= _3degres) && (angle >= (0 - _3degres))){
					LED_OK1 = 1;
				}else{
					LED_OK1 = 0;
				}
			}


			if(evitement ==1){
				tempo_e--;
				if(tempo_e == 0){
					GetDonneesMoteurs();
					tempo_e=25;
				}
				if(get_CT_AV_G() || get_CT_AV_D() || get_capteur_sonique_proche()){
					
						old_etat_strategie = etat_strategie;
						etat_strategie = EVITEMENT_RECULE;
						evitement=0;
					
				}
			}
		}
        
        /***********************
        *                      *
        * Gestion de la CMUcam *
        *                      *
        ***********************/

        
        if(cmucam_active){
        	switch(etat_cmucam){
            case INIT:
                if(cherche_couleur()){
                    etat_cmucam=RECUP_ID_1;
                    tempo_cmucam=200;
                    nb_essai_cmucam = 10;
                }
                break;
            case RECUP_ID_1:
            	tempo_cmucam--;
                if(rec_cmucam(chaine)){
                	tempo_cmucam=200;
                	if(get_erreur_RC()){
                		etat_cmucam=RE_RECUP_ID_1;
		        		break;
		        	}else{
			            if(chaine[0]=='g'){
					        chaine_to_figure(chaine,&mFigure);
					        etat_cmucam=TEST_ID;
		                }
                    }
                }
                if(tempo_cmucam==0){
                	etat_cmucam=RE_RECUP_ID_1;
                }
                break;
            case TEST_ID:
            	if(mFigure.y1==0 && mFigure.x1==0){
					etat_cmucam=RE_RECUP_ID_1;
					
            	}else{
            		etat_cmucam = ENVOI_ID;
            		id_forme=mFigure.id;
            	}
                break;
            case RE_RECUP_ID_1:
            	if(nouvelle_recherche()){
            		etat_cmucam=RE_RECUP_ID_2;
            		tempo_cmucam=200;
            	}
            	break;
            case RE_RECUP_ID_2:
            	tempo_cmucam--;
            	if(rec_cmucam(chaine)){
	            	tempo_cmucam=200;
            		if(get_erreur_RC()){
		        		etat_cmucam=RE_RECUP_ID_1;
		        		break;
		        	}else{
			        	if(chaine[0]=='g'){
				    		chaine_to_figure(chaine,&mFigure);
				    		etat_cmucam=TEST_ID;
		        		}
            		}
            	}
            	if(tempo_cmucam == 0){
            		etat_cmucam=RE_RECUP_ID_1;
            	}
            	break;
            case ENVOI_ID:
            	if(TX_libre()){
            		if(select_figure(id_forme)){
            			etat_asser=0;
	            		etat_cmucam=TRACKING;
            		}
            	}
            	break;
            case TRACKING:
            case TRACKING_PROCHE:
            	get_erreur_RC();
	            if(rec_cmucam(chaine)){
		            if(chaine[0]=='t'){
		            	chaine_to_figure(chaine,&mFigure);
				    	if(mFigure.x1!=0 && mFigure.y1!=0){
				    		int milieu;
				    		
				    		if(mFigure.y1 <= 68){
								etat_cmucam = TRACKING_PROCHE;
							}

				    		
				    		asser_actif=1;
					        LED_CMUCAM =1;
					        milieu = mFigure.x1/2 + mFigure.x0/2;
					        consigne_angle = (long)angle + ((long)(CMUCAM_MILIEU_X - milieu) * (long)FACTEUR_CMUCAM_ANGLE);
					        /*
					        if(a_lacher==1){
							    if((mFigure.x1 +mFigure.x0) < (176+200)*2 && (mFigure.x1 +mFigure.x0) > (176-20)*2){
									etat_cmucam=EN_FACE;
									consigne_angle = (long)angle;
								}
				    		}*/
					        cmucam_perdu=0;
				        }else{
				        	cmucam_perdu++;
				        	if(cmucam_perdu>4){
				        		etat_cmucam=PERDU;
				        	}
				            LED_CMUCAM =0;
				        }
                	}
               	}
               	break;
           case CMUCAM_RESET:
		    	if(TX_libre()){
		    		if(cmucam_reset()){
			    		cmucam_active=0;
		    		}
		    	}
		    	break;
        	}
        }
        
        
        /******************************
        *                             *
        * Gestion de l'asservissement *
        *                             *
        ******************************/
        
        // Essayer d'avancer droit (A24)   
        if(asser_actif){   
		    switch (etat_asser){

		    // Donner l'ordre d'avancer au robot. (A241)
		    case AVANCE_DROIT_INIT:
		        Avance();			// Commencer à avancer
		        consigne_pap_I=0;	// On remet l'intégrateur à 0
		        etat_asser = AVANCE_DROIT; // Passe à la correction
		        break;

		    case AVANCE_DROIT:
		    // Corriger la trajectoire (A243)
		      // Mesurer la déviation angulaire
		      // Corriger la position de la roue motrice        
		    //            consigne_pap=(int)((long)angle/(long)24000); 
		        // Avec un correcteur PI
		        // On vérifie que l'on est pas à l'arrêt

				
				consigne_pap_P=(int)((long)(consigne_angle-angle)/(long)5000); // Pour un asservissement plus nerveux (anciennement 12000)

				
				// On s'occupe du terme intégrale que si on avance
				if( get_CT_AV_G() || get_CT_AV_D() || get_capteur_sonique_proche() ){
				}else if(get_capteur_sonique_proche()){
					consigne_pap_I=consigne_pap_I + consigne_pap_P/8;	
				}else{
					consigne_pap_I=consigne_pap_I + consigne_pap_P; // Consigne I
				}

				
				// Saturation de la commande intégrale
				if(consigne_pap_I > (int)720){
					consigne_pap_I = (int)720;
				}
				if(consigne_pap_I < (int)-720){
					consigne_pap_I = (int)-720;
				}
				consigne_pap = consigne_pap_P+ (consigne_pap_I / (int)8);
				// Saturation
				if(consigne_pap > 90){
					consigne_pap = 90;
				}
				if(consigne_pap < -90){
					consigne_pap = -90;
				}
				// Envoie de la commande à la carte moteur
				pap_set_pos(consigne_pap);
				    

		        etat_asser = 3; // Boucle sur la tempo
		        

		        break;
		        

		    case AVANCE_DROIT_TEMPO:
		    // Attendre 100 ms (A244)
		        tempo++;
		        if(tempo > 33){ // On a atteint les 100 ms, on revient à l'état précédent.
//		        	GetDonneesMoteurs();
		            etat_asser = 2;
		            tempo = 0;
		        }
		        break;
		    case TOURNE_INIT:
				prop_stop();
				pap_set_pos(PAP_MAX_ROT);
				etat_asser = TOURNE_TEMPO;
				break;
			case TOURNE_TEMPO:
				tempo ++;
				if(tempo > 100){
					if((consigne_angle-angle) > 0){
						Avance();
					}else{
						Recule();
					}
					etat_asser = TOURNE;
				}
				break;
			case TOURNE:
				if((consigne_angle-angle) < SEUIL_ANGLE_LENT && (consigne_angle-angle) > -SEUIL_ANGLE_LENT){
					prop_set_vitesse(0);
				}else{
					prop_set_vitesse(1);
				}
				if((consigne_angle-angle) < SEUIL_ANGLE_ARRET && (consigne_angle-angle) > -SEUIL_ANGLE_ARRET){
					prop_stop();
					etat_asser = FIN_TOURNE;
					tempo=0;
				}
				// Penser à ignorer le capteur sonique
				if( prop_get_sens_avant() ){
					if( get_capteur_sonique_loin() || get_capteur_sonique_proche()){
						if( !get_CT_AV_D() && !get_CT_AV_G() ){
							ignore_contacteur();
						}
					}
				}
				break;
			case TOURNE_VERS_AVANCE:
				pap_set_pos(PAP_DROIT);
				tempo = 0;
				etat_asser = TOURNE_VERS_AVANCE_2;
				break;
			case TOURNE_VERS_AVANCE_2:
				tempo++;
				if(tempo> 100){
					etat_asser = AVANCE_DROIT_INIT;
				}
				break;
			case FIN_TOURNE:
				break;
		    }// Fin Switch
		    
        }
        


        /*****************************
        *                            *
        * Gestion des communications *
        *                            *
        *****************************/



        if( GetEnvoiMoteurs() && DernierEnvoi != MOTEUR  ){
	        // Echange avec la carte des servos si nécessaire
            transmission_moteur();
            DernierEnvoi = MOTEUR;
        }else{
			// Echange avec la carte moteurs si nécessaire
            transmission_servo();
            DernierEnvoi = SERVO;
        }


        

	}// Fin While
	LED_OK=0;
	LED_OK1=0;
	LED_CMUCAM=0;
	LED_ROUGE=0;
	LED_BLEUE=0;
	RELAIS=0;
	prop_stop();
	transmission_moteur();
	while(1);
} // Fin main



void Init(){

    init_io();
    init_i2c(); // Active les interruptions
    
    // Séquence visuelle (A11)
    Delay10KTCYx(0);
    LED_BLEUE = 1;
    Delay10KTCYx(0);
    LED_BLEUE = 0;
    LED_ROUGE = 1;
    Delay10KTCYx(0);
    LED_ROUGE = 0;
    LED_CMUCAM = 1;
    Delay10KTCYx(0);
    LED_CMUCAM = 0;
    LED_OK = 1;
    Delay10KTCYx(0);
    LED_OK = 0;
    LED_OK1 = 1;
    Delay10KTCYx(0);
    LED_OK1 = 0;
    
	LED_ROUGE=1;
	
    LED_OK1 = 0;

    CMUcam_Init();


    
    // WMP
	if(WMP_init()){
        LED_OK = 1;
    }else{
        LED_OK = 0;
    }
    Delay10KTCYx(0);
    Delay10KTCYx(0);
    
    // Notre compteur qui va nous rythmer notre robot sur un cycle de 3 ms.
    // Les interruptions doivent être activées pour que le compteur foncitonne "tout seul"
	// Initialisation du Timer 0
	OpenTimer0(	TIMER_INT_ON &  // interruption ON
				T0_16BIT &		// Timer 0 en 16 bits
				T0_SOURCE_INT & // Source interne (Quartz + PLL)
				T0_PS_1_1);		// 1 cycle, 1 incrémentation
	// Et hop, le timer est lancé
	WriteTimer0(65535 - 48000); // dt * 4ms
    

    // SUITE WMP
    LED_OK1 = 0;
    mTimer = getTimer();
    while(WMP_calibration()){           // Tant que la calibration est en cours
        while(mTimer == getTimer());
        mTimer = getTimer();
    }

    // A décommenter pour avoir un WMP stable
/*    WMP_init_2();
   
    while(WMP_calibration()){           // Tant que la calibration est en cours
        while(mTimer == getTimer());
        mTimer = getTimer();
    }
*/    
    LED_OK1 = 0;
    LED_OK=0;
	LED_ROUGE=0;
	LED_CMUCAM=1;
	

    while(TIRETTE);
    LED_OK1 = 1;
    LED_OK=1;
    nb3ms=80; //80 -> environ 82 secondes en réalité, et 8 secondes de marge au cas où on tire la tirette avant l'initialisation complète

	pap_set_pos(0);
	transmission_moteur();
    
   	WMP_init_timer(getTimer());
	mTimer = getTimer();
}



char getTimer(void){
    return timer;
}

void active_asser(char avance_droit, long _angle){
	consigne_angle = _angle;
	if(avance_droit == ASSER_AVANCE){
		if(etat_asser == FIN_TOURNE || etat_asser == TOURNE){
			etat_asser = TOURNE_VERS_AVANCE;
		}else{
			etat_asser = AVANCE_DROIT_INIT;
		}
	}else{
		etat_asser = TOURNE_INIT;	
	}
	asser_actif=1;
}

void desactive_asser(void){
	asser_actif=0;
}

char fin_asser(){
	if(etat_asser == FIN_ASSER || etat_asser == FIN_TOURNE){
		return 1;
	}
	return 0;
}


