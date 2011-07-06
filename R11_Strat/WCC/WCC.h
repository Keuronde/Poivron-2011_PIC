#include "../i2c_m/i2c_m.h"



// Fonction d'acces aux boutons
char WCC_get_DX(void);
char WCC_get_DY(void);
char WCC_get_GX(void);
char WCC_get_GY(void);
unsigned char WCC_get_L(void);
unsigned char WCC_get_R(void);
unsigned char WCC_get_haut(void);
unsigned char WCC_get_bas(void);
unsigned char WCC_get_gauche(void);
unsigned char WCC_get_droite(void);
unsigned char WCC_get_X(void);
unsigned char WCC_get_Y(void);
unsigned char WCC_get_A(void);
unsigned char WCC_get_B(void);
unsigned char WCC_get_Lend(void);
unsigned char WCC_get_Rend(void);
unsigned char WCC_get_ZL(void);
unsigned char WCC_get_ZR(void);
unsigned char WCC_get_Home(void);
unsigned char WCC_get_Select(void);
unsigned char WCC_get_Start(void);

// Fonction d'init et de mise à jour
char WiiClassic_Init(void);
char WiiClassic_Read(void);

// Fonction de bas niveau
char WiiClassicCom_Init(void);
char WiiClassicCom_Read(unsigned char *reception);
