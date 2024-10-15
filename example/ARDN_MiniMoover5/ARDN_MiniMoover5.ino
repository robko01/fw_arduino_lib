
// Mini Mover 5
// https://ro.uow.edu.au/cgi/viewcontent.cgi?article=1052&context=compsciwp

// Command Parser
// https://github.com/Uberi/Arduino-CommandParser

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "JointPosition.h"

#include "JointPositionUnion.h"

#include "BusConfig.h"

#include "Robko01.h"

#include <CommandParser.h>

#pragma endregion

#pragma region Variables

/**
 * @brief Command parser type.
 * 
 */
typedef CommandParser<16, 8> CommandParser_t;

/**
 * @brief Command parser instance.
 * 
 */
CommandParser_t CommandParser_g;

/**
 * @brief Line to enter command.
 * 
 */
char Line_g[LINE_LENGTH];

/**
 * @brief Response line.
 * 
 */
char Response_g[CommandParser_t::MAX_RESPONSE_SIZE];

/**
 * @brief Curent joint positions.
 * 
 */
JointPosition_t CurrentPositions_g;

#pragma endregion

/**
 * @brief 4.2.1 Initialise (INIT) Command
 * 
 * @param args 
 * @param response 
 */
void cmd_init(CommandParser_t::Argument *args, char *response)
{
	Robko01.stop_motors();
	Robko01.disable_motors();
	Robko01.enable_motors();
	Robko01.clear_motors();
  strlcpy(response, "Init the robot", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.2 Reset MINI-MOVER 5 and Database (RESET) Command
 * 
 * @param args 
 * @param response 
 */
void cmd_reset(CommandParser_t::Argument *args, char *response)
{
  Robko01.clear_motors();
  strlcpy(response, "Reset the robot", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.3 Robot Motor Driver (STEP) Command
 * 
 * @param args 
 * @param response 
 */
void cmd_step(CommandParser_t::Argument *args, char *response)
{
  // Set position.
  CurrentPositions_g.BasePos = (int16_t)args[1].asInt64;
  CurrentPositions_g.BaseSpeed = (int16_t)args[0].asInt64;
  
  CurrentPositions_g.ShoulderPos = (int16_t)args[2].asInt64;
  CurrentPositions_g.ShoulderSpeed = (int16_t)args[0].asInt64;

  CurrentPositions_g.ElbowPos = (int16_t)args[3].asInt64;
  CurrentPositions_g.ElbowSpeed = (int16_t)args[0].asInt64;

  CurrentPositions_g.LeftDiffPos = (int16_t)args[4].asInt64;
  CurrentPositions_g.LeftDiffSpeed = (int16_t)args[0].asInt64;

  CurrentPositions_g.RightDiffPos = (int16_t)args[5].asInt64;
  CurrentPositions_g.RightDiffSpeed = (int16_t)args[0].asInt64;

  CurrentPositions_g.GripperPos = (int16_t)args[6].asInt64;
  CurrentPositions_g.GripperSpeed = (int16_t)args[0].asInt64;

  // Display work.
  // Serial.print("STEP@");
  // Serial.print("D: "); Serial.print((int16_t)args[0].asInt64);
  // Serial.print(" J1:"); Serial.print(CurrentPositions_g.BasePos);
  // Serial.print(" J2:"); Serial.print(CurrentPositions_g.ShoulderPos);
  // Serial.print(" J3:"); Serial.print(CurrentPositions_g.ElbowPos);
  // Serial.print(" J4:"); Serial.print(CurrentPositions_g.LeftDiffPos);
  // Serial.print(" J5:"); Serial.print(CurrentPositions_g.RightDiffPos);
  // Serial.print(" J6:"); Serial.print(CurrentPositions_g.GripperPos);
  // Serial.println("");

  // Set motion data.
	Robko01.move_absolute(CurrentPositions_g);

  // Send response.
  strlcpy(response, "STEP", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.4 CLOSE Command
 * 
 * @param args 
 * @param response 
 */
void cmd_close(CommandParser_t::Argument *args, char *response)
{
  strlcpy(response, "Close the robot gripper", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.5 READ Command
 * 
 * @param args 
 * @param response 
 */
void cmd_read(CommandParser_t::Argument *args, char *response)
{
  CurrentPositions_g = Robko01.get_position();

  Serial.print("READ@");
  // Serial.print("D: "); Serial.print(args[0].asDouble);
  Serial.print("J1: "); Serial.print(CurrentPositions_g.BasePos);
  Serial.print(" J2: "); Serial.print(CurrentPositions_g.ShoulderPos);
  Serial.print(" J3: "); Serial.print(CurrentPositions_g.ElbowPos);
  Serial.print(" J4: "); Serial.print(CurrentPositions_g.LeftDiffPos);
  Serial.print(" J5: "); Serial.print(CurrentPositions_g.RightDiffPos);
  Serial.print(" J6:"); Serial.print(CurrentPositions_g.GripperPos);
  Serial.println("");

  strlcpy(response, "Read the robot", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.6 STATUS Command
 * 
 * @param args 
 * @param response 
 */
void cmd_status(CommandParser_t::Argument *args, char *response)
{
  Serial.println("Status of the robot");
}

/**
 * @brief 4.2.7 BOUNDS Command
 * 
 * @param args 
 * @param response 
 */
void cmd_bounds(CommandParser_t::Argument *args, char *response)
{
  // Serial.println("Bounds of the robot");
}

/**
 * @brief 4.2.8 HOME Command
 * 
 * @param args 
 * @param response 
 */
void cmd_home(CommandParser_t::Argument *args, char *response)
{
  CurrentPositions_g.BasePos = -CurrentPositions_g.BasePos;
  CurrentPositions_g.ShoulderPos = -CurrentPositions_g.ShoulderPos;
  CurrentPositions_g.ElbowPos = -CurrentPositions_g.ElbowPos;
  CurrentPositions_g.LeftDiffPos = -CurrentPositions_g.LeftDiffPos;
  CurrentPositions_g.RightDiffPos = -CurrentPositions_g.RightDiffPos;
  CurrentPositions_g.GripperPos = -CurrentPositions_g.GripperPos;

  // Set motion data.
	Robko01.move_absolute(CurrentPositions_g);

  strlcpy(response, "HOME", CommandParser_t::MAX_RESPONSE_SIZE);
}

/**
 * @brief 4.2.9.1 Forward Joint (FWDJOINT) Command
 * 
 * @param args 
 * @param response 
 */
void cmd_fwdjoint(CommandParser_t::Argument *args, char *response)
{
  Serial.print("FWDJOINT@");
  Serial.print("D: "); Serial.println(args[0].asDouble);
  Serial.print("J1: "); Serial.println(args[1].asDouble);
  Serial.print("J2: "); Serial.println(args[2].asDouble);
  Serial.print("J3: "); Serial.println(args[3].asDouble);
  Serial.print("J4: "); Serial.println(args[4].asDouble);
  Serial.print("J5: "); Serial.println(args[5].asDouble);
  Serial.print("J6: "); Serial.println(args[6].asDouble);
  Serial.println("");
}


/**
 * @brief 4.2.9.2 Forward Cartesian (FWDCART) Command
 * 
 * @param args 
 * @param response 
 */
void cmd_fwdcart(CommandParser_t::Argument *args, char *response)
{
  Serial.print("FWDCART@");
  Serial.print("D: "); Serial.println(args[0].asDouble);
  Serial.print("J1: "); Serial.println(args[1].asDouble);
  Serial.print("J2: "); Serial.println(args[2].asDouble);
  Serial.print("J3: "); Serial.println(args[3].asDouble);
  Serial.print("J4: "); Serial.println(args[4].asDouble);
  Serial.print("J5: "); Serial.println(args[5].asDouble);
  Serial.print("J6: "); Serial.println(args[6].asDouble);
  Serial.println("");
}

void setup()
{
  // set the digital pin as output:
  pinMode(PIN_STATUS_LED, OUTPUT);

  // Setup the robot bus driver.
	BusConfig_t RobotConfigL = {
        PIN_AO0,
        PIN_AO1,
        PIN_AO2,
        PIN_IOR,
        PIN_IOW,
        PIN_DI0,
        PIN_DI1,
        PIN_DI2,
        PIN_DI3,
        PIN_DO0,
        PIN_DO1,
        PIN_DO2,
        PIN_DO3,
	};

	// Initialize the robot controller.
	Robko01.init(&RobotConfigL);

  Serial.begin(BAUD_RATE);
  while (!Serial);

  CommandParser_g.registerCommand("INIT", "", &cmd_init);
  CommandParser_g.registerCommand("RESET", "", &cmd_reset);
  CommandParser_g.registerCommand("STEP", "iiiiiii", &cmd_step);
  CommandParser_g.registerCommand("CLOSE", "", &cmd_close);
  CommandParser_g.registerCommand("READ", "", &cmd_read);
  // CommandParser_g.registerCommand("STATUS", "", &cmd_status);
  // CommandParser_g.registerCommand("BOUNDS", "", &cmd_bounds);
  CommandParser_g.registerCommand("HOME", "", &cmd_home);
  // CommandParser_g.registerCommand("FWDJOINT", "dddddd", &cmd_fwdjoint);
  // CommandParser_g.registerCommand("FWDCART", "dddddd", &cmd_fwdcart);

  // CommandParser_g.registerCommand("TEST", "sdiu", &cmd_test);
  // Serial.println("registered command: TEST <string> <double> <int64> <uint64>");
  // Serial.println("example: TEST \"\\x41bc\\ndef\" -1.234e5 -123 123");
  Serial.println("Robko01 Robot");
}

void loop()
{
  Robko01.update();

  if (Serial.available())
  {
    // Terminate string.
    size_t LineLengthL = Serial.readBytesUntil('\n', Line_g, 127);
    Line_g[LineLengthL] = '\0';
    
    CommandParser_g.processCommand(Line_g, Response_g);
    Serial.println(Response_g);
  }
}
