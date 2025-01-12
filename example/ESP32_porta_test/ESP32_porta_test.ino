/*
    MIT License
    
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

/*

+------+----------+-----------+
| Axis |   Sign   | Direction |
+------+----------+-----------+
|    0 | Positive | CCW       |
|    0 | Negative | CW        |
|    1 | Positive | Forward   |
|    1 | Negative | Backward  |
|    2 | Positive | Backward  |
|    2 | Negative | Forward   |
|    3 | Positive | Down      |
|    3 | Negative | Up        |
|    4 | Positive | CW        |
|    4 | Negative | CCW       |
|    5 | Positive | Open      |
|    5 | Negative | Close     |
+------+----------+-----------+

*/

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "SUPER.h"

#include "BusConfig.h"

#include "Robko01.h"

#include "DebugPort.h"

#include "OperationsCodes.h"

#pragma endregion

#pragma region Prototypes

/**
 * @brief Display PORT A of the robot.
 * 
 */
void display_port_a(uint8_t port_state);

#pragma endregion

#pragma region Variables

/**
 * @brief Current time.
 * 
 */
unsigned long CurrentMillis_g = 0;

/**
 * @brief Previous time.
 * 
 */
unsigned long PreviousMillis_g = 0;

/**
 * @brief Update interval.
 * 
 */
const long UpdateInterval_g = 500;

/**
 * @brief Port A bits.
 * 
 */
uint8_t PortA_g = 0;

/**
 * @brief Target bit.
 * 
 */
bool TargetBit_g = false;

#pragma endregion

/**
 * @brief Setup the peripheral hardware and variables.
 * 
 */
void setup()
{
    // Setup the robot bus driver.
	BusConfig_t config = {
        PIN_AO0,
        PIN_AO1,
        PIN_AO2,
        PIN_IOR,
        PIN_IOW,
        PIN_DI0,
        PIN_DI1,
        PIN_DI2,
        PIN_DI3,
        PIN_DO0,
        PIN_DO1,
        PIN_DO2,
        PIN_DO3,
	};

	setup_debug_port(DBG_OUTPUT_PORT_BAUDRATE);

	// Initialize the robot controller.
	Robko01.init(&config);
}

/**
 * @brief Main loop of the program.
 * 
 */
void loop()
{
	Robko01.update();

	CurrentMillis_g = millis();
	
	if (CurrentMillis_g - PreviousMillis_g >= UpdateInterval_g)
	{
		PreviousMillis_g = CurrentMillis_g;

		PortA_g = Robko01.get_port_a();
		
		display_port_a(PortA_g);
	}
}

/**
 * @brief Display PORT A of the robot.
 * 
 */
void display_port_a(uint8_t port_state)
{
	for (int i = 7; i >= 0; i--)
	{
		TargetBit_g = bitRead(port_state, i);
        if (TargetBit_g)
        {
    		DEBUGLOG("1");
        }
        else
        {
    		DEBUGLOG("0");
        }
	}
	DEBUGLOG("\r\n");
}
