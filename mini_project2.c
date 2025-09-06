/*
 * mini_project2.c
 *
 *  Created on: Sep 18, 2023
 *      Author: Fady Essam
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Seven Segment displays */
unsigned char num_1= 0; // For Sec1//
unsigned char num_2= 0; // For Sec2//
unsigned char num_3= 0; // For Min1//
unsigned char num_4= 0; // For Min2//
unsigned char num_5= 0; // For Hour1//
unsigned char num_6= 0; // For Hour2//

void Timer1_CTC_Mode(unsigned short tick) /* Timer1 Function */
{
	TCNT1 = 0; /* Set the Initial value for the timer1 */
	TIMSK |= (1<<OCIE1A);
	TCCR1A |= (1<<FOC1A) ;
	TCCR1B = (1<<CS12) | (1<<CS10)| (1<<WGM12); /* Timer1 is in compare mode with prescaler = Fclk / 1024 */
	OCR1A = tick; /* 1sec will be done after 976 ticks */
	SREG |= (1<<7); /* Set the I_bit for timer1 */
}

ISR(TIMER1_COMPA_vect)
{
	/* Make the first 7_segment equals zero for Sec1 when it reaches 10 and start counting the second 7_segment for Sec2 */
	num_1++;
	if(num_1 == 10){
		num_1 = 0;
		num_2++;
	}
	/* Make the second 7_segment equals zero for Sec2 when it reaches 6 and start counting the third 7_segment for Min1 */
	if(num_2 == 6){
		num_2 = 0;
		num_3++;
	}
	/* Make the third 7_segment equals zero for Min1 when it reaches 10 and start counting the fourth 7_segment for Min2 */
	if(num_3 == 10){
		num_3 = 0;
		num_4++;
	}
	/* Make the fourth 7_segment equals zero for Min2 when it reaches 6 and start counting the fifth 7_segment for Hour1 */
	if(num_4 == 6){
		num_4 = 0;
		num_5++;
	}
	/* Make the fifth 7_segment equals zero for Hour1 when it reaches 10 and start counting the Sixth 7_segment for Hour2 */
	if(num_5 == 10){
		num_5 = 0;
		num_6++;
	}

	/* Reset the timer when the sixth segment reaches 10 Hour2 */

	if(num_6 == 10){
		num_1 = 0;
		num_2 = 0;
		num_3 = 0;
		num_4 = 0;
		num_5 = 0;
		num_6 = 0;
	}

}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	//configure INT0 as input pin//
	DDRD &=~ (1<<2);
	PORTD |= (1<<2); /* Enable PULL UP */
	MCUCR |= (1<<ISC01);
	MCUCR &=~ (1<<ISC00); /* Trigger INT0 with Falling edge */
	SREG |= (1<<7); /* Set the I_bit for INT0 */
	GICR |= (1<<INT0); /* Enable external interrupt pin INT0 */
}

/* External Interrupt Service Routine for INT0 */
ISR(INT0_vect)
{
	/* Function is to reset the STOP WATCH */
	//reset the timer//
	TCNT1 = 0;
	//reset the SEVEN_SEGMENT//
	num_1 = 0;
	num_2 = 0;
	num_3 = 0;
	num_4 = 0;
	num_5 = 0;
	num_6 = 0;
	//to resume counter again after it was reset : CS10 = 1,CS12 = 1 //
	TCCR1B |= (1<<CS10) | (1<<CS12);

}
/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	//configure INT1 as input pin//
	DDRD &=~ (1<<3);
	PORTD &=~ (1<<3); /* Enable PULL DOWN */
	MCUCR |= (1<<ISC11)| (1<<ISC10); /* Trigger INT1 with Rising edge */
	SREG |= (1<<7);  /* Set the I_bit for INT1 */
	GICR |= (1<<INT1); /* Enable external interrupt pin INT1 */
}

/* External Interrupt Service Routine for INT1 */
ISR(INT1_vect)
{
	/* Function is to stop the STOP WATCH */
	//to stop counter : CS10 = 0,CS12 = 0 //
	TCCR1B &=~ (1<<CS10) &~ (1<<CS12);
}

/* External INT2 enable and configuration function */
void INT2_Init(void)
{
	DDRB &=~ (1<<2);
	//configure INT2 as input pin//
	PORTB |= (1<<2);
	/* Enable PULL UP */
	MCUCSR &=~ (1<<ISC2);
	/* Trigger INT2 with Falling edge */
	SREG |= (1<<7);
	/* Set the I_bit for INT1 */
	GICR |= (1<<INT2);
	/* Enable external interrupt pin INT2 */
}

/* External Interrupt Service Routine for INT2 */
ISR(INT2_vect)
{
	/* Function is to resume the STOP WATCH */
	//to resume counter : CS10 = 1,CS12 = 1 //
	TCCR1B |= (1<<CS10) | (1<<CS12);
}

int main (void)
{

	DDRC |= 0x0F;
	//Configure the first 4 pins in the decoder as output pins //
	PORTC &= 0xF0;
	//Set the first 4 pins in the decoder with value 0 //
	DDRA |= 0x3F;
	//Configure the first 6 pins of the SEVEN_SEGMENT in port A as output pins //
	INT0_Init();
	// Calling INT0 Function //
	INT1_Init();
	// Calling INT1 Function //
	INT2_Init();
	// Calling INT2 Function //
	Timer1_CTC_Mode(976);
	// Calling timer Function //


	while(1)
	{

		PORTA |= 0x01;
		PORTC = (PORTC & 0xF0) | (num_1 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x01;


		PORTA |= 0x02;
		PORTC = (PORTC & 0xF0) | (num_2 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x02;


		PORTA |= 0x04;
		PORTC = (PORTC & 0xF0) | (num_3 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x04;


		PORTA |= 0x08;
		PORTC = (PORTC & 0xF0) | (num_4 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x08;


		PORTA |= 0x10;
		PORTC = (PORTC & 0xF0) | (num_5 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x10;


		PORTA |= 0x20;
		PORTC = (PORTC & 0xF0) | (num_6 & 0x0F);
		_delay_ms(2);
		PORTA &=~ 0x20;

	}

}


