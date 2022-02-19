
#include "Robko01.h"

/** 
 * @brief Generate IO Write signal.
 * 
 */
void Robko01Class::iow() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(SLOW)
	delay(1);
#else
	delayMicroseconds(100);
#endif
	digitalWrite(m_BusConfig.IOW, LOW);
#if defined(SLOW)
	delay(1);
#else
	delayMicroseconds(100);
#endif
	digitalWrite(m_BusConfig.IOW, HIGH);
}

/** 
 * @brief Generate IO Read signal.
 * 
 */
void Robko01Class::ior() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(SLOW)
	delay(10);
#else
	delayMicroseconds(100);
#endif
	digitalWrite(m_BusConfig.IOR, LOW);
#if defined(SLOW)
	delay(10);
#else
	delayMicroseconds(100);
#endif
	digitalWrite(m_BusConfig.IOR, HIGH);
}

/** 
 * @brief Set address bus.
 * 
 * @param uint8_t address, Address bus value.
 */
void Robko01Class::set_address_bus(uint8_t address) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

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

/** 
 * @brief Read digital signal of the outputs.
 * 
 * @return uint8_t, state of the pins.
 */
uint8_t Robko01Class::read_do() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	static uint8_t StateL = 0;

 	// Serial.print(analogRead(m_BusConfig.DO0) > 512);
 	// Serial.print(',');
 	// Serial.print(analogRead(m_BusConfig.DO1) > 512);
 	// Serial.print(',');
 	// Serial.print(analogRead(m_BusConfig.DO2) > 512);
 	// Serial.print(',');
 	// Serial.print(analogRead(m_BusConfig.DO3) > 512);
 	// Serial.println();

	bitWrite(StateL, 0, (analogRead(m_BusConfig.DO0) > 512));
	bitWrite(StateL, 1, (analogRead(m_BusConfig.DO1) > 512));
	bitWrite(StateL, 2, (analogRead(m_BusConfig.DO2) > 512));
	bitWrite(StateL, 3, (analogRead(m_BusConfig.DO3) > 512));

	return StateL;
}

/** 
 * @brief Write digital signal of the inputs.
 * 
 * @param State of the digital inputs.
 */
void Robko01Class::write_di(uint8_t data) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Serial.println(data);

	static uint8_t Di0StateL = 0;
	static uint8_t Di1StateL = 0;
	static uint8_t Di2StateL = 0;
	static uint8_t Di3StateL = 0;

	Di0StateL = bitRead(data, 0);
	Di1StateL = bitRead(data, 1);
	Di2StateL = bitRead(data, 2);
	Di3StateL = bitRead(data, 3);

 	// Serial.print(Di0StateL);
 	// Serial.print(',');
 	// Serial.print(Di1StateL);
 	// Serial.print(',');
 	// Serial.print(Di2StateL);
 	// Serial.print(',');
 	// Serial.print(Di3StateL);
 	// Serial.println();

	digitalWrite(m_BusConfig.DI0, Di0StateL == 1 ? HIGH : LOW);
	digitalWrite(m_BusConfig.DI1, Di1StateL == 1 ? HIGH : LOW);
	digitalWrite(m_BusConfig.DI2, Di2StateL == 1 ? HIGH : LOW);
	digitalWrite(m_BusConfig.DI3, Di3StateL == 1 ? HIGH : LOW);
}

/** 
 * @brief Set address bus.
 * 
 * @param uint8_t address, Address bus value.
 */
void Robko01Class::setup_bus() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

    m_currentAddressIndex = 0;
	m_portLoAIn = 0;
	m_portHiAIn = 0;
	m_portAOut = 0;

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
	pinMode(m_BusConfig.DO0, INPUT);
	pinMode(m_BusConfig.DO1, INPUT);
	pinMode(m_BusConfig.DO2, INPUT);
	pinMode(m_BusConfig.DO3, INPUT);

	// Address bus.
	pinMode(m_BusConfig.AO0, OUTPUT);
	pinMode(m_BusConfig.AO1, OUTPUT);
	pinMode(m_BusConfig.AO2, OUTPUT);
}

/** 
 * @brief Setup motors.
 * 
 */
void Robko01Class::setup_motors() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAME

	m_motorsEnabled = false;
	m_motorState = 0;

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

/** 
 * @brief Update motor regulators.
 * 
 * @param uint8_t address, Address of the axis.
 * @return uint8_t, Motors states.
 */
uint8_t Robko01Class::update_motor(uint8_t address) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	static bool state = false; 

	if (m_operationMode == OperationModes::Positioning)
	{
		state = m_steppers[address].run();
	}
	else if(m_operationMode == OperationModes::Speed)
	{
		state = m_steppers[address].runSpeed();
	}

	bitWrite(m_motorState, address, state);

	return m_motorState;
}

/** 
 * @brief Update Port A of the robot.
 * 
 * @param uint8_t address, Sub address of the port.
 */
void Robko01Class::update_port_a(uint8_t address) {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Read operation.
	if (address == AddressIndex::PortA1)
	{
		// Remember data.
		m_portLoAIn = read_do();
	}
	else if (address == AddressIndex::PortA2)
	{
		// Read data bus.
		m_portHiAIn = read_do();
	}

	// Write operation.
	if (address == AddressIndex::PortA1)
	{
		// Write data bus.
		write_di((m_portAOut & 0x0F));
	}
	else if (address == AddressIndex::PortA2)
	{
		// Write data bus.
		write_di(((m_portAOut >> 4) & 0x0F));
	}
}

/**
 * @brief Init the robot.
 * 
 * @param config Robot GPIO configuration.
 */
void Robko01Class::init(BusConfig_t* config) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

    m_BusConfig = *config;
	
#if defined(SLOW)
	m_updateRate = 100UL;
#else
	m_updateRate = 1UL;
#endif

	m_operationMode = OperationModes::NONE;

	// Setup gpio bus signals.
	setup_bus();

	// Setup motor regulators.
    setup_motors();
}

/**
 * @brief Update the robot.
 * 
 */
void Robko01Class::update() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Update time.
	m_timeNow = millis();

	if ((m_timeNow - m_timePrev) >= m_updateRate)
	{
		// Update motors.		
		if (m_currentAddressIndex < AXIS_COUNT)
		{
			// Update motor state.
			m_steppers[m_currentAddressIndex].disableOutputs();
			m_steppers[m_currentAddressIndex].enableOutputs();
			set_address_bus(m_currentAddressIndex);
			m_motorState = update_motor(m_currentAddressIndex);
			iow();
		}
		else
		{
			// Update port A.
			set_address_bus(m_currentAddressIndex);
			update_port_a(m_currentAddressIndex);
			iow();
			ior();
		}

		// Increment the address bus index.
		m_currentAddressIndex++;

		// Clear the address index.
		if (m_currentAddressIndex >= ADDRESS_COUNT)
		{
			m_currentAddressIndex = 0;
		}

		m_timePrev = m_timeNow;
	}
}

/**
 * @brief Motors enables flags.
 * 
 * @return true 
 * @return false 
 */
bool Robko01Class::motors_enabled() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	return m_motorsEnabled;
}

/**
 * @brief Get motors state.
 * 
 * @return uint8_t 
 */
uint8_t Robko01Class::get_motor_state() {
#ifdef SHOW_FUNC_NAMES_S
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

    return m_motorState;
}

/**
 * @brief Stop all motors.
 * 
 */
void Robko01Class::stop_motors() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].stop();
		//m_steppers[address].setSpeed(0);
	}
}

/**
 * @brief Disable all motors.
 * 
 */
void Robko01Class::disable_motors()
{
	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].disableOutputs();
		// set_address_bus(address);
	}

	m_motorsEnabled = false;
}

/**
 * @brief Enable all motors.
 * 
 */
void Robko01Class::enable_motors() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].enableOutputs();
		// set_address_bus(address);
	}

	m_motorsEnabled = true;
}

/**
 * @brief Clear all motors position to 0.
 * 
 */
void Robko01Class::clear_motors() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	for (uint8_t address = 0; address < AXIS_COUNT; address++)
	{
		m_steppers[address].setCurrentPosition(0);
	}
}

/** 
 * @brief Move relatively to position.
 * 
 * @param position JointPosition_t, robot position.
 */
void Robko01Class::move_relative(JointPosition_t position) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_operationMode = OperationModes::Positioning;

	m_steppers[AddressIndex::Base].move(position.BasePos);
	m_steppers[AddressIndex::Base].setSpeed(position.BaseSpeed);
	m_steppers[AddressIndex::Base].setMaxSpeed(position.BaseSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Shoulder].move(position.ShoulderPos);
	m_steppers[AddressIndex::Shoulder].setSpeed(position.ShoulderSpeed);
	m_steppers[AddressIndex::Shoulder].setMaxSpeed(position.ShoulderSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Elbow].move(position.ElbowPos);
	m_steppers[AddressIndex::Elbow].setSpeed(position.ElbowSpeed);
	m_steppers[AddressIndex::Elbow].setMaxSpeed(position.ElbowSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::DiffLeft].move(position.LeftDiffPos);
	m_steppers[AddressIndex::DiffLeft].setSpeed(position.LeftDiffSpeed);
	m_steppers[AddressIndex::DiffLeft].setMaxSpeed(position.LeftDiffSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::DiffRight].move(position.RightDiffPos);
	m_steppers[AddressIndex::DiffRight].setSpeed(position.RightDiffSpeed);
	m_steppers[AddressIndex::DiffRight].setMaxSpeed(position.RightDiffSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Gripper].move(position.GripperPos);
	m_steppers[AddressIndex::Gripper].setSpeed(position.GripperSpeed);
	m_steppers[AddressIndex::Gripper].setMaxSpeed(position.GripperSpeed + MAX_SPEED_OFFSET);
}

/** 
 * @brief Move absolutely to position.
 * 
 * @param position JointPosition_t, robot position.
 */
void Robko01Class::move_absolute(JointPosition_t position) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_operationMode = OperationModes::Positioning;

	m_steppers[AddressIndex::Base].moveTo(position.BasePos);
	m_steppers[AddressIndex::Base].setSpeed(position.BaseSpeed);
	m_steppers[AddressIndex::Base].setMaxSpeed(position.BaseSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Shoulder].moveTo(position.ShoulderPos);
	m_steppers[AddressIndex::Shoulder].setSpeed(position.ShoulderSpeed);
	m_steppers[AddressIndex::Shoulder].setMaxSpeed(position.ShoulderSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Elbow].moveTo(position.ElbowPos);
	m_steppers[AddressIndex::Elbow].setSpeed(position.ElbowSpeed);
	m_steppers[AddressIndex::Elbow].setMaxSpeed(position.ElbowSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::DiffLeft].moveTo(position.LeftDiffPos);
	m_steppers[AddressIndex::DiffLeft].setSpeed(position.LeftDiffSpeed);
	m_steppers[AddressIndex::DiffLeft].setMaxSpeed(position.LeftDiffSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::DiffRight].moveTo(position.RightDiffPos);
	m_steppers[AddressIndex::DiffRight].setSpeed(position.RightDiffSpeed);
	m_steppers[AddressIndex::DiffRight].setMaxSpeed(position.RightDiffSpeed + MAX_SPEED_OFFSET);
	m_steppers[AddressIndex::Gripper].moveTo(position.GripperPos);
	m_steppers[AddressIndex::Gripper].setSpeed(position.GripperSpeed);
	m_steppers[AddressIndex::Gripper].setMaxSpeed(position.GripperSpeed + MAX_SPEED_OFFSET);
}

/**
 * @brief Move by speed.
 * 
 * @param position JointPosition_t, robot position.
 */
void Robko01Class::move_speed(JointPosition_t position) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_operationMode = OperationModes::Speed;

	float BasePos = (float)position.BaseSpeed;
	m_steppers[AddressIndex::Base].setSpeed(BasePos);
	m_steppers[AddressIndex::Shoulder].setSpeed((float)position.ShoulderSpeed);
	m_steppers[AddressIndex::Elbow].setSpeed((float)position.ElbowSpeed);
	m_steppers[AddressIndex::DiffLeft].setSpeed((float)position.LeftDiffSpeed);
	m_steppers[AddressIndex::DiffRight].setSpeed((float)position.RightDiffSpeed);
	m_steppers[AddressIndex::Gripper].setSpeed((float)position.GripperSpeed);
}

/** 
 * @brief Move by speed.
 * 
 * @return JointPosition_t, current robot position.
 */
JointPosition_t Robko01Class::get_position() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	JointPosition_t PositionL;

	PositionL.BasePos = (int16_t)m_steppers[AddressIndex::Base].currentPosition();
	PositionL.BaseSpeed = (int16_t)m_steppers[AddressIndex::Base].speed();
	PositionL.ShoulderPos = (int16_t)m_steppers[AddressIndex::Shoulder].currentPosition();
	PositionL.ShoulderSpeed = (int16_t)m_steppers[AddressIndex::Shoulder].speed();
	PositionL.ElbowPos = (int16_t)m_steppers[AddressIndex::Elbow].currentPosition();
	PositionL.ElbowSpeed = (int16_t)m_steppers[AddressIndex::Elbow].speed();
	PositionL.LeftDiffPos = (int16_t)m_steppers[AddressIndex::DiffLeft].currentPosition();
	PositionL.LeftDiffSpeed = (int16_t)m_steppers[AddressIndex::DiffLeft].speed();
	PositionL.RightDiffPos = (int16_t)m_steppers[AddressIndex::DiffRight].currentPosition();
	PositionL.RightDiffSpeed = (int16_t)m_steppers[AddressIndex::DiffRight].speed();
	PositionL.GripperPos = (int16_t)m_steppers[AddressIndex::Gripper].currentPosition();
	PositionL.GripperSpeed = (int16_t)m_steppers[AddressIndex::Gripper].speed();

	return PositionL;
}

/** 
 * @brief Set Port A of the robot.
 * 
 * @param uint8_t value, Value of the port.
 */
void Robko01Class::set_port_a(uint8_t value) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_portAOut = value;
}

/** 
 * @brief Get Port A of the robot.
 * 
 * @return uint8_t, Port A input state.
 */
uint8_t Robko01Class::get_port_a() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	return ((m_portLoAIn | (m_portHiAIn << 4)));
}

/**
 * @brief Robko 01 instance.
 * 
 */
Robko01Class Robko01;
