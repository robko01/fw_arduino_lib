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

/** @file ArduinoRobko01.ino
 *  @brief Multi axis robot controller, for Robko01.
 *
 *  This file content a small time scheduler for regulating motor axises.
 *  The purpose of this software is to control Robko01 robot in multi axis mode.
 *
 *  @author Orlin Dimnitrov (orlin369)
 */

/*
	Resources.
	Timers:       https://et06.dunweber.com/atmega_timers/
	Steppers:     http://www.open.com.au/mikem/arduino/AccelStepper
	Robot:        http://orlin.happyrobotics.com/?page_id=143
	Access table: http://www.slideshare.net/orlin369/access-tablerobko01
	Schematic:    http://www.slideshare.net/orlin369/main-board-of-robko01-micro-robot-robotic-hand
	Generate graphviz graph: python cppdepends.py "~\Robko01\OrlinDimitrov\Firmware\ArduinoRobko01\ArduinoRobko01" ArduinoRobko01.pdf 50
	Graphviz viewr: http://www.webgraphviz.com/
*/

#ifdef ESP321

#error "TEST"

#endif

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "SUPER.h"

#include "BusConfig.h"

#include "Robko01.h"

#include "DebugPort.h"

#include "OperationsCodes.h"

#include <WiFi.h>

#pragma endregion

#pragma region Prototypes

/**
 * @brief CAllback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload);

/**
 * @brief Set the timer 2.
 * 
 */
void set_timer_2();

#pragma endregion

#pragma region Variables

/**
 * @brief Timer event lock bit.
 * 
 */
volatile uint8_t Timer2Event_g;

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

const char* SSID_g = "yourssid";

const char* PASS_g = "yourpasswd";

WiFiServer TCPServer_g(SERVICE_PORT);

#pragma endregion

/**
 * @brief Setup the peripheral hardware and variables.
 * 
 */
void setup()
{
	Timer2Event_g = 0;
	SafetyStopFlag_g = LOW;
	MotorState_g = 0;
	StorePosition_g = true;

    // Setup the robot bus driver.
	BusConfig_t config = {
        PIN_AO0,
        PIN_AO1,
        PIN_AO2,
        PIN_IOW,
        PIN_IOR,
        PIN_DI0,
        PIN_DI1,
        PIN_DI2,
        PIN_DI3,
        PIN_DO0,
        PIN_DO1,
        PIN_DO2,
        PIN_DO3,
	};

	// Initialize the robot controller.
	Robko01.init(&config);

	// Initialize the communication.
	init_communication();

	// Initialize the SUPER protocol parser.
	SUPER.setCbRequest(cbRequestHandler);

	// Setup timer 2.
	set_timer_2();
}

/**
 * @brief Main loop of the program.
 * 
 */
void loop()
{
	update_communication();

	if (Timer2Event_g == 1 && SafetyStopFlag_g == LOW)
	{
		// Switch off the bit.
		Timer2Event_g = 0;

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

void init_communication()
{
	// start the Wi-Fi connection and the server:
    WiFi.begin(SSID_g, PASS_g);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
	
	// Start the server.
	TCPServer_g.begin();
}

/**
 * @brief Update communication.
 * 
 */
void update_communication()
{
	// Listen for incoming clients.
	WiFiClient ClientL = TCPServer_g.available();

	if (ClientL)
	{
		SUPER.init(ClientL);

		if (ClientL.connected())
		{
			// Update the data processing.
			SUPER.update();
		}
		else
		{
		    // Close the connection.
		    ClientL.stop();
		}
	}
}

/**
 * @brief CAllback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload)
{
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

#pragma region Timer 2

/**
 * @brief Set the timer 2.
 * 
 */
void set_timer_2()
{
	// Set point of the timer is: 1250Hz.
	// 16000000 / 64         / 250   = 1000
	// 16000000 / 64         / 200   = 1250
	// 16000000 / 64         / 150   = 1666.(6)
	// 16000000 / 64         / 100   = 2500
	// 16000000 / 64         / 50   = 5000
	// Fosc     / pre scaler / OCR2A = time tics[Hz]

	// Disable global interrupts.
	cli();
	// Set timer mode to CTC.
	TCCR2A = (1 << WGM21);
	// Set timer 2 bits for 64 pre scaler:
	TCCR2B = (4 << CS20);
	// Clear Timer 2.
	TCNT2 = 0;
	// Set point.
	OCR2A = 200; // 250 
	// Set the enable interrupt.
	TIMSK2 = (1 << OCIE2A);
	// Enable global interrupts:
	sei();
}

/**
 * @brief Timer 2 interrupt sub routine.
 * 
 */
ISR(TIMER2_COMPA_vect)
{
	static uint16_t Timer2DividerL = 0;
	// Tell everyone the time was finish its job.
	if (++Timer2DividerL >= TIME_SCALER)
	{
		Timer2DividerL = 0;
		Timer2Event_g = 1;
	}
}

#pragma endregion
