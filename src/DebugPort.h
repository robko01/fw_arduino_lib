/*

IoTR - Robot Monitoring Device System

Copyright (C) [2020] [Orlin Dimitrov] GPLv3

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// DebugPort.h

#ifndef _DEBUGPORT_h
#define _DEBUGPORT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions


#if defined(ESP8266) || defined(ESP32)
#define DBG_OUTPUT_PORT_BAUDRATE 9600
#define DBG_OUTPUT_PORT Serial
#endif

#if defined(ESP8266) || defined(ESP32)
#define SHOW_FUNC_NAMES
#endif

#ifdef ENABLE_DEBUG_PORT
#define DEBUGLOG(...)
#else
#if defined(ESP8266) || defined(ESP32)
#define DEBUGLOG(...) DBG_OUTPUT_PORT.printf(__VA_ARGS__)
#endif
#endif

#pragma endregion

#pragma region Functions

/** @brief Setup debug port.
 *  @return Void
 */
void setup_debug_port();

#pragma region Prototypes

/** @brief Get MAC address.
 *  @return String, Returns the string of MAC address.
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

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties();

#ifdef BATT_MONITOR

/** @brief Read battery voltage.
 *  @return float voltage.
 */
float battery_voltage(int pin);

#endif

#pragma endregion
#endif
