#include "../commun/carte_strategie.h"
#include "CMUcam.h"
#include <p18f2550.h>
#include <string.h>



volatile char CMUcam_in[NB_DATA_IN]; // Au maximum : 1 lettre, 5 lots de trois chiffres, 5 espaces, un caractère de fin
volatile char CMUcam_out[NB_DATA_OUT]; // 3 chiffre plus le caractère de fin.
volatile char CMUcam_in_index;
volatile char CMUcam_out_index;
volatile enum booleen CMUcam_RX_libre;
volatile enum booleen CMUcam_TX_libre;
volatile enum booleen _nouvelle_reception = NON;
volatile enum booleen _donnees_envoyees = NON;
volatile char _erreur_RC = 0;
char couleur_cmucam='P';


// Initialisation
void CMUcam_Init(void){
	//Initialisation de la CMUcam
	TRISCbits.TRISC6 = 1; // Pattes configurées en sortie
	TRISCbits.TRISC7 = 1; // Pattes configurées en sortie
	SPBRGH = 0;
	SPBRG = 103;
	BAUDCONbits.BRG16 = 1;
	TXSTAbits.BRGH =1;  // High Speed
	TXSTAbits.SYNC = 0; // Asynchrone
	RCSTAbits.SPEN = 1; // Activation du module
	RCSTAbits.RX9 = 0; // Mots de 8 bits
	PIE1bits.RCIE = 1; // Activation des interruptions du port com en reception
	PIE1bits.TXIE = 0; // Activation des interruptions du port com en emission
	RCSTAbits.CREN = 1; // Activation reception
	TXSTAbits.TXEN = 1; // Activation Transmission
	
	// Indexs
	CMUcam_in_index = 0;
	CMUcam_out_index = 0;
	_nouvelle_reception = NON;
	_donnees_envoyees = NON;
	
	CMUcam_TX_libre=OUI;
	CMUcam_RX_libre=OUI;

}

void setCouleur(char _c){
	couleur_cmucam = _c	;
}

char cherche_couleur(void){
    CMUcam_out[0]=couleur_cmucam;
    CMUcam_out[1]=CMUCAM_FIN;
    return env_cmucam();
	
}

char cherche_pion(void){	
    CMUcam_out[0]='P';
    CMUcam_out[1]=CMUCAM_FIN;
    return env_cmucam();
}
char cherche_case_rouge(void){
    CMUcam_out[0]='R';
    CMUcam_out[1]=CMUCAM_FIN;
    return env_cmucam();
}
char cherche_case_bleue(void){
    CMUcam_out[0]='B';
    CMUcam_out[1]=CMUCAM_FIN;
    return env_cmucam();
}
char nouvelle_recherche(void){
    CMUcam_out[0]='/';
    CMUcam_out[1]=CMUCAM_FIN;
    if(env_cmucam()){
        _nouvelle_reception = NON;
        return 1;
    }
    return 0;
}
char get_erreur_RC(void){
	if(_erreur_RC == 1){
		_erreur_RC = 0;
		return 1;
	}
	return 0;
}

char cmucam_reset(void){
    CMUcam_out[0]='~';
    CMUcam_out[1]=CMUCAM_FIN;
    if(env_cmucam()){
        _nouvelle_reception = NON;
        return 1;
    }
    return 0;
}
char ask_figure(void){
    CMUcam_out[0]=' ';
    CMUcam_out[1]=CMUCAM_FIN;
    if(env_cmucam()){
        _nouvelle_reception = NON;
        return 1;
    }
    return 0;
}
char select_figure(unsigned char id){
    CMUcam_out[0]= id / 100 + '0';
    id = id % 100;
    CMUcam_out[1]= id / 10 + '0';
    id = id % 10;
    CMUcam_out[2]= id + '0'; 
    CMUcam_out[3]=CMUCAM_FIN;

    if(env_cmucam()){
        _nouvelle_reception = NON;
        return 1;
    }
    return 0;
}


char chaine_to_id(char *chaine){
    char i,j=0;
    unsigned char id=0;
    
    while(chaine[i] != CMUCAM_FIN && j<5){
        if(chaine[i] == ' '){
            j++;
        }
        i++;
    }
    while(chaine[i] >= '0' && chaine[i] <= '9'){
        id = id * 10;
        id = (chaine[i] - '0') + id;
        i++;
    }
    return id;
}

char chaine_to_figure(char *chaine,figure_t *figure){
    // chaine : "g 0 213 100 267 24"
    unsigned char i=0,j=0;
    unsigned int temp=0;
    figure->x0 = 0;
    figure->x1 = 0;
    figure->y0 = 0;
    figure->y1 = 0;
    figure->id = 0;
    
    while((chaine[i] != CMUCAM_FIN) && j<5){
        if(chaine[i] == ' '){
            j++;
            i++;
            temp=0;
            while(chaine[i] >= '0' && chaine[i] <= '9'){
                temp = temp * 10;
                temp = (chaine[i] - '0') + temp;
                i++;
            }
            switch(j){
            case 1:figure->x0 = temp; break;
            case 2:figure->y0 = temp; break;
            case 3:figure->x1 = temp; break;
            case 4:figure->y1 = temp; break;
            case 5:figure->id = (unsigned char)temp; break;
            }
        }else{
            i++;
        }
    }
    return figure->id;
}

char rec_cmucam(char *chaine){
    // Afin de ne pas recevoir le début de la trame n et la fin de la trame n+1
    // Ou de ne pas relire inutilement une trame déjà lue.
    if(CMUcam_RX_libre == NON || _nouvelle_reception == NON ){
        // Il s'agit d'une reception
        return 0;
    }
    memcpy( (void*) chaine, (void*) CMUcam_in, NB_DATA_IN );
    _nouvelle_reception = NON; // On se note que nous avons lu ces données. 
    return 1;
}



char rec_cmucam_cours(){
    if (CMUcam_RX_libre == OUI){
        return 0;
    }else{
        return 1;
    }
}

char env_cmucam(void){
    if (CMUcam_TX_libre == OUI){
        unsigned char i=0;
        while( (CMUcam_out[i] != CMUCAM_FIN) && (i < NB_DATA_OUT) ){
            i++;
        }
        if(i == NB_DATA_OUT){
            return 0;
        }
        if(i > 0){
            CMUcam_TX_libre = NON;
        }
        CMUcam_out_index = 0;
       	PIE1bits.TXIE = 1; // Activation des interruptions du port com en emission
        
        
        return 1;
    }
    return 0;
}
char set_tampon_env(char *chaine){
    unsigned char i=0;
    while( (chaine[i] != CMUCAM_FIN) && (i < NB_DATA_OUT) ){
        i++;
    }
    if(i == NB_DATA_OUT){
        return 0;
    }
    memcpy((void *)CMUcam_out,(void *)chaine,i);
    return 1;
    
}
char TX_libre(){
    if(CMUcam_TX_libre == NON){
        return 0;
    }else{
        return 1;
    }
}


// Partie en interruption
// Interruption du port com
// Créer une grosse fonction CMUcam_Int ou objet_proche
void CMUcam_int(void){
    // Reception
	if( PIR1bits.RCIF){
		// On réarme l'interruption
		PIR1bits.RCIF = 0;
		
		// S'il y a une erreur, 
		if(RCSTAbits.OERR == 1 || RCSTAbits.FERR == 1){
			_erreur_RC=1;
			RCSTAbits.CREN = 0;
			RCSTAbits.CREN = 1;

			CMUcam_in_index = 0;
			
		}
		
		// On ajoute la donnée reçue au tampon
		if(CMUcam_in_index == 0){
			CMUcam_RX_libre = NON;
			_nouvelle_reception = NON;
		}
		CMUcam_in[CMUcam_in_index] = RCREG;
		if(CMUcam_in[CMUcam_in_index] == 'g' || CMUcam_in[CMUcam_in_index] == 't' || (CMUcam_in[CMUcam_in_index] >= '0' && CMUcam_in[CMUcam_in_index] <= '9') || CMUcam_in[CMUcam_in_index] == ' ' || CMUcam_in[CMUcam_in_index] == 0x0D || CMUcam_in[CMUcam_in_index] == 0x0A){
			if(CMUcam_in[CMUcam_in_index] == CMUCAM_FIN || CMUcam_in_index == NB_DATA_IN || CMUcam_in[CMUcam_in_index] == 0 ){
				// Fin de la reception
				CMUcam_RX_libre = OUI;
				_nouvelle_reception = OUI;
				CMUcam_in_index = 0;
			}else{
				CMUcam_in_index++;
			}
		}
	}
	
	// Emission
	if( PIR1bits.TXIF){
		// On ne réarme pas l'interruption !

        // Si on a encore des choses à emettre
		if(CMUcam_TX_libre == NON){
		    // Envoi des données

		    TXREG = CMUcam_out[CMUcam_out_index];
		
		    // Si on vient d'envoyer le dernier caractère
		    if(CMUcam_out[CMUcam_out_index] == CMUCAM_FIN || CMUcam_out_index == NB_DATA_OUT){
		        // On dit qu'on n'a plus besoin d'émettre
		        // Meme si il faudrait attendre que l'octet soit envoyé pour ça
		        CMUcam_TX_libre = OUI;
		        CMUcam_out_index=0;
		        // On n'a plus besoin de l'interruption
            	PIE1bits.TXIE = 0;
		    }else{
       		    CMUcam_out_index++;
       		}
		}
	}
	
}

// Fin de la fonction CMUcam_Int
