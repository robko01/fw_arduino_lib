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

#ifndef _SUPER_h
#define _SUPER_h

#pragma region Definitions

// #define SHOW_STATES

/** @brief Communication port update rate. */
#define UPDATE_RATE 22

/** @brief Minimum frame length. */
#define FRAME_MIN_LEN 6

/** @brief Maximum frame length. */
#define FRAME_MAX_LEN 32

/** @brief Frame data size. */
#define FRAME_MAX_DATA_LEN 26

/** @brief Length of the CRC. */
#define FRAME_CRC_LEN 2

/** @brief Number 5 stand of sum of length of static size fields;
 * 1 - Start
 * 1 - Request / Response
 * 1 - Length
 * 2 - CRC
 */
#define FRAME_STATIC_FIELD_LENGTH 5

/** @brief Static frame field offset. */
#define FRAME_STATIC_FIELD_OFFSET 4

/** @brief Frame sentinel. */
#define FRAME_SENTINEL 0xAA

/** @brief Payload response offset value. */
#define FRAME_RESPONSE_PAYLOAD_OFFSET 2

/** @brief Number 4 stand of sum of Sentinel, Req/Res, Length, Operation Code;
 * 1 - Start
 * 1 - Request / Response
 * 1 - Length
 * 1 - OpCode
 */
#define FRAME_REQUEST_STATIC_FIELD_SIZE 4

#pragma endregion

#pragma region Headers

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DebugPort.h"

#pragma endregion

#pragma region Enums

/** @brief Status codes types. */
enum StatusCodes : uint8_t
{
	Ok = 1U, ///< When everything is OK.
	Error, ///< When error occurred.
	Busy, ///< When busy in other operation.
	TimeOut ///< Then time for the operation has timed out.
};

#pragma endregion

class SUPERClass
{

	protected:

#pragma region Enum

	/** @brief Communication state machine states. */
#if defined(ESP32)
	enum : uint8_t
#else
	typedef enum : uint8_t	
#endif
	{
		fsSentinel = 0U, ///< Beginning byte.
		fsRequestResponse, ///< Request / Response byte.
		fsLength, ///< Length of the frame byte.
		fsOperationCode, ///< Operation Code byte.
		fsData, //< Data part.
		fsCRC, ///< CRC state.
	};

	/** @brief Frame indexes. */
	enum FrameIndexes : uint8_t
	{
		Sentinel = 0U, ///< Sentinel byte.
		FrmType, ///< Request / Response byte.
		Length, ///< Length of the frame.
		OperationCode,  ///< Operation Code byte.
		StatusCode  ///< Status Code byte.
	};

	/** @brief Request / Response types. */
	enum FrameType : uint8_t
	{
		Request = 1U, ///< Request.
		Response, ///< Response.
	};

#pragma endregion

#pragma region Variables

	Stream * m_port;

	/** @brief Communication frame buffer. */
	uint8_t m_frameBuffer[FRAME_MAX_LEN];

	/** @brief Pointer to frame buffer. */
	uint8_t * m_ptrFrameBuffer;

	/** @brief Payload request buffer. */
	uint8_t m_payloadRequest[FRAME_MAX_DATA_LEN];

	/** @brief Payload response buffer. */
	uint8_t m_payloadResponse[FRAME_MAX_DATA_LEN];

	void(*cbRequest)(uint8_t opcode, uint8_t size, uint8_t * payload);

	/** @brief Will store last time that the bus was updated. */
	unsigned long m_previousMillis;

	/** @brief Will store current time that the bus is updated. */
	unsigned long m_currentMillis;


#pragma endregion

#pragma region Methods

	/** @brief Send RAW request frame.
	 *  @param opcode uint8_t, Operation code.
	 *  @param frame uint8_t*, Command for this operation code.
	 *  @param length const uint8_t, Length of the payload.
	 *  @return Void.
	 */
	void send_raw_request(uint8_t opcode, uint8_t * payload, const uint8_t length);

	/** @brief Validate the incoming commands.
	 *  @param frame The frame string.
	 *  @return True if successful; or False if failed.
	 */
	bool validate_frame(uint8_t * frame, uint8_t length);

	/** @brief Validate the CRC.
	 *  @param frame The frame string.
	 *  @return True if successful; or False if failed.
	 */
	bool validate_CRC(uint8_t * frame, uint8_t length);

	/** @brief Calculate the CRC.
	 *  @param frame uint8_t *, The frame.
	 *  @param length uint8_t, Length of the payload.
	 *  @param out uint8_t *, CRC frame.
	 *  @return Void
	 */
	void calculate_CRC(uint8_t * frame, uint8_t length, uint8_t * out);

	/** @brief Extract the payload of the frame.
	 *  @param frame String, Frame.
	 *  @return Void.
	 */
	void get_payload(uint8_t * frame, uint8_t length, uint8_t * out);

	/** @brief Extract the payload of the frame.
	 *  @param frame uint8_t *, Frame buffer.
	 *  @param length uint8_t, Length of the frame.
	 *  @return Void.
	 */
	void clear_frame(uint8_t * frame, uint8_t length);

	void parse_frame(uint8_t * frame, uint8_t length);

	void read_frame();

#pragma endregion

	public:

#pragma region Methods

	SUPERClass();

	void init(Stream &port);

	void update();

	/** @brief Set the read callback.
	 *  @param callback, Callback pointer.
	 *  @return Void.
	 */
	void setCbRequest(void(*callback)(uint8_t opcode, uint8_t size, uint8_t * payload));

	/** @brief Send RAW response frame.
	 *  @param opcode uint8_t, Operation code.
	 *  @param frame uint8_t*, Command for this operation code.
	 *  @param length const uint8_t, Length of the payload.
	 *  @return Void.
	 */
	void send_raw_response(uint8_t opcode, uint8_t status, uint8_t * payload, const uint8_t length);

#pragma endregion

};

/** @brief Instance of the SUPER communicator. */
extern SUPERClass SUPER;

#endif
