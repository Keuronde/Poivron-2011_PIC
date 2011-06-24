#include "../commun/carte_servo.h"


/** D E F I N E **************************************************************/
// Constantes
#define SERVO_MIN 0x220
#define SERVO_MAX 0x850
#define NB_SERVO 5

// Ok 
// SERVO 4
#define POUSSOIR_AR_G_HAUT 0x480,3
#define POUSSOIR_AR_G_BAS  0x710,3
// SERVO 3
#define POUSSOIR_AR_D_HAUT 0x670,2
#define POUSSOIR_AR_D_BAS  0x400,2

// A définir
// SERVO 1 (marqué servo 2 sur la carte réelle)
#define POUSSOIR_AV_HAUT 0x630,0
#define POUSSOIR_AV_BAS 0x770,0
// SERVO 2 (marqué servo 1 sur la carte réelle)
#define CMUCAM_HAUT 0x3D0,1
#define CMUCAM_BAS  0x4D0,1


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void Servo_Set(unsigned int,char);
void Servo_Init(void);
void Servo_Int(void);
int Servo_Get(char);
