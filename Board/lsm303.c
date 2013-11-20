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
*	\brief		Low level interface driver for LSM303 digital compass.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		12/19/2011
*	\copyright	Copyright 2011, Pat Satyshur
*
*	\ingroup	LSM303
*
*	@{
*/

#include "lsm303.h"
#include "NPSerial.h"

LSM303part LSM303Version;
uint8_t LSM303IsInitalized;

void LSM303Init( void )
{
	uint8_t stat;
	LSM303Version = LSM303DLH;
	LSM303IsInitalized = 0;
	
	//Detect and identify LSM303 part number
	stat = LSM303ReadReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_STATUS_REG);
	if(stat != 0x00)
	{
		LSM303Version = LSM303_NO_A;		//Accelerometer did not respond
	}
	
	stat = LSM303ReadReg(LSM303_MAGNETOMETER, LSM303_MAGNETOMETER_WHO_AM_I_DLM);
	if(stat != 0x00)
	{
		if(LSM303Version == LSM303_NO_A)
		{
			LSM303Version = LSM303_NONE;	//Neither accelerometer of magnetometer responded
		}
		else
		{
			LSM303Version = LSM303_NO_M;	//Magnetometer did not respond
		}
	}
	else if((LSM303ReadBuffer[0] == 0x3C) && (LSM303Version == LSM303DLH))
	{
		LSM303Version = LSM303DLM;
	}

	//If accelerometer of magnetometer do not respond, abort.
	if((LSM303Version == LSM303_NONE) || (LSM303Version == LSM303_NO_A) || (LSM303Version == LSM303_NO_M))
	{
		return;
	}

	//Setup accelerometer:
	//	Normal mode, output rate 50hz, all axis on.
	//	High pass used with INT1, cutoff frequency is .5Hz.
	//	INT1 latched, configured for INT1 interrupts.
	//	Block data updates while reading, LSB at lower address, full scale is +/-2G.
	//	High pass filter reference is 0x04	(whatever this means)
	//	'Turn on' feature not used.
	//	INT1 setup for as 6D OR.
	//	INT1 threshold set to 0x02 (whatever this means)
	//	INT1 duration set to 0x02 (whatever this means)
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_CTRL_REG1, LSM303_ACCELEROMETER_CR1_NORMAL|LSM303_ACCELEROMETER_CR1_ODR_50|LSM303_ACCELEROMETER_CR1_ALLON);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_CTRL_REG2, LSM303_ACCELEROMETER_CR2_HPF_NORMAL|LSM303_ACCELEROMETER_CR2_HPINT1_ON|LSM303_ACCELEROMETER_CR2_HPC_16);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_CTRL_REG3, LSM303_ACCELEROMETER_CR3_LIR1|LSM303_ACCELEROMETER_CR3_INT1_SRC_INT1);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_CTRL_REG4, LSM303_ACCELEROMETER_CR4_BDU);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_CTRL_REG5, 0x00);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_REFERENCE, 0x04);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_INT1_CFG, LSM303_ACCELEROMETER_INT_CFG_OR|LSM303_ACCELEROMETER_INT_CFG_6D|LSM303_ACCELEROMETER_INT_CFG_ALL);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_INT1_THS, 0x02);
	LSM303WriteReg(LSM303_ACCELEROMETER, LSM303_ACCELEROMETER_INT1_DURATION, 0x02);
	
	//Setup magnetometer
	LSM303WriteReg(LSM303_MAGNETOMETER, LSM303_MAGNETOMETER_CRA_REG, LSM303_MAGNETOMETER_DR_15HZ);
	LSM303WriteReg(LSM303_MAGNETOMETER, LSM303_MAGNETOMETER_CRB_REG, LSM303_MAGNETOMETER_GAIN_1_9G);
	LSM303WriteReg(LSM303_MAGNETOMETER, LSM303_MAGNETOMETER_MR_REG, LSM303_MAGNETOMETER_MODE_CONTINUOUS);
	
	LSM303IsInitalized = 1;
	return;
}


uint8_t LSM303WriteReg(LSM303Device LSM303_DEV, uint8_t RegToWrite, uint8_t ValueToWrite)
{
	int res = 0;
	LSM303WriteBuffer[0] = RegToWrite;
	LSM303WriteBuffer[1] = ValueToWrite;
	
	if(LSM303_DEV == LSM303_ACCELEROMETER)
	{
		res = TWIRW(LSM303_ACCELEROMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 2, 0);
	}
	else if (LSM303_DEV == LSM303_MAGNETOMETER)
	{
		res = TWIRW(LSM303_MAGNETOMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 2, 0);
	}
	return res;
}


uint8_t LSM303ReadReg(LSM303Device LSM303_DEV, uint8_t RegToRead)
{
	int res = 0;
	LSM303WriteBuffer[0] = RegToRead;

	if(LSM303_DEV == LSM303_ACCELEROMETER)
	{
		res = TWIRW(LSM303_ACCELEROMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 1, 1);
	}
	else if (LSM303_DEV == LSM303_MAGNETOMETER)
	{
		res = TWIRW(LSM303_MAGNETOMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 1, 1);
	}

	return res;
}

uint8_t LSM303ReadData(LSM303Device LSM303_DEV)
{
	int res;
	
	if(LSM303_DEV == LSM303_ACCELEROMETER)
	{
		LSM303WriteBuffer[0] = (LSM303_ACCELEROMETER_OUT_X_L | LSM303_ACCELEROMETER_READ_MULTIPLE);
		res = TWIRW(LSM303_ACCELEROMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 1, 6);
		if(res == 0)
		{
			//printf("%02X %02X %02X %02X %02X %02X\n", LSM303ReadBuffer[0], LSM303ReadBuffer[1], LSM303ReadBuffer[2], LSM303ReadBuffer[3], LSM303ReadBuffer[4], LSM303ReadBuffer[5]);
			LSM303AccelerometerData[0] = ~(LSM303ReadBuffer[0] | (LSM303ReadBuffer[1] << 8))+1;
			LSM303AccelerometerData[1] = ~(LSM303ReadBuffer[2] | (LSM303ReadBuffer[3] << 8))+1;
			LSM303AccelerometerData[2] = LSM303ReadBuffer[4] | (LSM303ReadBuffer[5] << 8);
			return 1;
		}
		return 0;
	}
	else if (LSM303_DEV == LSM303_MAGNETOMETER)
	{
		LSM303WriteBuffer[0] = LSM303_MAGNETOMETER_OUT_X_H;
		res = TWIRW(LSM303_MAGNETOMETER_ADDRESS, LSM303WriteBuffer, LSM303ReadBuffer, 1, 6);
		if(res == 0)
		{
			//printf("%02X %02X %02X %02X %02X %02X\n", LSM303ReadBuffer[0], LSM303ReadBuffer[1], LSM303ReadBuffer[2], LSM303ReadBuffer[3], LSM303ReadBuffer[4], LSM303ReadBuffer[5]);
			if(LSM303Version == LSM303DLM)
			{
				LSM303MagnetometerData[0] = ~(LSM303ReadBuffer[1] | (LSM303ReadBuffer[0] << 8))+1;	//MX
				LSM303MagnetometerData[1] = ~(LSM303ReadBuffer[5] | (LSM303ReadBuffer[4] << 8))+1;	//MY
				LSM303MagnetometerData[2] = LSM303ReadBuffer[3] | (LSM303ReadBuffer[2] << 8);			//MZ
			}
			else if (LSM303Version == LSM303DLH)
			{
				LSM303MagnetometerData[0] = ~(LSM303ReadBuffer[1] | (LSM303ReadBuffer[0] << 8))+1;	//MX
				LSM303MagnetometerData[1] = ~(LSM303ReadBuffer[3] | (LSM303ReadBuffer[2] << 8))+1;	//MY
				LSM303MagnetometerData[2] = LSM303ReadBuffer[5] | (LSM303ReadBuffer[4] << 8);			//MZ
			}
			return 1;
		}
		return 0;
	}
	return 0;
}

int8_t LSM303IsReg(LSM303Device LSM303_DEV, int reg)
{
	if(LSM303_DEV == LSM303_ACCELEROMETER)
	{
		if(((reg >= 0x20) && (reg <= 0x2D)) || ((reg >= 0x30) && (reg <= 0x37)))
		{
			if(((reg >= 0x27) && (reg <= 0x2D)) || (reg  == 0x25) || (reg == 0x31) || (reg == 0x35))
			{
				return 1;
			}
			return 2;
		}
		return 0;		
	}
	else if (LSM303_DEV == LSM303_MAGNETOMETER)
	{
		if(((reg >= 0x00) && (reg <= 0x0C)) || (reg == 0x0F))
		{
			if(((reg >= 0x00) && (reg <= 0x02)))
			{
				return 2;
			}
			return 1;
		}
		return 0;
	}
	return 0;
}


uint8_t LSM303PartNumber(void)
{
	/*
	if(LSM303Version == LSM303DLM)
	{
		printf_P(PSTR("LSM303DLM (new version) detected.\n"));
	}
	else if (LSM303Version == LSM303DLH)
	{
		printf_P(PSTR("LSM303DLH (old version) detected.\n"));
	}
	*/
	return LSM303Version;
}


/** @} */
