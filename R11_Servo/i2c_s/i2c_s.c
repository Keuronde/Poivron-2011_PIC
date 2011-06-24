/*****************************************************************************
*  Ce fichier avec i2c_s.h doit permettre à un PIC escalve I2C de d'envoyer  *
*  et de recevoir des informations de la part du pic maitre.                 *
*  Hypothèses :                                                              *
*  La communication est toujours structurée de la même manière :             *
*    Le maitre envoi l'adresse                                               *
*    Suivant le bit R/W,                                                     *
*      Le maitre envoi un nombre d'octets défini à la compilation            *
*      Le maitre reçoit un nombre d'octets définit à la conpilation          *
*  Le nombre d'octets reçus et envoyés doit pouvoir être facilement          *
*  parametrable.                                                             *
*                                                                            *
******************************************************************************/


/** I N C L U D E S **********************************************************/
#include "i2c_s.h"
#include <p18cxxx.h>
#include <string.h>



/** VARIABLES GLOBALES ***/
char data_out[NB_ENV];
char data_in[NB_REC];
char data_index;

volatile enum booleen _nouvelle_reception = NON;
volatile enum booleen _donnees_envoyees = NON;
volatile enum booleen _i2c_actif = NON;


/*
Fonction init_i2c

Fonction permettant d'initialiser l'i2c en mode esclave

*/
void init_i2c(unsigned char _adresse){
	// I2C INIT
	// Initialisation de l'I2C
	// Les pattes concernée avec TRIS à 1
	TRISB = TRISB | 0x03; // TRISB0 et TRISB1 à 1
	// I2C en mode ESCLAVE
	// Pas d'interruption pour les bits de START et STOP
	SSPCON1 = 0x06; // 0b 0000 0110
	SSPCON2 = 0; // Pas de masque d'adresse (pour l'instant)
	SSPADD = _adresse << 1;

	// Activation du SSP, module dont dépend l'I2C
	SSPCON1bits.SSPEN = 1;

    // Activation des interruptions I2C
	PIE1bits.SSPIE=1; // Interruption I2C activée
	IPR1bits.SSPIP=1; // Priorité elevée

}

/*
Fonction rec_i2c

Fonction permettant de récupérer le tampon de réception i2c.
Cette fonction prend en argument l'adresse de la chaine de caractere de destination
Cette fonction renvoi 0 si une reception est en cours, 1 si tout s'est bien passé.

*/
char rec_i2c(unsigned char *chaine){

    // Afin de ne pas recevoir le début de la trame n et la fin de la trame n+1
    if(_nouvelle_reception == NON)
        return 0;
    if(_i2c_actif == OUI){
        // s'il s'agit d'une reception
        if(SSPSTATbits.R_W == 0){
            // On ne fait rien
            return 0;
        }
    }
    memcpy( (void*) chaine, (void*) data_in, NB_REC );
    _nouvelle_reception = NON; // On se note que nous avons lu ces données. 
    return 1;
}


/*
Fonction envoi_i2c

Fonction permettant de mettre dans le tampon d'envoi i2c une chaine de caractère.
Cette fonction prend en argument l'adresse de la chaine de caractere à envoyer
Cette fonction renvoi 0 si une émission est en cours, 1 si tout s'est bien passé.

*/

char envoi_i2c(char *chaine){
    // Afin de ne pas envoyer le début de la trame n et la fin de la trame n+1
    // Si une transaction i2c est en cours
    if(_i2c_actif == OUI){
        // s'il s'agit d'un envoi
        if(SSPSTATbits.R_W == 0){
            // On ne fait rien
            return 0;
        }
    }
    memcpy( (void*) data_out, (void*) chaine, NB_ENV );
    _donnees_envoyees = NON;
    return 1;
}

// Fonction en interruption
void com_i2c(){
    if(PIR1bits.SSPIF == 1){
        // Réinitialisation de l'interruption
        PIR1bits.SSPIF = 0;
        
        // On vérifie qu'il n'y a pas d'erreur :
        // SSPOV = OVERFLOW => On vient de recevoir quelque chose, mais le tampon précédent n'a pas été lu.
        if(SSPCON1bits.SSPOV == 1){
            char temp = SSPBUF; // Efface BF
            SSPCON1bits.SSPOV = 0;
            return; // La fonction s'arrête la, 
        }
        
        //Adresse ou données ?
        if(SSPSTATbits.D_A == 1){
            // Données
            
            // Reception ou émission ?
            if(SSPSTATbits.R_W == 0){
                // Réception
                data_in[data_index] = SSPBUF;
                data_index++;
                // Dernier octet ? 
                if(data_index == NB_REC){
                    // Fin de transaction
                    _nouvelle_reception = OUI;
                    _i2c_actif = NON;
                }
            }else{
                //Émission
                SSPBUF = data_out[data_index];
                SSPCON1bits.CKP =1;
                data_index++;
                if(data_index == NB_ENV){
                    // Fin de transaction
                    _donnees_envoyees = OUI;
                    _i2c_actif = NON;
                }
            }

        }else{
            // Adresse
            char temp = SSPBUF; // Pour eviter l'overflow, efface BF
            data_index = 0;     // On prépare la réception, ou l'émision
            // On se note qu'une transaction est en cours
            _i2c_actif = OUI;
            if(SSPSTATbits.R_W == 1){
                //Émission
                SSPBUF = data_out[data_index];
                SSPCON1bits.CKP =1;
                data_index++;
                if(data_index == NB_ENV){
                    // Fin de transaction
                    _donnees_envoyees = OUI;
                    _i2c_actif = NON;
                }
            }
        }
    }
}


