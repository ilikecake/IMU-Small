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
*	\brief		GPS Function header file.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		12/11/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _GPS_H_
#define _GPS_H_

#include "stdint.h"

uint8_t GPS_Init(void);
void GPS_SetBias(uint8_t Bias);
void GPS_Reset(uint8_t ToReset);

#endif
/** @} */
