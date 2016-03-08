/***************************************************************************
*
* $Revision: 1.4 $
* $Date: Monday, September 13, 2004 12:08:54 UTC
* $Author: L.M.Zuccarelli
*
****************************************************************************/

#include <avr/io.h>
#include <util/delay.h>

//clock I/O pin.  7 PB2 <- CLK PI 22
//data output pin.6 PB1 -> DI PI 23
//data input pin. 5 PB0 <- DO PI 27

// ATTINY85
// ADC0 PB5  1    8 VCC
// ADC3 PB3  2    7 PB2 ADC1 CLK
// ADC2 PB4  3    6 PB1 ADC2 MISO
// GND       4    5 PB0 AREF MOSI

// PORTB  =   –   |   –   | PORTB5 | PORTB4 | PORTB3 | PORTB2 | PORTB1 | PORTB0
// DDRB   =   –   |   –   | DDB5   | DDB4   | DDB3   | DDB2   | DDB1   | DDB0
// PINB   =   –   |   –   | PINB5  | PINB4  | PINB3  | PINB2  | PINB1  | PINB0

// ADMUX  = REFS1 | REFS0 | ADLAR  | REFS2  | MUX3   | MUX2   | MUX1   | MUX0
// ADCSRA = ADEN  | ADSC  | ADATE  | ADIF   | ADIE   | ADPS2  | ADPS1  | ADPS0
// ADCSRB = BIN   | ACME  | IPR    |   –    |   –    | ADTS2  | ADTS1  | ADTS0
// DIDR0  =   –   |   –   | ADC0D  | ADC2D  | ADC3D  | ADC1D  | AIN1D  | AIN0D
// ADCL
// ADCH

//         MSB                                                 LSB
// REG     7      6      5      4      3       2       1       0
// USIDR
// USIBR
// USICR   USISIE USIOIE USIWM1 USIWM0 USICS1  USICS0  USICLK  USITC
// USISR   USISIF USIOIF USIPF  USIDC  USICNT3 USICNT2 USICNT1 USICNT0

// For 3 wire spi mode set USIWM1=0 and USIWM0=1
// External clock rising edge set USICS1=1  USICS0=0 USICLK=0
// Enable counter overflow interrupt USIOIE=1


void spiInitSlave(void) {
	// Configure port directions.
	DDRB =  0b00000010; // PB1 output
	PORTB = 0b00000101; // PB0 and PB2 pull up
	
	// Configure USI to 3-wire slave mode with no overflow interrupt.
	USICR = 0b00011000;
	USISR = 0b00000000;
}

unsigned char spiGet(void) {
	//unsigned char in;
	USICR = 0b00011000;
	USISR = 0b01000000;
	while ( (USISR & 0b01000000) != 0b01000000 ){}
	//in = USIDR;
	USISR = 0b00000000;
	USICR = 0b00010000;
	return USIDR;
}

void spiPut(unsigned char byte) {
	USIDR = byte;
	USICR = 0b00011000;
	USISR = 0b01000000;
	while ( (USISR & 0b01000000) != 0b01000000 ){}
	USISR = 0b00000000;
	USICR = 0b00010000;
}

void initADC(void) {
	ADMUX  = 0b10010011; // 7:6-Internal 2.56v reference; ADC3 PB3 2
	ADCSRA = 0b10000110; // ADC turned on; conversion not started; auto trigger off; interrupt flag; interrupt disabled; Prescaler divide by 64
	ADCSRB = 0b00000000; // Free running
	return;
}

int main(void) {

	unsigned char avgHi_A = 0;
	unsigned char avgLo_A = 0;
	unsigned char avgHi_B = 0;
	unsigned char avgLo_B = 0;
	unsigned char inSpi = 0;
	int nLoop;

	initADC();
  spiInitSlave();

	// delay  for settling
	_delay_ms(100);

  while(1) {

		ADMUX  = 0b10010011; // 7:6-Internal 2.56v reference; ADC3 PB3 2
		_delay_us(100);

		for (nLoop = 0; nLoop < 5 ; nLoop++) {
			ADCSRA |= (1<<ADSC);
			while (!(ADCSRA & (1<<ADSC))){}
			avgLo_A = ADCL;
			avgHi_A = ADCH;
		}

		// now switch to the second channel
		ADMUX  = 0b10010010; // 7:6-Internal 2.56v reference; ADC2 PB4 3
		_delay_us(100);

		for (nLoop = 0; nLoop < 5 ; nLoop++) {
			ADCSRA |= (1<<ADSC);
			while (!(ADCSRA & (1<<ADSC))){}
			avgLo_B = ADCL;
			avgHi_B = ADCH;
		}

		// wait for input from master
		inSpi = spiGet();
		// valid address
		if ((inSpi & 0b11111111) == 0b00000011) {
		  // send adc lo byte
			spiPut(avgLo_A);
			_delay_us(50);
			// send adc hi byte
			spiPut(avgHi_A);

			_delay_us(50);
			// send adc hi byte
			spiPut(avgLo_B);
			// send adc lo byte
			_delay_us(50);
			// send adc hi byte
			spiPut(avgHi_B);

		}
  }

}
