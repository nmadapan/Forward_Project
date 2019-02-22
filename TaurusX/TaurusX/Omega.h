#ifndef __OMEGA_H__
#define __OMEGA_H__
#include <stdio.h>
#include "dhdc.h"
#include "Basic.h"
#define REFRESH_INTERVAL  0   // sec, now we don't want any delay here, so put it zero.
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <conio.h>

namespace bnu = boost::numeric::ublas;

class Omega
{
private:
	Point3D pt[2]; 
	double fx, fy, fz;
	double freq;
	double t1,t0;
	double gripper[2];
	int	done;
	double L_rotation[3][3];
	double R_rotation[3][3];
	int deviceID[2];
	bool increase;
	int deviceCount;

public:

	double roll[2], pitch[2], yaw[2];
	int init();
	void update_position();
	Point3D get_position(bool left_or_right);
	Point3D get_rpy(bool left_or_right);
	double get_gripper(bool left_or_right);
	bnu::matrix<double> get_rotation(bool left_or_right);
	void clear_all_force();
	bnu::matrix<double> Omega::get_rotation_from_rpy(bool left_or_right);
	int get_done()
	{
		return done;
	}
	int get_device_count()
	{
		return deviceCount;
	}
	void terminate()
	{
		done = 0;
		// close the connection to the first device
		if (dhdClose (0) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		}
		// close the connection to the second device
		if (dhdClose (1) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		}

		/*std::cout<<"Terminate connection"<<std::endl;
		dhdClose ();*/
	}
};

#endif