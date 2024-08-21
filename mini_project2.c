/**********************************************************************************************************
 * [FILE NAME]		:			Mini_Project2.c													          *
 * [DESCRIPTION]	:			Stop Watch Project											              *
 * [DATE CREATED]	: 			May 5, 2024															      *
 * [AURTHOR]		: 			Karima Mahmoud															  *
 *********************************************************************************************************/

/*******************************************Include Libraries*********************************************/
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
/***************************************Global Variables Definition***************************************/
unsigned char sec_digit1 = 0;
unsigned char sec_digit2 = 0;
unsigned char min_digit1 = 0;
unsigned char min_digit2 = 0;
unsigned char hr_digit1 = 0;
unsigned char hr_digit2 = 0;

/***************************************Interrupt Surface Routineس***************************************/
/*Timer1 ISR counts the seconds, , and hours  */
ISR(TIMER1_COMPA_vect){
	if(9 == (sec_digit1 & 0x0F))
	{
		sec_digit1 = 0;
		sec_digit2++;
	}
	else
	{
		sec_digit1++;
	}
	if((6 == (sec_digit2 & 0x0F)) && (0 == (sec_digit1 & 0x0F)))
	{
		sec_digit1 = 0;
		sec_digit2 = 0;
		if(9 == (min_digit1 & 0x0F))
		{
			min_digit1 = 0;
			min_digit2++;
		}
		else
		{
			min_digit1++;
		}
	}
	if((6 == (min_digit2 & 0x0F)) && (0 == (min_digit1 & 0x0F)))
	{
		min_digit1 = 0;
		min_digit2 = 0;
		if(9 == (hr_digit1 & 0x0F))
		{
			hr_digit1 = 0;
			hr_digit2++;
		}
		else
		{
			hr_digit1++;
		}
	}
}

/* INT0 ISR resets the stop watch at each falling edge detected at INT0 pin */
ISR(INT0_vect)
{
	/* Reset all counters --> Reset Stop watch */
	sec_digit1 = 0;
	sec_digit2 = 0;
	min_digit1 = 0;
	min_digit2 = 0;
	hr_digit1 = 0;
	hr_digit2 = 0;
}

/* INT1 ISR pauses the stop watch at each rising edge detected at INT1 pin */
ISR(INT1_vect)
{
	TCCR1B &= 0xF8; /* stop clk source (Timer stopped) */
}


/*INT2 ISR resumes the stop watch at each falling edge detected at INT2 pin */
ISR(INT2_vect)
{
	TCCR1B |= 0x05; /* Enable clock again with prescaler = 1024 */
}

/***************************************Functions Definitions***************************************/
/* Timer1 Registers initialization */
void Timer1_CTC_Init()
{
	TCNT1 = 0;
	OCR1A = 976;           /* configure compare value with 976 to count one second at each interrupt using N = 1024 and FCPU = 1Mhz */
	TCCR1A |= (1<<FOC1A);  /* Force output compare at compare unit A */
	TCCR1B |= (1<<CS12) | (1<<CS10) | (1<<WGM12);  /*N = 1024*/
	TIMSK |= (1<<OCIE1A);  /* Enable Timer1 Interrupt */
}

/* External Interrupts initialization */
void EXT_INT_Init()
{
	DDRD &= 0xF3;      /* Configure INT0 and INT1 pins as input pins*/
	PORTD |= (1<<PD2); /* Activate pull-up resistor on PD2 */
	DDRB &= ~(1<<PB2); /* Configure INT2 pin as input pin */
	PORTB |= (1<<PB2); /* Activate pull-up resistor on PB2 */
	/* Trigger INT0 with falling edge
	 * Trigger INT1 with rising edge
	 * Trigger INT2 with falling edge --> ISC2 = 0 */
	MCUCR |= (1<<ISC01) | (1<<ISC11) | (1<<ISC10);
	MCUCSR &= ~(1<<ISC2);
	/* Enable INT0, INT1, and INT2 */
	GICR |= 0xE0;
}

/* Seven Segment Initialization */
void Seven_Seg_Init()
{
	DDRC |= 0x0F; 			 // Configure the first four pins in PORTC as output pins.
	PORTC &= 0xF0; 		    // 7-segment display zero at the beginning.
	DDRA |= 0x3F;          // Configure the first six pins in PORTA as output pins.
	PORTA &= 0xC0;        // Clear all the six pins at the first
}

void Seven_Seg_Display()
{
	/* Enable the first seven segment to display the lower digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA0);
	/* Display the lower digit of seconds */
	PORTC = (PORTC & 0xF0) | (sec_digit1 & 0x0F);
	_delay_ms(2);

	/* Enable the second seven segment to display the higher digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA1);
	/* Display the higher digit of seconds */
	PORTC = (PORTC & 0xF0) | (sec_digit2 & 0x0F);
	_delay_ms(4);

	/* Enable the third seven segment to display the lower digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA2);
	/* Display the lower digit of minutes */
	PORTC = (PORTC & 0xF0) | (min_digit1 & 0x0F);
	_delay_ms(4);

	/* Enable the forth seven segment to display the higher digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA3);
	/* Display the higher digit of minutes */
	PORTC = (PORTC & 0xF0) | (min_digit2 & 0x0F);
	_delay_ms(4);

	/* Enable the fifth seven segment to display the lower digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA4);
	/* Display the lower digit of hours */
	PORTC = (PORTC & 0xF0) | (hr_digit1 & 0x0F);
	_delay_ms(4);

	/* Enable the Sixth seven segment to display the higher digit of seconds */
	PORTA = (PORTA & 0xC0) | (1<<PA5);
	/* Display the higher digit of hours */
	PORTC = (PORTC & 0xF0) | (hr_digit2 & 0x0F);
	_delay_ms(4);
}

int main(void)
{
	SREG |= (1<<7);
	Seven_Seg_Init();
	Timer1_CTC_Init();
	EXT_INT_Init();

	while(1)
	{
		Seven_Seg_Display();
	}
}
