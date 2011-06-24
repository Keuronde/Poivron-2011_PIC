#include "../commun/carte_moteurs.h"
/** D E F I N E **************************************************************/
#define RESET     P_RESET
#define HALF      P_HALF
#define CLOCK     P_CLOCK
#define CW        P_SENS
#define CONTROLE  P_CONTROL
#define ENABLE    P_ENABLE

#define TRIS_RESET     TRIS_P_RESET
#define TRIS_HALF      TRIS_P_HALF
#define TRIS_CLOCK     TRIS_P_CLOCK
#define TRIS_CW        TRIS_P_SENS
#define TRIS_CONTROLE  TRIS_P_CONTROL
#define TRIS_ENABLE    TRIS_P_ENABLE

void set_vitesse(int);
void setHalf(char);
int  get_pos(void);
void set_consigne(int);
void mPaP_int(void);
void MoteurPInit(void);
void Pas(void);

