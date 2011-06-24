#define HAUT 1
#define BAS  0

#define ADRESSE_SERVO 0x33

char transmission_servo(void);

void SetServoPArG(char haut);
void SetServoPArD(char haut);
void SetServoPAv(char haut);
void SetServoCMUcam(char haut);
void SetCremaillere(char haut);
void GetDonneesServo(void);
char GetEnvoiServo(void);

char get_CC_Droit(void);
char get_CC_Gauche(void);
char get_CC_Avant(void);
char get_Presence_Pion(void);


