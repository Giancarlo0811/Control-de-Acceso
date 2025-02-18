#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include "eeprom.h"

void EEPROM_Write(uint8_t dir, uint8_t data)
{
    EEADR = dir;
    EEDAT = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while(PIR2bits.EEIF == 0);
    PIR2bits.EEIF = 0;
}

uint8_t EEPROM_Read(uint8_t dir)
{
    EEADR = dir;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDAT;
}

void EEPROM_Clear(void)
{
    for(int i=0; i<256; i++)
    {
        EEPROM_Write((uint8_t)i, 0xFF);
    }
}