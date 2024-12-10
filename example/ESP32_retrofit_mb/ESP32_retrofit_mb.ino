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

/*

+------+----------+-----------+
| Axis |   Sign   | Direction |
+------+----------+-----------+
|    0 | Positive | CCW       |
|    0 | Negative | CW        |
|    1 | Positive | Forward   |
|    1 | Negative | Backward  |
|    2 | Positive | Backward  |
|    2 | Negative | Forward   |
|    3 | Positive | Down      |
|    3 | Negative | Up        |
|    4 | Positive | CW        |
|    4 | Negative | CCW       |
|    5 | Positive | Open      |
|    5 | Negative | Close     |
+------+----------+-----------+

*/

#pragma region Headers

#include <WiFi.h>

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "SUPER.h"

#include "DebugPort.h"

#include "OperationsCodes.h"

#include "DefaultCredentials.h"

#include "GeneralHelper.h"

/* Stepper motor controller. */
#include <AccelStepper.h>
#include <MultiStepper.h>

#pragma endregion // Headers

#pragma region Enums

enum OperationModes : uint8_t {
  NONE = 0U,
  Positioning,
  Speed,
};

#pragma endregion // Enums

#pragma region Prototypes

/**
 * @brief Initialize the pins of the board.
 * 
 */
void init_pins();

/**
 * @brief Initialize the stepper drivers.
 * 
 */
void init_stepper_drivers();

/**
 * @brief Update the stepper drivers.
 * 
 */
void update_driver();

/**
 * @brief Initialize the communication.
 * 
 */
void init_communication();

/**
 * @brief Update communication.
 * 
 */
void update_communication();

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t * payload);

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties();

#pragma endregion

#pragma region Variables

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

uint8_t OperationMode_g;

bool MotorsEnabled_g;

/**
 * @brief Safety stop flag.
 * 
 */
int SafetyStopFlag_g;

/**
 * @brief Move joint in absolute mode.
 * 
 */
JointPositionUnion MoveAbsolute_g;

/**
 * @brief Move joint in relative mode.
 * 
 */
JointPositionUnion MoveRelative_g;

/**
 * @brief Move joint in speed mode.
 * 
 */
JointPositionUnion MoveSpeed_g;

/**
 * @brief Curent joint positions.
 * 
 */
JointPositionUnion CurrentPositions_g;

#ifdef DEAFULT_CREDENTIALS_H_

/**
 * @brief SSID
 * 
 */
const char* SSID_g = DEFAULT_SSID;

/**
 * @brief Password
 * 
 */
const char* PASS_g = DEFAULT_PASS;

#else

/**
 * @brief SSID
 * 
 */
const char* SSID_g = "yourssid";

/**
 * @brief Password
 * 
 */
const char* PASS_g = "yourpasswd";

#endif

/**
 * @brief TCP server for robot operation service.
 * 
 */
WiFiServer TCPServer_g(SERVICE_PORT);

/**
 * @brief TCP server for robot operation service.
 * 
 */
AccelStepper stepper1;

/**
 * @brief Stepper driver instance for .
 * 
 */
AccelStepper stepper2;

/**
 * @brief Stepper driver instance for .
 * 
 */
AccelStepper stepper3;

/**
 * @brief Stepper driver instance for .
 * 
 */
AccelStepper stepper4;

/**
 * @brief Stepper driver instance for .
 * 
 */
AccelStepper stepper5;

/**
 * @brief Stepper driver instance for .
 * 
 */
AccelStepper stepper6;

MultiStepper steppers;

#pragma endregion

/**
 * @brief Setup the peripheral hardware and variables.
 * 
 */
void setup() {
  //
  setup_debug_port();

  OperationMode_g = OperationModes::NONE;
  SafetyStopFlag_g = LOW;
  MotorState_g = 0;
  StorePosition_g = true;

  //
  init_pins();

  //
  init_stepper_drivers();

  //
  show_device_properties();

  // Initialize the communication.
  init_communication();

  // Initialize the SUPER protocol parser.
  SUPER.setCbRequest(cbRequestHandler);
}

/**
 * @brief Main loop of the program.
 * 
 */
void loop() {
  update_communication();

  if (SafetyStopFlag_g == LOW) {
    // Robko01.update();
    // MotorState_g = Robko01.get_motor_state();
    update_driver();
  }

  if (MotorState_g == 0 && StorePosition_g) {
    StorePosition_g = false;

    // TODO: Save position.
  } else {
    StorePosition_g = true;
  }
}

#pragma region Functions

/**
 * @brief Initialize the pins of the board.
 * 
 */
void init_pins() {
#ifdef SHOW_FUNC_NAMES
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

  // Set the pin direction.
  pinMode(MX_ENB, OUTPUT);
  // Disable all drivers.
  digitalWrite(MX_ENB, HIGH);


  //
  pinMode(M1_DIR, OUTPUT);
  //
  digitalWrite(M1_DIR, HIGH);
  //
  pinMode(M1_STP, OUTPUT);
  //
  digitalWrite(M1_STP, HIGH);

  //
  pinMode(M2_DIR, OUTPUT);
  //
  digitalWrite(M2_DIR, HIGH);
  //
  pinMode(M2_STP, OUTPUT);
  //
  digitalWrite(M2_STP, HIGH);

  //
  pinMode(M3_DIR, OUTPUT);
  //
  digitalWrite(M3_DIR, HIGH);
  //
  pinMode(M3_STP, OUTPUT);
  //
  digitalWrite(M3_STP, HIGH);

  //
  pinMode(M4_DIR, OUTPUT);
  //
  digitalWrite(M4_DIR, HIGH);
  //
  pinMode(M4_STP, OUTPUT);
  //
  digitalWrite(M4_STP, HIGH);

  //
  pinMode(M5_DIR, OUTPUT);
  //
  digitalWrite(M5_DIR, HIGH);
  //
  pinMode(M5_STP, OUTPUT);
  //
  digitalWrite(M5_STP, HIGH);

  //
  pinMode(M6_DIR, OUTPUT);
  //
  digitalWrite(M6_DIR, HIGH);
  //
  pinMode(M6_STP, OUTPUT);
  //
  digitalWrite(M6_STP, HIGH);

  // Enable all drivers.
  digitalWrite(MX_ENB, LOW);

  //
  pinMode(M1_LIMIT, INPUT_PULLUP);
  //
  pinMode(M2_LIMIT, INPUT_PULLUP);
  //
  pinMode(M3_LIMIT, INPUT_PULLUP);
  //
  pinMode(M6_LIMIT, INPUT_PULLUP);
}

/**
 * @brief Initialize the stepper drivers.
 * 
 */
void init_stepper_drivers() {
#ifdef SHOW_FUNC_NAMES
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

  stepper1 = AccelStepper(AccelStepper::DRIVER, M1_STP, M1_DIR);
  stepper2 = AccelStepper(AccelStepper::DRIVER, M2_STP, M2_DIR);
  stepper3 = AccelStepper(AccelStepper::DRIVER, M3_STP, M3_DIR);
  stepper4 = AccelStepper(AccelStepper::DRIVER, M4_STP, M4_DIR);
  stepper5 = AccelStepper(AccelStepper::DRIVER, M5_STP, M5_DIR);
  stepper6 = AccelStepper(AccelStepper::DRIVER, M6_STP, M6_DIR);

  stepper1.setAcceleration(ACCELERATION);
  stepper2.setAcceleration(ACCELERATION);
  stepper3.setAcceleration(ACCELERATION);
  stepper4.setAcceleration(ACCELERATION);
  stepper5.setAcceleration(ACCELERATION);
  stepper6.setAcceleration(ACCELERATION);

  stepper1.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper2.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper3.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper4.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper5.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper6.setMaxSpeed(MOTOR_MAX_SPEED);

  // Set directions.
  stepper5.setPinsInverted(true, false, false);

  steppers.addStepper(stepper4);  //q1
  steppers.addStepper(stepper1);  //q2
  steppers.addStepper(stepper2);  //q3
  steppers.addStepper(stepper5);
}

/**
 * @brief Update the stepper drivers.
 * 
 */
void update_driver() {
#ifdef SHOW_FUNC_NAMES_S
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES
  static bool state;
  state = false;
  if (OperationMode_g == OperationModes::Positioning) {
    state = stepper1.run();
    bitWrite(MotorState_g, 0, state);
    state = stepper2.run();
    bitWrite(MotorState_g, 1, state);
    state = stepper3.run();
    bitWrite(MotorState_g, 2, state);
    state = stepper4.run();
    bitWrite(MotorState_g, 3, state);
    state = stepper5.run();
    bitWrite(MotorState_g, 4, state);
    state = stepper6.run();
    bitWrite(MotorState_g, 5, state);
  } else if (OperationMode_g == OperationModes::Speed) {
    state = stepper1.runSpeed();
    bitWrite(MotorState_g, 0, state);
    state = stepper2.runSpeed();
    bitWrite(MotorState_g, 1, state);
    state = stepper3.runSpeed();
    bitWrite(MotorState_g, 2, state);
    state = stepper4.runSpeed();
    bitWrite(MotorState_g, 3, state);
    state = stepper5.runSpeed();
    bitWrite(MotorState_g, 4, state);
    state = stepper6.runSpeed();
    bitWrite(MotorState_g, 5, state);
  }
}

/**
 * @brief Initialize the communication.
 * 
 */
void init_communication() {
#ifdef SHOW_FUNC_NAMES
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

  // start the Wi-Fi connection and the server:
  WiFi.begin(SSID_g, PASS_g);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUGLOG(".");
  }

  DEBUGLOG("\r\n");
  DEBUGLOG("Connected:  %s\r\n", SSID_g);
  DEBUGLOG("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
  DEBUGLOG("Gateway:    %s\r\n", WiFi.gatewayIP().toString().c_str());
  DEBUGLOG("DNS:        %s\r\n", WiFi.dnsIP().toString().c_str());
  DEBUGLOG("\r\n");

  // Start the server.
  TCPServer_g.begin();
}

/**
 * @brief Update communication.
 * 
 */
void update_communication() {
#ifdef SHOW_FUNC_NAMES_S
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

  static uint8_t StateL = 0;

  static WiFiClient ClientL;

  if (StateL == 0) {
    // Listen for incoming clients.
    ClientL = TCPServer_g.available();

    if (ClientL) {
      SUPER.init(ClientL);
      StateL = 1;
      DEBUGLOG("Connected...\r\n");
      // Enable all drivers.
      digitalWrite(MX_ENB, LOW);
    }
  }
  if (StateL == 1) {
    if (ClientL.connected()) {
      SUPER.update();
    } else {
      StateL = 2;
    }
  }
  if (StateL == 2) {
    ClientL.stop();
    StateL = 0;
    DEBUGLOG("Disconnected...\r\n");
    // Disable all drivers.
    digitalWrite(MX_ENB, HIGH);
  }
}

/**
 * @brief Callback handler function.
 * 
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t* payload) {
#ifdef SHOW_FUNC_NAMES_S
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

  if (opcode == OpCodes::Ping) {
#ifdef SHOW_FUNC_NAMES
    DEBUGLOG("\r\n");
    DEBUGLOG(__PRETTY_FUNCTION__);
    DEBUGLOG("\r\n");
    DEBUGLOG("Ping...\r\n");
#endif  // SHOW_FUNC_NAMES
    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  } else if (opcode == OpCodes::Stop) {
    // Robko01.stop_motors();
    stepper1.stop();
    stepper2.stop();
    stepper3.stop();
    stepper4.stop();
    stepper5.stop();
    stepper6.stop();

    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::Disable) {
    // Robko01.disable_motors();
    stepper1.disableOutputs();
    stepper2.disableOutputs();
    stepper3.disableOutputs();
    stepper4.disableOutputs();
    stepper5.disableOutputs();
    stepper6.disableOutputs();
    digitalWrite(MX_ENB, HIGH);
    MotorsEnabled_g = false;

    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::Enable) {
    // Robko01.enable_motors();
    stepper1.enableOutputs();
    stepper2.enableOutputs();
    stepper3.enableOutputs();
    stepper4.enableOutputs();
    stepper5.enableOutputs();
    stepper6.enableOutputs();
    digitalWrite(MX_ENB, LOW);
    MotorsEnabled_g = true;

    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::Clear) {
    // Robko01.clear_motors();
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    stepper6.setCurrentPosition(0);

    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::MoveRelative) {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false) {
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
      return;
    }
    // If it is move, do not execute the command.
    if (MotorState_g != 0) {
      uint8_t m_payloadResponse[1] = { MotorState_g };
      SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
      return;
    }

    // TODO: Move to function.
    size_t DataLengthL = sizeof(JointPosition_t);
    for (uint8_t index = 0; index < DataLengthL; index++) {
      MoveRelative_g.Buffer[index] = payload[index];
    }

    // Set motion data.
    // Robko01.move_relative(MoveRelative_g.Value);
    OperationMode_g = OperationModes::Positioning;

    stepper1.move(MoveRelative_g.Value.BasePos);
    stepper1.setSpeed(MoveRelative_g.Value.BaseSpeed);
    // stepper1.setMaxSpeed(MoveRelative_g.Value.BaseSpeed + MAX_SPEED_OFFSET);
    stepper2.move(MoveRelative_g.Value.ShoulderPos);
    stepper2.setSpeed(MoveRelative_g.Value.ShoulderSpeed);
    // stepper2.setMaxSpeed(MoveRelative_g.Value.ShoulderSpeed + MAX_SPEED_OFFSET);
    stepper3.move(MoveRelative_g.Value.ElbowPos);
    stepper3.setSpeed(MoveRelative_g.Value.ElbowSpeed);
    // stepper3.setMaxSpeed(MoveRelative_g.Value.ElbowSpeed + MAX_SPEED_OFFSET);
    stepper4.move(MoveRelative_g.Value.LeftDiffPos);
    stepper4.setSpeed(MoveRelative_g.Value.LeftDiffSpeed);
    // stepper4.setMaxSpeed(MoveRelative_g.Value.LeftDiffSpeed + MAX_SPEED_OFFSET);
    stepper5.move(MoveRelative_g.Value.RightDiffPos);
    stepper5.setSpeed(MoveRelative_g.Value.RightDiffSpeed);
    // stepper5.setMaxSpeed(MoveRelative_g.Value.RightDiffSpeed + MAX_SPEED_OFFSET);
    stepper6.move(MoveRelative_g.Value.GripperPos);
    stepper6.setSpeed(MoveRelative_g.Value.GripperSpeed);
    // stepper6.setMaxSpeed(MoveRelative_g.Value.GripperSpeed + MAX_SPEED_OFFSET);

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::MoveAbsolute) {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false) {
      // Respond with error.
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);

      // Exit
      return;
    }
    // If it is move, do not execute the command.
    if (MotorState_g != 0) {
      // Respond with busy.
      uint8_t m_payloadResponse[1];
      m_payloadResponse[0] = MotorState_g;
      SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);

      // Exit
      return;
    }

    // Extract motion data.
    size_t DataLengthL = sizeof(JointPosition_t);
    for (uint8_t index = 0; index < DataLengthL; index++) {
      MoveAbsolute_g.Buffer[index] = payload[index];
    }

    // Set motion data.
    // Robko01.move_absolute(MoveAbsolute_g.Value);

    OperationMode_g = OperationModes::Positioning;

    stepper1.moveTo(MoveRelative_g.Value.BasePos);
    stepper1.setSpeed(MoveRelative_g.Value.BaseSpeed);
    stepper1.setMaxSpeed(MoveRelative_g.Value.BaseSpeed + MAX_SPEED_OFFSET);
    stepper2.moveTo(MoveRelative_g.Value.ShoulderPos);
    stepper2.setSpeed(MoveRelative_g.Value.ShoulderSpeed);
    stepper2.setMaxSpeed(MoveRelative_g.Value.ShoulderSpeed + MAX_SPEED_OFFSET);
    stepper3.moveTo(MoveRelative_g.Value.ElbowPos);
    stepper3.setSpeed(MoveRelative_g.Value.ElbowSpeed);
    stepper3.setMaxSpeed(MoveRelative_g.Value.ElbowSpeed + MAX_SPEED_OFFSET);
    stepper4.moveTo(MoveRelative_g.Value.LeftDiffPos);
    stepper4.setSpeed(MoveRelative_g.Value.LeftDiffSpeed);
    stepper4.setMaxSpeed(MoveRelative_g.Value.LeftDiffSpeed + MAX_SPEED_OFFSET);
    stepper5.moveTo(MoveRelative_g.Value.RightDiffPos);
    stepper5.setSpeed(MoveRelative_g.Value.RightDiffSpeed);
    stepper5.setMaxSpeed(MoveRelative_g.Value.RightDiffSpeed + MAX_SPEED_OFFSET);
    stepper6.moveTo(MoveRelative_g.Value.GripperPos);
    stepper6.setSpeed(MoveRelative_g.Value.GripperSpeed);
    stepper6.setMaxSpeed(MoveRelative_g.Value.GripperSpeed + MAX_SPEED_OFFSET);


    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::DO) {
    // Set port A.
    // Robko01.set_port_a(payload[0]);

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::DI) {
    uint8_t m_payloadResponse[1];
    m_payloadResponse[0] = 0;  // Robko01.get_port_a();

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
  } else if (opcode == OpCodes::IsMoving) {
#ifdef SHOW_FUNC_NAMES_S
    DEBUGLOG("\r\n");
    DEBUGLOG(__PRETTY_FUNCTION__);
    DEBUGLOG("\r\n");
    DEBUGLOG("IsMoveing...\r\n");
#endif  // SHOW_FUNC_NAMES
    uint8_t m_payloadResponse[1];
    m_payloadResponse[0] = MotorState_g;

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
  } else if (opcode == OpCodes::CurrentPosition) {
    // CurrentPositions_g.Value = Robko01.get_position();

    CurrentPositions_g.Value.BasePos = (int16_t)stepper1.currentPosition();
    CurrentPositions_g.Value.BaseSpeed = (int16_t)stepper1.speed();
    CurrentPositions_g.Value.ShoulderPos = (int16_t)stepper2.currentPosition();
    CurrentPositions_g.Value.ShoulderSpeed = (int16_t)stepper2.speed();
    CurrentPositions_g.Value.ElbowPos = (int16_t)stepper3.currentPosition();
    CurrentPositions_g.Value.ElbowSpeed = (int16_t)stepper3.speed();
    CurrentPositions_g.Value.LeftDiffPos = (int16_t)stepper4.currentPosition();
    CurrentPositions_g.Value.LeftDiffSpeed = (int16_t)stepper4.speed();
    CurrentPositions_g.Value.RightDiffPos = (int16_t)stepper5.currentPosition();
    CurrentPositions_g.Value.RightDiffSpeed = (int16_t)stepper5.speed();
    CurrentPositions_g.Value.GripperPos = (int16_t)stepper6.currentPosition();
    CurrentPositions_g.Value.GripperSpeed = (int16_t)stepper6.speed();

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, CurrentPositions_g.Buffer, sizeof(JointPosition_t));
  } else if (opcode == OpCodes::MoveSpeed) {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false) {
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
      return;
    }

    for (uint8_t index = 0; index < size; index++) {
      MoveSpeed_g.Buffer[index] = payload[index];
    }

    // Set motion data.
    // Robko01.move_speed(MoveSpeed_g.Value);
    OperationMode_g = OperationModes::Speed;

    stepper1.setSpeed(MoveSpeed_g.Value.BaseSpeed);
    stepper2.setSpeed(MoveSpeed_g.Value.ShoulderSpeed);
    stepper3.setSpeed(MoveSpeed_g.Value.ElbowSpeed);
    stepper4.setSpeed(MoveSpeed_g.Value.LeftDiffSpeed);
    stepper5.setSpeed(MoveSpeed_g.Value.RightDiffSpeed);
    stepper6.setSpeed(MoveSpeed_g.Value.GripperSpeed);

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  } else if (opcode == OpCodes::SetRobotID) {
    // TODO: Write to I2C EEPROM.
    //for (uint8_t index = 0; index < DataLengthL; index++)
    //{
    //	Motion.Buffer[index] = m_payloadRequest[index];
    //}

    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  } else if (opcode == OpCodes::GetRobotID) {
    // TODO: Read from I2C EEPROM.
    //for (uint8_t index = 0; index < DataLengthL; index++)
    //{
    //	m_payloadRequest[index] = Motion.Buffer[index];
    //}

    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  }
}

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties() {
#ifdef SHOW_FUNC_NAMES
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif  // SHOW_FUNC_NAMES

#if defined(ESP8266)
  // ESP8266
  DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
#endif

  DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
  DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
  DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
  DEBUGLOG("Firmware version: %d\r\n", ESP_FW_VERSION);
  DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
  DEBUGLOG("MAC address: %s\r\n", WiFi.macAddress().c_str());
  DEBUGLOG("\r\n");
}

#pragma endregion
