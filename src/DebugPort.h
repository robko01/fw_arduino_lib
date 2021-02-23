// DebugPort.h

#ifndef _DEBUGPORT_h
#define _DEBUGPORT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions

#ifdef ENABLE_DEBUG_OUT
#define DEBUGLOG(...) DEBUG_PORT.write(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

#pragma endregion

#endif
