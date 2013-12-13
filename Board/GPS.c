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
*	\brief		GPS Functions.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		12/11/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

/** Returns 0 if the device initalizes correctly, 0xFF if not */
uint8_t GPS_Init(void)
{

	//Set up PORT C pin 7 for enabling the antenna bias
	DDRC |= (1<<6)|(1<<7);
	GPS_SetBias(0);		//Bias is off
	GPS_Reset(1);		//GPS is reset

	//Set up UART
	UARTinit();
	
	return 0x00;
}

//bias = 1 turns on the bias
//Bias switch is active low
void GPS_SetBias(uint8_t Bias)
{
	if(Bias == 1)
	{
		//Bias on
		PORTC &= ~(1<<7);
	}
	else
	{
		//Bias off
		PORTC |= (1<<7);
	}
	return;
}

//ToReset = 1 to reset the GPS
void GPS_Reset(uint8_t ToReset)
{
	if(ToReset == 1)
	{
		//Reset
		PORTC &= ~(1<<6);
	}
	else
	{
		//Unreset
		PORTC |= (1<<6);
	}
	return;
}

ISR(USART1_RX_vect)
{
	uint8_t c;
	c = UDR1;			//Get char from UART recieve buffer
	printf("%c", c);
}


/** @} */