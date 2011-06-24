#include "../i2c_m/i2c_m.h"
#include "i2c_servo.h"


unsigned char envoi_servo;
unsigned char a_envoyer=0;
unsigned char CC_droit,CC_gauche,CC_avant;
unsigned char presence_pion;


void SetServoPArG(char haut){
    if(haut){
        envoi_servo |= 0x01;
    }else{
        envoi_servo &= 0xFE;
    }
    a_envoyer = 1;
}

void SetServoPArD(char haut){
    if(haut){
        envoi_servo |= 0x02;
    }else{
        envoi_servo &= 0xFD;
    }
    a_envoyer = 1;
}

void SetServoPAv(char haut){
    if(haut){
        envoi_servo |= 0x04;
    }else{
        envoi_servo &= 0xFB;
    }
    a_envoyer = 1;
}

void SetServoCMUcam(char haut){
    if(haut){
        envoi_servo |= 0x08;
    }else{
        envoi_servo &= 0xF7;
    }
    a_envoyer = 1;
}

void SetCremaillere(char haut){
    if(!haut){
        envoi_servo |= 0x20;
    }else{
        envoi_servo &= 0xDF;
    }
    a_envoyer = 1;
}

void GetDonneesServo(void){
    a_envoyer = 1;
}
char GetEnvoiServo(void){
    return a_envoyer;
}

char transmission_servo(void){

    if(a_envoyer == 1){
        unsigned char chaine;
        if(transmission_i2c(ADRESSE_SERVO,1,1,&envoi_servo)){
            a_envoyer =0;
            while(i2c_en_cours());
			if(!get_erreur_i2c()){
				get_i2c_data(&chaine);
				CC_gauche = chaine & 0x03;
				CC_droit = (chaine & 0x0C) >> 2;
				CC_avant = (chaine & 0x30) >> 4;
			}
            return 1;
        }
    }
    return 0;
}

char get_CC_Droit(void){
    return CC_droit;
} 
char get_CC_Gauche(void){
    return CC_gauche;
} 
char get_CC_Avant(void){
    return CC_avant;
}
