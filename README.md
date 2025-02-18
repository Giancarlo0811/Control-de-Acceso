# Control-de-Acceso

## Descripción del proyecto

Diseño e implementación de un circuito basado en un microcontrolador PIC16F1937 que hace la función de control de acceso a un recinto, marcando una clave de ingreso de 4 dígitos (por defecto "1234"). 
Para esto el circuito cuenta con un Teclado Matricial (4X3) por el cual se ingresa la clave y una pantalla LCD, en la cual se muestran los diversos mensajes como la hora (en formato de 24 hrs), la temperatura, el código enmascarado con "*", entre otros.

Al pulsar cualquiera de las teclas del Teclado, aparece un pequeño menú de opciones, donde el usuario puede elegir entre:
-	Ingresar Clave: Esta opción permite ingresar la clave actual para acceder. 
-	Cambiar Clave: Esta opción permite cambiar la clave actual por una nueva. Se preguntará primero por la clave actual para confirmar el cambio de clave.

El circuito cuenta también con un Led RGB que indica que el acceso se autorizó o fue negado. Además, también tiene un Zumbador (Buzzer) que suena sólo cuando el acceso fue negado. Se agregó un Relé que controla una carga (por ejemplo un bombillo), el cual simulará la apertura de la puerta.

Mientras no se está ingresando ningún código, en la pantalla se muestra el nombre de una empresa ficticia, la Hora y la Temperatura.

La base de tiempo para el reloj se implementó con un integrado externo que lleva la base de tiempo (RTC), el DS1307 con protocolo de comunicación I2C.

Para la parte de la temperatura, se utilizó el sensor Analógico LM35.

### Funcionalidades específicas

- Mostrar el nombre de la empresa.
- Mostrar hora actual usando el RTC y la temperatura usando un sensor analógico.
- Menú con 2 opciones: "INGRESAR CLAVE" o "CAMBIAR CLAVE". En este menú, si se pulsa cualquier tecla diferente de "1" o "2", se vuelve al programa principal.
    - Para acceder a "INGRESAR CLAVE" se debe pulsar la tecla "1".
    - Para acceder a "CAMBIAR CLAVE" se debe pulsar la tecla "2".
- LED RGB y Buzzer (Zumbador): 
    - LED Azul: Para indicar que el teclado está en uso. 
    - LED Verde: Para indicar que el acceso fue aprobado o si el cambio de clave fue correcto.
    - LED Rojo: Para indicar que el acceso fue denegado o que el cambio de clave fue incorrecto.
    - Buzzer: Suena cuando el acceso fue denegado o el cambio de clave fue incorrecto.
- Relay: Simula si se abrió o no la puerta. En caso de que el acceso fue permitido se abre la puerta (en la simulación un bombillo se prende), en caso de que fue denegado no se abre la puerta (bombillo apagado).
- La clave es almacenada en la memoria EEPROM del PIC.

CLAVE POR DEFECTO: 1234

## Montaje en protoboard

![alt text](./Imagenes/Montaje-protoboard-circuito.jpeg)

![alt text](./Imagenes/montaje_protoboard_con_programador.PNG)

## Esquemático

![alt text](./Imagenes/esquematico.PNG)

## Componentes
- PIC16F1937
- Pantalla LCD 16x2
- Teclado Matricial 4x3
- Reloj en tiempo real DS1307 RTC (Real Time Clock)
- Sensor de Temperatura LM35
- Tiny RTC Module
- Relé
- Buzzer
- Led RGB
- Resistencias, capacitores, diodos, transistores
- Oscilador de cristal de 4MHz

PDF del proyecto: [Proyecto](Proyecto_Control_de_Acceso.pdf)