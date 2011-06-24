#include "../commun/carte_moteurs.h"

#define AVANT 0
#define ARRIERE 1 
#define AUCUN 2

void M_prop_init(void);
void V_stop(void);
void V_lent(void);
void V_rapide(void);
void Avance(void);
void Recule(void);
void Stop(void);
char get_Sens(void);
