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

#include "RobotControl.h"

/** @brief Port A input low 4 bits. */
static uint8_t PortLoAIn_g;

/** @brief Port A input high 4 bits. */
static uint8_t PortHiAIn_g;

/** @brief Port A output. */
static uint8_t PortAOut_g;

/** @brief Motor states, does it active. */
static uint8_t MotorState_g;

/** @brief Operation mode. */
static uint8_t OperationMode_g;

/** @brief Create 6 stepper motors. */
static AccelStepper Steppers_g[AXIS_COUNT];

/** @brief Setup robot.
 *  @return Void.
 */
void setup_robot()
{
	PortLoAIn_g = 0;
	PortHiAIn_g = 0;
	PortAOut_g = 0;
	MotorState_g = 0;
	OperationMode_g = OperationModes::NONE;

	// Setup gpio bus signals.
	setup_bus();

	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		Steppers_g[address] = AccelStepper(AccelStepper::FULL4WIRE, PIN_DI0, PIN_DI1, PIN_DI2, PIN_DI3); // HALF4WIRE, FULL4WIRE
		//Steppers_g[address].setMaxSpeed(DEFAULT_SPEED);
		Steppers_g[address].setAcceleration(DEFAULT_ACCELERATION);
		Steppers_g[address].setCurrentPosition(0);
		Steppers_g[address].stop();
		Steppers_g[address].enableOutputs();
		set_address_bus(address);
		iow();
	}	
}

/** @brief Stop all motors.
 *  @return Void.
 */
void stop_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		Steppers_g[address].stop();
		//Steppers_g[address].setSpeed(0);
		set_address_bus(address);
		iow();
	}
}

/** @brief Disable all motors.
 *  @return Void.
 */
void disable_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		Steppers_g[address].disableOutputs();
		set_address_bus(address);
		iow();
	}
}

/** @brief Enable all motors.
 *  @return Void.
 */
void enable_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		Steppers_g[address].enableOutputs();
		set_address_bus(address);
		iow();
	}
}

/** @brief Clear all motors position to 0.
 *  @return Void.
 */
void clear_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		Steppers_g[address].setCurrentPosition(0);
	}
}

/** @brief Move relatively to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_relative(JointPosition_t position)
{
	OperationMode_g = OperationModes::Positioning;

	Steppers_g[AXIS_BASE].move(position.BasePos);
	Steppers_g[AXIS_BASE].setSpeed(position.BaseSpeed);
	Steppers_g[AXIS_SHOULDER].move(position.ShoulderPos);
	Steppers_g[AXIS_SHOULDER].setSpeed(position.ShoulderSpeed);
	Steppers_g[AXIS_ELBOW].move(position.ElbowPos);
	Steppers_g[AXIS_ELBOW].setSpeed(position.ElbowSpeed);
	Steppers_g[AXIS_LEFT_DEFFERENT].move(position.LeftDiffPos);
	Steppers_g[AXIS_LEFT_DEFFERENT].setSpeed(position.LeftDiffSpeed);
	Steppers_g[AXIS_RIGHT_DEFFERENT].move(position.RightDiffPos);
	Steppers_g[AXIS_RIGHT_DEFFERENT].setSpeed(position.RightDiffSpeed);
	Steppers_g[AXIS_GRIPPER].move(position.GripperPos);
	Steppers_g[AXIS_GRIPPER].setSpeed(position.GripperSpeed);
}

/** @brief Move absolutely to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_absolute(JointPosition_t position)
{
	OperationMode_g = OperationModes::Positioning;

	Steppers_g[AXIS_BASE].moveTo(position.BasePos);
	Steppers_g[AXIS_BASE].setSpeed(position.BaseSpeed);
	Steppers_g[AXIS_BASE].setMaxSpeed(position.BaseSpeed + 20);
	Steppers_g[AXIS_SHOULDER].moveTo(position.ShoulderPos);
	Steppers_g[AXIS_SHOULDER].setSpeed(position.ShoulderSpeed);
	Steppers_g[AXIS_SHOULDER].setMaxSpeed(position.ShoulderSpeed + 20);
	Steppers_g[AXIS_ELBOW].moveTo(position.ElbowPos);
	Steppers_g[AXIS_ELBOW].setSpeed(position.ElbowSpeed);
	Steppers_g[AXIS_ELBOW].setMaxSpeed(position.ElbowSpeed + 20);
	Steppers_g[AXIS_LEFT_DEFFERENT].moveTo(position.LeftDiffPos);
	Steppers_g[AXIS_LEFT_DEFFERENT].setSpeed(position.LeftDiffSpeed);
	Steppers_g[AXIS_LEFT_DEFFERENT].setMaxSpeed(position.LeftDiffSpeed + 20);
	Steppers_g[AXIS_RIGHT_DEFFERENT].moveTo(position.RightDiffPos);
	Steppers_g[AXIS_RIGHT_DEFFERENT].setSpeed(position.RightDiffSpeed);
	Steppers_g[AXIS_RIGHT_DEFFERENT].setMaxSpeed(position.RightDiffSpeed + 20);
	Steppers_g[AXIS_GRIPPER].moveTo(position.GripperPos);
	Steppers_g[AXIS_GRIPPER].setSpeed(position.GripperSpeed);
	Steppers_g[AXIS_GRIPPER].setMaxSpeed(position.GripperSpeed + 20);
}

/** @brief Move by speed.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void move_speed(JointPosition_t position)
{
	OperationMode_g = OperationModes::Speed;

	Steppers_g[AXIS_BASE].setSpeed(position.BaseSpeed);
	Steppers_g[AXIS_SHOULDER].setSpeed(position.ShoulderSpeed);
	Steppers_g[AXIS_ELBOW].setSpeed(position.ElbowSpeed);
	Steppers_g[AXIS_LEFT_DEFFERENT].setSpeed(position.LeftDiffSpeed);
	Steppers_g[AXIS_RIGHT_DEFFERENT].setSpeed(position.RightDiffSpeed);
	Steppers_g[AXIS_GRIPPER].setSpeed(position.GripperSpeed);
}

/** @brief Move by speed.
 *  @return JointPosition_t, current robot position.
 */
JointPosition_t get_position()
{
	JointPosition_t PositionL;

	PositionL.BasePos = (int16_t)Steppers_g[AXIS_BASE].currentPosition();
	PositionL.BaseSpeed = (int16_t)Steppers_g[AXIS_BASE].speed();
	PositionL.ShoulderPos = (int16_t)Steppers_g[AXIS_SHOULDER].currentPosition();
	PositionL.ShoulderSpeed = (int16_t)Steppers_g[AXIS_SHOULDER].speed();
	PositionL.ElbowPos = (int16_t)Steppers_g[AXIS_ELBOW].currentPosition();
	PositionL.ElbowSpeed = (int16_t)Steppers_g[AXIS_ELBOW].speed();
	PositionL.LeftDiffPos = (int16_t)Steppers_g[AXIS_LEFT_DEFFERENT].currentPosition();
	PositionL.LeftDiffSpeed = (int16_t)Steppers_g[AXIS_LEFT_DEFFERENT].speed();
	PositionL.RightDiffPos = (int16_t)Steppers_g[AXIS_RIGHT_DEFFERENT].currentPosition();
	PositionL.RightDiffSpeed = (int16_t)Steppers_g[AXIS_RIGHT_DEFFERENT].speed();
	PositionL.GripperPos = (int16_t)Steppers_g[AXIS_GRIPPER].currentPosition();
	PositionL.GripperSpeed = (int16_t)Steppers_g[AXIS_GRIPPER].speed();

	return PositionL;
}

/** @brief Update motor regulators.
 *  @param uint8_t address, Address of the axis.
 *  @return uint8_t, Motors states.
 */
uint8_t update_motor(uint8_t address)
{
	Steppers_g[address].disableOutputs();
	Steppers_g[address].enableOutputs();

	if (OperationMode_g == OperationModes::Positioning)
	{
		bitWrite(MotorState_g, address, Steppers_g[address].run());
	}
	else if(OperationMode_g == OperationModes::Speed)
	{
		bitWrite(MotorState_g, address, Steppers_g[address].runSpeed());
	}

	set_address_bus(address);
	iow();

	return MotorState_g;
}

/** @brief Update Port A of the robot.
 *  @param uint8_t address, Sub address of the port.
 *  @return Void.
 */
void update_port_a(uint8_t address)
{
	// Read operation.
	if (address == PORT_A_1)
	{
		// Set the address bus.
		set_address_bus(address);

		// Remember data.
		PortLoAIn_g = read_do();

		// Validate bus.
		ior();
	}
	else if (address == PORT_A_2)
	{
		// Set the address bus.
		set_address_bus(address);

		// Read data bus.
		PortHiAIn_g = read_do();

		// Validate bus.
		ior();
	}

	// Write operation.
	if (address == PORT_A_1)
	{
		// Set the address bus.
		set_address_bus(address);

		// Write data bus.
		write_di((PortAOut_g & 0x0F));

		// Validate bus.
		iow();
	}
	else if (address == PORT_A_2)
	{
		// Set the address bus.
		set_address_bus(address);

		// Write data bus.
		write_di(((PortAOut_g >> 4) & 0x0F));

		// Validate bus.
		iow();
	}
}

/** @brief Set Port A of the robot.
 *  @param uint8_t value, Value of the port.
 *  @return Void.
 */
void set_port_a(uint8_t value)
{
	PortAOut_g = value;
}

/** @brief Get Port A of the robot.
 *  @return uint8_t, Port A input state.
 */
uint8_t get_port_a()
{
	return ((PortLoAIn_g | (PortHiAIn_g << 4)));
}



