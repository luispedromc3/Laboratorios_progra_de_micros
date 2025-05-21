/*
 * Proyecto2.c
 *
 * Created: 4/30/2025 10:26:55 AM
 * Author: Luis Pedro Monterroso Campos
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> // para itoa()
#include <stdbool.h>
#include <stdlib.h>
#define MAX_BUFFER 10

uint8_t contador;
uint8_t modo;
uint8_t adc_adafruit = 0;
uint8_t eeprom_address = 0;

uint8_t pos1 = 0;
uint8_t pos2 = 0;
uint8_t pos3 = 0;
uint8_t pos4 = 0;
uint8_t posicion_eeprom = 0;
uint8_t cantidad_posiciones = 0;
//uint8_t posicion_actual = 0;
volatile uint8_t posicion_actual = 0;  // global


bool bandera_modo = false;
bool bandera_m1 = false;
bool bandera_m2 = false;
bool bandera_m3 = false;
bool bandera_m4 = false;
bool bandera_posicion_eeprom = false;

char buffer[MAX_BUFFER];
uint8_t indice = 0;

/****************************************/
// Function prototypes
void setup();
void initADC();
void PWM1();
void PWM2();
void initUART();
void agregar_char(char c);
void writeEEPROM(uint8_t dato, uint16_t direccion);
uint8_t readEEPROM(uint16_t direccion);

/****************************************/
// Main Function
int main(void)
{
	setup();
	while (1)
	{
		if (modo == 2){
			if (posicion_eeprom == 1){
				OCR1A = readEEPROM(0);
				OCR2A = readEEPROM(1);
				OCR1B = readEEPROM(2);
				OCR2B = readEEPROM(3);
			}else if (posicion_eeprom == 2){
				OCR1A = readEEPROM(4);
				OCR2A = readEEPROM(5);
				OCR1B = readEEPROM(6);
				OCR2B = readEEPROM(7);
			}else if (posicion_eeprom == 3){
				OCR1A = readEEPROM(8);
				OCR2A = readEEPROM(9);
				OCR1B = readEEPROM(10);
				OCR2B = readEEPROM(11);
			}else if (posicion_eeprom == 4){
				OCR1A = readEEPROM(12);
				OCR2A = readEEPROM(13);
				OCR1B = readEEPROM(14);
				OCR2B = readEEPROM(15);
			}
			
		}
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();
	
	//CONFIGURAR presecaler general del CLK A 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	//adc
	initADC();
	initUART();
	// configurar salidas de PWM
	//CONFIGURAMOS 0CR1A Y OCR2A COMO LA SALIDA EN PB1
	DDRB |= (1 << DDB1); //0CR1A
	DDRB |= (1 << DDB2); //OCR2A 
	//IGUAL LAS DEL TIMER2
	DDRB |= (1 << DDB3); //OCR2A
	DDRD |= (1 << PORTD3); //OCR2B
	
	//inicializar los PWM
	PWM1();
	PWM2();
	
	//incializo el boton que utilizare para guardar las posiciones
		// Configurar PB0 como entrada (pin D8)
	DDRB &= ~(1 << DDB0);   // PB0 input
	PORTB |= (1 << PORTB0); // Activar pull-up interno (opcional)
		// Habilitar interrupciones por cambio de pin en PCINT0 (PB0)
	PCICR |= (1 << PCIE0);   // Habilita grupo PCINT[7:0]
	PCMSK0 |= (1 << PCINT0); // Habilita específicamente PCINT0 (PB0)
	
	//inicializo las leds que indicaran el modo actual
	DDRB |= (1 << DDB4); 
	DDRB |= (1 << DDB5); 
	DDRD |= (1 << DDD7); 
	
	//inicializar variables
	contador = 0;
	modo = 0; 
	sei();
}

void initADC(){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
	ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
	ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //PRESCALER DE 128
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE); //ACTIVAR INTERRUPCIONES
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}

void PWM1(){
	
	
	//CONFIGURAMOS FASTPWM no invertido
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);

	TCCR1B = (1 << WGM12) | (1 << WGM13);
	
	//64 POR DEFECTO
	TCCR1B |= (1 << CS11) | (1 << CS10);
	ICR1 = 312; //top
	
}
void PWM2(){
	//CONFIGURAMOS FASTPWM NO INVERTIDO
	TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

	TCCR2B |= (1 << CS22);  //PRESCALER DE 64
}
void initUART(){
	//SALIDA Y ENTRADA DEL RX Y TX
	DDRD |= (1 << DDD1);
	DDRD &= ~(1 << DDD0);
	//conf de banderas
	UCSR0A = 0;
	UCSR0A |= (1 << U2X0) ;
	//configurar UCSR0B
	UCSR0B |= (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
	//8 bits de datos, sin polaridad, 1 STOP BIT y asincrono
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	//configurar BAUDRATE, es decir velocidad de transmision. UBRR0 = 12 -> 9600 @ 1MHz
	UBRR0 = 12;
}

void agregar_char(char c) {
	if (indice < MAX_BUFFER - 1) {  // Deja espacio para '\0'
		buffer[indice++] = c;
		buffer[indice] = '\0';  // Siempre mantener cadena terminada en '\0'
	}
}
void writeEEPROM(uint8_t dato, uint16_t direccion){
	while(EECR & (1 << EEPE));	//espero a que bit EEPE = 0;	
	//aca ya puedo escribir
	//establecer direccion a donde quiero guardar
	EEAR = direccion;
	//establecer dato que quiero guardar
	EEDR = dato;
	//master write enable
	EECR |= (1 << EEMPE);
	// master write enable
	EECR |= (1 << EEPE);
}
uint8_t readEEPROM(uint16_t direccion){
	while(EECR & (1 << EEPE));	//espero a que bit EEPE = 0;	
	//direccion a donde quiero leer
	EEAR = direccion;
	//empezar lectura
	EECR |= (1 << EERE);
	//regreso la lectura
	return EEDR;
}
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	//En esta interrupcion, leo y despues cambio al siguiente adc a leer.
	// verifico si estamos en modo 1 (modo manual), si no estamos en este modo no modifico el PWM.
	switch(contador){
		case 1:
			if(modo == 1){
				OCR1A = ((ADCH*30/255) +7);
				pos1 = ((ADCH*30/255) +7);
			}
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX1)|(1 << MUX0); //ADC3
			break;
		case 2:
			if(modo == 1){
				OCR1B	= ((ADCH*30/255) +7) ;
				pos3 = OCR1B	= ((ADCH*30/255) +7) ;
			}
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2)|(1 << MUX0); //ADC5
			break;
		case 3:
			if(modo == 1){
				OCR2A = ((ADCH*30/255)+7);
				pos2 = ((ADCH*30/255)+7);
			}
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2) ; //ADC4
			break;
		default:
			if(modo == 1){
				OCR2B = ((ADCH*30/255)+7);
				pos4 = ((ADCH*30/255)+7);
			}
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
			contador = 0;
			break;
	}
	contador++;
	
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}

ISR(PCINT0_vect) {
	//aca voy a hacer el codigo para guardar una posicion en eeprom
	
	if (posicion_eeprom == 1){
		writeEEPROM(OCR1A, 0);
		writeEEPROM(OCR2A, 1);
		writeEEPROM(OCR1B, 2);
		writeEEPROM(OCR2B, 3);
	} else if (posicion_eeprom == 2){
		writeEEPROM(OCR1A, 4);
		writeEEPROM(OCR2A, 5);
		writeEEPROM(OCR1B, 6);
		writeEEPROM(OCR2B, 7);
	}else if (posicion_eeprom == 3){
		writeEEPROM(OCR1A, 8);
		writeEEPROM(OCR2A, 9);
		writeEEPROM(OCR1B, 10);
		writeEEPROM(OCR2B, 11);
	}else if (posicion_eeprom == 4){
		writeEEPROM(OCR1A, 12);
		writeEEPROM(OCR2A, 13);
		writeEEPROM(OCR1B, 14);
		writeEEPROM(OCR2B, 15);
	}
}

ISR(USART0_RX_vect){
	//temporal posee lo que el usuario ingreso
	char temporal = UDR0;
	if(bandera_modo){
		if(temporal == '1'){
			//eligio modo manual
			PORTD &= ~(1 << PORTB7);  // PB4 = 0
			PORTB |= (1 << PORTB5);   // PB5 = 1
			
			modo = 1;
			
		}else if(temporal == '2'){
			//ascii
			//modo eepromDDRB |= (1 << DDB5); 
			modo = 2;
			PORTD |= (1 << PB7);   // PB4 = 1
			PORTB &= ~(1 << PB5);  // PB5 = 0
		}else if (temporal == '3'){
			//modo uart
			PORTB |= (1 << PB5);   // PB5 = 1
			PORTD |= (1 << PB7);   // PB5 = 1
			modo = 3;
		}
		
		bandera_modo = false;
	}else if(bandera_m1){
		if (temporal == 'H') {
			adc_adafruit = atoi(&buffer[0]);
			// Usar valor... salida pb1
			if (modo == 3){
				OCR1A = ((adc_adafruit*30/255) +7);
			}
			
			bandera_m1 = false;
			// Reiniciar para siguiente número
			indice = 0;
			buffer[0] = '\0';
		} else {
			agregar_char(temporal);
		}
	}else if(bandera_m2){
		if (temporal == 'H') {
			adc_adafruit = atoi(&buffer[0]);
			// Usar valor... salida pb1
			if (modo == 3){
				OCR1B = ((adc_adafruit*30/255) +7);
			}
			
			bandera_m2 = false;
			// Reiniciar para siguiente número
			indice = 0;
			buffer[0] = '\0';
		} else {
			agregar_char(temporal);
		}
	}else if(bandera_m3){
		if (temporal == 'H') {
			adc_adafruit = atoi(&buffer[0]);
			// Usar valor... salida pb1
			if (modo == 3){
				OCR2A = ((adc_adafruit*30/255) +7);
			}
			
			bandera_m3 = false;
			// Reiniciar para siguiente número
			indice = 0;
			buffer[0] = '\0';
		} else {
			agregar_char(temporal);
		}
	}else if(bandera_m4){
		if (temporal == 'H') {
			adc_adafruit = atoi(&buffer[0]);
			// Usar valor... salida pb1
			if (modo == 3){
				OCR2B = ((adc_adafruit*30/255) +7);
			}
			
			bandera_m4 = false;
			// Reiniciar para siguiente número
			indice = 0;
			buffer[0] = '\0';
		} else {
			agregar_char(temporal);
		}
	}else if(bandera_posicion_eeprom){
		//aca elijo posicion
		if(temporal == '1'){
			posicion_eeprom = 1;	
		}else if(temporal == '2'){
			posicion_eeprom = 2;
		}else if (temporal == '3'){
			posicion_eeprom = 3;
		}else if (temporal == '4'){
			posicion_eeprom = 4;
		}
		bandera_posicion_eeprom = false;
	}
	
	//verifico que estoy recibiendo, es decir cambiar de modo o mover alguno de los 4 motores
	// esto lo hago verificando el primer caracter que recibo por serial.
	if(temporal == '.'){
		bandera_modo = true;
	}else if(temporal == 'A'){
		bandera_m1 = true;
		indice = 0;
		buffer[0] = '\0';
	}else if(temporal == 'B'){
		bandera_m2 = true;
		indice = 0;
		buffer[0] = '\0';
	}else if(temporal == 'C'){
		bandera_m3 = true;
		indice = 0;
		buffer[0] = '\0';
	}else if(temporal == 'D'){
		bandera_m4 = true;
		indice = 0;
		buffer[0] = '\0';
	} else if (temporal == 'P'){
		bandera_posicion_eeprom = true;
	}
	
}


