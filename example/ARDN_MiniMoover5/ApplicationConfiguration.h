#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#pragma region Definitions

/**
 * @brief Send data via serial port what is going on.
 * 
 */
#define DEBUG

/**
 * @brief Debug port baud rate.
 * 
 */
#define BAUD_RATE 9600

/**
 * @brief Line length.
 * 
 */
#define LINE_LENGTH 128

#define PIN_STATUS_LED 13

#define BLINK_INTERVAL 1000

#pragma endregion


#pragma region Robot Port B pin map
 
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
#define PIN_DO0 A3 // 17

/** @brief Output data pin 1. */
#define PIN_DO1 A2 // 16

/** @brief Output data pin 2. */
#define PIN_DO2 A7 // 21

/** @brief Output data pin 3. */
#define PIN_DO3 A6 // 20

#pragma endregion

#pragma region Headers

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma endregion

#pragma region Enums

#pragma endregion

#endif
