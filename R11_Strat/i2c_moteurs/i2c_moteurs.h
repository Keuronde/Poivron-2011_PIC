#define ADRESSE_PAP 0x31
#define PAP_MAX_ROT 116
#define PAP_MIN_ROT (int)-285
#define PAP_DROIT 0

void Avance(void);
void Recule(void);
void pap_set_pos(int pos);
void prop_set_vitesse(char rapide);
void prop_set_sens(char avant);
void prop_stop(void);
void ignore_contacteur(void);
char transmission_moteur(void);
char get_capteur_sonique_loin(void);
char get_capteur_sonique_proche(void);
void GetDonneesMoteurs(void);
char GetEnvoiMoteurs(void);
char get_CT_AV_G(void);
char get_CT_AV_D(void);
char prop_get_sens_avant(void);
