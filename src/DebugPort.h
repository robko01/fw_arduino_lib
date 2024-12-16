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

#define ENABLE_DEBUG_PORT

#define SHOW_FUNC_NAMES

#define DBG_OUTPUT_PORT_BAUDRATE 115200

#define DBG_OUTPUT_PORT Serial

#ifndef ENABLE_DEBUG_PORT
#define DEBUGLOG(...)
#else
#define DEBUGLOG(...) DBG_OUTPUT_PORT.printf(__VA_ARGS__)
#endif

#pragma endregion

#pragma region Functions

/** @brief Setup debug port.
 *  @return Void
 */
void setup_debug_port();

#pragma endregion
#endif
