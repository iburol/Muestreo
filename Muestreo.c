/*
 * Análisis espectral de vibraciones: Arduino + acelerómetro ADXL335.c
 *
 * Created: 07/05/2015 13:49:21
 * Author: Ismael Burgos
 *
 *
 */
 
 #define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TX_Buffer 6
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD) - 1)

void ADC_Init (void);
void USART_Init (void);
void TIMER1_Init (void);
void InicioConversion (void);

uint8_t dato[TX_Buffer];
int TX_i = 0;

void main (void)
{
	USART_Init();
	ADC_Init();
	TIMER1_Init();
	
	sei();
	
	while(1)
	{
		
	}
}

//Inicialmente configurado para conversión por el canal 0
void ADC_Init(void)
{
	ADMUX = (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

//Configuración básica USART
void USART_Init (void)
{
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

//Timer1 a 10 ms.
//100 muestras cada seg. (100 hz)
//Habilitada interrupcíón modo CTC
void TIMER1_Init (void)
{
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	OCR1A = 2499;
	TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
  	//Llamada a función para la conversión
	InicioConversion();
	//Habilitación de Interrupción de Registro de Datos Vacío del USART 
	UCSR0B |= (1 << UDRIE0);
}

void InicioConversion (void)
{
	//Incio de conversión simple
	ADCSRA |= (1<<ADSC);
	//Espera hasta fin de conversión
	while(ADCSRA & (1<<ADSC));
	//Datos obtenidos en la conversión al buffer de datos
	dato[0] = ADCL;
	dato[1] = ADCH;
	//Limpieza bits selección de canal registro ADMUX
	ADMUX &= 0xC8;
	//Configuración siguiente canal 1 del ADC
	ADMUX |= (1 << REFS0) | (1 << MUX0);
	_delay_us(15);
	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	dato[2] = ADCL;
	dato[3] = ADCH;
	ADMUX &= 0xC8;
	//Configuración siguiente canal 2 del ADC
	ADMUX |= (1 << REFS0) | (1 << MUX1);
	_delay_us(15);
	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	dato[4] = ADCL;
	dato[5] = ADCH;
	ADMUX &= 0xC8;
	//Configuración siguiente canal 0 del ADC
	ADMUX |= (1 << REFS0);
	_delay_us(15);
}

//Función de interrupcion del USART
//Si el contador de bytes enviados es inferior al tamaño del buffer de datos se sigue enviando bytes
//... en caso contrario, se limpia el contador de bytes enviados y se deshabilita la interrupción
//del USART
ISR(USART_UDRE_vect)
{
	if (TX_i < TX_Buffer)
	{
		UDR0 = dato[TX_i];
		TX_i++;
	}
  	else
	{
		TX_i = 0;
		UCSR0B &= ~(1 << UDRIE0);
	}
}