// DebugPort.h

#ifndef _DEBUGPORT_h
#define _DEBUGPORT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

#pragma region Definitions

#define DEBUG_PORT_BAUDRATE 9600
#define DEBUG_PORT_RX 11
#define DEBUG_PORT_TX 12
#define DEBUG_PORT DebugPort_g

#ifdef EANBLE_DEBUG_OUT
#define DEBUGLOG(...) DEBUG_PORT.write(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

#pragma endregion

static SoftwareSerial DebugPort_g(DEBUG_PORT_RX, DEBUG_PORT_TX); // RX, TX


#endif

