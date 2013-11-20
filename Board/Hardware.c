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

uint8_t StartTemperatureController(uint8_t Record)
{
	//Green LED is on when the controller is active
	/*LED(2,1);
	
	RedTempAverage = 0;
	BlackTempAverage = 0;
	IntTempAverage = 0;
	HeaterVoltageAverage = 0;
	HeaterCurrentAverage = 0;
	NumberOfSamples = 0;
	
	
	
	//TODO: Check for restart here
	//check for overload bits and clear them if possible?
	
	//Calibrate the device
	
	//Write this status to EEMEM to check for restarts.
	BH_SetStatus(BH_STATUS_PROG, BH_STATUS_PROG_CONTROL_ON, 1);
	
	Datalogger_Init( (DATALOGGER_INIT_APPEND|DATALOGGER_INIT_RESTART_IF_FULL) );
	
	//Check the heater temperature?
	//Start data recording here...
	
*/
	return 0;
}

uint8_t StopTemperatureController(uint8_t Discard)
{
	//BH_SetStatus(BH_STATUS_PROG, BH_STATUS_PROG_CONTROL_ON, 0);
	
	//Write final datapoint?
	//Write final status to EEMEM to avoid accidental restarts
	
	
	//Turn off green LED
	//LED(2,0);
	
	return 0;
}


void TemperatureControllerTask( void )
{
	uint8_t ProgStatus;
	
	/*ProgStatus = BH_GetStatus(BH_STATUS_PROG);

	if(CountsFromRTC == 10 && ((ProgStatus&BH_STATUS_PROG_CONTROL_ON) == BH_STATUS_PROG_CONTROL_ON))
	{
		LED(3,1);
		
		uint8_t Dataset[16];
		GetData(Dataset);
		
		if(NumberOfSamples < 6)
		{
			
			//Add this sample to the average
			
		
		
		
		
			NumberOfSamples++;
		}
		else
		{
			//Write samples to datalogger
			//clear averages
		}
		
		
		
		
		LED(3,0);
		
		//printf_P(PSTR("Taking Measurments\n"));
		CountsFromRTC = 0;
	}
*/

	return;
}

void HandleButtonPress( void )
{
	uint8_t ButtonsPending;
	
	/*ButtonsPending = BH_GetStatus(BH_STATUS_HIO);
	if((ButtonsPending & BH_STATUS_HIO_B1_PEND) == BH_STATUS_HIO_B1_PEND)
	{
		//Red button was pressed
		//printf_P(PSTR("b1\n"));
		BH_SetStatus(BH_STATUS_HIO, BH_STATUS_HIO_B1_PEND, 0);
		
		if((ButtonsPending & BH_STATUS_HIO_LED1) == BH_STATUS_HIO_LED1)
		{
			if((ButtonsPending & BH_STATUS_HIO_LED2) == BH_STATUS_HIO_LED2)
			{
				if((ButtonsPending & BH_STATUS_HIO_LED3) == BH_STATUS_HIO_LED3)
				{
					return;
				}
				else
				{
					LED(3,1);
				}
			}
			else
			{
				LED(2,1);
			}
		}
		else
		{
			LED(1,1);
		}
		
	}
	else if((ButtonsPending & BH_STATUS_HIO_B2_PEND) == BH_STATUS_HIO_B2_PEND)
	{
		//Black button was pressed
		//printf_P(PSTR("b2\n"));
		BH_SetStatus(BH_STATUS_HIO, BH_STATUS_HIO_B2_PEND, 0);
		
		if((ButtonsPending & BH_STATUS_HIO_LED3) == 0x00)
		{
			if((ButtonsPending & BH_STATUS_HIO_LED2) == 0x00)
			{
				if((ButtonsPending & BH_STATUS_HIO_LED1) == 0x00)
				{
					return;
				}
				else
				{
					LED(1,0);
				}
			}
			else
			{
				LED(2,0);
			}
		}
		else
		{
			LED(3,0);
		}
	}*/
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

void Beep(uint16_t BeepTimeMS)
{
	//BuzzerOn();
	//DelayMS(BeepTimeMS);
	//BuzzerOff();
}

void BuzzerOn(void)
{
	//Clear timer
	//TCNT3H = 0x00;
	//TCNT3L = 0x00;

	//Turn on timer
	//TCCR3B = 0x09;
	return;
}


void BuzzerOff(void)
{
	//Turn off timer
	//TCCR3B = 0x00;
	//PORTF &= ~(1<<5);
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

void Relay(uint8_t RelayState)
{
	/*if(RelayState == 1)
	{
		PORTD |= (1<<6);
	}
	else
	{
		PORTD &= ~(1<<6);
	}*/
	return;
}

/** Determine the zero point for the current sensor and save it into EEPROM. The zero point of the current sensor is measured by shutting down the relay and measuring the reading from the sensor. */
void CalibrateHeaterCurrent(void)
{
	/*uint32_t CalValue;
	uint8_t CalString[3];
	
	//eeprom_read_block((void*)&CalString, (const void*)&NV_CURRENT_ZERO_CAL, 3);
	//CalValue = CalString[2] + CalString[1]*256 + CalString[0]*65536;
	
	//printf_P(PSTR("Old Calibration 0x%06lX\n"), CalValue);
	//printf_P(PSTR("Old Calibration 0x%02X 0x%02X 0x%02X\n"), CalString[0], CalString[1], CalString[2]);
	//Turn of the relay
	//TODO: try to save the current state of the relay first?
	Relay(0);

	CalValue = GetHeaterCurrent();
	CalValue += GetHeaterCurrent();
	CalValue += GetHeaterCurrent();
	CalValue += GetHeaterCurrent();
	CalValue += GetHeaterCurrent();
	CalValue = CalValue / 5;
	
	//printf_P(PSTR("New Calibration 0x%06lX\n"), CalValue);
	
	CalString[0] = ((CalValue>>16) & 0xFF);
	CalString[1] = ((CalValue>>8) & 0xFF);
	CalString[2] = (CalValue & 0xFF);
	
	//printf_P(PSTR("New Calibration 0x%02X 0x%02X 0x%02X\n"), CalString[0], CalString[1], CalString[2]);
	
	eeprom_update_block ((const void*)CalString, (void*)NV_CURRENT_ZERO_CAL, 3);
*/
	return;
}

//Get a full set of data from the A/D converter
//This function will be used by the datalogger to get data to save
//TODO: Process the thermistor data into deg C in this function instead of other places
//TODO: Change decimal storage format to two 16-bit numbers, one containing the LHS and sign, the other containing the RHS.
//TODO: Add calibraion for the current sensor where it shuts off the relay and measures current
void GetData(uint8_t *TheData)
{
	//uint8_t SendData[3];
	//uint32_t TempData;
	//uint8_t DataLocation;
	
	//DataLocation = 0;
	
	//Turn on excitation current to red thermistor
	//SendData[0] = (AD7794_IO_DIR_IOUT1 | AD7794_IO_10UA);
	//AD7794WriteReg(AD7794_CR_REG_IO, SendData);

	//Set up channel 2 (red thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	/*SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();*/
	//TempData = GetRedTemp();
	//TheData[0] = ((TempData>>16) & 0xFF);
	//TheData[1] = ((TempData>>8) & 0xFF);
	//TheData[2] = (TempData & 0xFF);
	//printf_P(PSTR("Red: %lu counts or %d %d %d\n"), TempData, TheData[0], TheData[1], TheData[2]);
	
	//Turn on excitation current to black thermistor
	//SendData[0] = (AD7794_IO_DIR_IOUT2 | AD7794_IO_10UA);
	//AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Set up channel 3 (black thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	/*SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();*/
	//TempData = GetBlackTemp();
	//TheData[3] = ((TempData>>16) & 0xFF);
	//TheData[4] = ((TempData>>8) & 0xFF);
	//TheData[5] = (TempData & 0xFF);
	
	//printf_P(PSTR("Black: %lu counts or %d %d %d\n"), TempData, TheData[3], TheData[4], TheData[5]);

	//Turn off excitation currents
	//SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	//AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Set up channel 6 (heater voltage)
	//	-Unipolar
	//	-Gain of 1
	//	-Internal 1.17V reference
	//	-Buffered
	/*SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();*/
	//TempData = GetHeaterVoltage();
	//TheData[6] = ((TempData>>16) & 0xFF);
	//TheData[7] = ((TempData>>8) & 0xFF);
	//TheData[8] = (TempData & 0xFF);
	
	//printf_P(PSTR("Heater Voltage: %lu counts or %d %d %d\n"), TempData, TheData[6], TheData[7], TheData[8]);
	
	//Get internal temperature
	//TempData = AD7794GetInternalTemp();
	//TheData[9] = ((TempData>>16) & 0xFF);
	//TheData[10] = ((TempData>>8) & 0xFF);
	//TheData[11] = (TempData & 0xFF);
	
	//printf_P(PSTR("Internal Temperature: %lu counts or %d %d %d\n"), TempData, TheData[9], TheData[10], TheData[11]);
	
	//Set up channel 1 (heater current)
	//	-Bipolar
	//	-Gain of 1
	//	-Internal 1.17V reference
	//	-Buffered
	/*SendData[1] = (AD7794_CRH_BIAS_AIN1|AD7794_CRH_BIPOLAR|AD7794_CRH_BOOST|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();*/
	//TempData = GetHeaterCurrent();
	//TheData[12] = ((TempData>>16) & 0xFF);
	//TheData[13] = ((TempData>>8) & 0xFF);
	//TheData[14] = (TempData & 0xFF);
	
	//printf_P(PSTR("Heater Current: %lu counts or %d %d %d\n"), TempData, TheData[12], TheData[13], TheData[14]);

	return;
}

/** Get the heater current from channel 1 of the AD7794. Heater current is measured by an ACS711 current sensor on the low side of the DC plug.
*
*	Channel 1 setup:
*		-Bipolar
*		-AIN1- biased to VCC/2
*		-Bias boost on
*		-Gain of 1
*		-Internal 1.17V reference
*		-Buffered	
*/
uint32_t GetHeaterCurrent(void)
{
	/*uint8_t SendData[3];
	uint32_t TempData;
	
	SendData[1] = (AD7794_CRH_BIAS_AIN1|AD7794_CRH_BIPOLAR|AD7794_CRH_BOOST|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();*/
	return 0;
}

/** Get the heater voltage from channel 6 of the AD7794. Heater current is measured through a voltage divider on the DC input line.
*	Resistors for the divider are 280K/10K. This gives a maximum measurement range of ~34V.
*
*	Channel 6 setup:
*		-Unipolar
*		-Gain of 1
*		-Internal 1.17V reference
*		-Buffered
*/
uint32_t GetHeaterVoltage(void)
{
	/*uint8_t SendData[3];
	uint32_t TempData;

	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();*/
	return 0;
}

/** Get the thermistor reading from the red plug attached to channel 2. The thermistor is excited by a 10nA current.
*
*	Channel 2 setup:
*		-Unipolar
*		-Gain of 2
*		-Internal 1.17V reference
*		-Buffered
*/
uint32_t GetRedTemp(void)
{
	/*uint8_t SendData[3];
	uint32_t TempData;

	//Turn on excitation current to red thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT1 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);

	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	
	//Turn off excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	*/
	return 0;
}

/** Get the thermistor reading from the black plug attached to channel 3. The thermistor is excited by a 10nA current.
*
*	Channel 3 setup:
*		-Unipolar
*		-Gain of 2
*		-Internal 1.17V reference
*		-Buffered
*/
uint32_t GetBlackTemp(void)
{
	/*uint8_t SendData[3];
	uint32_t TempData;
	
	//Turn on excitation current to black thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT2 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	TempData = AD7794GetData();
	
	//Turn off excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	*/
	return 0;
}










//Converts the measured heater voltage to the real voltage in volts/10000
//Uses the measured values of resistors as R1=10K, R2=279.2K
//TODO: Make these resistances compiler defines?
//TODO: These functions should probably be combined somewhere
uint32_t ConvertHeaterVoltage(uint32_t InputCounts)
{
	//double HeaterVoltage;
	//HeaterVoltage = ((double)InputCounts*1.17F*289.2F)/((double)(0xFFFFFF * 10));
	//return (uint32_t)(HeaterVoltage*10000);
	return 0;
}

//This measurment is assumed to be taken in bipolar mode
int32_t ConvertHeaterCurrent(uint32_t InputCounts)
{
	/*int32_t CountsFromZero;
	double HeaterCurrent;
	uint32_t CalValue;
	uint8_t CalString[3];
	
	//Get the calibraion data
	eeprom_read_block((void*)&CalString, (const void*)&NV_CURRENT_ZERO_CAL, 3);
	if((CalString[2] == 0xFF) && (CalString[2] == 0xFF) && (CalString[2] == 0xFF))
	{
		//If no calibraion exsists
		//TODO: Add a warning here?
		CalValue = 8388608;
	}
	else
	{
		CalValue = CalString[2] + CalString[1]*256 + CalString[0]*65536;
	}
	//printf_P(PSTR("Cal: 0x%06lX\n"), CalValue);
	//printf_P(PSTR("Counts: 0x%06lX\n"), InputCounts);
	
	if(InputCounts > CalValue)
	{
		CountsFromZero = (int32_t)(InputCounts-CalValue);
		
	}
	else
	{
		CountsFromZero = -1*((int32_t)(CalValue-InputCounts));
	}
	//CountsFromZero = -4000;
	//printf_P(PSTR("Counts fz: 0x%06lX\n"), CountsFromZero);
	//printf_P(PSTR("Counts fz: %ld\n"), CountsFromZero);
	HeaterCurrent = ((double)CountsFromZero*1170.0F)/((double)(0xFFFFFF));
	HeaterCurrent = HeaterCurrent / (double)(220);*/
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
