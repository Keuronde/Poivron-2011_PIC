/** I N C L U D E S **********************************************************/ 
#include <timers.h>
	
#include <p18f2550.h>
#include "servo.h"

/** V A R I A B L E S ********************************************************/
static char timer_servo = 1; // Compteur qui permet d'avoir une periode de 10 ms.
static char servo_courant = 0; // Compteur qui sauvegarde le servo courant.
static int pos_servo[NB_SERVO]; 		// Tableau qui contient les valeurs 



void Servo_Init(){
	char i;
	// On active toutes les interruptions
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
	// Ouverture du timer
	OpenTimer2( TIMER_INT_ON & T2_PS_1_16 & T2_POST_1_1);
	WriteTimer2(0);
	TRIS_SERVO1 = 0;
	for(i=0;i<NB_SERVO;i++){
		pos_servo[i]=0x0400;
	}
    Servo_Set(POUSSOIR_AR_G_HAUT);
    Servo_Set(POUSSOIR_AR_D_HAUT);
    Servo_Set(POUSSOIR_AV_HAUT);
}
void Servo_Set(int unsigned position, char num_servo){
	if(position > SERVO_MAX)
		position = SERVO_MAX;

	if(position < SERVO_MIN)
		position = SERVO_MIN;

	pos_servo[num_servo] = position;
}
int Servo_Get(char num){
	return pos_servo[num];
}

void Servo_Int(){
	/* timer qui se charge de l'impulsion */
	// On prend le timer deux qui ne sert à rien sinon.
	if (PIR1bits.TMR2IF)
	{
		// On réarme le timer
		PIR1bits.TMR2IF = 0;
		WriteTimer2(0);
		timer_servo--;
		if(timer_servo <= 0){
			if(servo_courant == 0){
				SERVO1=1;
				timer_servo = pos_servo[0] >> 8;
				if((pos_servo[servo_courant] & 0x00FF) != 0){
					WriteTimer2((unsigned char)(0x100 - (pos_servo[servo_courant] & 0x00FF)));
				}else{
					WriteTimer2(0);
					timer_servo--;
				}
				
				servo_courant = 1;
			}else if(servo_courant == 1){
    			SERVO1=0;
				SERVO2=1;
				timer_servo = pos_servo[1] >> 8;
				if((pos_servo[servo_courant] & 0x00FF) != 0){
					WriteTimer2((unsigned char)(0x100 - (pos_servo[servo_courant] & 0x00FF)));
				}else{
					WriteTimer2(0);
					timer_servo--;
				}
				
				servo_courant = 2;
			}else if(servo_courant == 2){
    			SERVO2=0;
				SERVO3=1;
				timer_servo = pos_servo[2] >> 8;
				if((pos_servo[servo_courant] & 0x00FF) != 0){
					WriteTimer2((unsigned char)(0x100 - (pos_servo[servo_courant] & 0x00FF)));
				}else{
					WriteTimer2(0);
					timer_servo--;
				}
				
				servo_courant = 3;
			}else if(servo_courant == 3){
    			SERVO3=0;
				SERVO4=1;
				timer_servo = pos_servo[3] >> 8;
				if((pos_servo[servo_courant] & 0x00FF) != 0){
					WriteTimer2((unsigned char)(0x100 - (pos_servo[servo_courant] & 0x00FF)));
				}else{
					WriteTimer2(0);
					timer_servo--;
				}
				
				servo_courant = 4;
			}else if(servo_courant == 4){
    			SERVO4=0;
				SERVO5=1;
				timer_servo = pos_servo[4] >> 8;
				if((pos_servo[servo_courant] & 0x00FF) != 0){
					WriteTimer2((unsigned char)(0x100 - (pos_servo[servo_courant] & 0x00FF)));
				}else{
					WriteTimer2(0);
					timer_servo--;
				}
				
				servo_courant = 5;
			}else{
				servo_courant = 0;
				SERVO5=0;
				timer_servo = 25;//25 - (pos_servo[servo_courant] >> 8);
			}
		}		
	}
	
}
