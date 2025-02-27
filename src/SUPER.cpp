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

/*
    S - Synchronous
    U - Unified
    P - Protocol
    E - for Extendable
    R - Robots
 */

#include "SUPER.h"

/** @brief Send RAW request frame.
 *  @param opcode uint8_t, Operation code.
 *  @param frame uint8_t*, Command for this operation code.
 *  @param length const uint8_t, Length of the payload.
 *  @return Void.
 */
void SUPERClass::send_raw_request(uint8_t opcode, uint8_t * payload, const uint8_t length) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

	const int FrameLengthL = length + FRAME_STATIC_FIELD_LENGTH + 1;

	uint8_t FrameL[(const int)FrameLengthL];

	FrameL[FrameIndexes::Sentinel] = FRAME_SENTINEL;
	FrameL[FrameIndexes::FrmType] = FrameType::Request;
	FrameL[FrameIndexes::Length] = length + 1;
	FrameL[FrameIndexes::OperationCode] = opcode;
	for (uint8_t index = 0; index < length; index++)
	{
		FrameL[index + FRAME_STATIC_FIELD_OFFSET] = payload[index];
	}

	uint8_t CRCL[FRAME_CRC_LEN] = { 0, 0 };
	calculate_CRC(FrameL, (uint8_t)(FrameLengthL - FRAME_CRC_LEN), CRCL);

	for (uint8_t index = 0; index < FRAME_CRC_LEN; index++)
	{
		uint8_t FrmaeIndexL = index + FRAME_STATIC_FIELD_OFFSET + length;
		FrameL[FrmaeIndexL] = CRCL[index];
	}

	m_port->write(FrameL, FrameLengthL);
}

/** @brief Send RAW response frame.
 *  @param opcode uint8_t, Operation code.
 *  @param frame uint8_t*, Command for this operation code.
 *  @param length const uint8_t, Length of the payload.
 *  @return Void.
 */
void SUPERClass::send_raw_response(uint8_t opcode, uint8_t status, uint8_t * payload, const uint8_t length) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

	const int FrameLengthL = length + FRAME_STATIC_FIELD_LENGTH + FRAME_RESPONSE_PAYLOAD_OFFSET;

	uint8_t FrameL[(const int)FrameLengthL];

	FrameL[FrameIndexes::Sentinel] = FRAME_SENTINEL;
	FrameL[FrameIndexes::FrmType] = FrameType::Response;
	FrameL[FrameIndexes::Length] = length + FRAME_RESPONSE_PAYLOAD_OFFSET;
	FrameL[FrameIndexes::OperationCode] = opcode;
	FrameL[FrameIndexes::StatusCode] = status;
	for (uint8_t index = 0; index < length; index++)
	{
		FrameL[index + FRAME_STATIC_FIELD_OFFSET + 1] = payload[index];
	}

	uint8_t CRCL[FRAME_CRC_LEN] = { 0, 0 };
	calculate_CRC(FrameL, (uint8_t)(FrameLengthL - FRAME_CRC_LEN), CRCL);

	for (uint8_t index = 0; index < FRAME_CRC_LEN; index++)
	{
		uint8_t FrmaeIndexL = index + FRAME_STATIC_FIELD_OFFSET + length + 1;
		FrameL[FrmaeIndexL] = CRCL[index];
	}

	m_port->write(FrameL, FrameLengthL);
}

/** @brief Validate the incoming commands.
 *  @param frame The frame string.
 *  @return True if successful; or False if failed.
 */
bool SUPERClass::validate_frame(uint8_t * frame, uint8_t length)
{
	// If frame is less the minimal lent of 6 directly exit.
	if (length < FRAME_MIN_LEN)
	{
		return false;
	}

	// If frame is less the minimal lent of 32 directly exit.
	if (length > FRAME_MAX_LEN)
	{
		return false;
	}

	// Check defined size.
	if (frame[FrameIndexes::Length] != (length - FRAME_STATIC_FIELD_LENGTH))
	{
		return false;
	}

	// Check sentinel value.
	if (frame[FrameIndexes::Sentinel] != FRAME_SENTINEL)
	{
		return false;
	}

	// Check request or response value.
	if ((frame[FrameIndexes::FrmType] != FrameType::Request) &&
		(frame[FrameIndexes::FrmType] != FrameType::Response))
	{
		return false;
	}

	if (validate_CRC(frame, length) == false)
	{
		return false;
	}

	return true;
}

/** @brief Validate the CRC.
 *  @param frame The frame string.
 *  @return True if successful; or False if failed.
 */
bool SUPERClass::validate_CRC(uint8_t * frame, uint8_t length)
{
	uint8_t PayloadLen = length - FRAME_CRC_LEN;
	uint8_t CRCL[FRAME_CRC_LEN] = { 0, 0 };

	calculate_CRC(frame, PayloadLen, CRCL);

	// Check odd byte and even bytes.
	return (frame[PayloadLen] == CRCL[0]) && (frame[PayloadLen + 1] == CRCL[1]);
}

/** @brief Calculate the CRC.
 *  @param frame uint8_t *, The frame.
 *  @param length uint8_t, Length of the payload.
 *  @param out uint8_t *, CRC frame.
 *  @return Void
 */
void SUPERClass::calculate_CRC(uint8_t * frame, uint8_t length, uint8_t * out)
{
	for (uint8_t index = 0; index < length; index++)
	{
		// Odd
		if ((index % 2 == 0))
		{
			// Sum all odd indexes.
			out[0] ^= frame[index];
		}
		// Even
		else
		{
			// Sum all even indexes.
			out[1] ^= frame[index];
		}
	}
}

/** @brief Extract the payload of the frame.
 *  @param frame String, Frame.
 *  @return Void.
 */
void SUPERClass::get_payload(uint8_t * frame, uint8_t length, uint8_t * out)
{
	uint8_t EndIndexL = frame[FrameIndexes::Length] - 1;

	for (uint8_t index = 0; index < EndIndexL; index++)
	{
		out[index] = frame[index + FRAME_STATIC_FIELD_OFFSET];
	}
}

/** @brief Extract the payload of the frame.
 *  @param frame uint8_t *, Frame buffer.
 *  @param length uint8_t, Length of the frame.
 *  @return Void.
 */
void SUPERClass::clear_frame(uint8_t * frame, uint8_t length)
{
	for (uint8_t index = 0; index < length; index++)
	{
		frame[index] = 0;
	}
}

/** @brief Parse and execute the incoming commands.
 *  @param frame, uint8_t *, The frame array.
 *  @param length, uint8_t, The frame array length.
 *  @return Void.
 */
void SUPERClass::parse_frame(uint8_t * frame, uint8_t length)
{
	if (frame[FrameIndexes::FrmType] == Request)
	{
		if (cbRequest != nullptr)
		{
			get_payload(frame, length, m_payloadRequest);

			// cbRequest(frame[FrameIndexes::OperationCode], length, m_payloadRequest);
			cbRequest(frame[FrameIndexes::OperationCode], frame[FrameIndexes::Length], m_payloadRequest);
		}
	}
	else if (frame[FrameIndexes::FrmType] == FrameType::Response)
	{
		/*
		if (frame[FrameIndexes::OperationCode] == OpCodes::Ping)
		{
		}
		else if (frame[FrameIndexes::OperationCode] == OpCodes::Stop)
		{
		}
		*/
	}
}

/** @brief Read incoming commands.
 *  @return Void.
 */
void SUPERClass::read_frame() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

	static uint8_t TemporalDataLengthL = 0;
	static uint8_t CommStateL = fsSentinel;
	static uint8_t InByteL = 0;

	if (m_port->available() < 1)
	{
		return;
	}

	InByteL = 0;
	while (m_port->available() > 0)
	{
		InByteL = m_port->read();

		switch (CommStateL)
		{
		case fsSentinel:
			if (InByteL == FRAME_SENTINEL)
			{
				m_frameBuffer[FrameIndexes::Sentinel] = InByteL;
				CommStateL = fsRequestResponse;
#ifdef SHOW_STATES
				DEBUGLOG("fsSentinel -> fsRequestResponse\r\n");
				DEBUGLOG("Sentinel: %02X (%d)\r\n", InByteL, InByteL);
#endif
			}
			break;

		case fsRequestResponse:
			// Check request or response value.
			if ((InByteL == FrameType::Request) ||
				(InByteL == FrameType::Response))
			{
				m_frameBuffer[FrameIndexes::FrmType] = InByteL;
				CommStateL = fsLength;
#ifdef SHOW_STATES
				DEBUGLOG("fsRequestResponse -> fsLength\r\n");
				DEBUGLOG("FrmType: %02X (%d)\r\n", InByteL, InByteL);
#endif
			}
			else
			{
				CommStateL = fsSentinel;
#ifdef SHOW_STATES
				DEBUGLOG("fsRequestResponse -> fsSentinel\r\n");
#endif
			}
			break;

		case fsLength:
			if ((InByteL >= 1) &&
				(InByteL <= 27))
			{
				m_frameBuffer[FrameIndexes::Length] = InByteL;
				CommStateL = fsOperationCode;
#ifdef SHOW_STATES
				DEBUGLOG("fsLength -> fsOperationCode\r\n");
				DEBUGLOG("Length: %02X (%d)\r\n", InByteL, InByteL);
#endif
			}
			else
			{
				CommStateL = fsSentinel;
#ifdef SHOW_STATES
				DEBUGLOG("fsLength -> fsSentinel\r\n");
#endif
			}
			break;

		case fsOperationCode:
			m_frameBuffer[FrameIndexes::OperationCode] = InByteL;
			if (m_frameBuffer[FrameIndexes::Length] > 1)
			{
				TemporalDataLengthL = m_frameBuffer[FrameIndexes::Length] - 1;
				CommStateL = fsData;
#ifdef SHOW_STATES
				DEBUGLOG("fsOperationCode -> fsData\r\n");
				DEBUGLOG("OpCode: %02X (%d)\r\n", InByteL, InByteL);
#endif
			}
			else
			{
				TemporalDataLengthL = FRAME_CRC_LEN;
				CommStateL = fsCRC;
#ifdef SHOW_STATES
				DEBUGLOG("fsOperationCode -> fsCRC\r\n");
#endif
			}
			m_ptrFrameBuffer = &m_frameBuffer[FRAME_REQUEST_STATIC_FIELD_SIZE];
			break;

		case fsData:
			*m_ptrFrameBuffer++ = InByteL;
#ifdef SHOW_STATES
			DEBUGLOG("Data: %02X (%d)\r\n", InByteL, InByteL);
#endif
			if (--TemporalDataLengthL == 0)
			{
				TemporalDataLengthL = FRAME_CRC_LEN;
				CommStateL = fsCRC;
#ifdef SHOW_STATES
				DEBUGLOG("fsData -> fsCRC\r\n");
#endif
			}
			break;

		case fsCRC:
			*m_ptrFrameBuffer++ = InByteL;
			if (--TemporalDataLengthL == 0)
			{
#ifdef SHOW_STATES
				for (uint8_t index = 0; index < m_frameBuffer[FrameIndexes::Length] + 5; index++)
				{
					DEBUGLOG("%02X ", m_frameBuffer[index]);
				}
				DEBUGLOG("\r\n");
#endif
				if (validate_CRC(m_frameBuffer, m_frameBuffer[FrameIndexes::Length] + FRAME_REQUEST_STATIC_FIELD_SIZE - 1 + FRAME_CRC_LEN))
				{
					parse_frame(m_frameBuffer, m_frameBuffer[FrameIndexes::Length] + FRAME_REQUEST_STATIC_FIELD_SIZE - 1);
				}
				else
				{
#ifdef SHOW_STATES
					DEBUGLOG("Invalid CRC\r\n");
#endif
				}
				CommStateL = fsSentinel;
			}
			break;

		default:
			break;
		}
	}
}

/**
 * @brief Construct a new SUPERClass::SUPERClass object
 * 
 */
SUPERClass::SUPERClass()
{
	m_previousMillis = 0;
	m_currentMillis = 0;
}

/**
 * @brief Initialize the SUPER.
 * 
 * @param port 
 */
void SUPERClass::init(Stream &port) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

#ifdef ARDUINO_ARCH_ESP32

	if (&port == m_port)
	{
		return;
	}

#else

	if (&port == &m_port)
	{
		return;
	}

#endif


	m_port = &port;
}

void SUPERClass::update() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

	m_currentMillis = millis();

	if (m_currentMillis - m_previousMillis >= UPDATE_RATE)
	{
		// Save the last time you updated the bus.
		m_previousMillis = m_currentMillis;

		// Read frame form serial.
		read_frame();
	}
}

/** @brief Set the callback.
 *  @param callback, Callback pointer.
 *  @return Void.
 */
void SUPERClass::setCbRequest(void(*callback)(uint8_t opcode, uint8_t size, uint8_t * payload)) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif

	cbRequest = callback;
}

SUPERClass SUPER;
