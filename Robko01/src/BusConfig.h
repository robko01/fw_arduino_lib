/*
	Copyright (c) [2019] [Orlin Dimitrov]

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef _BUSCONFIG_h
#define _BUSCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef struct
{

	uint8_t AO0; ///< Address output 0 pin.
	uint8_t AO1; ///< Address output 1 pin.
	uint8_t AO2; ///< Address output 2 pin.
	uint8_t IOR; ///< IO Read pin.
	uint8_t IOW; ///< IO Write pin.
	uint8_t DI0; ///< Digital input 0 pin.
	uint8_t DI1; ///< Digital input 1 pin. 
	uint8_t DI2; ///< Digital input 2 pin.
	uint8_t DI3; ///< Digital input 3 pin.
	uint8_t DO0; ///< Digital output 0 pin.
	uint8_t DO1; ///< Digital output 1 pin. 
	uint8_t DO2; ///< Digital output 2 pin.
	uint8_t DO3; ///< Digital output 3 pin.
		
/** @brief IO configuration of the robot. */
} BusConfig_t;

#endif
