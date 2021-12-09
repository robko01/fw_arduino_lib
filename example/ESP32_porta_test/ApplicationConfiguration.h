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

#pragma region Serial Port

#pragma region IO Pins Definitions

/** @brief Address pin 0. */
#define PIN_AO0 33

/** @brief Address pin 1. */
#define PIN_AO1 25

/** @brief Address pin 2. */
#define PIN_AO2 26

/** @brief Control pin IO write.*/
#define PIN_IOW 32

/** @brief Control pin IO read.*/
#define PIN_IOR 23

/** @brief Input data pin 0. */
#define PIN_DI0 13

/** @brief Input data pin 1. */
#define PIN_DI1 12

/** @brief Input data pin 2. */
#define PIN_DI2 14

/** @brief Input data pin 3. */
#define PIN_DI3 27

/** @brief Output data pin 0. */
#define PIN_DO0 35

/** @brief Output data pin 1. */
#define PIN_DO1 34

/** @brief Output data pin 2. */
#define PIN_DO2 39

/** @brief Output data pin 3. */
#define PIN_DO3 36

#pragma endregion

#endif
