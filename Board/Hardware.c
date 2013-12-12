/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Hardware definitions and functions for the beer heater.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/26/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

volatile uint16_t	ElapsedMS;
volatile uint8_t CountsFromRTC;

//uint8_t ButtonState;
//uint8_t OldButtonState;

/**Saftey limits. If these temperatures are exceeded, the relay will be shut off. */
//uint8_t EEMEM NV_RED_TEMP_SAFTEY_LIMIT;				//The maximum allowable temperature on the red thermistor (in degrees C)
//uint8_t EEMEM NV_BLACK_TEMP_SAFTEY_LIMIT;			//The maximum allowable temperature on the black thermistor (in degrees C)
//uint8_t EEMEM NV_INTERNAL_TEMP_SAFTEY_LIMIT;		//The maximum allowable temperature on the ADC temperature sensor (in degrees C)

//uint8_t EEMEM NV_SET_TEMPERATURE;					//The target fermentation temperature (in degrees C)
//uint8_t EEMEM NV_TEMP_REGULATING = 0;				//Set to 1 when the temperature regulation is active
//uint8_t EEMEM NV_CURRENT_ZERO_CAL[3];				//The zero point for the current calibration


/**This is a set of data that is saved for the running average */
//TODO: Make this into a struct?
uint8_t NumberOfSamples;
uint32_t RedTempAverage;
uint32_t BlackTempAverage;
uint32_t IntTempAverage;
uint32_t HeaterVoltageAverage;
uint32_t HeaterCurrentAverage;




void HardwareInit( void )
{
	//uint8_t stat;
	
	BH_InitStatus();

	CountsFromRTC = 0;
	ElapsedMS	= 0x0000;
	//OldButtonState = 0xFF;
	//ButtonState = 0xFF;
	
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	//Disable JTAG (this command must be sent twice)
	MCUCR = 0x80;
	MCUCR = 0x80;

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	//LEDs_Init();
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);
	InitTWI();

	
	//Setup timer 0 for 1ms interrupts
	//CTC Mode
	//Clock is Fcpu/64
	//OCR0A interrupt ~every 1ms
	TCCR0A = 0x02;
	TCCR0B = 0x03;
	TIMSK0 = 0x02;
	OCR0A = 124;

	//Setup INT2 for button interrupts
	//EICRA = 0xA0;	//Interrupt on falling edge of INT2, Interrupt on any edge of INT3
	//EIFR = 0xFF;	//Clear pending interrupts
	//EIMSK = 0x0C;	//Enable INT2
	
	//Setup GPIO Pins
	
	//PORT B:
	//	0: Dataflash CS line	(Out, high)
	//DDRB |= (1<<0);
	//PORTB |= (1<<0);
	
	//PORT C:
	//	6: RTC CS line			(Out, high)
	//DDRC = 0x00;
	//PORTC = 0x00;
	
	//PORT D:
	//	2:  Button interrupt		(Input)
	//	3:	RTC interrupt			(Input, pullup)
	//	4:	A/D CS line				(Out, high)
	//	6:	Relay control signal	(Out, low)
	//DDRD = (1<<4)|(1<<6);
	//PORTD = (1<<3)|(1<<4);
	
	//PORT F:
	//	0:	LED	(Out, low)
	//	1:	LED	(Out, low)
	//	4:	LED	(Out, low)
	DDRF = (1<<0)|(1<<1)|(1<<4);
	PORTF = 0x00;

	//Setup timer 3 for buzzer
	//CTC mode
	//Clock is FCPU
	//This clock is not turned on here
	//TCCR3A = 0x00;
	//TCCR3B = 0x09;
	//OCR3AH = 0x03;
	//OCR3AL = 0xE8;
	//TIMSK3 = 0x02;
	//TCNT3H = 0x00;
	//TCNT3L = 0x00;
	
	//Initalize peripherals
	/*
	if(AD7794Init() == 0x00)	//TODO: check for SPI Init errors here and set the CPU flag as well...
	{
		BH_SetStatus(BH_STATUS_HW, BH_STATUS_HW_AD7794, STATUS_HW_OK);
	}
	//AD7794Init();
	
	if(MAX7315Init() == 0x00)	//TODO: check for I2C Init errors here and set the CPU flag as well...
	{
		BH_SetStatus(BH_STATUS_HW, BH_STATUS_HW_MAX7315, STATUS_HW_OK);
	}
	
	if(AT45DB321D_Init() == 0x00)	//todo: check for SPI Init errors here and set the CPU flag as well...
	{
		BH_SetStatus(BH_STATUS_HW, BH_STATUS_HW_AT45DB321D, STATUS_HW_OK);
	}
	//AT45DB321D_Init();
	
	if(DS3232M_Init() == 0x00)	//todo: check for SPI Init errors here and set the CPU flag as well...
	{
		BH_SetStatus(BH_STATUS_HW, BH_STATUS_HW_DS3232M, STATUS_HW_OK);
	}
	
	
	//DS3232M_Init();*/
	
	LSM303Init();
	
	
	//Enable USB and interrupts
	USB_Init();
	sei();
	
	return;
}

void LED(uint8_t LEDValue, uint8_t LEDState)
{
	uint8_t stat;
	
	//Check to see if the MAX7315 is initalized properly
	//stat = BH_GetStatus(BH_STATUS_HW);
	//if((stat & BH_STATUS_HW_MAX7315) != 0x00)
	//{
	//	return;
	//}
	
	if(LEDValue == 2)
	{
		if(LEDState == 1)
		{
			PORTF |= (1<<0);
		}
		else
		{
			PORTF &= ~(1<<0);
		}
		
	}
	else if(LEDValue == 3)
	{
		if(LEDState == 1)
		{
			PORTF |= (1<<1);
		}
		else
		{
			PORTF &= ~(1<<1);
		}
		
	}
	else if(LEDValue == 1)
	{
		if(LEDState == 1)
		{
			PORTF |= (1<<4);
		}
		
		else
		{
			PORTF &= ~(1<<4);
		}
		
	}
	return;
}

void HandleButtonPress( void )
{
	uint8_t ButtonsPending;
	return;
}

void DelayMS(uint16_t ms)
{
	uint16_t WaitMS = 0;
	
	if(ms == 0) return;
	
	//Delay is too long
	if (ms >= 60000)
	{
		return;
	}
	
	//Look for milisecond overflow
	if((ms + ElapsedMS) > 60000)
	{
		WaitMS = ms + ElapsedMS - 60001;
	}
	else
	{
		WaitMS = ms + ElapsedMS;
	}
	
	while (WaitMS != ElapsedMS)
	{
		asm volatile ("nop");
	}
	return;
}

//Check the state of the buttons
//The button state is inverted, as the output of the buttons is high when the button is not pressed
uint8_t GetButtonState( void )
{
	/*uint8_t DataToReceive;
	MAX7315ReadReg(MAX7315_REG_INPUTS, &DataToReceive);
	DataToReceive = (DataToReceive >> 4) & 0x03;*/
	//return ~DataToReceive;
	return 0;
}

//Timer interrupt 0 for basic timing stuff
ISR(TIMER0_COMPA_vect)
{
	uint16_t inByte;
	ElapsedMS++;
	
	//Handle USB stuff
	//This happens every ~8 ms
	if( ((ElapsedMS & 0x0007) == 0x0000) )// && (USB_IsInitialized == true) )
	{
		//receiv and process a character from the USB CDC interface
		inByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if((inByte > 0) && (inByte < 255))
		{
			CommandGetInputChar(inByte);	//NOTE: this limits the device to recieve a single character every 8ms (I think). This should not be a problem for user input.
		}
		
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
	
	//Count up to 60000ms (1 min) then reset to zero.
	if(ElapsedMS >= 60000)
	{
		ElapsedMS = 0;
	}
}

//Timer 3 toggles the buzzer pin
//ISR(TIMER3_COMPA_vect)
//{
//	PORTF ^= (1<<5);
//}

//Triggered on a change of button state
/*ISR(INT2_vect)
{
	uint8_t TheButtonState;
	uint8_t TheOldButtonState;
	
	//Get the button state
	TheButtonState = GetButtonState();
	//printf_P(PSTR("bs: 0x%02X\n"), TheButtonState);
	TheOldButtonState = BH_GetStatus(BH_STATUS_HIO);
	
	//Check to see what button state changed
	if ( ((TheButtonState & 0x01) == 0x01) && ((TheOldButtonState & BH_STATUS_HIO_B1_OLD) == 0x00) )
	{
		//Button 1 down
		//printf_P(PSTR("b1\n"));
		BH_SetStatus(BH_STATUS_HIO, BH_STATUS_HIO_B1_PEND, 1);
	}
	
	if ( ((TheButtonState & 0x02) == 0x02) && ((TheOldButtonState & BH_STATUS_HIO_B2_OLD) == 0x00) )
	{
		//Button 2 down
		//printf_P(PSTR("b2\n"));
		BH_SetStatus(BH_STATUS_HIO, BH_STATUS_HIO_B2_PEND, 1);
	}

}

ISR(INT3_vect)
{
	CountsFromRTC++;
	//printf_P(PSTR("%d\n"), CountsFromRTC);
	if(CountsFromRTC > 10)
	{
		CountsFromRTC = CountsFromRTC - 10;
	}
}*/

/** @} */
