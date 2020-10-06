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

#ifndef _ROBKO01BUSCONTROL_h
#define _ROBKO01BUSCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//#include "GeneralHelper.h"

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

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void setup_bus();

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void set_address_bus(uint8_t address);

/** @brief Generate IO Write signal.
 *  @return Void.
 */
void iow();

/** @brief Generate IO Read signal.
 *  @return Void.
 */
void ior();

/** @brief Read digital signal of the outputs.
 *  @return uint8_t, state of the pins.
 */
uint8_t read_do();

/** @brief Write digital signal of the inputs.
 *  @param State of the digital inputs.
 *  @return Void.
 */
void write_di(uint8_t data);

#endif

