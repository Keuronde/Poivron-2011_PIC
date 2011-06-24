/*****************************************************************************
*  Ce fichier avec i2c_m.h doit permettre à un PIC maitre I2C de d'envoyer   *
*  et de recevoir des informations à un périphérique esclave.                *
*  Hypothèses :                                                              *
*  La communication est toujours structurée de la même manière :             *
*    Le maitre envoi l'adresse de l'esclave pour une lecture                 *
*    Le maitre envoi un nombre d'octets prédéfini                            *
*    Le maitre envoi l'adresse de l'esclave pour une ecriture                *
*    Le maitre reçoit un nombre d'octets prédéfini                           *
*                                                                            *
******************************************************************************/

#include "i2c_m.h"
#include <p18cxxx.h>
#include <timers.h>
#include <string.h>
#include "../commun/carte_strategie.h"
//#include <delays.h>


#define NB_MAX 6

char data_out[NB_MAX];
volatile char data_in[NB_MAX];
char i2c_slave_adresse;
volatile char data_index;
volatile char nb_rec;
volatile char nb_env;
volatile char erreur_i2c;
volatile enum i2c_m_flag etat_i2c = I2C_LIBRE; // Etat de notre i2c


/* 
La fonction à qui on donne :
* L'adresse de l'esclave
* Le nombre d'octet à envoyer
* Le nombre d'octet à recevoir
* L'adresse de la chaine à envoyer
* L'adresse de la chaine recevant les données
Elle renvoie :
* 0 si l'i2c est occupé
* 1 si la transaction à bien pu s'initialiser
*/

char transmission_i2c(char _adresse, char _nb_env, char _nb_rec, unsigned char *s_envoi){

// Faire les tests pour vérifier que la ligne est libre.
    if(etat_i2c != I2C_LIBRE){
        return 0;
    }
    etat_i2c = I2C_START;

// On dit qu'il n'y a pas d'erreur 
    erreur_i2c=0;

// On prépare l'envoi
    nb_env = _nb_env;
    nb_rec = _nb_rec;
    i2c_slave_adresse = _adresse;
    memcpy( (void*) data_out, (void*) s_envoi, nb_env );

    // Envoyer un bit de début
    SSPCON2bits.SEN = 1; // 0b 0000 0001
    
    // Il faudra envoyer l'adresse
    etat_i2c = I2C_ENV_ADRESSE;


    return 1;
}

char get_erreur_i2c(){
    return erreur_i2c;
}
char get_etat_i2c(){
    return etat_i2c;
}


char i2c_en_cours(){
    if (etat_i2c == I2C_LIBRE){
        return 0;
    }else{
        return 1;
    }
}

void get_i2c_data(unsigned char *str){
    memcpy( (void*) str, (void*) data_in, nb_rec );
}

/*
La fonction en interruption
Celle qui fait tout, sauf envoyer le bit de début
*/	
void gestion_i2c(){

	// Si c'est une interruption du Timer 2
	if(PIR1bits.TMR2IF == 1){

		// On réinitialise le drapeau
		PIR1bits.TMR2IF = 0;
		if(etat_i2c == I2C_REC_ADRESSE){
			// On envoie un bit de début
			SSPCON2bits.SEN = 1; 
		}
		
		CloseTimer2();
		
	}
	
    // Si c'est bien une interruption I2C
    if(PIR1bits.SSPIF == 1){
        static char nacq = 1;

        // On réinitialise le drapeau.
        PIR1bits.SSPIF = 0;
        
        // Si on attendait un NACK, on vérifie qu'il est bon
        if(nacq == 1){
            nacq = 0;
            if(SSPCON2bits.ACKSTAT){
                // En cas de problème, on arrête l'I2C.
                SSPCON2bits.PEN = 1;
                erreur_i2c =(char)etat_i2c + 1;
                etat_i2c = I2C_STOP;
                return;
            }
        }
        
        switch(etat_i2c){
          case I2C_START:     
          case I2C_ENV_ADRESSE:
            SSPBUF = ((i2c_slave_adresse << 1) | 0);
            data_index = 0;
            etat_i2c = I2C_ENV_DONNEE;
            nacq = 1;
            break;
            
          case I2C_ENV_DONNEE:

            SSPBUF = data_out[data_index];
            data_index++;
            nacq = 1;
            // Si on a tout envoyé
            if(data_index == nb_env){
                etat_i2c = I2C_RESTART;                
            }
            break;
                        
          case I2C_RESTART:
            if(nb_rec == 0){
                //Arrêt de l'I2C
                SSPCON2bits.PEN = 1;
                etat_i2c = I2C_STOP;
                break;
            }
            
//          SSPCON2bits.RSEN=1; // Ne semble pas marcher avec le WMP
//			Le temps où l'I2C est au repos est probablement trop court.
//			Voir fiche technique du PCF8574 (ou qqch comme ça)
			// Bit d'arret 
			SSPCON2bits.PEN = 1;
			etat_i2c = I2C_RESTART_2;
			break;
			
		  case I2C_RESTART_2:
		  	
			// On ouvre le Timer 2, préscaler de 1, postscaler de 1;
			OpenTimer2(TIMER_INT_ON & T2_PS_1_1 & T2_POST_1_1);
			// On attend un peu
//			WriteTimer2(0xFFFF - 12000); // 1ms !!
			// 50 µs est probablement suffisant
			//WriteTimer2(0xFFFF - 600); // 50µs !!
			// Test à 10 µs => Ok !
			WriteTimer2(0xFFFF - 120); // 10µs !!
			

// Attente
//			while(SSPCON2bits.PEN == 1);
//			PIR1bits.SSPIF = 0;	
			// attente XXXµs
//			Delay100TCYx(120); //60

			// Envoyer un bit de début
//			SSPCON2bits.SEN = 1; 
//  Fin de solution paliative 

            etat_i2c = I2C_REC_ADRESSE;

            break;
                        
          case I2C_REC_ADRESSE:
            SSPBUF = (i2c_slave_adresse << 1 | 1);
            data_index=0;
            nacq = 1;
            etat_i2c = I2C_REC_DONNEE;
            break;
                        
          case I2C_REC_DONNEE:
            if(data_index == nb_rec){
                //Arrêt de l'I2C
                SSPCON2bits.PEN = 1;
                while(SSPCON2bits.PEN == 1);
                etat_i2c = I2C_LIBRE;
                
                break;
            }
            SSPCON2bits.RCEN=1;            
            etat_i2c = I2C_REC_ACK;
            break;            
            
          case I2C_REC_ACK:
            data_in[data_index]=SSPBUF;
            // Envoi d'un accusé réception
            SSPCON2bits.ACKDT=0;
            if(data_index == (nb_rec-1))
                SSPCON2bits.ACKDT=1;
            SSPCON2bits.ACKEN=1;

            data_index++;
            etat_i2c = I2C_REC_DONNEE;
            break;

          case I2C_STOP:
            etat_i2c = I2C_LIBRE;
       		
       		break;            
          default: break;

        }
        
    }
}

void init_i2c(){
	// I2C INIT
    // Initialisation de l'I2C
	// Les pattes concernée avec TRIS à 1
	TRISB = TRISB | 0x03; // TRISB0 et TRISB1 à 1
	// I2C en mode MASTER
	SSPCON1 = 0x08; // 0b 0000 1000
	// Pour la fréquence, on utilise les 7 dernier bits de SSPADD
	// La formule semble être la suivante Fosc/2/(SSPADD +1)
	// à 48 MHz, 59 nous donne 400kHz
	SSPADD = 59; // 59
	// Activation du SSP, module dont dépend l'I2C
	SSPCON1bits.SSPEN = 1;
	// On peut probablement faire ça d'un coups :
	// SSPCON1 = 0x28; // 0b 0010 1000
	PIE1bits.SSPIE=1; // Interruption I2C activée
	IPR1bits.SSPIP=1; // Priorité elevée
	// On active toutes les interruptions
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;

}






