/*
	Copyright (c) [2024] [Orlin Dimitrov]

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

#define ESP_FW_VERSION 1

#pragma region Options

#define ENABLE_MOTORS_IO

#define ENABLE_MOTORS

#define ENABLE_SUPER

#define ENABLE_OTA

#pragma endregion // Options

#pragma region IO Pins Definitions
#if defined(ENABLE_MOTORS_IO)

/**
 * @brief Motors enables pin.
 * 
 */
#define MX_ENB 2

/**
 * @brief Motor 1 direction pin.
 * 
 */
#define M1_DIR 4

/**
 * @brief Motor 1 steps pin.
 * 
 */
#define M1_STP 0

/**
 * @brief Motor 2 direction pin.
 * 
 */
#define M2_DIR 17

/**
 * @brief Motor 2 steps pin.
 * 
 */

#define M2_STP 16

/**
 * @brief Motor 3 direction pin.
 * 
 */
#define M3_DIR 12

/**
 * @brief Motor 3 steps pin.
 * 
 */

#define M3_STP 13

/**
 * @brief Motor 4 direction pin.
 * 
 */
#define M4_DIR 27

/**
 * @brief Motor 4 steps pin.
 * 
 */

#define M4_STP 14

/**
 * @brief Motor 5 direction pin.
 * 
 */
#define M5_DIR 25

/**
 * @brief Motor 5 steps pin.
 * 
 */
#define M5_STP 26

/**
 * @brief Motor 6 direction pin.
 * 
 */
#define M6_DIR 32

/**
 * @brief Motor 6 steps pin.
 * 
 */
#define M6_STP 33

/**
 * @brief Motort 1 limit switch.
 * 
 */
#define M1_LIMIT 36

/**
 * @brief Motort 2 limit switch.
 * 
 */
#define M2_LIMIT 39

/**
 * @brief Motort 3 limit switch.
 * 
 */
#define M3_LIMIT 34

/**
 * @brief Motort 6 limit switch.
 * 
 */
#define M6_LIMIT 35
#endif // defined(ENABLE_MOTORS_IO)
#pragma endregion

#pragma region Motors Params

#define ACCELERATION 1E+4
#define MOTOR_SPEED (1000 * 3) //100*16
#define MOTOR_SLOW_SPEED (MOTOR_SPEED / 2)
#define MOTOR_MAX_SPEED MOTOR_SPEED
#define MOTOR_SPEED_5  (MOTOR_SPEED * 0.6F)
#define MOTOR_SLOW_SPEED_5 (MOTOR_SPEED_5 / 2)
#define MOTOR_MAX_SPEED_5 (MOTOR_SPEED_5)

/**
 * @brief Maximum stapes per second for one motor.
 * 
 */
#define MAX_SPEED_OFFSET 0

#pragma endregion // Params

#pragma region Networ Configuration

#define SERVICE_PORT 10182

#pragma endregion // Networ Configuration

#endif
