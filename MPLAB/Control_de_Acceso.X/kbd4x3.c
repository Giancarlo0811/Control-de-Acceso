#include "kbd4x3.h"

void Keypad_Init(void)
{
    R1_DIR = 1;
    R2_DIR = 1;
    R3_DIR = 1;
    R4_DIR = 1;
    C1_DIR = 0;
    C2_DIR = 0;
    C3_DIR = 0;
}

char Keypad_Get_Char(void)
{
    C1_PIN = 1;
    C2_PIN = 0;
    C3_PIN = 0;
    if(R1_PIN == 1){
        __delay_ms(2);
        while(R1_PIN == 1);
        __delay_ms(5);
        return '1';
    }
    if(R2_PIN == 1){
        __delay_ms(2);
        while(R2_PIN == 1);
        __delay_ms(5);
        return '4';
    }
    if(R3_PIN == 1){
        __delay_ms(2);
        while(R3_PIN == 1);
        __delay_ms(5);
        return '7';
    }
    if(R4_PIN == 1){
        __delay_ms(2);
        while(R4_PIN == 1);
        __delay_ms(5);
        return '*';
    }
    
    C1_PIN = 0;
    C2_PIN = 1;
    C3_PIN = 0;
    if(R1_PIN == 1){
        __delay_ms(2);
        while(R1_PIN == 1);
        __delay_ms(5);
        return '2';
    }
    if(R2_PIN == 1){
        __delay_ms(2);
        while(R2_PIN == 1);
        __delay_ms(5);
        return '5';
    }
    if(R3_PIN == 1){
        __delay_ms(2);
        while(R3_PIN == 1);
        __delay_ms(5);
        return '8';
    }
    if(R4_PIN == 1){
        __delay_ms(2);
        while(R4_PIN == 1);
        __delay_ms(5);
        return '0';
    }
    
    C1_PIN = 0;
    C2_PIN = 0;
    C3_PIN = 1;
    if(R1_PIN == 1){
        __delay_ms(2);
        while(R1_PIN == 1);
        __delay_ms(5);
        return '3';
    }
    if(R2_PIN == 1){
        __delay_ms(2);
        while(R2_PIN == 1);
        __delay_ms(5);
        return '6';
    }
    if(R3_PIN == 1){
        __delay_ms(2);
        while(R3_PIN == 1);
        __delay_ms(5);
        return '9';
    }
    if(R4_PIN == 1){
        __delay_ms(2);
        while(R4_PIN == 1);
        __delay_ms(5);
        return '#';
    }
    return 0;
}