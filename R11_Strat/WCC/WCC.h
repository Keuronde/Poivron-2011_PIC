#include "../i2c_m/i2c_m.h"



// Fonction d'acces aux boutons
char WC_get_DX(void);
char WC_get_DY(void);
char WC_get_GX(void);
char WC_get_GY(void);
unsigned char WC_get_L(void);
unsigned char WC_get_R(void);
unsigned char WC_get_haut(void);
unsigned char WC_get_bas(void);
unsigned char WC_get_gauche(void);
unsigned char WC_get_droite(void);
unsigned char WC_get_X(void);
unsigned char WC_get_Y(void);
unsigned char WC_get_A(void);
unsigned char WC_get_B(void);
unsigned char WC_get_Lend(void);
unsigned char WC_get_Rend(void);
unsigned char WC_get_ZL(void);
unsigned char WC_get_ZR(void);
unsigned char WC_get_Home(void);
unsigned char WC_get_Select(void);
unsigned char WC_get_Start(void);

// Fonction d'init et de mise à jour
void WiiClassic_Init(void);
void WiiClassic_Read(void);

// Fonction de bas niveau
char WiiClassicCom_Init(void);
char WiiClassicCom_Read(unsigned char *reception);
