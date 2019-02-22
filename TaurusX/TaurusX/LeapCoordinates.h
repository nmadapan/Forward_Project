#ifndef __LEAPCOORDINATES_H__
#define __LEAPCOORDINATES_H__
#include <stdio.h>
#include <tchar.h>
#include "Leap.h"
#include <iostream>
#include <windows.h>
#include <WinBase.h>
#include <fstream>
#include <cmath>

#define R2D 57.296
using namespace Leap;
class LeapCoordinates
{
	public:
	double x[2], y[2], z[2]; // units in real-world millimeters
	double roll[2], pitch[2], yaw[2];
	double pinch[2];
	float confidence[2];
	Finger thumb[2];
	Vector ThumbTipPosition[2];
	int num_of_hands;
	int left_hand_count, right_hand_count;
	int fail_check;
	LeapCoordinates()
	{
		for (size_t i = 0; i<2; i++)
		{
		roll[i] = 0.0;
		pitch[i] = 0.0;
		yaw[i] = 0.0;
		x[i] = 0.0;
		y[i] = 0.0;
		z[i] = 0.0;
		pinch[i] = 0.0;
		}
		left_hand_count = 0;
		right_hand_count = 0;
		fail_check = 0;
	}
	void calcParam();      // Calcualtes various parameters
	void displayParam();   // Displays all paramters
	double get_gripper_value(const bool hand_flag );
	int CheckSafety();
};

#endif