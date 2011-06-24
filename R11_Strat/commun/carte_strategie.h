
// Definition des TRIS
#define TRIS_RELAIS      TRISAbits.TRISA0
#define TRIS_LED_OK      TRISAbits.TRISA1
#define TRIS_LED_OK1     TRISAbits.TRISA2

#define TRIS_TIRETTE     TRISAbits.TRISA3
#define TRIS_COULEUR     TRISAbits.TRISA4


#define TRIS_LED_CMUCAM  TRISBbits.TRISB7
#define TRIS_LED_ROUGE   TRISBbits.TRISB6
#define TRIS_LED_BLEUE   TRISBbits.TRISB5

#define TRIS_RESERVE1   TRISCbits.TRISC1
#define TRIS_RESERVE2   TRISCbits.TRISC2
#define TRIS_ABSENCE_PION   TRISCbits.TRISC2

#define TRIS_BOOT       TRISBbits.TRISB4

// Definition des PORTs ou LATs
#define RELAIS      LATAbits.LATA0
#define LED_OK      LATAbits.LATA1
#define LED_OK1     LATAbits.LATA2

#define LED_CMUCAM  LATBbits.LATB7
#define LED_ROUGE   LATBbits.LATB6
#define LED_BLEUE   LATBbits.LATB5



#define TIRETTE     PORTAbits.RA3
#define COULEUR     PORTAbits.RA4
#define ABSENCE_PION   PORTCbits.RC2


#define RESERVE1    LATCbits.LATC1
#define RESERVE2    LARCbits.LATC2
#define BOOT        PORTBbits.RB4
// Public prototypes
void init_io(void);

