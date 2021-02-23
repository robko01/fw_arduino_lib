
#include "robko01.h"

/** @brief Generate IO Write signal.
 *  @return Void.
 */
void Robko01Class::iow()
{
	digitalWrite(m_BusConfig.IOW, LOW);
	delayMicroseconds(500);
	digitalWrite(m_BusConfig.IOW, HIGH);
}

/** @brief Generate IO Read signal.
 *  @return Void.
 */
void Robko01Class::ior()
{
	digitalWrite(m_BusConfig.IOR, LOW);
	delayMicroseconds(500);
	digitalWrite(m_BusConfig.IOR, HIGH);
}

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void Robko01Class::set_address_bus(uint8_t address)
{
	static uint8_t a0State = 0;
	static uint8_t a1State = 0;
	static uint8_t a2State = 0;

	a0State = bitRead(address, 0);
	a1State = bitRead(address, 1);
	a2State = bitRead(address, 2);

	digitalWrite(m_BusConfig.AO0, a0State);
	digitalWrite(m_BusConfig.AO1, a1State);
	digitalWrite(m_BusConfig.AO2, a2State);
}

/** @brief Read digital signal of the outputs.
 *  @return uint8_t, state of the pins.
 */
uint8_t Robko01Class::read_do()
{
	static uint8_t StateL = 0;

	bitWrite(StateL, 0, (analogRead(m_BusConfig.DO0) > 512));
	bitWrite(StateL, 1, (analogRead(m_BusConfig.DO1) > 512));
	bitWrite(StateL, 2, (analogRead(m_BusConfig.DO2) > 512));
	bitWrite(StateL, 3, (analogRead(m_BusConfig.DO3) > 512));

	return StateL;
}

/** @brief Write digital signal of the inputs.
 *  @param State of the digital inputs.
 *  @return Void.
 */
void Robko01Class::write_di(uint8_t data)
{
	static uint8_t Di0StateL = 0;
	static uint8_t Di1StateL = 0;
	static uint8_t Di2StateL = 0;
	static uint8_t Di3StateL = 0;

	Di0StateL = bitRead(data, 0);
	Di1StateL = bitRead(data, 1);
	Di2StateL = bitRead(data, 2);
	Di3StateL = bitRead(data, 3);

	digitalWrite(m_BusConfig.DI0, Di0StateL);
	digitalWrite(m_BusConfig.DI1, Di1StateL);
	digitalWrite(m_BusConfig.DI2, Di2StateL);
	digitalWrite(m_BusConfig.DI3, Di3StateL);
}

/** @brief Set address bus.
 *  @param uint8_t address, Address bus value.
 *  @return Void.
 */
void Robko01Class::setup_bus()
{
	// Control bus.
	pinMode(m_BusConfig.IOW, OUTPUT);
	pinMode(m_BusConfig.IOR, OUTPUT);

	// Set default pin state.
	iow();
	ior();

	// Output data bus.
	pinMode(m_BusConfig.DI0, OUTPUT);
	pinMode(m_BusConfig.DI1, OUTPUT);
	pinMode(m_BusConfig.DI2, OUTPUT);
	pinMode(m_BusConfig.DI3, OUTPUT);

	// Input data bus.
	pinMode(m_BusConfig.DO0, INPUT_PULLUP);
	pinMode(m_BusConfig.DO1, INPUT_PULLUP);
	pinMode(m_BusConfig.DO2, INPUT_PULLUP);
	pinMode(m_BusConfig.DO3, INPUT_PULLUP);

	// Address bus.
	pinMode(m_BusConfig.AO0, OUTPUT);
	pinMode(m_BusConfig.AO1, OUTPUT);
	pinMode(m_BusConfig.AO2, OUTPUT);
}

/** @brief Setup robot.
 *  @return Void.
 */
void Robko01Class::setup_robot()
{
	m_portLoAIn = 0;
	m_portHiAIn = 0;
	m_portAOut = 0;
	m_motorState = 0;
	m_operationMode = OperationModes::NONE;

	// Setup gpio bus signals.
	setup_bus();

	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address] = AccelStepper(AccelStepper::FULL4WIRE, m_BusConfig.DI0, m_BusConfig.DI1, m_BusConfig.DI2, m_BusConfig.DI3); // HALF4WIRE, FULL4WIRE
		//m_steppers[address].setMaxSpeed(DEFAULT_SPEED);
		m_steppers[address].setAcceleration(DEFAULT_ACCELERATION);
		m_steppers[address].setCurrentPosition(0);
		m_steppers[address].stop();
		m_steppers[address].enableOutputs();
		set_address_bus(address);
		iow();
	}	
}

/** @brief Update motor regulators.
 *  @param uint8_t address, Address of the axis.
 *  @return uint8_t, Motors states.
 */
uint8_t Robko01Class::update_motor(uint8_t address)
{
	m_steppers[address].disableOutputs();
	m_steppers[address].enableOutputs();

	if (m_operationMode == OperationModes::Positioning)
	{
		bitWrite(m_motorState, address, m_steppers[address].run());
	}
	else if(m_operationMode == OperationModes::Speed)
	{
		bitWrite(m_motorState, address, m_steppers[address].runSpeed());
	}

	set_address_bus(address);
	iow();

	return m_motorState;
}

/** @brief Update Port A of the robot.
 *  @param uint8_t address, Sub address of the port.
 *  @return Void.
 */
void Robko01Class::update_port_a(uint8_t address)
{
	// Read operation.
	if (address == PORT_A_1)
	{
		// Set the address bus.
		set_address_bus(address);

		// Remember data.
		m_portLoAIn = read_do();

		// Validate bus.
		ior();
	}
	else if (address == PORT_A_2)
	{
		// Set the address bus.
		set_address_bus(address);

		// Read data bus.
		m_portHiAIn = read_do();

		// Validate bus.
		ior();
	}

	// Write operation.
	if (address == PORT_A_1)
	{
		// Set the address bus.
		set_address_bus(address);

		// Write data bus.
		write_di((m_portAOut & 0x0F));

		// Validate bus.
		iow();
	}
	else if (address == PORT_A_2)
	{
		// Set the address bus.
		set_address_bus(address);

		// Write data bus.
		write_di(((m_portAOut >> 4) & 0x0F));

		// Validate bus.
		iow();
	}
}


void Robko01Class::init(BusConfig_t* config)
{
    m_BusConfig = *config;

    m_currentAddressIndex = 0;

	m_motorsEnabled = false;

    setup_bus();
    setup_robot();
}

void Robko01Class::update()
{
		// Update motors.		
		if (m_currentAddressIndex < AXIS_COUNT)
		{
			// Update motor state.
			m_motorState = update_motor(m_currentAddressIndex);
		}
		else
		{
			// Update port A.
			update_port_a(m_currentAddressIndex);
		}

		// Increment the address bus index.
		m_currentAddressIndex++;

		// Clear the address index.
		if (m_currentAddressIndex >= ADDRESS_COUNT)
		{
			m_currentAddressIndex = 0;
		}
}

bool Robko01Class::motors_enabled()
{
	return m_motorsEnabled;
}

uint8_t Robko01Class::get_motor_state()
{
    return m_motorState;
}

/** @brief Stop all motors.
 *  @return Void.
 */
void Robko01Class::stop_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].stop();
		//m_steppers[address].setSpeed(0);
		set_address_bus(address);
		iow();
	}
}

/** @brief Disable all motors.
 *  @return Void.
 */
void Robko01Class::disable_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].disableOutputs();
		set_address_bus(address);
		iow();
	}

	m_motorsEnabled = false;
}

/** @brief Enable all motors.
 *  @return Void.
 */
void Robko01Class::enable_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].enableOutputs();
		set_address_bus(address);
		iow();
	}

	m_motorsEnabled = true;
}

/** @brief Clear all motors position to 0.
 *  @return Void.
 */
void Robko01Class::clear_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].setCurrentPosition(0);
	}
}

/** @brief Move relatively to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void Robko01Class::move_relative(JointPosition_t position)
{
	m_operationMode = OperationModes::Positioning;

	m_steppers[AXIS_BASE].move(position.BasePos);
	m_steppers[AXIS_BASE].setSpeed(position.BaseSpeed);
	m_steppers[AXIS_SHOULDER].move(position.ShoulderPos);
	m_steppers[AXIS_SHOULDER].setSpeed(position.ShoulderSpeed);
	m_steppers[AXIS_ELBOW].move(position.ElbowPos);
	m_steppers[AXIS_ELBOW].setSpeed(position.ElbowSpeed);
	m_steppers[AXIS_LEFT_DIFFERENT].move(position.LeftDiffPos);
	m_steppers[AXIS_LEFT_DIFFERENT].setSpeed(position.LeftDiffSpeed);
	m_steppers[AXIS_RIGHT_DIFFERENT].move(position.RightDiffPos);
	m_steppers[AXIS_RIGHT_DIFFERENT].setSpeed(position.RightDiffSpeed);
	m_steppers[AXIS_GRIPPER].move(position.GripperPos);
	m_steppers[AXIS_GRIPPER].setSpeed(position.GripperSpeed);
}

/** @brief Move absolutely to position.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void Robko01Class::move_absolute(JointPosition_t position)
{
	m_operationMode = OperationModes::Positioning;

	m_steppers[AXIS_BASE].moveTo(position.BasePos);
	m_steppers[AXIS_BASE].setSpeed(position.BaseSpeed);
	m_steppers[AXIS_BASE].setMaxSpeed(position.BaseSpeed + 20);
	m_steppers[AXIS_SHOULDER].moveTo(position.ShoulderPos);
	m_steppers[AXIS_SHOULDER].setSpeed(position.ShoulderSpeed);
	m_steppers[AXIS_SHOULDER].setMaxSpeed(position.ShoulderSpeed + 20);
	m_steppers[AXIS_ELBOW].moveTo(position.ElbowPos);
	m_steppers[AXIS_ELBOW].setSpeed(position.ElbowSpeed);
	m_steppers[AXIS_ELBOW].setMaxSpeed(position.ElbowSpeed + 20);
	m_steppers[AXIS_LEFT_DIFFERENT].moveTo(position.LeftDiffPos);
	m_steppers[AXIS_LEFT_DIFFERENT].setSpeed(position.LeftDiffSpeed);
	m_steppers[AXIS_LEFT_DIFFERENT].setMaxSpeed(position.LeftDiffSpeed + 20);
	m_steppers[AXIS_RIGHT_DIFFERENT].moveTo(position.RightDiffPos);
	m_steppers[AXIS_RIGHT_DIFFERENT].setSpeed(position.RightDiffSpeed);
	m_steppers[AXIS_RIGHT_DIFFERENT].setMaxSpeed(position.RightDiffSpeed + 20);
	m_steppers[AXIS_GRIPPER].moveTo(position.GripperPos);
	m_steppers[AXIS_GRIPPER].setSpeed(position.GripperSpeed);
	m_steppers[AXIS_GRIPPER].setMaxSpeed(position.GripperSpeed + 20);
}

/** @brief Move by speed.
 *  @param position JointPosition_t, robot position.
 *  @return Void.
 */
void Robko01Class::move_speed(JointPosition_t position)
{
	m_operationMode = OperationModes::Speed;

	float BasePos = (float)position.BaseSpeed;
	m_steppers[AXIS_BASE].setSpeed(BasePos);
	m_steppers[AXIS_SHOULDER].setSpeed((float)position.ShoulderSpeed);
	m_steppers[AXIS_ELBOW].setSpeed((float)position.ElbowSpeed);
	m_steppers[AXIS_LEFT_DIFFERENT].setSpeed((float)position.LeftDiffSpeed);
	m_steppers[AXIS_RIGHT_DIFFERENT].setSpeed((float)position.RightDiffSpeed);
	m_steppers[AXIS_GRIPPER].setSpeed((float)position.GripperSpeed);
}

/** @brief Move by speed.
 *  @return JointPosition_t, current robot position.
 */
JointPosition_t Robko01Class::get_position()
{
	JointPosition_t PositionL;

	PositionL.BasePos = (int16_t)m_steppers[AXIS_BASE].currentPosition();
	PositionL.BaseSpeed = (int16_t)m_steppers[AXIS_BASE].speed();
	PositionL.ShoulderPos = (int16_t)m_steppers[AXIS_SHOULDER].currentPosition();
	PositionL.ShoulderSpeed = (int16_t)m_steppers[AXIS_SHOULDER].speed();
	PositionL.ElbowPos = (int16_t)m_steppers[AXIS_ELBOW].currentPosition();
	PositionL.ElbowSpeed = (int16_t)m_steppers[AXIS_ELBOW].speed();
	PositionL.LeftDiffPos = (int16_t)m_steppers[AXIS_LEFT_DIFFERENT].currentPosition();
	PositionL.LeftDiffSpeed = (int16_t)m_steppers[AXIS_LEFT_DIFFERENT].speed();
	PositionL.RightDiffPos = (int16_t)m_steppers[AXIS_RIGHT_DIFFERENT].currentPosition();
	PositionL.RightDiffSpeed = (int16_t)m_steppers[AXIS_RIGHT_DIFFERENT].speed();
	PositionL.GripperPos = (int16_t)m_steppers[AXIS_GRIPPER].currentPosition();
	PositionL.GripperSpeed = (int16_t)m_steppers[AXIS_GRIPPER].speed();

	return PositionL;
}

/** @brief Set Port A of the robot.
 *  @param uint8_t value, Value of the port.
 *  @return Void.
 */
void Robko01Class::set_port_a(uint8_t value)
{
	m_portAOut = value;
}

/** @brief Get Port A of the robot.
 *  @return uint8_t, Port A input state.
 */
uint8_t Robko01Class::get_port_a()
{
	return ((m_portLoAIn | (m_portHiAIn << 4)));
}

Robko01Class Robko01;
