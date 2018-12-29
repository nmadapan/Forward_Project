// - InterfaceKit simple -
// This simple example simply creates an InterfaceKit handle, hooks the event handlers and opens it.  It then waits
// for an InterfaceKit to be attached and waits for events to be fired. We progress through three steps, 1. Normal settings, 
// 2. Setting analog sensor sensitivity to 100, 3. Toggling Ratiometric, waiting for user input to proceed to next step to allow 
// data to be read.
//
// Copyright 2008 Phidgets Inc.  All rights reserved.
// This work is licensed under the Creative Commons Attribution 2.5 Canada License. 
// view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/

#include "Stepper.h"

bool Stepper::Stepper_Left_State = 0;
bool Stepper::Stepper_Right_State = 0;
int Stepper::Stepper_Last_Left_State = 0;
int Stepper::Stepper_Last_Right_State = 0;
int Stepper::Left_Count = 0;
int Stepper::Right_Count = 0;

//**********************************************************************************************************//
// from this moment on, all the def of the member function


//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
//Will also display the number of inputs, outputs, and analog inputs on the interface kit as well as the state of the ratiometric flag
//and the current analog sensor sensitivity.
int Stepper::display_properties(CPhidgetInterfaceKitHandle phid)
{
	int serialNo, version, numInputs, numOutputs, numSensors, triggerVal, ratiometric, i;
	const char* ptr;

	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);

	CPhidgetInterfaceKit_getInputCount(phid, &numInputs);
	CPhidgetInterfaceKit_getOutputCount(phid, &numOutputs);
	CPhidgetInterfaceKit_getSensorCount(phid, &numSensors);
	CPhidgetInterfaceKit_getRatiometric(phid, &ratiometric);

	//printf("%s\n", ptr);
	//printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	//printf("# Digital Inputs: %d\n# Digital Outputs: %d\n", numInputs, numOutputs);
	//printf("# Sensors: %d\n", numSensors);
	//printf("Ratiometric: %d\n", ratiometric);

	for(i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_getSensorChangeTrigger (phid, i, &triggerVal);

		//printf("Sensor#: %d > Sensitivity Trigger: %d\n", i, triggerVal);
	}

	return 0;
}


int Stepper::initialize_stepper()
{
	ifKit = 0;
	int result;
	const char *err;
	printf("Clearing clutch count....\n");
	Left_Count = 0;
	Right_Count = 0;
	Stepper_Last_Left_State = false;
	Stepper_Last_Left_State = false;



	//create the InterfaceKit object
	CPhidgetInterfaceKit_create(&ifKit);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

	//Registers a callback that will run if an input changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnInputChange_Handler (ifKit, InputChangeHandler, NULL);

	//Registers a callback that will run if the sensor value changes by more than the OnSensorChange trig-ger.
	//Requires the handle for the IntefaceKit, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnSensorChange_Handler (ifKit, SensorChangeHandler, NULL);

	//Registers a callback that will run if an output changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnOutputChange_Handler (ifKit, OutputChangeHandler, NULL);

	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)ifKit, -1);

	//get the program to wait for an interface kit device to be attached
	printf("Waiting for interface kit to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10))) // 10 for Leap Motion, I guess??? I changed it before and I don't remember why.
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}

	////Display the properties of the attached interface kit device
	//display_properties(ifKit);

	////read interface kit event data
	//printf("Reading.....\n");

	////keep displaying interface kit data until user input is read
	//printf("Press any key to go to next step\n");
	//getchar();

	//printf("Modifying sensor sensitivity triggers....\n");

	////get the number of sensors available
	//CPhidgetInterfaceKit_getSensorCount(ifKit, &numSensors);

	////Change the sensitivity trigger of the sensors
	//for(i = 0; i < numSensors; i++)
	//{
	//	CPhidgetInterfaceKit_setSensorChangeTrigger(ifKit, i, 100);  //we'll just use 10 for fun
	//}

	////read interface kit event data
	//printf("Reading.....\n");

	////keep displaying interface kit data until user input is read
	//printf("Press any key to go to next step\n");
	//getchar();

	//printf("Toggling Ratiometric....\n");

	//CPhidgetInterfaceKit_setRatiometric(ifKit, 0);

	////read interface kit event data
	//printf("Reading.....\n");

	////keep displaying interface kit data until user input is read
	//printf("Press any key to end\n");
	//getchar();

	////since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	//printf("Closing...\n");
	//CPhidget_close((CPhidgetHandle)ifKit);
	//CPhidget_delete((CPhidgetHandle)ifKit);

	//all done, exit
	return 0;
}


int Stepper::left_down()
{
	if (Stepper_Left_State && !Stepper_Right_State) return 1;
	else return 0;
}

int Stepper::right_down()
{
	if (!Stepper_Left_State && Stepper_Right_State) return 1;
	else return 0;
}

int Stepper::both_down()
{
	if (Stepper_Left_State && Stepper_Right_State) return 1;
	else return 0;
}

int Stepper::neither_down()
{
	if (!Stepper_Left_State && !Stepper_Right_State) return 1;
	else return 0;
}

int Stepper::terminate_stepper()
{
	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing Stepper...\n");
	return 1;
	CPhidget_close((CPhidgetHandle)ifKit);
	CPhidget_delete((CPhidgetHandle)ifKit);

	//all done, exit
	return 0;
}

