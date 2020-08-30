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

#include "BusControl.h"

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void setup_bus()
{
	// Control bus.
	pinMode(PIN_IOW, OUTPUT);
	pinMode(PIN_IOR, OUTPUT);

	// Set default pin state.
	digitalWrite(PIN_IOW, HIGH);
	digitalWrite(PIN_IOR, HIGH);

	// Output data bus.
	pinMode(PIN_DI0, OUTPUT);
	pinMode(PIN_DI1, OUTPUT);
	pinMode(PIN_DI2, OUTPUT);
	pinMode(PIN_DI3, OUTPUT);

	// Input data bus.
	pinMode(PIN_DO0, INPUT_PULLUP);
	pinMode(PIN_DO1, INPUT_PULLUP);
	pinMode(PIN_DO2, INPUT_PULLUP);
	pinMode(PIN_DO3, INPUT_PULLUP);

	// Address bus.
	pinMode(PIN_AO0, OUTPUT);
	pinMode(PIN_AO1, OUTPUT);
	pinMode(PIN_AO2, OUTPUT);
}

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void set_address_bus(uint8_t address)
{
	static uint8_t a0State = 0;
	static uint8_t a1State = 0;
	static uint8_t a2State = 0;

	a0State = bitRead(address, 0);
	a1State = bitRead(address, 1);
	a2State = bitRead(address, 2);

	digitalWrite(PIN_AO0, a0State);
	digitalWrite(PIN_AO1, a1State);
	digitalWrite(PIN_AO2, a2State);
}

/** @brief Generate IO Write signal.
 *  @return Void.
 */
void iow()
{
	digitalWrite(PIN_IOW, LOW);
	delayMicroseconds(500);
	digitalWrite(PIN_IOW, HIGH);
}

/** @brief Generate IO Read signal.
 *  @return Void.
 */
void ior()
{
	digitalWrite(PIN_IOR, LOW);
	delayMicroseconds(500);
	digitalWrite(PIN_IOR, HIGH);
}

/** @brief Read digital signal of the outputs.
 *  @return uint8_t, state of the pins.
 */
uint8_t read_do()
{
	static uint8_t StateL = 0;

	bitWrite(StateL, 0, (analogRead(PIN_DO0) > 512));
	bitWrite(StateL, 1, (analogRead(PIN_DO1) > 512));
	bitWrite(StateL, 2, (analogRead(PIN_DO2) > 512));
	bitWrite(StateL, 3, (analogRead(PIN_DO3) > 512));

	return StateL;
}

/** @brief Write digital signal of the inputs.
 *  @param State of the digital inputs.
 *  @return Void.
 */
void write_di(uint8_t data)
{
	static uint8_t Di0StateL = 0;
	static uint8_t Di1StateL = 0;
	static uint8_t Di2StateL = 0;
	static uint8_t Di3StateL = 0;

	Di0StateL = bitRead(data, 0);
	Di1StateL = bitRead(data, 1);
	Di2StateL = bitRead(data, 2);
	Di3StateL = bitRead(data, 3);

	digitalWrite(PIN_DI0, Di0StateL);
	digitalWrite(PIN_DI1, Di1StateL);
	digitalWrite(PIN_DI2, Di2StateL);
	digitalWrite(PIN_DI3, Di3StateL);
}