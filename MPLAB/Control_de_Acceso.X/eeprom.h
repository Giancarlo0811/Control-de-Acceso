#define	EEPROM_H

void EEPROM_Write(uint8_t dir, uint8_t data);
uint8_t EEPROM_Read(uint8_t dir);
void EEPROM_Clear(void);