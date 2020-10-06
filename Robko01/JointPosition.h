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

// JointPosition.h

#ifndef _JOINTPOSITION_h
#define _JOINTPOSITION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

/** @brief Motion description of the joint position. */
typedef struct __attribute__((packed))
{
	int16_t BasePos; ///< Base joint position.
	int16_t BaseSpeed; ///< Base joint speed.
	int16_t ShoulderPos; ///< Shoulder joint position.
	int16_t ShoulderSpeed;///< Shoulder joint speed.
	int16_t ElbowPos; ///< Elbow joint position.
	int16_t ElbowSpeed;///< Elbow joint speed.
	int16_t LeftDiffPos; ///< Left Differential joint position.
	int16_t LeftDiffSpeed;///< Left Differential joint speed.
	int16_t RightDiffPos; ///< Right Differential joint position.
	int16_t RightDiffSpeed;///< Right Differential joint speed.
	int16_t GripperPos; ///< Gripper joint position.
	int16_t GripperSpeed;///< Gripper joint speed.
} JointPosition_t;

#endif

