
// Definition des TRIS
#define TRIS_P_RESET      TRISAbits.TRISA0
#define TRIS_P_HALF       TRISAbits.TRISA1
#define TRIS_P_CLOCK      TRISAbits.TRISA2
#define TRIS_P_SENS       TRISAbits.TRISA3
#define TRIS_P_CONTROL    TRISAbits.TRISA4
#define TRIS_P_ENABLE     TRISAbits.TRISA5

#define TRIS_M_SENS_1     TRISCbits.TRISC1
#define TRIS_M_SENS_2     TRISCbits.TRISC0
#define TRIS_M_ENABLE     TRISCbits.TRISC2

#define TRIS_CT1          TRISBbits.TRISB7
#define TRIS_CT2          TRISBbits.TRISB6
#define TRIS_CT3          TRISBbits.TRISB5
#define TRIS_CT4          TRISBbits.TRISB3
#define TRIS_CT5          TRISCbits.TRISC7
#define TRIS_CT6          TRISCbits.TRISC6

#define TRIS_SONIC        TRISBbits.TRISB2


// Definition des PORTs ou LATs
#define P_RESET      LATAbits.LATA0
#define P_HALF       LATAbits.LATA1
#define P_CLOCK      LATAbits.LATA2
#define P_SENS       LATAbits.LATA3
#define P_CONTROL    LATAbits.LATA4
#define P_ENABLE     LATAbits.LATA5

#define M_SENS_1     LATCbits.LATC1
#define M_SENS_2     LATCbits.LATC0
#define M_ENABLE     LATCbits.LATC2

#define CT1          PORTBbits.RB7
#define CT2          PORTBbits.RB6
#define CT3          PORTBbits.RB5
#define CT4          PORTBbits.RB3
#define CT5          PORTCbits.RC7
#define CT6          PORTCbits.RC6

#define LED_OK       LATBbits.LATB7

#define SONIC        PORTBbits.RB2


// Public prototypes
void init_io(void);

