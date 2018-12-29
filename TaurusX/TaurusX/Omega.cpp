#include "Omega.h"


int Omega::init()
{
	int done = 0;

	// get device count
	deviceCount = dhdGetDeviceCount ();
	if (deviceCount < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		return -1;
	}
	else if (deviceCount < 1) {
		printf ("error: no device detected\n");
		return -1;
	}
	else if (deviceCount < 2) {
		printf ("error: single device detected\n");
		return -1;
	}
	// open the first available device
	// OK we change the order here because I want device 0 to be left arm and device 1 to be right arm
	if ((deviceID[0] = dhdOpenID (0)) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		return -1;
	}
	// open the second available device
	if ((deviceID[1] = dhdOpenID (1)) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		return -1;
	}	

	ushort serial_number_left,serial_number_right;
	dhdGetSerialNumber(&serial_number_left,0);
	dhdGetSerialNumber(&serial_number_right,1);
	if (serial_number_left != 11282 || serial_number_right != 11288)
	{
		printf ("Error!!!!!\n");
		printf ("Serial Number for left hand (should be 11282): %d\n", serial_number_left);
		printf ("Serial Number for right hand (should be 11288): %d\n", serial_number_right);
		return -1;
	}

	// message
	//int major, minor, release, revision;
	//dhdGetSDKVersion (&major, &minor, &release, &revision);
	//printf ("\n");
	//printf ("Force Dimension - Gravity Compensation Example %d.%d.%d.%d\n", major, minor, release, revision);
	//printf ("(C) 2013 Force Dimension\n");
	//printf ("All Rights Reserved.\n\n");

	//// required to change asynchronous operation mode
	//dhdEnableExpertMode ();

	//// open the first available device
	//if (dhdOpen () < 0) {
	//	printf ("error: cannot open device (%s)\n", dhdErrorGetLastStr());
	//	dhdSleep (2.0);
	//	return -1;
	//}

	// identify device
	//printf ("%s device detected\n\n", dhdGetSystemName());

	//// display instructions
	//printf ("press BUTTON or 'q' to quit\n");
	//printf ("      's' to disable asynchronous USB operation\n");
	//printf ("      'a' to enable asynchronous USB operation\n\n");

	// enable force
	dhdEnableForce (DHD_ON);
	dhdGetPosition (&pt[0].x, &pt[0].y, &pt[0].z, deviceID[0]);
	dhdGetPosition (&pt[1].x, &pt[1].y, &pt[1].z, deviceID[1]);
	t0 = dhdGetTime ();
	done = 0;

	return 0;
}

void Omega::update_position()
{
	for (char arm = 0; arm < deviceCount; arm++)
	{
		// apply zero force
		if (dhdSetForceAndTorqueAndGripperForce (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, deviceID[arm]) < DHD_NO_ERROR) {
			printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
			done = 1;
		}

		// display refresh rate and position at 1 kHz
		t1 = dhdGetTime ();
		if ((t1-t0) > REFRESH_INTERVAL) {

			const char *mode = "SALVN";

			// retrieve information to display
			freq = dhdGetComFreq ();
			t0   = t1;


			// write down position
			if (dhdGetPosition (&pt[arm].x, &pt[arm].y, &pt[arm].z, deviceID[arm]) < 0) {
				printf ("error: cannot read position (%s)\n", dhdErrorGetLastStr());
				done = 1;
			}
			if (dhdGetForce (&fx, &fy, &fz, deviceID[arm]) < 0) {
				printf ("error: cannot read force (%s)\n", dhdErrorGetLastStr());
				done = 1;
			}
			//printf ("p (%+0.03f %+0.03f %+0.03f) m  |  f (%+0.01f %+0.01f %+0.01f) N  |  freq [%0.02f kHz] (%c)   \r", pt.x, pt.y, pt.z, fx, fy, fz, freq, mode[dhdGetComMode()]);

			// write down gripper open // only work for the left arm, because left Omega has gripper
			if (dhdGetGripperAngleDeg (&gripper[0], deviceID[0]) < 0) {
				printf ("error: cannot read gripper open value (%s)\n", dhdErrorGetLastStr());
				done = 1;
			}

			// write down orientation frame
			if (arm == 0)
			{
				if (dhdGetOrientationFrame(L_rotation, deviceID[arm])<0) {
					printf ("error: cannot read gripper open value (%s)\n", dhdErrorGetLastStr());
					done = 1;
				}
			}
			else
			{
				if (dhdGetOrientationFrame(R_rotation, deviceID[arm])<0) {
					printf ("error: cannot read gripper open value (%s)\n", dhdErrorGetLastStr());
					done = 1;
				}
			}

			// write down roll, pitch and yaw
			if (dhdGetOrientationRad(&roll[arm], &pitch[arm], &yaw[arm], deviceID[arm])<0) {
				printf ("error: cannot read gripper open value (%s)\n", dhdErrorGetLastStr());
				done = 1;
			}


			// test for exit condition
			if (dhdGetButton(0)) 
			{
				if (increase)
				{
					gripper[1] += 0.3;
					if (gripper[1]>27)
						increase = 0;

				}
				else 
				{
					gripper[1] -= 0.3;
					if (gripper[1]<2)
						increase = 1;
				}
			}
			if (dhdKbHit()) {
				switch (dhdKbGet()) {
				case 'q': done = 1; break;
				case 's': dhdSetComMode (DHD_COM_MODE_SYNC);  break;
				case 'a': dhdSetComMode (DHD_COM_MODE_ASYNC); break;
				}
			}
		}
	}
}

void Omega::clear_all_force()
{
	// apply zero force
	if (dhdSetForceAndTorqueAndGripperForce (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, deviceID[0]) < DHD_NO_ERROR) {
		printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
		done = 1;
	}

	if (dhdSetForceAndTorqueAndGripperForce (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, deviceID[1]) < DHD_NO_ERROR) {
		printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
		done = 1;
	}
}

Point3D Omega::get_position(bool left_or_right) // unit in meter
{
	return pt[left_or_right]; 
}

Point3D Omega::get_rpy(bool left_or_right)
{
	Point3D rpy;
	rpy.set_point(roll[left_or_right], pitch[left_or_right], yaw[left_or_right]);
	return rpy;
}

bnu::matrix<double> Omega::get_rotation(bool left_or_right)
{
	bnu::matrix<double> return_matrix (3,3);
	if (left_or_right == 0)
	{
		for (int i = 0; i<3; i++)
			for (int j = 0; j<3; j++)
			{
				if (j == 1) // direction of x axis and z axis are opposite between Omega and Taurus, we need to rotate along y axis for 180 degrees
					// when make it negative, Taurus won't move at all. Need to figure it out!
					return_matrix(i,j) = L_rotation[i][j]; 
				else
					return_matrix(i,j) = L_rotation[i][j]; 
			}
			//std::cout<< roll<< ' ' <<pitch<<' '<<yaw<<std::endl;
	}
	else
	{
		for (int i = 0; i<3; i++)
			for (int j = 0; j<3; j++)
			{
				if (j == 1) // direction of x axis and z axis are opposite between Omega and Taurus, we need to rotate along y axis for 180 degrees
					// when make it negative, Taurus won't move at all. Need to figure it out!
					return_matrix(i,j) = R_rotation[i][j]; 
				else
					return_matrix(i,j) = R_rotation[i][j]; 
			}
			//std::cout<< roll<< ' ' <<pitch<<' '<<yaw<<std::endl;
	}
	return return_matrix;
}

bnu::matrix<double> Omega::get_rotation_from_rpy(bool left_or_right)
{
	    // here is where all the coordinate mapping takes place
		double p_roll = -yaw[left_or_right];
		double p_pitch = -pitch[left_or_right];
		double p_yaw = -roll[left_or_right];
		bnu::matrix <double> Composit_Rotation(3,3);
		bnu::matrix <double> Single_Rotation_Roll(3,3);
		bnu::matrix <double> Single_Rotation_Pitch(3,3);
		bnu::matrix <double> Single_Rotation_Yaw(3,3);

		Single_Rotation_Roll(0,0) = 1; 
		Single_Rotation_Roll(0,1) = 0;
		Single_Rotation_Roll(0,2) = 0; 
		Single_Rotation_Roll(1,0) = 0; 
		Single_Rotation_Roll(1,1) = cos(p_roll); 
		Single_Rotation_Roll(1,2) = -sin(p_roll); 
		Single_Rotation_Roll(2,0) = 0; 
		Single_Rotation_Roll(2,1) = sin(p_roll); 
		Single_Rotation_Roll(2,2) = cos(p_roll); 

		Single_Rotation_Pitch(0,0) = cos(p_pitch); 
		Single_Rotation_Pitch(0,1) = 0;
		Single_Rotation_Pitch(0,2) = sin(p_pitch); 
		Single_Rotation_Pitch(1,0) = 0; 
		Single_Rotation_Pitch(1,1) = 1; 
		Single_Rotation_Pitch(1,2) = 0; 
		Single_Rotation_Pitch(2,0) = -sin(p_pitch); 
		Single_Rotation_Pitch(2,1) = 0; 
		Single_Rotation_Pitch(2,2) = cos(p_pitch); 

		Single_Rotation_Yaw(0,0) = cos(p_yaw); 
		Single_Rotation_Yaw(0,1) = -sin(p_yaw);
		Single_Rotation_Yaw(0,2) = 0; 
		Single_Rotation_Yaw(1,0) = sin(p_yaw); 
		Single_Rotation_Yaw(1,1) = cos(p_yaw); 
		Single_Rotation_Yaw(1,2) = 0; 
		Single_Rotation_Yaw(2,0) = 0; 
		Single_Rotation_Yaw(2,1) = 0; 
		Single_Rotation_Yaw(2,2) = 1; 

		Composit_Rotation = bnu::prod(Single_Rotation_Yaw,Single_Rotation_Pitch);
		Composit_Rotation = bnu::prod(Composit_Rotation, Single_Rotation_Roll);
		//std::cout<<Composit_Rotation;
		return Composit_Rotation;
}

double Omega::get_gripper(bool left_or_right)
{
	return (4*(gripper[left_or_right]-2));  // 2 - 27 map to 0 - 100
}











