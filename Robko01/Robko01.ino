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

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "RobotControl.h"

#include "OperationsCodes.h"

#include "JointPositionUnion.h"

#include "JointPosition.h"

#include "super.h"

#include "DebugPort.h"

#include "RobotSettings.h"

#pragma endregion

#pragma region Prototypes

/** @brief Prepare the memory.
 *  @return Void.
 */
void prepare_all_variables();

/** @brief Setup the communication port.
 *  @return Void.
 */
void setup_port();

/** @brief Read incoming commands.
 *  @return Void.
 */
void read_frame();

/** @brief Parse and execute the incoming commands.
 *  @param frame, uint8_t *, The frame array.
 *  @param length, uint8_t, The frame array length.
 *  @return Void.
 */
void parse_frame(uint8_t * frame, uint8_t length);

/** @brief Initialize Timer 2.
 *  @return Void.
 */
void set_timer_2();

#pragma endregion

#pragma region Variables

/** \brief Timer event lock bit. */
volatile uint8_t Timer2Event_g;

/** @brief Pointer to frame buffer. */
static uint8_t * ptrFrameBuffer;

/** @brief Communication frame buffer. */
uint8_t FrameBuffer_g[FRAME_MAX_LEN];

/** @brief Payload request buffer. */
uint8_t PayloadRequest_g[FRAME_MAX_DATA_LEN];

/** @brief Payload response buffer. */
uint8_t PayloadResponse_g[FRAME_MAX_DATA_LEN];

/** @brief Current address index. */
uint8_t CurrentAddressIndex_g;

/** @brief Safty stop flag. */
int SaftyStopFlag_g;

/** @brief Will store last time that the bus was updated. */
unsigned long PreviousMillis_g;

/** @brief Will store current time that the bus is updated. */
unsigned long CurrentMillis_g;

/** @brief Curent joint positions. */
JointPositionUnion CurrentPositions_g;

/** @brief Motors enabled flag. */
bool MotorsEnabled_g;

/** @brief Motors motion states flag. */
uint8_t MotorState_g;

bool StorePosition_g;

#pragma endregion

/** @brief Setup the peripheral hardware and variables.
 *  @return Void.
 */
void setup()
{  
	DEBUG_PORT.begin(DEBUG_PORT_BAUDRATE);
	DEBUGLOG("Initializing ...\r\n");

	prepare_all_variables();

	// This is Safty stop input.
	pinMode(PIN_SS, INPUT_PULLUP);
	
	// Set the motors.
	setup_robot();

	// Setup the communication port.
	setup_port();

	// Setup timer 2.
	set_timer_2();
}

/** @brief Main loop of the program.
 *  @return Void.
 */
void loop()
{
	CurrentMillis_g = millis();

#ifndef NO_SAFTY_SWITCH
	// Read safty switch.
	SaftyStopFlag_g = digitalRead(PIN_SS);
#endif

	if (CurrentMillis_g - PreviousMillis_g >= COM_UPDATE)
	{
		// Save the last time you updated the bus.
		PreviousMillis_g = CurrentMillis_g;

		// read frame form serial.
		read_frame();
	}

	if (Timer2Event_g && SaftyStopFlag_g == LOW)
	{
		// Switch off the bit.
		Timer2Event_g = 0;

		// Update motors.		
		if (CurrentAddressIndex_g < AXIS_COUNT)
		{
			// Update motor state.
			MotorState_g = update_motor(CurrentAddressIndex_g);
		}
		else
		{
			// Update port A.
			update_port_a(CurrentAddressIndex_g);
		}

		// Increment the address bus index.
		CurrentAddressIndex_g++;

		// Clear the address index.
		if (CurrentAddressIndex_g >= ADDRESS_COUNT)
		{
			CurrentAddressIndex_g = 0;
		}
	}

	if (MotorState_g == 0 && StorePosition_g)
	{
		StorePosition_g = false;

		// TODO: Write position to I2C EEPROM.
	}
	else
	{
		StorePosition_g = true;
	}
}

#pragma region Communication

/** @brief Prepare the memory.
 *  @return Void.
 */
void prepare_all_variables()
{
	Timer2Event_g = 0;
	CurrentAddressIndex_g = 0;
	SaftyStopFlag_g = LOW;
	PreviousMillis_g = 0;
	CurrentMillis_g = 0;
	MotorsEnabled_g = false;
	MotorState_g = 0;
	StorePosition_g = true;
}

/** @brief Setup communication port.
 *  @return Void.
 */
void setup_port()
{
	// Set serial communication.
	COM_PORT.begin(COM_BAUDRATE);
	COM_PORT.setTimeout(COM_PORT_TIMEOUT);
}

/** @brief Read incoming commands.
 *  @return Void.
 */
void read_frame()
{
	static uint8_t TemporalDataLengthL = 0;
	static uint8_t CommStateL = CommState::fsSentinel;

	if (COM_PORT.available() < 1)
	{
		return;
	}

	byte InByteL = 0;
	while (COM_PORT.available() > 0)
	{
		InByteL = COM_PORT.read();

		switch (CommStateL)
		{
		case CommState::fsSentinel:
			if (InByteL == FRAME_SENTINEL)
			{
				FrameBuffer_g[FrameIndexes::Sentinel] = InByteL;
				//DEBUGLOG("fsSentinel -> fsRequestResponse\r\n");
				CommStateL = CommState::fsRequestResponse;
			}
			break;

		case CommState::fsRequestResponse:
			// Check request or response value.
			if ((InByteL == FrameType::Request) ||
				(InByteL == FrameType::Response))
			{
				FrameBuffer_g[FrameIndexes::FrmType] = InByteL;
				//DEBUGLOG("fsRequestResponse -> fsLength\r\n");
				CommStateL = CommState::fsLength;
			}
			else
			{
				//DEBUGLOG("fsRequestResponse -> fsSentinel\r\n");
				CommStateL = CommState::fsSentinel;
			}
			break;

		case CommState::fsLength:
			if ((InByteL >= 1) &&
				(InByteL <= 27))
			{
				FrameBuffer_g[FrameIndexes::Length] = InByteL;
				//DEBUGLOG("fsLength -> fsOperationCode\r\n");
				CommStateL = CommState::fsOperationCode;
			}
			else
			{
				//DEBUGLOG("fsLength -> fsSentinel\r\n");
				CommStateL = CommState::fsSentinel;
			}
			break;

		case CommState::fsOperationCode:
			FrameBuffer_g[FrameIndexes::OperationCode] = InByteL;
			if (FrameBuffer_g[FrameIndexes::Length] > 1)
			{
				TemporalDataLengthL = FrameBuffer_g[FrameIndexes::Length] - 1;
				//DEBUGLOG("fsOperationCode -> fsData\r\n");
				CommStateL = CommState::fsData;
			}
			else
			{
				TemporalDataLengthL = FRAME_CRC_LEN;
				//DEBUGLOG("fsOperationCode -> fsCRC\r\n");
				CommStateL = CommState::fsCRC;
			}
			ptrFrameBuffer = &FrameBuffer_g[FRAME_REQUEST_STATIC_FIELD_SIZE];
			break;

		case CommState::fsData:
			*ptrFrameBuffer++ = InByteL;
			if (--TemporalDataLengthL == 0)
			{
				TemporalDataLengthL = FRAME_CRC_LEN;
				//DEBUGLOG("fsData -> fsCRC\r\n");
				CommStateL = CommState::fsCRC;
			}
			break;

		case CommState::fsCRC:
			*ptrFrameBuffer++ = InByteL;
			if (--TemporalDataLengthL == 0)
			{
				//for (uint8_t index = 0; index < FrameBuffer_g[FrameIndexes::Length] + 5; index++)
				//{
				//	//DEBUGLOG("%02X ", FrameBuffer_g[index]);
				//}
				//DEBUGLOG("\r\n");

				if (validate_CRC(FrameBuffer_g, FrameBuffer_g[FrameIndexes::Length] + FRAME_REQUEST_STATIC_FIELD_SIZE - 1 + FRAME_CRC_LEN))
				{
					parse_frame(FrameBuffer_g, FrameBuffer_g[FrameIndexes::Length] + FRAME_REQUEST_STATIC_FIELD_SIZE - 1);
				}
				else
				{
					//DEBUGLOG("Invalid CRC\r\n");
				}

				CommStateL = CommState::fsSentinel;
			}
			break;

		default:
			break;
		}
	}
}

/** @brief Parse and execute the incoming commands.
 *  @param frame, uint8_t *, The frame array.
 *  @param length, uint8_t, The frame array length.
 *  @return Void.
 */
void parse_frame(uint8_t * frame, uint8_t length)
{
	if (frame[FrameIndexes::FrmType] == Request)
	{
		if (frame[FrameIndexes::OperationCode] == OpCodes::Ping)
		{
			get_payload(frame, length, PayloadRequest_g);

			send_raw_response(OpCodes::Ping, StatusCodes::Ok, PayloadRequest_g, frame[FrameIndexes::Length] - 1);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Stop)
		{
			stop_motors();

			send_raw_response(OpCodes::Stop, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Disable)
		{
			disable_motors();
			MotorsEnabled_g = false;

			send_raw_response(OpCodes::Disable, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Enable)
		{
			enable_motors();
			MotorsEnabled_g = true;

			send_raw_response(OpCodes::Enable, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Clear)
		{
			clear_motors();

			send_raw_response(OpCodes::Clear, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::MoveRelative)
		{
			// If it is not enabled, do not execute.
			if (MotorsEnabled_g == false)
			{
				send_raw_response(OpCodes::MoveRelative, StatusCodes::Error, NULL, 0);
				return;
			}

			// If it is move, do not execute the command.
			if (MotorState_g != 0)
			{
				PayloadResponse_g[0] = MotorState_g;
				send_raw_response(OpCodes::MoveRelative, StatusCodes::Busy, PayloadResponse_g, 1);
				return;
			}

			// Extract motion data.
			get_payload(frame, length, PayloadRequest_g);

			// TODO: Move to function.
			JointPositionUnion Motion;
			size_t DataLengthL = sizeof(JointPosition_t);
			for (uint8_t index = 0; index < DataLengthL; index++)
			{
				Motion.Buffer[index] = PayloadRequest_g[index];
			}

			// Set motion data.
			move_relative(Motion.Value);

			// Respond with success.
			send_raw_response(OpCodes::MoveRelative, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::MoveAbsolute)
		{
			// If it is not enabled, do not execute.
			if (MotorsEnabled_g == false)
			{
				// Respond with error.
				send_raw_response(OpCodes::MoveAbsolute, StatusCodes::Error, NULL, 0);

				// Exit
				return;
			}

			// If it is move, do not execute the command.
			if (MotorState_g != 0)
			{
				PayloadResponse_g[0] = MotorState_g;

				// Respond with busy.
				send_raw_response(OpCodes::MoveAbsolute, StatusCodes::Busy, PayloadResponse_g, 1);
				
				// Exit
				return;
			}

			// Extract motion data.
			// TODO: Move to function.
			get_payload(frame, length, PayloadRequest_g);
			JointPositionUnion Motion;
			size_t DataLengthL = sizeof(JointPosition_t);
			for (uint8_t index = 0; index < DataLengthL; index++)
			{
				Motion.Buffer[index] = PayloadRequest_g[index];
			}

			// Set motion data.
			move_absolute(Motion.Value);

			// Respond with success.
			send_raw_response(OpCodes::MoveAbsolute, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::DO)
		{
			// Extract port A data.
			get_payload(frame, length, PayloadRequest_g);

			// Set port A.
			set_port_a(PayloadRequest_g[0]);

			// Respond with success.
			send_raw_response(OpCodes::DO, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::DI)
		{
			PayloadResponse_g[0] = get_port_a();
			DEBUGLOG("Payload: ");
			DEBUGLOG(PayloadResponse_g[0]);
			DEBUGLOG("\r\n");

			send_raw_response(OpCodes::DI, StatusCodes::Ok, PayloadResponse_g, 1);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::IsMoving)
		{
			PayloadResponse_g[0] = MotorState_g;

			send_raw_response(OpCodes::IsMoving, StatusCodes::Ok, PayloadResponse_g, 1);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::CurrentPosition)
		{
			CurrentPositions_g.Value = get_position();

			send_raw_response(OpCodes::CurrentPosition, StatusCodes::Ok, CurrentPositions_g.Buffer, sizeof(JointPosition_t));
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::MoveSpeed)
		{
			// If it is not enabled, do not execute.
			if (MotorsEnabled_g == false)
			{
				send_raw_response(OpCodes::MoveSpeed, StatusCodes::Error, NULL, 0);
				return;
			}
			
			// Extract motion data.
			get_payload(frame, length, PayloadRequest_g);
			
			// TODO: Move to function.
			JointPositionUnion Motion;
			uint8_t DataLengthL = frame[FrameIndexes::Length];
			for (uint8_t index = 0; index < DataLengthL; index++)
			{
				Motion.Buffer[index] = PayloadRequest_g[index];
			}
			
			// Set motion data.
			move_speed(Motion.Value);
			
			// Respond with success.
			send_raw_response(OpCodes::MoveSpeed, StatusCodes::Ok, NULL, 0);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::SetRobotID)
		{
			get_payload(frame, length, PayloadRequest_g);

			/*
			// TODO: Write to I2C EEPROM.
			for (uint8_t index = 0; index < DataLengthL; index++)
			{
				Motion.Buffer[index] = PayloadRequest_g[index];
			}
			*/

			send_raw_response(OpCodes::SetRobotID, StatusCodes::Ok, PayloadRequest_g, frame[FrameIndexes::Length] - 1);
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::GetRobotID)
		{

		/*
		// TODO: Read from I2C EEPROM.
		for (uint8_t index = 0; index < DataLengthL; index++)
		{
			PayloadRequest_g[index] = Motion.Buffer[index];
		}
		*/

		send_raw_response(OpCodes::GetRobotID, StatusCodes::Ok, PayloadRequest_g, frame[FrameIndexes::Length] - 1);
		}
	}
	else if (frame[FrameIndexes::FrmType] == FrameType::Response)
	{
		if (frame[FrameIndexes::OperationCode] == OpCodes::Ping)
		{
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Stop)
		{
		}
	}
}

#pragma endregion

#pragma region Timing

/** @brief Initialize Timer 2.
 *  @return Void.
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

/** @brief Timer 2 interrupt sub routine.
 *  @return Void.
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
