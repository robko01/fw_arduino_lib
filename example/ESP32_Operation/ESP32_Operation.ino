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

#include <WiFi.h>

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "SUPER.h"

#include "BusConfig.h"

#include "Robko01.h"

#include "DebugPort.h"

#include "OperationsCodes.h"

#include "DefaultCredentials.h"

#include "GeneralHelper.h"

#pragma endregion

#pragma region Prototypes

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload);

#pragma endregion

#pragma region Variables

/**
 * @brief Motors state bits.
 * 
 */
uint8_t MotorState_g;

/**
 * @brief Store position flag.
 * 
 */
bool StorePosition_g;

/**
 * @brief Safety stop flag.
 * 
 */
int SafetyStopFlag_g;

/**
 * @brief Curent joint positions.
 * 
 */
JointPositionUnion CurrentPositions_g;

#ifdef DEAFULT_CREDENTIALS_H_

/**
 * @brief SSID
 * 
 */
const char* SSID_g = DEFAULT_SSID;

/**
 * @brief Password
 * 
 */
const char* PASS_g = DEFAULT_PASS;

#else

/**
 * @brief SSID
 * 
 */
const char* SSID_g = "yourssid";

/**
 * @brief Password
 * 
 */
const char* PASS_g = "yourpasswd";

#endif

WiFiServer TCPServer_g(SERVICE_PORT);

#pragma endregion

/**
 * @brief Setup the peripheral hardware and variables.
 * 
 */
void setup()
{
	SafetyStopFlag_g = LOW;
	MotorState_g = 0;
	StorePosition_g = true;

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

	setup_debug_port();

	show_device_properties();

	// Initialize the robot controller.
	Robko01.init(&config);

	// Initialize the communication.
	init_communication();

	// Initialize the SUPER protocol parser.
	SUPER.setCbRequest(cbRequestHandler);
}

/**
 * @brief Main loop of the program.
 * 
 */
void loop()
{
	update_communication();

	if (SafetyStopFlag_g == LOW)
	{
        Robko01.update();
        MotorState_g = Robko01.get_motor_state();
	}

	if (MotorState_g == 0 && StorePosition_g)
	{
		StorePosition_g = false;

		// TODO: Save position.
	}
	else
	{
		StorePosition_g = true;
	}
}

#pragma region Functions

/**
 * @brief Initialize the communication.
 * 
 */
void init_communication() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// start the Wi-Fi connection and the server:
    WiFi.begin(SSID_g, PASS_g);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
		DEBUGLOG(".");
    }
	
	DEBUGLOG("\r\n");
	DEBUGLOG("Connected:  %s\r\n", SSID_g);
	DEBUGLOG("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
	DEBUGLOG("Gateway:    %s\r\n", WiFi.gatewayIP().toString().c_str());
	DEBUGLOG("DNS:        %s\r\n", WiFi.dnsIP().toString().c_str());
	DEBUGLOG("\r\n");

	// Start the server.
	TCPServer_g.begin();
}

/**
 * @brief Update communication.
 * 
 */
void update_communication() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	static uint8_t StateL = 0;
	
	static WiFiClient ClientL;
	
	if (StateL == 0)
	{
		// Listen for incoming clients.
		ClientL = TCPServer_g.available();

		if (ClientL)
		{
			SUPER.init(ClientL);
			StateL = 1;
			DEBUGLOG("Connected...\r\n");
		}
	}
	if (StateL == 1)
	{
		if (ClientL.connected())
		{
			SUPER.update();
		}
		else
		{
			StateL = 2;
		}
	}
	if (StateL == 2)
	{
		ClientL.stop();
		StateL = 0;
		DEBUGLOG("Disconnected...\r\n");
	}
}

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (opcode == OpCodes::Ping)
	{
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
	else if (opcode == OpCodes::Stop)
	{
		Robko01.stop_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Disable)
	{
		Robko01.disable_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Enable)
	{
		Robko01.enable_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::Clear)
	{
		Robko01.clear_motors();

		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::MoveRelative)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
			return;
		}
		// If it is move, do not execute the command.
		if (MotorState_g != 0)
		{
			uint8_t m_payloadResponse[1] = { MotorState_g };
			SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
			return;
		}

		// TODO: Move to function.
		JointPositionUnion Motion;
		size_t DataLengthL = sizeof(JointPosition_t);
		for (uint8_t index = 0; index < DataLengthL; index++)
		{
			Motion.Buffer[index] = payload[index];
		}

		// Set motion data.
		Robko01.move_relative(Motion.Value);
		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::MoveAbsolute)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			// Respond with error.
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);

			// Exit
			return;
		}
		// If it is move, do not execute the command.
		if (MotorState_g != 0)
		{
			// Respond with busy.
			uint8_t m_payloadResponse[1];
			m_payloadResponse[0] = MotorState_g;
			SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
			
			// Exit
			return;
		}

		// Extract motion data.
		JointPositionUnion Motion;
		size_t DataLengthL = sizeof(JointPosition_t);
		for (uint8_t index = 0; index < DataLengthL; index++)
		{
			Motion.Buffer[index] = payload[index];
		}

		// Set motion data.
		Robko01.move_absolute(Motion.Value);

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::DO)
	{
		// Set port A.
		Robko01.set_port_a(payload[0]);

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::DI)
	{
		uint8_t m_payloadResponse[1];
		m_payloadResponse[0] = Robko01.get_port_a();

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
	}
	else if (opcode == OpCodes::IsMoving)
	{
		uint8_t m_payloadResponse[1];
		m_payloadResponse[0] = MotorState_g;

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
	}
	else if (opcode == OpCodes::CurrentPosition)
	{
		CurrentPositions_g.Value = Robko01.get_position();

		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, CurrentPositions_g.Buffer, sizeof(JointPosition_t));
	}
	else if (opcode == OpCodes::MoveSpeed)
	{
		// If it is not enabled, do not execute.
		if (Robko01.motors_enabled() == false)
		{
			SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
			return;
		}
		
		JointPositionUnion Motion;
		for (uint8_t index = 0; index < size; index++)
		{
			Motion.Buffer[index] = payload[index];
		}
		
		// Set motion data.
		Robko01.move_speed(Motion.Value);
		
		// Respond with success.
		SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
	}
	else if (opcode == OpCodes::SetRobotID)
	{
		// TODO: Write to I2C EEPROM.
		//for (uint8_t index = 0; index < DataLengthL; index++)
		//{
		//	Motion.Buffer[index] = m_payloadRequest[index];
		//}
		
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
	else if (opcode == OpCodes::GetRobotID)
	{
		// TODO: Read from I2C EEPROM.
		//for (uint8_t index = 0; index < DataLengthL; index++)
		//{
		//	m_payloadRequest[index] = Motion.Buffer[index];
		//}
		
		SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
	}
}

#pragma endregion
