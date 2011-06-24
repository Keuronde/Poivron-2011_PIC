
// Definition des TRIS
#define    TRIS_SERVO1 TRISBbits.TRISB7
#define    TRIS_SERVO2 TRISBbits.TRISB6
#define    TRIS_SERVO3 TRISBbits.TRISB5
#define    TRIS_SERVO4 TRISBbits.TRISB3
#define    TRIS_SERVO5 TRISBbits.TRISB2
#define    TRIS_SERVO_SUP1 TRISCbits.TRISC0
#define    TRIS_SERVO_SUP2 TRISCbits.TRISC6
#define    TRIS_LED_OK TRISCbits.TRISC0


#define    TRIS_CDE_LED_ROUGES TRISAbits.TRISA4
#define    TRIS_CDE_LED_BLEUES TRISAbits.TRISA5
#define    TRIS_CRE_SENS       TRISCbits.TRISC2
#define    TRIS_CRE_ENABLE     TRISCbits.TRISC1

#define    TRIS_BOOT           TRISBbits.TRISB4

// Definition des LAT
#define    SERVO1 LATBbits.LATB7
#define    SERVO2 LATBbits.LATB6
#define    SERVO3 LATBbits.LATB5
#define    SERVO4 LATBbits.LATB3
#define    SERVO5 LATBbits.LATB2
#define    SERVO_SUP1 LATCbits.LATC0
#define    SERVO_SUP2 LATCbits.LATC6
#define    LED_OK LATCbits.LATC0
#define    CRE_SENS       LATCbits.LATC2
#define    CRE_ENABLE     LATCbits.LATC1
#define    BOOT           PORTBbits.RB4

#define    CDE_LED_ROUGES PORTAbits.RA4
#define    CDE_LED_BLEUES PORTAbits.RA5



// Public prototypes
void init_io(void);

