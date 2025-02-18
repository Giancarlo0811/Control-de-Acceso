#include <xc.h>
#define _XTAL_FREQ 4000000

#define R1_PIN PORTBbits.RB4
#define R2_PIN PORTBbits.RB5
#define R3_PIN PORTBbits.RB6
#define R4_PIN PORTBbits.RB7
#define C1_PIN PORTBbits.RB1
#define C2_PIN PORTBbits.RB2
#define C3_PIN PORTBbits.RB3

#define R1_DIR TRISBbits.TRISB4
#define R2_DIR TRISBbits.TRISB5
#define R3_DIR TRISBbits.TRISB6
#define R4_DIR TRISBbits.TRISB7
#define C1_DIR TRISBbits.TRISB1
#define C2_DIR TRISBbits.TRISB2
#define C3_DIR TRISBbits.TRISB3

void Keypad_Init(void);
char Keypad_Get_Char(void);