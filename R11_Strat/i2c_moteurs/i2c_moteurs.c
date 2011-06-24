#include "i2c_moteurs.h"
#include "../i2c_m/i2c_m.h"
#include <p18cxxx.h>
#include "../commun/carte_strategie.h"

// Variables globales
// Envoi
char acquittement=0;
int pos_pap = 0;
char sens = 0;
char vitesse = 0;
// Reception
// Contacteurs non acquités (6 bits)
// Capteur sonique (2 bits)
// A tester faire passer la communication avec la carte moteur sur 2 octets d'envoi, 1 en réception
char contacteurs = 0;
char capteur_sonique = 0;
unsigned char a_envoyer_moteur =0;

void pap_set_pos(int pos){
    // On envoie un entier positif compris entre  0 et 400;
    while(pos < 0){
        pos += 800;
    }
    while(pos >= 800){
        pos -= 800;
    }
    pos_pap = pos;
    a_envoyer_moteur =1;
}

void Avance(){
    prop_set_vitesse(1);
    prop_set_sens(1);
    a_envoyer_moteur=1;
}
void Recule(){
    prop_set_vitesse(1);
    prop_set_sens(0);
    a_envoyer_moteur=1;
}

void prop_set_vitesse(char rapide){
    if(rapide){
        vitesse = 1;
    }else{
        vitesse = 0;
    }
    a_envoyer_moteur=1;
}

void prop_set_sens(char avant){
    if(avant> 0){
        sens = 1;
    }else{
        sens = 2;
    }
    a_envoyer_moteur =1;
    
}

char prop_get_sens_avant(void){
	if(sens == 1){
		return 1;
	}
	return 0;
}

void prop_stop(void){
    sens = 0;
    a_envoyer_moteur =1;
}

void ignore_contacteur(void){
    acquittement = 1;
}
char get_contacteurs(){
    return contacteurs;
}
char get_CT_AV_G(){
    return contacteurs & 0x01;
}
char get_CT_AV_D(){
    return contacteurs & 0x02;
}
char get_capteur_sonique(){
    return capteur_sonique;
}
char get_capteur_sonique_loin(){
    return contacteurs & 0x40;
}
char get_capteur_sonique_proche(){
    return contacteurs & 0x80;
}
void GetDonneesMoteurs(void){
    a_envoyer_moteur = 1;
}
char GetEnvoiMoteurs(void){
    return a_envoyer_moteur;
}

char transmission_moteur(){
    // Préparation de l'envoi
    unsigned char envoi[2];
    unsigned char recu;
    if(a_envoyer_moteur == 1){
        // 9 bits pour la position du moteur pas à pas.
        envoi[0] = pos_pap & 0xFF;
        envoi[1] = (pos_pap >> 8) & 0x03;
        // 2 bits pour le sens du moteur
        envoi[1] |= ((sens & 0x03) << 2);
        // 1 bit pour la vitesse
        envoi[1] |= ((vitesse & 0x01) << 4);
        // 1 bit pour l'acquittement
        envoi[1] |= ((acquittement & 0x01) << 5);
        
        // Initialisation de l'i2c
        // renvoi 1 si ok, 0 sinon
        if(transmission_i2c(ADRESSE_PAP,2,1,envoi)){ // anciennement (ADRESSE_PAP,2,0,envoi)
            a_envoyer_moteur=0;
            acquittement =0;
			if(!get_erreur_i2c()){
            while(i2c_en_cours());
		        get_i2c_data(&recu);
		        contacteurs = recu;
      		}
            return 1;
        }
    }    
    return 0;
    
}
