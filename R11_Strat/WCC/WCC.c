#include <delays.h>
#include <i2c.h>
#include "WCC.h"
#include "../commun/carte_strategie.h"

// Bibli WiiClassic
unsigned char WiiClassicData[6];
char DY_init=0,DX_init=0,GY_init=0,GX_init=0;

char WiiClassicCom_Init() {
	unsigned char WCC_init_1[]={0xF0,0x55};
	unsigned char WCC_init_2[]={0xFB,0x00};
	if(!transmission_i2c(0x52,2,0,WCC_init_1)){
		return 0;
	}
	while(i2c_en_cours());
	if(get_erreur_i2c()){
		return 0;
	}
	Delay10KTCYx(0);
	if(!transmission_i2c(0x52,2,0,WCC_init_2)){
		return 0;
	}
	while(i2c_en_cours());
	if(get_erreur_i2c()){
		return 0;
	}
	return 1;
	/*
      Wire.begin();
    
      Wire.beginTransmission(0x52);
      Wire.send(0xF0);
      Wire.send(0x55);
      Wire.endTransmission();
    
      Wire.beginTransmission(0x52);
      Wire.send(0xFB);
      Wire.send(0x00);
      Wire.endTransmission();
      */
}

char WiiClassicCom_Read(unsigned char *reception){
    unsigned char index = 0;
    unsigned char WCC_read[] = {0x00};
    
    
    
    StartI2C();
    IdleI2C();
    putcI2C(0xA4);
    IdleI2C();
    
    if(SSPCON2bits.ACKSTAT){
		LED_ROUGE=0;
		LED_BLEUE=1;
		StopI2C();
		IdleI2C();
		return 0;
	}else{
		LED_BLEUE=0;
	}
    putcI2C(0x00);
    IdleI2C();
    StopI2C();
    IdleI2C();
    Delay1KTCYx(10);
    
    
    StartI2C();
    IdleI2C();
    putcI2C(0xA5);
    if(SSPCON2bits.ACKSTAT){
		LED_ROUGE=1;
		StopI2C();
		IdleI2C();
		return 0;
	}else{
		LED_ROUGE=0;
	}
    IdleI2C();
    getsI2C(reception,6);
    IdleI2C();
    StopI2C();
    IdleI2C();
    
    
/*
    if(transmission_i2c(0x52,1,6,WCC_read)){
		while(i2c_en_cours());
		if(!get_erreur_i2c()){
			get_i2c_data(reception);

			return 1;
		}
		if(get_erreur_i2c() == I2C_REC_DONNEE+1){
			LED_BLEUE=1;
		}else{
			LED_BLEUE=0;
		}
		if (get_erreur_i2c() == I2C_ENV_DONNEE+1){
			LED_ROUGE=1;
		}else{
			LED_ROUGE=0;
		}
		
    }
	return 0;*/
    /*
    Wire.begin();
    Wire.beginTransmission(0x52);
    Wire.send(0x00);
    Wire.endTransmission();
    delay(3);
    Wire.requestFrom(0x52,6);
    while (Wire.available ()) {
      reception[index]=Wire.receive();
      index++;
    } */
    return 1;
}

// Fin bibliothèque WiiClassicCom


char WiiClassic_Init(){
  DY_init=0;
  DX_init=0;
  GY_init=0;
  GX_init=0;
  if(!WiiClassicCom_Init()){
	  return 0;
  }
  Delay10KTCYx(0);
  if(!WiiClassicCom_Read(WiiClassicData)){
	  return 0;
  }
  
  DY_init = WCC_get_DY();
  DX_init = WCC_get_DX();
  GY_init = WCC_get_GY();
  GX_init = WCC_get_GX();
  
  return 1;
}
char WiiClassic_Read(){
   return WiiClassicCom_Read(WiiClassicData);
}

char WCC_get_DX(){ // Test OK!
  return ((WiiClassicData[2] &0x80) >> 7 | 
         (WiiClassicData[1] &0xC0) >> 5 |
         (WiiClassicData[0] &0xC0) >> 3 ) - DX_init;
}

char WCC_get_DY(){ // Test OK!
  return (WiiClassicData[2] &0x1F) - DY_init;
}

char WCC_get_GX(){ // Test OK!
  return (WiiClassicData[0] &0x3F) - GX_init;
}

char WCC_get_GY(){ // Test OK!
  return (WiiClassicData[1] &0x3F) - GY_init;
}

unsigned char WCC_get_L(){ // Test OK!
  return (WiiClassicData[3] &0xE0) >> 5 |
         (WiiClassicData[2] &0x60) >> 2 ;
}
unsigned char WCC_get_R(){ // Test OK!
  return (WiiClassicData[3] &0x1F) ;
}

unsigned char WCC_get_haut(){ //test Ok !
  return !(WiiClassicData[5] &0x01) ;
}
unsigned char WCC_get_bas(){ //test Ok !
  return !(WiiClassicData[4] &0x40) ;
}
unsigned char WCC_get_gauche(){ //test Ok !
  return !(WiiClassicData[5] &0x02) ;
}
unsigned char WCC_get_droite(){ //test Ok !
  return !(WiiClassicData[4] &0x80) ;
}

unsigned char WCC_get_X(){ // Test OK!
  return !(WiiClassicData[5] &0x08) ;
}
unsigned char WCC_get_Y(){ // Test OK!
  return !(WiiClassicData[5] &0x20) ;
}
unsigned char WCC_get_A(){ // Test OK!
  return !(WiiClassicData[5] &0x10) ;
}
unsigned char WCC_get_B(){ // Test OK!
  return !(WiiClassicData[5] &0x40) ;
}

unsigned char WCC_get_Lend(){ // Test OK!
  return !((WiiClassicData[4] &0x20) >> 5);
}
unsigned char WCC_get_Rend(){ // Test OK!
  return !((WiiClassicData[4] &0x02) >> 1);
}

unsigned char WCC_get_ZL(){
  return !((WiiClassicData[5] &0x80));
}
unsigned char WCC_get_ZR(){
  return !((WiiClassicData[5] &0x04));
}
unsigned char WCC_get_Home(){
  return !((WiiClassicData[4] &0x08));
}
unsigned char WCC_get_Select(){
  return !((WiiClassicData[4] &0x10));
}
unsigned char WCC_get_Start(){
  return !((WiiClassicData[4] &0x04));
}



