// Commandes pour le Wii Motion Plus
// Requiert l'I2C

#include "WMP.h"
#include "../i2c_m/i2c_m.h"


#define F_PRECISION 2

unsigned char init_WMP[2]={0xFE,0x04};
unsigned char read_WMP[1]={0x00};
int iteration;              // Compteur d'itération (nombre de valeurs utilisées pour la calibration)
unsigned long moyenne_temp; // Variable utilisée pour le calcul du zéro du WMP
unsigned long moyenne_X;    // Valeur du zéro du WMP.
signed long angle_X;        // Variable qui stocke l'angle du WMP
unsigned char old_timer;

void WMP_set_Angle(int angle_degres){
    angle_X = (long)((long)angle_degres * (long)20 * (long)333) < F_PRECISION;
}

long WMP_get_Angle(void){
    return angle_X;
}

void WMP_init_timer(unsigned char timer){
    old_timer = timer;
}


// S'occupe d'interroger le WMP et de calculer la vitesse puis l'angle
void WMP_calcul(unsigned char timer){
    unsigned char chaine_recue[6];
	long vitesse_X;
	unsigned char WMP_timer;

	// On compte le nombr d'incrément de 3 ms qui se sont écoulés
	if(timer > old_timer){
	    WMP_timer = timer - old_timer;
	}else{
	    WMP_timer = ((int)256 - old_timer) + timer ;
	}

	WMP_read(chaine_recue);


	// Calcul de l'angle actuel.
	// Reconstruciton de la valeur X :
	if((chaine_recue[3] & 0x02) == 2){ // mode lent - mode rapide
		//mode lent
		vitesse_X = ( (long)( (int)chaine_recue[0] | (((int)chaine_recue[3] & 0xFC) << 6)) << F_PRECISION )- moyenne_X;
	}else{
		vitesse_X = (long)4*(( (long)( (int)chaine_recue[0] | (((int)chaine_recue[3] & 0xFC) << 6)) << F_PRECISION )- moyenne_X);
	}
		
	
	angle_X += vitesse_X * WMP_timer; // en 1/(2^F_PRECISION * 333) °
	                             // Soit en 1/20°/s * 0.003s * 1/(2^F_PRECISION)
	                             // V_gyroscope * Pas de temps * Précision (voir moyenne)
	WMP_timer = 0;
	old_timer = timer;

}

// Calibration du Wii Motion Plus
// Calcul du zéro.
// Renvoie 0 si la calibration est terminée
// Renvoie 1 si la calibration est en cours
char WMP_calibration(){
    unsigned char chaine_recue[6];
	
	// Initialisation de la moyenne.
	if (iteration ==0){
		moyenne_temp = 0;
	}

	WMP_read(chaine_recue);

	moyenne_temp += (int)chaine_recue[0] | (((int)(chaine_recue[3] & 0xFC)) << 6);

	iteration++;
	if(iteration>= 2000){
		// on prend 2000 valeurs, mais on veut une précision supérieur à une unité.
		// commençons par une précision de 1/4 d'unitée
		// 1 unitée = 1/20° par seconde
		moyenne_X = moyenne_temp/(2000 >> F_PRECISION);
		return 0;
	}
	return 1;

}

// Lecture du WMP
char WMP_read(unsigned char * str){

    if(transmission_i2c(0x52,1,6,read_WMP)){
        while(i2c_en_cours());
	    if(!get_erreur_i2c()){
	        get_i2c_data(str);
	        return 1;
	    }else{
            return 0;
	    }
	}else{
        return 0;
    }
}



// Initialisation du WMP
char WMP_init(){
    iteration = 0; // Servira pour la calibration
    angle_X =0;
	if(transmission_i2c(0x53,2,0,init_WMP)){
        while(i2c_en_cours());

	    if(!get_erreur_i2c()){
	        // Init OK !
	        return 1;
	    }else{
	        // Erreur lors de l'envoie
	        return 0;
	    }
	}else{
	    // L'envoie n'a pas été tenté
		return 0;
	}    
}

char WMP_init_2(void){
    iteration = 0; // Servira pour la calibration
    angle_X =0;
}


