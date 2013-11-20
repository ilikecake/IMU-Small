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
*	\brief		Header for the low level interface driver for LSM303 digital compass.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		12/18/2011
*	\copyright	Copyright 2011, Pat Satyshur
*	\ingroup 	LSM303
*
*	\defgroup	LSM303 LSM303 Hardware Drivers
*
*	@{
*/

#ifndef _LSM303_H_
#define _LSM303_H_

#include "main.h"
//#include <stdint.h>

/** The size of the send and recieve buffers. */
#define LSM303_BUFFER_SIZE	8

/** Accelerometer 7-bit I2C address.	*/
#define LSM303_ACCELEROMETER_ADDRESS		0x18

/** Magnetometer 7-bit I2C address.		*/
#define LSM303_MAGNETOMETER_ADDRESS			0x1E

/** \name Accelerometer register addresses */
/** @{*/
#define LSM303_ACCELEROMETER_READ_MULTIPLE		0x80
#define LSM303_ACCELEROMETER_CTRL_REG1			0x20
#define LSM303_ACCELEROMETER_CTRL_REG2			0x21
#define LSM303_ACCELEROMETER_CTRL_REG3			0x22
#define LSM303_ACCELEROMETER_CTRL_REG4			0x23
#define LSM303_ACCELEROMETER_CTRL_REG5			0x24
#define LSM303_ACCELEROMETER_HP_FILTER_RESET	0x25
#define LSM303_ACCELEROMETER_REFERENCE			0x26
#define LSM303_ACCELEROMETER_STATUS_REG			0x27
#define LSM303_ACCELEROMETER_OUT_X_L			0x28
#define LSM303_ACCELEROMETER_OUT_X_H			0x29
#define LSM303_ACCELEROMETER_OUT_Y_L			0x2A
#define LSM303_ACCELEROMETER_OUT_Y_H			0x2B
#define LSM303_ACCELEROMETER_OUT_Z_L			0x2C
#define LSM303_ACCELEROMETER_OUT_Z_H			0x2D
#define LSM303_ACCELEROMETER_INT1_CFG			0x30
#define LSM303_ACCELEROMETER_INT1_SOURCE		0x31
#define LSM303_ACCELEROMETER_INT1_THS			0x32
#define LSM303_ACCELEROMETER_INT1_DURATION		0x33
#define LSM303_ACCELEROMETER_INT2_CFG			0x34
#define LSM303_ACCELEROMETER_INT2_SOURCE		0x35
#define LSM303_ACCELEROMETER_INT2_THS			0x36
#define LSM303_ACCELEROMETER_INT2_DURATION		0x37
/** @} */ 

/** \name Magnetometer register addresses */
/** @{*/
#define LSM303_MAGNETOMETER_CRA_REG				0x00
#define LSM303_MAGNETOMETER_CRB_REG				0x01
#define LSM303_MAGNETOMETER_MR_REG				0x02
#define LSM303_MAGNETOMETER_OUT_X_H				0x03
#define LSM303_MAGNETOMETER_OUT_X_L				0x04

//Old Version (DLH)
#define LSM303_MAGNETOMETER_OUT_Y_H_DLH			0x05
#define LSM303_MAGNETOMETER_OUT_Y_L_DLH			0x06
#define LSM303_MAGNETOMETER_OUT_Z_H_DLH			0x07
#define LSM303_MAGNETOMETER_OUT_Z_L_DLH			0x08

//New Version (DLM)
#define LSM303_MAGNETOMETER_OUT_Z_H_DLM			0x05
#define LSM303_MAGNETOMETER_OUT_Z_L_DLM			0x06
#define LSM303_MAGNETOMETER_OUT_Y_H_DLM			0x07
#define LSM303_MAGNETOMETER_OUT_Y_L_DLM			0x08

#define LSM303_MAGNETOMETER_SR_REG				0x09
#define LSM303_MAGNETOMETER_IRA_REG				0x0A
#define LSM303_MAGNETOMETER_IRB_REG				0x0B
#define LSM303_MAGNETOMETER_IRC_REG				0x0C

//Not present on DLH
#define LSM303_MAGNETOMETER_WHO_AM_I_DLM		0x0F
/** @} */

/** \name Accelerometer register descriptions */
/** @{*/
#define LSM303_ACCELEROMETER_CR1_POWERDOWN		0X00
#define LSM303_ACCELEROMETER_CR1_NORMAL			0X20
#define LSM303_ACCELEROMETER_CR1_LOW_POWER_05	0X40
#define LSM303_ACCELEROMETER_CR1_LOW_POWER_1	0X60
#define LSM303_ACCELEROMETER_CR1_LOW_POWER_2	0X80
#define LSM303_ACCELEROMETER_CR1_LOW_POWER_5	0XA0
#define LSM303_ACCELEROMETER_CR1_LOW_POWER_10	0XC0
#define LSM303_ACCELEROMETER_CR1_ODR_50			0X00
#define LSM303_ACCELEROMETER_CR1_ODR_100		0X08
#define LSM303_ACCELEROMETER_CR1_ODR_400		0X10
#define LSM303_ACCELEROMETER_CR1_ODR_1000		0X18
#define LSM303_ACCELEROMETER_CR1_XON			0X01
#define LSM303_ACCELEROMETER_CR1_XOFF			0X00
#define LSM303_ACCELEROMETER_CR1_YON			0X02
#define LSM303_ACCELEROMETER_CR1_YOFF			0X00
#define LSM303_ACCELEROMETER_CR1_ZON			0X04
#define LSM303_ACCELEROMETER_CR1_ZOFF			0X00
#define LSM303_ACCELEROMETER_CR1_ALLON			0X07
#define LSM303_ACCELEROMETER_CR1_ALLOFF			0X00

#define LSM303_ACCELEROMETER_CR2_REBOOT			0X80
#define LSM303_ACCELEROMETER_CR2_HPF_NORMAL		0X00
#define LSM303_ACCELEROMETER_CR2_HPF_REF		0X20
#define LSM303_ACCELEROMETER_CR2_HPDF_ON		0X10
#define LSM303_ACCELEROMETER_CR2_HPDF_OFF		0X00
#define LSM303_ACCELEROMETER_CR2_HPINT2_ON		0X08
#define LSM303_ACCELEROMETER_CR2_HPINT2_OFF		0X00
#define LSM303_ACCELEROMETER_CR2_HPINT1_ON		0X04
#define LSM303_ACCELEROMETER_CR2_HPINT1_OFF		0X00
#define LSM303_ACCELEROMETER_CR2_HPC_8			0X00
#define LSM303_ACCELEROMETER_CR2_HPC_16			0X01
#define LSM303_ACCELEROMETER_CR2_HPC_32			0X02
#define LSM303_ACCELEROMETER_CR2_HPC_64			0X03

#define LSM303_ACCELEROMETER_CR3_INT_LOW		0X80
#define LSM303_ACCELEROMETER_CR3_INT_HIGH		0X00
#define LSM303_ACCELEROMETER_CR3_INT_PP			0X00
#define LSM303_ACCELEROMETER_CR3_INT_OD			0X40
#define LSM303_ACCELEROMETER_CR3_LIR2			0X20
#define LSM303_ACCELEROMETER_CR3_LIR1			0X04
#define LSM303_ACCELEROMETER_CR3_INT2_SRC_INT2	0X00
#define LSM303_ACCELEROMETER_CR3_INT2_SRC_BOTH	0X08
#define LSM303_ACCELEROMETER_CR3_INT2_SRC_DR	0X10
#define LSM303_ACCELEROMETER_CR3_INT2_SRC_BOOT	0X18
#define LSM303_ACCELEROMETER_CR3_INT1_SRC_INT1	0X00
#define LSM303_ACCELEROMETER_CR3_INT1_SRC_BOTH	0X01
#define LSM303_ACCELEROMETER_CR3_INT1_SRC_DR	0X02
#define LSM303_ACCELEROMETER_CR3_INT1_SRC_BOOT	0X03

#define LSM303_ACCELEROMETER_CR4_BDU			0X80
#define LSM303_ACCELEROMETER_CR4_LITTLE_ENDIAN	0X00
#define LSM303_ACCELEROMETER_CR4_BIG_ENDIAN		0X40
#define LSM303_ACCELEROMETER_CR4_FS_2G			0X00
#define LSM303_ACCELEROMETER_CR4_FS_4G			0X10
#define LSM303_ACCELEROMETER_CR3_FS_8G			0X30

#define LSM303_ACCELEROMETER_CR5_WAKE_OFF		0X00
#define LSM303_ACCELEROMETER_CR5_WAKE_ON		0X03

#define LSM303_ACCELEROMETER_STATUS_XYZOR		0X80
#define LSM303_ACCELEROMETER_STATUS_ZOR			0X40
#define LSM303_ACCELEROMETER_STATUS_YOR			0X20
#define LSM303_ACCELEROMETER_STATUS_XOR			0X10
#define LSM303_ACCELEROMETER_STATUS_XYZDA		0X08
#define LSM303_ACCELEROMETER_STATUS_ZDA			0X04
#define LSM303_ACCELEROMETER_STATUS_YDA			0X02
#define LSM303_ACCELEROMETER_STATUS_XDA			0X01

#define LSM303_ACCELEROMETER_INT_CFG_OR			0X00		
#define LSM303_ACCELEROMETER_INT_CFG_AND		0X80
#define LSM303_ACCELEROMETER_INT_CFG_6D			0X40
#define LSM303_ACCELEROMETER_INT_CFG_ZHIE		0X20
#define LSM303_ACCELEROMETER_INT_CFG_ZLIE		0X10
#define LSM303_ACCELEROMETER_INT_CFG_YHIE		0X08
#define LSM303_ACCELEROMETER_INT_CFG_YLIE		0X04
#define LSM303_ACCELEROMETER_INT_CFG_XHIE		0X02
#define LSM303_ACCELEROMETER_INT_CFG_XLIE		0X01
#define LSM303_ACCELEROMETER_INT_CFG_ALL		0X3F

#define LSM303_ACCELEROMETER_INT_SRC_IA			0X40
#define LSM303_ACCELEROMETER_INT_SRC_ZH			0X20
#define LSM303_ACCELEROMETER_INT_SRC_ZL			0X10
#define LSM303_ACCELEROMETER_INT_SRC_YH			0X08
#define LSM303_ACCELEROMETER_INT_SRC_YL			0X04
#define LSM303_ACCELEROMETER_INT_SRC_XH			0X02
#define LSM303_ACCELEROMETER_INT_SRC_HL			0X01
/** @} */

/** \name Magnetometer register descriptions */
/** @{*/
#define LSM303_MAGNETOMETER_DR_0_75HZ			0x00
#define LSM303_MAGNETOMETER_DR_1_5HZ			0x04
#define LSM303_MAGNETOMETER_DR_3_0HZ			0x08
#define LSM303_MAGNETOMETER_DR_7_5HZ			0x0C
#define LSM303_MAGNETOMETER_DR_15HZ				0x10
#define LSM303_MAGNETOMETER_DR_30HZ				0x14
#define LSM303_MAGNETOMETER_DR_75HZ				0x18
#define LSM303_MAGNETOMETER_DR_220HZ			0x1C

#define LSM303_MAGNETOMETER_GAIN_1_3G			0x20
#define LSM303_MAGNETOMETER_GAIN_1_9G			0x40
#define LSM303_MAGNETOMETER_GAIN_2_5G			0x60
#define LSM303_MAGNETOMETER_GAIN_4_0G			0x80
#define LSM303_MAGNETOMETER_GAIN_4_7G			0xA0
#define LSM303_MAGNETOMETER_GAIN_5_6G			0xC0
#define LSM303_MAGNETOMETER_GAIN_8_1G			0xE0

#define LSM303_MAGNETOMETER_MODE_CONTINUOUS		0x00
#define LSM303_MAGNETOMETER_MODE_SINGLE			0x01
#define LSM303_MAGNETOMETER_MODE_SLEEP			0x02
/** @} */

/** Global variable to hold the most recent magnetometer readings */
uint16_t LSM303MagnetometerData[3];

/** Global variable to hold the most recent accelerometer readings */
uint16_t LSM303AccelerometerData[3];

/** Definition of the LSM303 devices. */
typedef enum
{
	LSM303_ACCELEROMETER,
	LSM303_MAGNETOMETER
} LSM303Device;

/** Definition of the LSM303 devices. */
typedef enum
{
	LSM303_NONE,
	LSM303_NO_A,
	LSM303_NO_M,
	LSM303DLH,
	LSM303DLM
} LSM303part;


/**Holds data to send to the LSM303		*/
uint8_t LSM303WriteBuffer[LSM303_BUFFER_SIZE];

/**Holds data recieved from the LSM303	*/
uint8_t LSM303ReadBuffer[LSM303_BUFFER_SIZE];

/** Initalizes the LSM303.  */
void LSM303Init(void);


/** Writes to a register on the LSM303.
*
* \param[in]	LSM303_DEV The device to write, should be LSM303_ACCELEROMETER or LSM303_MAGNETOMETER
* \param[in]	RegToWrite the register to write
* \param[in]	ValueToWrite The value to write
* \return 		1 for success, 0 for error
*/
uint8_t LSM303WriteReg(LSM303Device LSM303_DEV, uint8_t RegToWrite, uint8_t ValueToWrite);

/** Reads a register from the LSM303.
*
* \param[in]	LSM303_DEV The device to read, should be LSM303_ACCELEROMETER or LSM303_MAGNETOMETER
* \param[in]	RegToRead the register to read
* \return 		1 for success, 0 for error
*/
uint8_t LSM303ReadReg(LSM303Device LSM303_DEV, uint8_t RegToRead);

/** Reads measured data from the LSM303. The data is available when the function returns in the LSM303MagnetometerData and LSM303AccelerometerData global variables.
*
* \param[in]	LSM303_DEV The device to read, should be LSM303_ACCELEROMETER or LSM303_MAGNETOMETER
* \return 		1 for success, 0 for error
*/
uint8_t LSM303ReadData(LSM303Device LSM303_DEV);


/** Checks if a register address is valid for the LSM303.
*
* \param[in]	LSM303_DEV The device to check, should be LSM303_ACCELEROMETER or LSM303_MAGNETOMETER
* \param[in]	reg the register to check
* \return 		2 if the register is read/write, 1 if the register is read only, 0 otherwise
*/
int8_t LSM303IsReg(LSM303Device LSM303_DEV, int reg);

/** Checks the identified part number of the LSM303.
*
* \return 		LSM303 part number ID
*/
uint8_t LSM303PartNumber(void);



#endif

/** @} */
