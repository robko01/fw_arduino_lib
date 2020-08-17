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

// MotorControl.h

#ifndef _ROBOTCONTROL_h
#define _ROBOTCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions

/** @brief Default speed of the motors. */
#define DEFAULT_SPEED 40.0f

/** @brief Default acceleration of the motors. */
#define DEFAULT_ACCELERATION 500.0f //250

#pragma endregion

#pragma region Headers

/* Stepper motor controller. */
#include <AccelStepper.h>

#include "BusControl.h"

#include "JointPositionUnion.h"

#include "JointPosition.h"

#pragma endregion

#pragma region Addresses Indexes

/** @brief Count of the robot axises. */
#define AXIS_COUNT 6

/** @brief Base address axis. */
#define AXIS_BASE 0

/** @brief Shoulder address axis. */
#define AXIS_SHOULDER 1

/** @brief Elbow address axis. */
#define AXIS_ELBOW 2

/** @brief Left different address axis. */
#define AXIS_LEFT_DEFFERENT 3

/** @brief Right different address axis. */
#define AXIS_RIGHT_DEFFERENT 4

/** @brief Gripper address axis. */
#define AXIS_GRIPPER 5

#define PORT_A_1 6

#define PORT_A_2 7

#define ADDRESS_COUNT 8

#pragma endregion

#pragma region Enums

enum OperationModes : uint8_t
{
	NONE = 0U,
	Positioning,
	Speed,
};
#pragma endregion


#pragma region Function Prototypes

/** @brief Setup the robot.
 *  @return Void.
 */
void setup_robot();

/** @brief Stop all motors.
 *  @return Void.
 */
void stop_motors();

/** @brief Disable all motors.
 *  @return Void.
 */
void disable_motors();

/** @brief Enable all motors.
 *  @return Void.
 */
void enable_motors();

/** @brief Clear all motors position to 0.
 *  @return Void.
 */
void clear_motors();

/** @brief Move relatively to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_relative(JointPosition_t position);

/** @brief Move absolutely to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_absolute(JointPosition_t position);

/** @brief Move by speed.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_speed(JointPosition_t position);

/** @brief Move by speed.
 *  @return JointPosition_t, current robot position.
 */
JointPosition_t get_position();

/** @brief Update motor regulators.
 *  @param uint8_t address, Address of the axis.
 *  @return uint8_t, Motors states.
 */
uint8_t update_motor(uint8_t address);

/** @brief Update Port A of the robot.
 *  @param uint8_t address, Sub address of the port.
 *  @return Void.
 */
void update_port_a(uint8_t address);

/** @brief Set Port A of the robot.
 *  @param uint8_t value, Value of the port.
 *  @return Void.
 */
void set_port_a(uint8_t value);

/** @brief Get Port A of the robot.
 *  @return uint8_t, Port A input state.
 */
uint8_t get_port_a();

#pragma endregion

#endif

