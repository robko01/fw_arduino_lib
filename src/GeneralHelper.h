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

#ifndef _GENERALHELPER_h
#define _GENERALHELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#if defined(ESP32)
#include <WiFi.h>
#endif

#include "DebugPort.h"

#ifndef WL_MAC_ADDR_LENGTH
#define WL_MAC_ADDR_LENGTH 6
#endif

#pragma region Prototypes

/**
 * @brief 
 * 
 * @param arr 
 * @param count 
 * @return uint8_t 
 */
uint8_t bit_array_to_uint8(bool arr[], uint8_t count);

/**
 * @brief Get MAC address.
 * 
 * @param mac 
 * @return String, Returns the string of MAC address. 
 */
String mac2str(const uint8_t * mac);

/** @brief Convert a single hex digit character to its integer value. Based on https://code.google.com/p/avr-netino/
 *  @return String, Returns the string of unified URL string.
 */
unsigned char hex2dec(char c);

/** @brief Converts binary array to heximal string.
 *  @param unsigned char * input, Binary input.
 *  @param unsigned int input_size, Binary input size.
 *  @param char * output, Output result.
 *  @return boolean, Returns the true if value is in the range.
 */
void bin_to_strhex(uint8_t *input, unsigned int input_size, uint8_t *output);

/** @brief Check the Values is between: [0 - 255].
 *  @param value String, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(String value);

/** @brief Check the Values is between: [0 - 255].
 *  @param value int, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(int value);

/** @brief Convert size to nice text type.
 *  @param bytes, size_t Size of the file.
 *  @return String, Nice formated size text.
 */
String formatBytes(size_t bytes);

/** @brief Convert many minutes to ending minutes.
 *  @param int minutes, Minutest.
 *  @return int, Ending minutes.
 */
int to_minutes(int minutes);

/** @brief Convert many minutes to hours.
 *  @param int minutes, Minutest.
 *  @return int, Hours.
 */
int to_hours(int minutes);

#ifdef BATT_MONITOR

/** @brief Read battery voltage.
 *  @return float voltage.
 */
float battery_voltage(int pin);

#endif

#pragma endregion

/**
 * @brief Generate cookie string.
 * 
 * @return String cookie string.
 */
String generate_random_str(uint8_t size = 20);

#endif

