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

#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef ESP321

#error "TEST"

#endif


//#define ENABLE_DEBUG_OUT

#ifndef DEBUGLOG
#define DEBUGLOG(...)
#endif // !DEBUGLOG


/** @brief Safety switch pin definition. */
#define PIN_SS 9

/** @brief No Safety Switch - coment to enable. */
#define NO_SAFETY_SWITCH

/** @brief Additional time schedule divider. */
#define TIME_SCALER 2

/** @brief Additional time schedule divider. */
#define TIME_SCALER 2 // 10

#pragma region Definitions

/** @brief Address pin 0. */
#define PIN_AO0 6

/** @brief Address pin 1. */
#define PIN_AO1 7

/** @brief Address pin 2. */
#define PIN_AO2 8

/** @brief Control pin IO write.*/
#define PIN_IOW A0

/** @brief Control pin IO read.*/
#define PIN_IOR A1

/** @brief Input data pin 0. */
#define PIN_DI0 2

/** @brief Input data pin 1. */
#define PIN_DI1 3

/** @brief Input data pin 2. */
#define PIN_DI2 4

/** @brief Input data pin 3. */
#define PIN_DI3 5

/** @brief Output data pin 0. */
#define PIN_DO0 A6

/** @brief Output data pin 1. */
#define PIN_DO1 A7

/** @brief Output data pin 2. */
#define PIN_DO2 A2

/** @brief Output data pin 3. */
#define PIN_DO3 A3

#pragma endregion

#endif
