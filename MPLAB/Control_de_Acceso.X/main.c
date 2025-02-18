/***********LIBRERIAS*************/
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h" // libreria para pantalla LCD
#include "kbd4x3.h" // libreria para teclado 4x3
#include "eeprom.h" // libreria para la EEPROM
#include "mcc_generated_files/mcc.h" // MCC

/**************DEFINICIONES*********/
#define _XTAL_FREQ 4000000 
#define TRIS_LR TRISAbits.TRISA1 // LED RED
#define TRIS_LG TRISAbits.TRISA2 // LED GREEN
#define TRIS_LB TRISAbits.TRISA3 // LED BLUE
#define TRIS_BUZ TRISCbits.TRISC2 // BUZZER
#define TRIS_REL TRISCbits.TRISC5 // RELAY
#define LAT_LR LATAbits.LATA1 // LED RED
#define LAT_LG LATAbits.LATA2 // LED GREEN
#define LAT_LB LATAbits.LATA3 // LED BLUE
#define LAT_BUZ LATCbits.LATC2 // BUZZER
#define LAT_REL LATCbits.LATC5 // RELAY

/**********VARIABLES GLOBALES**************/
// Clave por defecto "1234"
// Para cambiar la clave, solo se deben modificar los primeros 4 bytes de la clave,
// el resto de bytes se deja en 0xFF
__EEPROM_DATA('1','2','3','4',0xFF,0xFF,0xFF,0xFF); // almacenar clave por defecto en EEPROM

char tecla;                  // Almacena el valor de la tecla presionada
char clave[5];               // Almacena la clave ingresada por el usuario
char clave_enter[5]; // guardar clave en la EEPROM
uint8_t cont_key = 0; // contador de la clave ingresada
uint8_t estado = 0; // estado para saber si INGRESAR CLAVE O CAMBIAR CLAVE
char buffer[20]; // buffer para lcd
uint16_t convertedValue; // valor ADC convertido
float temp; // Almacenar valor temperatura LM35
char seg,min,hor; // guardar datos del RTC

/*******FUNCIONES EEPROM***********/
void Escribir_Clave(char* str);
void Leer_Clave(char* str);

/**********PROGRAMA PRINCIPAL*****************/
void main() {
    
    // configuracion puertos y pines
    ANSELB = 0; // PORTB como digital
    TRIS_LR = 0; // LED RED como salida
    TRIS_LG = 0; // LED GREEN como salida
    TRIS_LB = 0; // LED BLUE como salida
    TRIS_BUZ = 0; // BUZZER como salida
    TRIS_REL = 0; // RELAY como salida
    TRISCbits.TRISC3 = 1; // SCL como entrada
    TRISCbits.TRISC4 = 1; // SDA como entrada
    LAT_LR = 1; // Led RED inicialmente apagado (CA)
    LAT_LG = 1; // Led GREEN inicialmente apagado (CA)
    LAT_LB = 1; // Led BLUE inicialmente apagado (CA)
    LAT_BUZ = 0; // BUZZER inicialmente apagado
    LAT_REL = 1; // puerta cerrada inicialmente
    
    // resistencias pull up en las columnas del teclado para las interrupciones
    OPTION_REGbits.nWPUEN = 0;
    WPUB = 0b00001110; // pull ups activadas en RB1,RB2,RB3 (columnas)
    
    // columnas teclado como salidas con valor inicial de 0 para detectar interrupcion
    C1_DIR = 0;
    LATBbits.LATB1 = 0;
    C2_DIR = 0;
    LATBbits.LATB2 = 0;
    C3_DIR = 0;
    LATBbits.LATB3 = 0;
    
    // interrupciones
    INTCONbits.GIE = 1; // habilitar interrupciones globales
    INTCONbits.IOCIE = 0; // limpiar flag interrupcion
    INTCONbits.IOCIE = 1; // habilitar interrupciones IoC (interrupt on change)
    //IOCBP = 0b11110000; // Interrupciones por flanco de subida
    IOCBN = 0b11110000; // Interrupciones por flanco de bajada
    IOCBF = 0; // flag
    
    // MSSP
    SSPCON1bits.SSPEN = 1; // habilitar puerto MSSP
    SSPCON1bits.SSPM3 = 1; 
    SSPCONbits.SSPM2 = 0; // Modo I2C Maestro
    SSPCONbits.SSPM1 = 0;
    SSPCON1bits.SSPM0 = 0;
    SSPADD = 0x4F; // 100KHz Baud Rate
    
    // Inicializaciones
    ADC_Initialize(); // inicializar ADC
    Lcd_Init();      // Inicializa la pantalla lcd
    Lcd_Clear(); // limpiar lcd
    
    __delay_ms(200); // delay de estabilizacion
    
    // Trama Escritura RTC -> Reloj 24 hrs
    // Hora de prueba: 15:00:00 (se puede modificar para configurar la hora adecuada)
    SSPCON2bits.SEN = 1; // generar senal de start
    while(SSPCON2bits.SEN); // esperar que se genere senal
    SSPBUF = 0b11010000; // direccion +0 de escritura
    while(SSPSTATbits.BF); // esperar que se envie el dato
    while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
    SSPBUF = 0; // direccion 0x00 (segundos)
    while(SSPSTATbits.BF); // esperar que se envie el dato
    while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
    SSPBUF = 0; // 0 segundos
    while(SSPSTATbits.BF); // esperar que se envie el dato
    while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
    SSPBUF = 0; // 0 minutos
    while(SSPSTATbits.BF); // esperar que se envie el dato
    while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
    SSPBUF = 0b00010101; // 15 horas
    while(SSPSTATbits.BF); // esperar que se envie el dato
    while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
    SSPCON2bits.PEN = 1; // genera senal de stop
    while(SSPCON2bits.PEN); // esperar senal de stop
    
    // mensaje inicial
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("ControlTech CA");
    
    __delay_ms(200); // delay de estabilizacion
    
    while (1) {
        
        // Trama Lectura RTC -> Reloj 24 hrs
        SSPCON2bits.SEN = 1; // genera senal de start
        while(SSPCON2bits.SEN); // esperar que se genere senal
        SSPBUF = 0b11010000; // direccion +0 de escritura
        while(SSPSTATbits.BF); // esperar que se envie el dato
        while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
        SSPBUF = 0; // direccion 0x00 (segundos)
        while(SSPSTATbits.BF); // esperar que se envie el dato
        while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
        SSPCON2bits.RSEN = 1; // generar senal de restart
        while(SSPCON2bits.RSEN); // esperar que se genere la senal
        SSPBUF = 0b11010001; // Direccion +1 de lectura
        while(SSPSTATbits.BF); // esperar que se envie el dato
        while(SSPSTATbits.R_nW); // esperar que transcurra la senal de ACK
        SSPCON2bits.RCEN = 1; // Modo de recepcion
        while(SSPSTATbits.BF==0); // esperar a que llegue el dato
        seg = SSPBUF; // segundos
        SSPCON2bits.ACKDT = 0; // dato ACK
        SSPCON2bits.ACKEN = 1; // genera senal de ACK o NACK
        while(SSPCON2bits.ACKEN); // esperar que se envie la senal
        SSPCON2bits.RCEN = 1; // Modo de recepcion
        while(SSPSTATbits.BF==0); // esperar a que llegue el dato
        min = SSPBUF; // minutos
        SSPCON2bits.ACKDT = 0; // dato ACK
        SSPCON2bits.ACKEN = 1; // genera senal de ACK o NACK
        while(SSPCON2bits.ACKEN); // esperar que se envie la senal
        SSPCON2bits.RCEN = 1; // Modo de recepcion
        while(SSPSTATbits.BF==0); // esperar a que llegue el dato
        hor = SSPBUF; // horas
        SSPCON2bits.ACKDT = 1; // dato NACK
        SSPCON2bits.ACKEN = 1; // genera senal de ACK o NACK
        while(SSPCON2bits.ACKEN); // esperar que se envie la senal
        SSPCON2bits.PEN = 1; // genera senal de stop
        while(SSPCON2bits.PEN); // esperar senal de stop
        
        // Lectura del ADC (Temperatura LM35)
        ADC_StartConversion(); // iniciar conversi�n ADC
        while (!ADC_IsConversionDone()); // esperar conversi�n
        convertedValue = ADC_GetConversionResult(); // guardar resultado
        temp = (float)(convertedValue*5*100)/1024; // formula temperatura
        
        // mostrar hora en LCD
        sprintf(buffer,"%d%d:%d%d:%d%d",hor>>4,hor&=0x0F,min>>4,min&=0x0F,seg>>4,seg&=0x0F);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(buffer);
        
        // mostrar temperatura en LCD
        Lcd_Set_Cursor(2,10);
        sprintf(buffer,"%0.2f\xDF",temp);
        Lcd_Write_String(buffer);
        Lcd_Set_Cursor(2,16);
        Lcd_Write_String("C");
        
        __delay_ms(1000); // lectura de tiempo y temp cada segundo
    }
}

/*************FUNCION INTERRUPCION TECLADO************/
void __interrupt() IOC(void) {
    Keypad_Init();   // Inicializa el teclado matricial 4x3
    Lcd_Clear();
    LAT_LB = 0; // Led azul encendido
    __delay_ms(500); // delay de estabilizacion
    
    while(IOCBF != 0) {
        switch(estado) { 
            case 0: // Menu de opciones
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("1:INGRESAR CLAVE");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String("2:CAMBIAR CLAVE");
                
                tecla = Keypad_Get_Char();
                if(tecla != 0) {
                    switch(tecla) {
                        case '1':
                            estado = 1; // modo ingresar clave
                            break;
                        case '2':
                            estado = 2; // modo cambiar clave
                            break;
                        default:
                            estado = 0; // menu de opciones
                            
                         // si se pulsa otra tecla diferente de 1 o 2, se vuelve al programa principal
                            WPUB = 0b11111110; // activar pull up en filas para deshabilitar teclado
                            __delay_ms(200);
                            // columnas vuelven a estado original 0
                            LATBbits.LATB1 = 0;
                            LATBbits.LATB2 = 0;
                            LATBbits.LATB3 = 0;
                            // desactivar pull ups en filas y activar en columnas
                            WPUB = 0b00001110;
                            IOCBF = 0; // reiniciar flag
                            break;
                    }
                    Lcd_Clear();
                }
                break;
            case 1: // Modo Ingresar Clave
                Lcd_Set_Cursor(1,2);
                Lcd_Write_String("INGRESAR CLAVE");
                
                while(cont_key < 4)
                {
                    tecla = Keypad_Get_Char(); // Lee el dato de la tecla presionada
                    if(tecla != 0)  // Verifica si se ha presionado alguna tecla
                    {
                        clave[cont_key] = tecla;   // Almacena cada tecla presionada en el arreglo
                        Lcd_Set_Cursor(2,7+cont_key);
                        Lcd_Write_Char('*'); // Muestra la tecla presionada. 
                        cont_key++;  // Incrementa el contador
                    }
                }
                /*hay que activar pull ups en filas para que el teclado no funcione y se bloquee 
                durante el chequeo de clave. Esto evita bugs ya que si se presionan teclas durante esta fase, 
                la interrupcion no es detectada correctamente en ciertas teclas */
                
                WPUB = 0b11111110; // activar pull up en filas para deshabilitar teclado
                __delay_ms(200);
                Lcd_Clear();   // Limpia la pantalla lcd
                Leer_Clave(clave_enter); // Lee la clave almacenada en la memoria EEPROM
                LAT_LB = 1; // LED BLUE apagado

                if(!strcmp(clave, clave_enter)) // Compara si la clave es la correcta
                {   
                    Lcd_Set_Cursor(1,6);
                    Lcd_Write_String("ACCESO");
                    Lcd_Set_Cursor(2,5);
                    Lcd_Write_String("APROBADO");
                    LAT_LG = 0; // LED GREEN encendido
                    LAT_REL = 0; // Puerta abierta
                    __delay_ms(10000); // 10s
                }
                else   // Sino es la clave correcta, no permite el acceso
                {   
                    Lcd_Set_Cursor(1,6);
                    Lcd_Write_String("ACCESO");
                    Lcd_Set_Cursor(2,5);
                    Lcd_Write_String("DENEGADO");
                    LAT_LR = 0; // LED RED encendido
                    LAT_BUZ = 1; // BUZZER encendido
                    LAT_REL = 1; // Puerta cerrada
                    __delay_ms(5000); // 5s
                }
                cont_key = 0;         // Reinicia el contador
                estado = 0; // reiniciar estado
                Lcd_Clear();  // Limpia la pantalla lcd
                LAT_LG = 1; // LED GREEN apagado
                LAT_LR = 1; // LED RED apagado
                LAT_BUZ = 0; // BUZZER apagado
                LAT_REL = 1; // Puerta cerrada
                // columnas vuelven a estado original 0
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                LATBbits.LATB3 = 0;
                // desactivar pull ups en filas y activar en columnas
                WPUB = 0b00001110;
                IOCBF = 0; // reiniciar flag
                break;
            case 2: // Modo cambiar clave
                Lcd_Set_Cursor(1,3);
                Lcd_Write_String("CLAVE ACTUAL");
                
                while(cont_key < 4)
                {
                    tecla = Keypad_Get_Char(); // Lee el dato de la tecla presionada
                    if(tecla != 0)  // Verifica si se ha presionado alguna tecla
                    {
                        clave[cont_key] = tecla;   // Almacena cada tecla presionada en el arreglo
                        Lcd_Set_Cursor(2,7+cont_key);
                        Lcd_Write_Char('*'); // Muestra la tecla presionada. 
                        cont_key++;  // Incrementa el contador
                    }
                }
                WPUB = 0b11111110; // activar pull up en filas para deshabilitar teclado
                __delay_ms(200);
                Lcd_Clear();   // Limpia la pantalla lcd
                Leer_Clave(clave_enter); // Lee la clave almacenada en la memoria EEPROM

                if(!strcmp(clave, clave_enter)) // Compara si la clave es la correcta
                {   
                    Lcd_Set_Cursor(1,3);
                    Lcd_Write_String("CLAVE NUEVA");
                    cont_key = 0;
                    
                    WPUB = 0b00001110; // activar pull ups columnas, desactivar en filas para que teclado funcione
            
                    while(cont_key < 4) {
                        tecla = Keypad_Get_Char();
                        if (tecla != 0) {
                            clave[cont_key] = tecla;
                            Lcd_Set_Cursor(2,7+cont_key);
                            Lcd_Write_Char('*');  
                            cont_key++; 
                        }
                    }
                    WPUB = 0b11111110; // activar pull up en filas para deshabilitar teclado
                    __delay_ms(200);
                    Lcd_Clear();   // Limpia la pantalla lcd
                    Escribir_Clave(clave);
                    
                    Lcd_Set_Cursor(1,2);
                    Lcd_Write_String("CLAVE CAMBIADA");
                    Lcd_Set_Cursor(2,2);
                    Lcd_Write_String("CORRECTAMENTE");
                    LAT_LB = 1; // LED BLUE apagado
                    LAT_LG = 0; // LED GREEN encendido
                    __delay_ms(4000); // 4s
                } else {
                    Lcd_Set_Cursor(1,6);
                    Lcd_Write_String("CLAVE");
                    Lcd_Set_Cursor(2,4);
                    Lcd_Write_String("INCORRECTA");
                    LAT_LB = 1; // LED BLUE apagado
                    LAT_BUZ = 1; // BUZZER encendido
                    LAT_LR = 0; // encender LED RED
                    __delay_ms(3000); // 3s
                }
                cont_key = 0;         // Reinicia el contador
                estado = 0; // reiniciar estado
                Lcd_Clear();  // Limpia la pantalla lcd
                // columnas vuelven a estado original 0
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                LATBbits.LATB3 = 0;
                // desactivar pull ups en filas y activar en columnas
                WPUB = 0b00001110;
                LAT_LR = 1; // LED RED apagado
                LAT_LG = 1; // LED GREEN apagado
                LAT_BUZ = 0; // BUZZER apagado
                IOCBF = 0; // reiniciar flag
                break;
        }
    }
    LAT_LB = 1; // LED BLUE apagado
    Lcd_Clear();
    // volver a mostrar mensaje inicial
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("ControlTech CA");
    INTCONbits.IOCIF = 0; // reiniciar flag interrupcion
}

/********FUNCIONES EEPROM**************/
void Escribir_Clave(char* str) // funcion para guardar clave en la EEPROM
{
    for(uint8_t pos=0; pos<4; pos++)
    {
        EEPROM_Write(pos, str[pos]);
    }
}

void Leer_Clave(char* str) // funcion para leer la clave guardada en la EEPROM
{
    for(uint8_t pos=0; pos<4; pos++)
    {
        str[pos] = (char)EEPROM_Read(pos);
    }
}