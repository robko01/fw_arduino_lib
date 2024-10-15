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

#define STOP for(;;){}

#define SERVICE_PORT 10182

#pragma region General Configuration

/** @brief Brand name. */
#define DEVICE_BRAND "Robko01-WEB"

// NOTE: Be careful with the Version number in the server and here in the source code.
// 1000
// 10 - Major
// 00 - Minor
/** @brief Firmware version string. */
#define ESP_FW_VERSION 1

/** @brief Show functions names. */
#define SHOW_FUNC_NAMES

/** @brief Show configuration file. */
#define SHOW_CONFIG

/** @brief Enable Arduino updates. */
//#define ENABLE_ARDUINO_OTA

/** @brief Enable HTTP updates. */
//#define ENABLE_HTTP_OTA

/** @brief Enable IR controls interface. */
//#define ENABLE_IR_INTERFACE

/** @brief Enable Status LED. */
//#define ENABLE_STATUS_LED

/** @brief Enable device control. */
//#define ENABLE_DEVICE_CONTROL

#define USE_PROGMEM_FS

/** @brief Enable rescue button. */
// #define ENABLE_RESCUE_BTN

#pragma endregion

#pragma region Device Configuration

/** @brief Default STA SSID. */
#define DEAFULT_USER "admin"

/** @brief Default STA password. */
#define DEAFULT_PASS "admin"

/** @brief Default device baud rate. */
#define DEFAULT_BAUDRATE 115200

/** @brief Serial port. */
#define COM_PORT Serial

#pragma endregion

#pragma region AP Configuration

/** @brief Default AP password. */
#define DEFAULT_AP_PASSWORD "12345678"

#pragma endregion

#pragma region STA Configuration

/** @brief Maximum time to connect to local AP. */
#define TIMEOUT_TO_CONNECT 10

/** @brief Default STA SSID. */
#define DEFAULT_STA_SSID "YOUR_DEFAULT_WIFI_SSID"

/** @brief Default STA password. */
#define DEFAULT_STA_PASSWORD "YOUR_DEFAULT_WIFI_PASS"

/** @brief Default enabled DHCP client. */
#define DEFAULT_ENABLED_DHCP 1

/** @brief Default IP address of the station. */
#define DEAFULUT_IP IPAddress(192, 168, 1, 4)

/** @brief Default mask of the station. */
#define DEAFULT_NET_MASK IPAddress(255, 255, 255, 0)

/** @brief Default gateway of the station. */
#define DEAFULT_GW IPAddress(192, 168, 1, 1)

/** @brief Default DNS of the station. */
#define DEAFULT_DNS IPAddress(192, 168, 1, 1)

#pragma endregion

#pragma region NTP Server Configuration

#define DEFAULT_NTP_DOMAIN "europe.pool.ntp.org"

#define DEFAULT_NTP_PORT 1337

#define DEFAULT_NTP_UPDATE_INTERVAL 60000UL

#define DEFAULT_NTP_TIMEZONE 2

#define SECS_IN_HOUR 3600

#pragma endregion

#pragma region WEB Server Configuration

/** @brief WEB server port. */
#define WEB_SERVER_PORT 80

/** @brief Enable editor WEB editor. */
#define ENABLE_EDITOR

/** @brief Stay connected time to the WEB server. */
#define ALIVE_TIME 300 // 300s -> 5min

/** @brief WEB socket maximum queue messages. */
#define WS_MAX_QUEUED_MESSAGES 12

#pragma endregion

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
#define PIN_IOR 15

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
