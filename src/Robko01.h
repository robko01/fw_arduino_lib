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

// OperationsCodes.h

#ifndef _ROBKO01_h
#define _ROBKO01_h

/** @brief Count of the robot axises. */
#define AXIS_COUNT 6

/** @brief Base address axis. */
#define AXIS_BASE 0

/** @brief Shoulder address axis. */
#define AXIS_SHOULDER 1

/** @brief Elbow address axis. */
#define AXIS_ELBOW 2

/** @brief Left different address axis. */
#define AXIS_LEFT_DIFFERENT 3

/** @brief Right different address axis. */
#define AXIS_RIGHT_DIFFERENT 4

/** @brief Gripper address axis. */
#define AXIS_GRIPPER 5

#define PORT_A_1 6

#define PORT_A_2 7

#define ADDRESS_COUNT 8

#define DEFAULT_ACCELERATION 500.0f //250

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "BusConfig.h"

#include "JointPosition.h"

/* Stepper motor controller. */
#include <AccelStepper.h>

enum OperationModes : uint8_t
{
	NONE = 0U,
	Positioning,
	Speed,
};

class Robko01Class
{

    protected:

#pragma region Variables

    bool m_motorsEnabled;

	BusConfig_t m_BusConfig;

    /** @brief Port A input low 4 bits. */
    uint8_t m_currentAddressIndex;

    /** @brief Port A input low 4 bits. */
    uint8_t m_portLoAIn;

    /** @brief Port A input high 4 bits. */
    uint8_t m_portHiAIn;

    /** @brief Port A output. */
    uint8_t m_portAOut;

    /** @brief Motor states, does it active. */
    uint8_t m_motorState;

    /** @brief Operation mode. */
    uint8_t m_operationMode;

    /** @brief Create 6 stepper motors. */
    AccelStepper m_steppers[AXIS_COUNT];

#pragma endregion

#pragma region Protected Methods

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

    /** @brief Set address bus.
     *  @param uint8_t address, Address bus value.
     *  @return Void.
     */
    void setup_bus();

    /** @brief Set address bus.
     *  @param uint8_t address, Address bus value.
     *  @return Void.
     */
    void set_address_bus(uint8_t address);

    /** @brief Generate IO Write signal.
     *  @return Void.
     */
    void iow();

    /** @brief Generate IO Read signal.
     *  @return Void.
     */
    void ior();

    /** @brief Read digital signal of the outputs.
     *  @return uint8_t, state of the pins.
     */
    uint8_t read_do();

    /** @brief Write digital signal of the inputs.
     *  @param State of the digital inputs.
     *  @return Void.
     */
    void write_di(uint8_t data);

    /** @brief Setup robot.
     *  @return Void.
     */
    void setup_robot();

#pragma endregion

    public:

#pragma region Methods

    void init(BusConfig_t* config);

	void update();

    bool motors_enabled();

    uint8_t get_motor_state();

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

};

/** @brief Instance of the Robko01 robot. */
extern Robko01Class Robko01;

#endif

