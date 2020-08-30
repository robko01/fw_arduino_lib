// RobotSettings.h

#ifndef _ROBOTSETTINGS_h
#define _ROBOTSETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/** @brief Motion description of the joint position. */
typedef struct __attribute__((packed))
{
	uint32_t SerialNumber; ///< Robot serial number.
	int16_t BasePos; ///< Base joint speed.
	int16_t ShoulderPos; ///< Shoulder joint position.
	int16_t ElbowPos; ///< Elbow joint position.
	int16_t LeftDiffPos; ///< Left Differential joint position.
	int16_t RightDiffPos; ///< Right Differential joint position.
	int16_t GripperPos; ///< Gripper joint position.
} RobotSettings_t;


class RobotSettingsStore
{
public:
	RobotSettingsStore();
	~RobotSettingsStore();
	void save(RobotSettings_t setings);

private:

};


#endif

