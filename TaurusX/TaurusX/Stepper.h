// - InterfaceKit simple -
// This simple example simply creates an InterfaceKit handle, hooks the event handlers and opens it.  It then waits
// for an InterfaceKit to be attached and waits for events to be fired. We progress through three steps, 1. Normal settings, 
// 2. Setting analog sensor sensitivity to 100, 3. Toggling Ratiometric, waiting for user input to proceed to next step to allow 
// data to be read.
//
// Copyright 2008 Phidgets Inc.  All rights reserved.
// This work is licensed under the Creative Commons Attribution 2.5 Canada License. 
// view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/
#ifndef __STEPPER_H__
#define __STEPPER_H__
#include <stdio.h>
#include <phidget21.h>
// OK, all the callback functions can't be a member function of a class, so we define it beforehand
class Stepper
{
public: 
	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit;
	// define two variable which holds value of Index and State, before Omega works, check the value to see if control should work.
	static bool Stepper_Left_State;
	static bool Stepper_Right_State;
	static int Stepper_Last_Left_State;
	static int Stepper_Last_Right_State;
	static int Left_Count;
	static int Right_Count;

	int display_properties(CPhidgetInterfaceKitHandle phid);
	int initialize_stepper();
	int terminate_stepper();

	int left_down();
	int right_down();
	int both_down();
	int neither_down();
	static int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)
	{
		int serialNo;
		const char *name;

		CPhidget_getDeviceName(IFK, &name);
		CPhidget_getSerialNumber(IFK, &serialNo);

		//printf("%s %10d attached!\n", name, serialNo);

		return 0;
	}

	static int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr)
	{
		int serialNo;
		const char *name;

		CPhidget_getDeviceName (IFK, &name);
		CPhidget_getSerialNumber(IFK, &serialNo);

		//printf("%s %10d detached!\n", name, serialNo);

		return 0;
	}

	static int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
	{
		//printf("Error handled. %d - %s", ErrorCode, unknown);
		return 0;
	}

	//callback that will run if an input changes.
	//Index - Index of the input that generated the event, State - boolean (0 or 1) representing the input state (on or off)
	static int CCONV InputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State)
	{
		//printf("Digital Input: %d > State: %d\n", Index, State);
		if (Index == 1)
			{ 
				if (State == 0)  Stepper_Left_State = 0;
				else if (State == 1) Stepper_Left_State = 1;
				if (Stepper_Last_Left_State != State && State == 1) // prevent adding twice for both up and down movement, so adding State == 1 
				{
					Left_Count++;
					printf ("Left Counter: %i \n", Left_Count);
				}
				Stepper_Last_Left_State = State;
			}
		if (Index == 0)
			{ 
				if (State == 0)  Stepper_Right_State = 0;
				else if (State == 1) Stepper_Right_State = 1;
				if (Stepper_Last_Right_State != State && State == 1) 
				{
					Right_Count++;
					printf ("Right Counter: %i \n", Right_Count);
				}
					
				Stepper_Last_Right_State = State;
			}	
		return 0;
	}

	//callback that will run if an output changes.
	//Index - Index of the output that generated the event, State - boolean (0 or 1) representing the output state (on or off)
	static int CCONV OutputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State)
	{
		//printf("Digital Output: %d > State: %d\n", Index, State);
		return 0;
	}

	//callback that will run if the sensor value changes by more than the OnSensorChange trigger.
	//Index - Index of the sensor that generated the event, Value - the sensor read value
	static int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value)
	{
		//printf("Sensor: %d > Value: %d\n", Index, Value);
		return 0;
	}

};


#endif