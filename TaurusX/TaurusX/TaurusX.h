#ifndef __TAURUSX_H__
#define __TAURUSX_H__

#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4800)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/thread.hpp>
#include <time.h>
#include <math.h>
#include <vector>


// My inlcude
#include "Basic.h"
#include "LeapCoordinates.h"
#include "Omega.h"
#include "Stepper.h"
#include "Text2Speech.h"
#include "Sphinx.h"
#include "Kinect.h" 
#include "TimeRecorder.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
using namespace boost::numeric::ublas;

#define BUFFER_SIZE 1312 // this is the buffer size which is decided by SRI, we can't change it

/// define global variables
SAM sam;
pthread_mutex_t mutexHaptic;
pthread_mutex_t mutexHeartbeat;
pthread_mutex_t mutex_leap;
pthread_mutex_t mutex_User_tracking;
// finish

void horiz_star(){
	for (int i = 0; i < 40; i++) printf("*");
		printf("\n");
}
void horiz_line(){
	for (int i = 0; i < 40; i++) printf("*");
		printf("\n");
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
	SYSTEMTIME st;

	//GetSystemTime(&st);
	//cout << SystemTime.wHour << endl;
	GetLocalTime(&st);

	char buffer[256];
	sprintf(buffer,
		"%d_%02d_%02d_%02d_%02d_%02d_%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);

	std::string str(buffer);
	return str;
}

// define the structure to hold different attributes in the packet
class Packet_Pose {
public:
	u_int magicNumber, sequence, pactyp, version, current_state;
	int surgeon_mode;
	__int64 performanceCountWhenSent,performanceCountWhenReceived,performanceFrequency;
	double TipPose[2][4][4];
	double grip[2];

	// extract the information from buffer and store it in the Packet structure. For the pose matrix
	// left-up 3x3 matrix contain trigonometry values which range from -1 to 1. right-up 3x1 vector contains
	// the xyz coordinates of arm tip (shown in cm). 
	virtual void Read(istream& is) {
		is.read(reinterpret_cast<char *>(&magicNumber), sizeof(magicNumber)); 
		is.read(reinterpret_cast<char *>(&sequence), sizeof(sequence));
		is.read(reinterpret_cast<char *>(&pactyp), sizeof(pactyp));
		is.read(reinterpret_cast<char *>(&version), sizeof(version));
		is.read(reinterpret_cast<char *>(&performanceCountWhenSent), sizeof(performanceCountWhenSent));
		is.read(reinterpret_cast<char *>(&performanceCountWhenReceived), sizeof(performanceCountWhenReceived));
		is.read(reinterpret_cast<char *>(&performanceFrequency), sizeof(performanceFrequency));
		is.read(reinterpret_cast<char *>(&surgeon_mode), sizeof(surgeon_mode));
		is.read(reinterpret_cast<char *>(&current_state), sizeof(current_state));

		for (int arm = 0; arm<2;arm++)
		{
			for (int i=0;i<4;i++){
				for (int j=0;j<4;j++){
					is.read((char*)(&TipPose[arm][i][j]), sizeof(double));
				}
			}
			is.read( (char*)(&grip[arm]), sizeof(double));
		}
	}

	Packet_Pose& operator =(const Packet_Pose& rhs)
	{
		magicNumber = rhs.magicNumber;
		sequence = rhs.sequence;
		pactyp = rhs.pactyp;
		version = rhs.version;
		performanceCountWhenSent = rhs.performanceCountWhenSent;
		performanceCountWhenReceived = rhs.performanceCountWhenReceived;
		performanceFrequency = rhs.performanceFrequency;
		surgeon_mode = rhs.surgeon_mode;
		current_state = rhs.current_state;
		for (int Arm = 0; Arm < 2; Arm++) {
			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					TipPose[Arm][i][j] = rhs.TipPose[Arm][i][j];
				}

			}
			grip[Arm] = rhs.grip[Arm];
		}
		return *this;
	}

	friend std::ostream &operator<< (std::ostream &stream, const Packet_Pose& _p);
};
class Packet_Camera {
public:
	u_int magicNumber, sequence, pactyp, version, current_state;
	int surgeon_mode;
	__int64 performanceCountWhenSent,performanceCountWhenReceived,performanceFrequency;

	double mZoom;					// Zoom percentage
	double mFocusDistance;			// Focus distance value - units TBD, numeric range TBD (ie. 0 to infinity?)
	int mAutoFocusEnabled;			// mAutoFocusEnabled - 0 is no, 1 is yes
	double mBaseline;				// Baseline distance. Units?
	double mConvergence;			// Convergence angle. Units?
	double mPan;					//Pan represents min pan to max pan in a 0 -> 100 range
	double mTilt;					//The camera's tilt in radians
	double mRoll;					 //The camera's roll in radians. Probably 0, but the member is here for completeness.

	// camera controls
	// Note: some of the camera controls overlap capabilities of the base class, but have more options (e.g., triggered focus mode)
	int mFocusMode;	
	int mTriggerFocus;				// trigger one-shot focus (only works if camera is in TriggeredFocus mode)	
	int mIRFilterMode;				
	int mWhiteBalMode;				
	int mTriggerWhiteBal;			// trigger white balance (only works if camera is in TriggeredWhiteBalance mode)
	int mExposureMode;				
	int mExposureCompensation;		// affects image brightness when in auto-exposure mode.  Sony FCB-H11 uses [-7,7]

	//	mOrientation is three angles that can be used for anything. They don't have to be camera related.
	tAngleArray mOrientation;

	//	mOrientation is three coordinates that can be used for anything. They don't have to be camera related.
	tLengthArray mPosition;

	// lighting controls
	int mWhiteLights;		
	int mIRLights;				
	int mUVLights;	

	// extra variables for experimentation, expansion, etc.
	int mAux1;
	int mAux2;

	virtual void Read(istream& is) {
		is.read(reinterpret_cast<char *>(&magicNumber), sizeof(magicNumber)); 
		is.read(reinterpret_cast<char *>(&sequence), sizeof(sequence));
		is.read(reinterpret_cast<char *>(&pactyp), sizeof(pactyp));
		is.read(reinterpret_cast<char *>(&version), sizeof(version));
		is.read(reinterpret_cast<char *>(&performanceCountWhenSent), sizeof(performanceCountWhenSent));
		is.read(reinterpret_cast<char *>(&performanceCountWhenReceived), sizeof(performanceCountWhenReceived));
		is.read(reinterpret_cast<char *>(&performanceFrequency), sizeof(performanceFrequency));
		is.read(reinterpret_cast<char *>(&surgeon_mode), sizeof(surgeon_mode));
		is.read(reinterpret_cast<char *>(&current_state), sizeof(current_state));

		is.read(reinterpret_cast<char *>(&mZoom), sizeof(mZoom)); 
		is.read(reinterpret_cast<char *>(&mAutoFocusEnabled), sizeof(mAutoFocusEnabled)); 
		is.read(reinterpret_cast<char *>(&mFocusDistance), sizeof(mFocusDistance)); 
		is.read(reinterpret_cast<char *>(&mPan), sizeof(mPan));
		is.read(reinterpret_cast<char *>(&mBaseline), sizeof(mBaseline)); 
		is.read(reinterpret_cast<char *>(&mConvergence), sizeof(mConvergence)); 
		is.read(reinterpret_cast<char *>(&mRoll), sizeof(mRoll));
		is.read(reinterpret_cast<char *>(&mTilt), sizeof(mTilt));     

		for (size_t i = 0; i < 3; i++) {
			double ARad;
			is.read(reinterpret_cast<char *>(&ARad), sizeof(ARad));
			//mOrientation[i].SetAngle(M7System::Rad, ARad);
		}

		for (size_t i = 0; i < 3; i++) { 
			double ARad;
			is.read(reinterpret_cast<char *>(&ARad), sizeof(ARad));
			//mPosition[i].SetLength(M7System::mm, ARad);
		}   

		is.read(reinterpret_cast<char *>(&mFocusMode), sizeof(mFocusMode)); 
		is.read(reinterpret_cast<char *>(&mTriggerFocus), sizeof(mTriggerFocus)); 
		is.read(reinterpret_cast<char *>(&mIRFilterMode), sizeof(mIRFilterMode)); 
		is.read(reinterpret_cast<char *>(&mWhiteBalMode), sizeof(mWhiteBalMode)); 
		is.read(reinterpret_cast<char *>(&mTriggerWhiteBal), sizeof(mTriggerWhiteBal)); 
		is.read(reinterpret_cast<char *>(&mExposureMode), sizeof(mExposureMode)); 
		is.read(reinterpret_cast<char *>(&mExposureCompensation), sizeof(mExposureCompensation)); 
		is.read(reinterpret_cast<char *>(&mWhiteLights), sizeof(mWhiteLights)); 
		//is.read(reinterpret_cast<char *>(&mIRLights), sizeof(mIRLights)); 
		//is.read(reinterpret_cast<char *>(&mUVLights), sizeof(mUVLights));

		is.read(reinterpret_cast<char *>(&mAux1), sizeof(mAux1)); 
		is.read(reinterpret_cast<char *>(&mAux2), sizeof(mAux2)); 
	}
};
class Packet_Haptic {
public:
	u_int magicNumber, sequence, pactyp, version, current_state;
	int surgeon_mode;
	__int64 performanceCountWhenSent,performanceCountWhenReceived,performanceFrequency;

	static const int NumberOfArms = 2;
	static const int MaxJointsPerArm = 10;
	static const unsigned NumberForceComponents = 3;
	static const unsigned NumberOfTaurusCameras = 2;

	unsigned short int JointsInLimits[NumberOfArms]; // If a bit is set then current joint value is within the valid range for that joint
	int x[NumberOfArms]; // Microns
	int y[NumberOfArms]; // Microns
	int z[NumberOfArms]; // Microns
	int yaw[NumberOfArms]; // Radians * 1e6
	int pitch[NumberOfArms]; // Radians * 1e6
	int roll[NumberOfArms]; // Radians * 1e6
	double grasp[NumberOfArms]; //100.0 is open, 0 closed
	double TipPose[NumberOfArms][4][4]; // The 16 elements of a tip-pose matrix (inches and radians)
	double toolEndPoint[NumberOfArms][3];  // X,Y,Z coords of the true end-point of the tool (useful for measuring distances between the tips
	double stiffness[NumberOfArms][6];
	Point3D TipPosition[NumberOfArms]; // The 3 elements of a tip position vector (microns) 

	// It is wasteful to send the array of joint units in each message.  In future, send arm ID and both sender and receiver look up units
	//M7S::DimensionalUnit JointUnitValues[NumberOfArms][MaxJointsPerArm];		// unit specification per joint value
	int JointUnitValues[NumberOfArms][MaxJointsPerArm];		// unit specification per joint value
	double RawJointValues[NumberOfArms][MaxJointsPerArm];		// joint values in units specified by corresponding element of JointUnits
	int JointCurrents[NumberOfArms][MaxJointsPerArm];		// milliamps of current for each arm joint
	double NormAngleFromMid[NumberOfArms][MaxJointsPerArm];   // normalized angular distance from joint midpoint: range [-1,+1]

	double cameraTilt;        // cameraTilt in radians
	double cameraZoomPercent; // camera zoom [0,100]


	// camera mode info
	int cameraFocusMode[NumberOfTaurusCameras];
	int cameraExposureMode[NumberOfTaurusCameras];
	int cameraExposureCompensation[NumberOfTaurusCameras];
	int cameraWhiteBalMode[NumberOfTaurusCameras];
	int cameraIRFilterMode[NumberOfTaurusCameras];

	// lighting control
	unsigned char whiteLEDLightMode;	// 0 = disabled; 1 = enabled
	unsigned char whiteLEDPercent; // % in [0,100]
	unsigned char irLEDLightMode;	// 0 = disabled; 1 = enabled
	unsigned char irLEDPercent;    // % in [0,100]

	// mode that user has selected for operating the arm
	int armControlMode[NumberOfArms];	
	// gripperControlModes are define in GripperControlModes.h
	int gripperControlMode[NumberOfArms];			// mode that user has selected for operating the gripper

	// qualifying info for the current state (held in the core ustruct portion of the message)
	// contents depend on the current state --- e.g., for the ToolInTransition state, aux1 is the from state, aux2 is the to state
	unsigned int currentState_aux1;
	unsigned int currentState_aux2;

	int autonomousMoveP;      // 1 if an autonomous move is in progress; 0 otherwise
	int ax[NumberOfArms]; // Measured accelerations of each tool, in G*10000
	int ay[NumberOfArms]; 
	int az[NumberOfArms]; 
	unsigned short int JointsPerArm[NumberOfArms]; // Number of joints on each arm on the slave robot.
	double TipForceVector[NumberOfArms][NumberForceComponents]; //Forces at the tip (Fx, Fy, Fz);

	void Read(istream& is) {
		is.read(reinterpret_cast<char *>(&magicNumber), sizeof(magicNumber)); 
		is.read(reinterpret_cast<char *>(&sequence), sizeof(sequence));
		is.read(reinterpret_cast<char *>(&pactyp), sizeof(pactyp));
		is.read(reinterpret_cast<char *>(&version), sizeof(version));
		is.read(reinterpret_cast<char *>(&performanceCountWhenSent), sizeof(performanceCountWhenSent));
		is.read(reinterpret_cast<char *>(&performanceCountWhenReceived), sizeof(performanceCountWhenReceived));
		is.read(reinterpret_cast<char *>(&performanceFrequency), sizeof(performanceFrequency));
		is.read(reinterpret_cast<char *>(&surgeon_mode), sizeof(surgeon_mode));
		is.read(reinterpret_cast<char *>(&current_state), sizeof(current_state));


		for (int Arm = 0; Arm < 2; Arm++) {

			//double temp;
			//is.read(reinterpret_cast<char *>(&temp), sizeof(x[Arm]));	
			//x[Arm] = (int)temp; 
			is.read(reinterpret_cast<char *>(&x[Arm]), sizeof(x[Arm]));
			is.read(reinterpret_cast<char *>(&y[Arm]), sizeof(y[Arm]));
			is.read(reinterpret_cast<char *>(&z[Arm]), sizeof(z[Arm]));
			is.read(reinterpret_cast<char *>(&yaw[Arm]), sizeof(yaw[Arm]));
			is.read(reinterpret_cast<char *>(&pitch[Arm]), sizeof(pitch[Arm]));
			is.read(reinterpret_cast<char *>(&roll[Arm]), sizeof(roll[Arm]));
			is.read(reinterpret_cast<char *>(&grasp[Arm]), sizeof(grasp[Arm]));
			is.read(reinterpret_cast<char *>(&armControlMode[Arm]), sizeof(armControlMode[Arm]));
			is.read(reinterpret_cast<char *>(&gripperControlMode[Arm]), sizeof(gripperControlMode[Arm]));
			is.read(reinterpret_cast<char *>(&ax[Arm]), sizeof(ax[Arm]));
			is.read(reinterpret_cast<char *>(&ay[Arm]), sizeof(ay[Arm]));
			is.read(reinterpret_cast<char *>(&az[Arm]), sizeof(az[Arm]));
			is.read(reinterpret_cast<char *>(&JointsInLimits[Arm]), sizeof(JointsInLimits[Arm]));
			is.read(reinterpret_cast<char *>(&JointsPerArm[Arm]), sizeof(JointsPerArm[Arm]));

			for(int i=0;i<MaxJointsPerArm;i++) {
				is.read(reinterpret_cast<char *>(&JointUnitValues[Arm][i]), sizeof(JointUnitValues[Arm][i]));
			}
			for(int i=0;i<MaxJointsPerArm;i++) {
				is.read(reinterpret_cast<char *>(&RawJointValues[Arm][i]), sizeof(RawJointValues[Arm][i]));
			}
			for(int i=0;i<MaxJointsPerArm;i++) {
				is.read(reinterpret_cast<char *>(&JointCurrents[Arm][i]), sizeof(JointCurrents[Arm][i]));
		    }

			// In the Href tip pose is ControlledArmPpose
			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					is.read(reinterpret_cast<char *>(&TipPose[Arm][i][j]), sizeof(TipPose[Arm][i][j]));
				}
			}

			is.read(reinterpret_cast<char *>(&toolEndPoint[Arm][0]), sizeof(toolEndPoint[Arm][0]));
			is.read(reinterpret_cast<char *>(&toolEndPoint[Arm][1]), sizeof(toolEndPoint[Arm][1]));
			is.read(reinterpret_cast<char *>(&toolEndPoint[Arm][2]), sizeof(toolEndPoint[Arm][2]));

			for (size_t ii = 0; ii < 6; ii++) { 
				is.read(reinterpret_cast<char *>(&stiffness[Arm][ii]), sizeof(stiffness[Arm][ii]));
			}

			for (size_t jj = 0; jj < MaxJointsPerArm; jj++) {
				is.read(reinterpret_cast<char *>(&NormAngleFromMid[Arm][jj]), sizeof(NormAngleFromMid[Arm][jj]));
			}

			for (size_t i = 0; i < NumberForceComponents; i++) {
				is.read(reinterpret_cast<char *>(&TipForceVector[Arm][i]), sizeof(TipForceVector[Arm][i]));
			}
		}

		is.read(reinterpret_cast<char *>(&cameraTilt), sizeof(cameraTilt));
		is.read(reinterpret_cast<char *>(&cameraZoomPercent), sizeof(cameraZoomPercent));
		// camera mode info
		is.read(reinterpret_cast<char *>(&cameraFocusMode), sizeof(cameraFocusMode));
		is.read(reinterpret_cast<char *>(&cameraExposureMode), sizeof(cameraExposureMode));
		is.read(reinterpret_cast<char *>(&cameraExposureCompensation), sizeof(cameraExposureCompensation));
		is.read(reinterpret_cast<char *>(&cameraWhiteBalMode), sizeof(cameraWhiteBalMode));
		is.read(reinterpret_cast<char *>(&cameraIRFilterMode), sizeof(cameraIRFilterMode));

		// lighting control
		is.read(reinterpret_cast<char *>(&whiteLEDLightMode), sizeof(whiteLEDLightMode));
		is.read(reinterpret_cast<char *>(&whiteLEDPercent), sizeof(whiteLEDPercent));
		is.read(reinterpret_cast<char *>(&irLEDLightMode), sizeof(irLEDLightMode));
		is.read(reinterpret_cast<char *>(&irLEDPercent), sizeof(irLEDPercent));


		is.read(reinterpret_cast<char *>(&currentState_aux1), sizeof(currentState_aux1));
		is.read(reinterpret_cast<char *>(&currentState_aux2), sizeof(currentState_aux2));

		is.read(reinterpret_cast<char *>(&autonomousMoveP), sizeof(autonomousMoveP));
	}

	void DisplayFrame(void)
	{
		for (char Arm = 0; Arm < 2; Arm ++)
		{
			roll[Arm] = (int)(atan2(TipPose[Arm][2][1], TipPose[Arm][2][2])*MEGA);
			yaw[Arm] = (int)(atan2(TipPose[Arm][1][0], TipPose[Arm][0][0])*MEGA);
			pitch[Arm] = (int)(atan2(-TipPose[Arm][2][0], sqrt(pow(TipPose[Arm][0][0],2)+pow(TipPose[Arm][1][0],2)))*MEGA);

			printf ("\n Arm %i Pose: \n", Arm);
			std::cout << ("roll:  ") << roll[Arm]*DEG_PER_RAD/MEGA<<std::endl;
			std::cout << ("pitch: ") << pitch[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
			std::cout << ("yaw:   ") << yaw[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
			std::cout << ("gripper: ") << grasp[Arm]<<std::endl; 
			//std::cout<< ("ax, ay, az:") << ax[Arm] <<' ' << ay[Arm] <<' ' << az[Arm] << std::endl;
			//std::cout<< ("Tip Force Vector: ") << TipForceVector[Arm][0] <<' ' << TipForceVector[Arm][1] <<' ' << TipForceVector[Arm][2] << std::endl;
			std::cout << ("Controlled Coordinate frame: ")<<std::endl;
			for (size_t i = 0; i < 3; i++) {
				for (size_t j = 0; j < 4; j++) {
					std::cout<<setw(12)<<(int)TipPose[Arm][i][j];
					if (j==3)
						std::cout << std::endl;
				}
			}


			std::cout<<"Tool Endpoint:  " << toolEndPoint[Arm][0] <<",\t"<< toolEndPoint[Arm][1] <<",\t"<< toolEndPoint[Arm][2]<<std::endl;

			std::cout<<"Control Joint Position(mm):  " << TipPose[Arm][0][3]/1000 <<",\t"<< TipPose[Arm][1][3]/1000<<",\t"<< TipPose[Arm][2][3]/1000<<std::endl;
			if (Arm == 0)
			{
				TipPosition[Arm].x = TipPose[Arm][0][3] + 0.072 * TipPose[Arm][0][0] - 0.005 * TipPose[Arm][0][1];
				TipPosition[Arm].y = TipPose[Arm][1][3] + 0.072 * TipPose[Arm][1][0] - 0.005 * TipPose[Arm][1][1];
				TipPosition[Arm].z = TipPose[Arm][2][3] + 0.072 * TipPose[Arm][2][0] - 0.005 * TipPose[Arm][2][1];
			}
			else if (Arm == 1)
			{
				TipPosition[Arm].x = TipPose[Arm][0][3] + 0.03 * TipPose[Arm][0][0];
				TipPosition[Arm].y = TipPose[Arm][1][3] + 0.03 * TipPose[Arm][1][0];
				TipPosition[Arm].z = TipPose[Arm][2][3] + 0.03 * TipPose[Arm][2][0];
			}
			std::cout<<"Scalpel tip position(mm): " <<TipPosition[Arm].x/1000 <<",\t"<< TipPosition[Arm].y/1000<<",\t"<< TipPosition[Arm].z/1000<<std::endl;
		}
		std::cout<<"\n\n"; 
	}

	friend std::ostream &operator<<(std::ostream &stream, const Packet_Haptic& _p);
};

class Haptic_Seed{

private:
	Point3D in; // current control input
	Point3D last_in; // last control input
	Point3D v; // current velocity
	
	Point3D report;
	Point3D diff;

	// the composite version
	double diff_comp;
	double v_comp;
	double warning_comp;
	double warning_level_comp;
	
	double damping_factor;
	double warning_threshold;

public:
	
	enum verbosity {QUIET, VERBOSE};
	Haptic_Seed():damping_factor(1), warning_threshold(1), warning_level_comp(0) {
	}

	void feed_input(Point3D &_input)
	{
		in = _input/1000; // all in millimeter
	}

	void feed_report(Point3D &_report)
	{
		report = _report/1000;
	}

	void feed_both(Point3D &_input, Point3D &_report)
	{
		in = _input/1000; report = _report/1000;
	}

	double calculate_warning_level (const verbosity status = QUIET) // return a value from 0 to 1, which 0 means fine and 1 means terrible.
	{
		if (!last_in.empty()) 
		{
			warning_level_comp = 0;
			last_in = in;
		}
		else
		{
			v = in - last_in;
			diff = in - report;
			
			// the composite version
			diff_comp = sqrt(pow(diff.x,2) + pow(diff.y,2) + pow(diff.z,2));   
			v_comp = sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));   
			
			warning_comp = abs(diff_comp - v_comp * damping_factor);

			// now is the decision part
			if (v_comp > 5) sam.speak("Slow down!", SAM::ASYNC);

			if (warning_comp < warning_threshold)
				warning_level_comp = 0;
			else if (warning_comp > 1000) // probably error happens, or it's the begining of system, Pose packet has not been initialized
				warning_level_comp = 0;
			else
			{
				warning_level_comp = abs(warning_comp - warning_threshold) / 1;  // the value 1 will decide the scale of the bar graph
				if (warning_level_comp > 1)
					warning_level_comp = 1;
			}


			// display output
			if (status == VERBOSE)
			{
				std::ofstream out;
				//printf("Difference:%-5.2f Velocity:%-5.2f Warning:%-5.2f Warning_level:%-5.2f \n", diff_comp, v_comp, warning_comp, warning_level_comp);
				out.open("haptic_info.txt", ios::app);
				out<< diff_comp << ' '<< v_comp <<std::endl;
			}


			// update last point
			last_in = in;

		}
		return warning_level_comp;
	}

	void set_warning_threshold(const double _p)
	{
		warning_threshold = _p;
	}

	void set_damping_factor(const double _p)
	{
		damping_factor = _p;
	}

	double get_raw_warning_signal()
	{
		return warning_level_comp;
	}
};

class Packet_Heartbeat {
public:
	u_int magicNumber, sequence, pactyp, version, current_state;
	int surgeon_mode;
	__int64 performanceCountWhenSent,performanceCountWhenReceived,performanceFrequency;

	static const int NumberOfArms = 2;
	static const int MaxJointsPerArm = 10;
	static const int NumberOfTaurusArms = 2;
	int senderId;					// which subsystem is sending the heartbeat message (that is, M7, Taurus, Atlas)

	int numberOfArms;				//  number of elements used in each array
	int numberOfCameraSystems;	
	int numberOfNetConverters;

	// Note: the master needs joint values, tip pose and camera tilt data when re-connecting to agent
	unsigned short int jointsPerArm[NumberOfArms]; // Number of joints on each arm on the slave robot.
	int jointUnitValues[NumberOfArms][MaxJointsPerArm];			// unit specification per joint value
	double rawJointValues[NumberOfArms][MaxJointsPerArm];		// joint values in units specified by corresponding element of JointUnits
	double normJointDistFromMid[NumberOfArms][MaxJointsPerArm];		// normalized joint distance from joint midpoint: range [-1,+1]

	// JointsInLimits and the 6-dimensional tip pose does not appear to be used, retain for backward compatability?
	unsigned short int jointsInLimits[NumberOfArms];			// If a bit is set then current joint value is within the valid range for that joint

	int x[NumberOfArms]; // Microns
	int y[NumberOfArms]; // Microns
	int z[NumberOfArms]; // Microns
	int yaw[NumberOfArms]; // Radians * 1e6
	int pitch[NumberOfArms]; // Radians * 1e6
	int roll[NumberOfArms]; // Radians * 1e6


	double TipPose[NumberOfArms][4][4]; // The 16 elements of a tip-pose matrix (inches and radians) for each arm
	double grasp[NumberOfArms]; //100.0 is open, 0 closed
	double stiffness[NumberOfArms][6];	// 6-tuple cartesian stiffness vector
	int	armLocked[NumberOfArms];		// 0 for not locked; 1 for locked
	int	brakesEngaged[NumberOfArms];	// 0 for OFF; 1 for ON
	int	gripperControlMode[NumberOfArms];	// 0 for Normal (see GripperControlModes.h)

	double cameraTilt;  // cameraTilt in radians
	double cameraZoomPercent; // camera zoom [0,100]

	// camera mode info
	int cameraFocusMode;
	int cameraExposureMode;
	int cameraExposureCompensation;
	int cameraWhiteBalMode;
	int cameraIRFilterMode;

	// lighting control
	int whiteLEDLightMode;
	int irLEDLightMode;


	// qualifying info for the current state (held in the core ustruct portion of the message)
	// contents depend on the current state --- e.g., for the ToolInTransition state, aux1 is the from state, aux2 is the to state
	unsigned int currentState_aux1;
	unsigned int currentState_aux2;

	int vmeCommsState;

	// identify remote network converters (aka NetBurner) for remote system
	//int remoteNetConvId[MaxNetworkConverters];
	//int remoteNetConvSwVer[MaxNetworkConverters];
	//int	remoteNetConvCommsState[NumberOfArms];	// 0 for no comms; 1 for comms OK
	//char remoteNetConvAddr[MaxNetworkConverters][MAC_AddressStringSize];

	int	remoteSelfTestPercent;	// 0 to 100 percent complete
	int	eStopEnergized;			// 0 for OFF; 1 for ENERGIZED
	int	totalSystemUpMinutes;		// total up time for system in minutes
	int	batteryPercent;			// 0 to 100 percent
	int	batteryMinutes;			// predicted minutes until battery is drained
	double dvmValue;


	void Read(istream& is) {
		is.read(reinterpret_cast<char *>(&magicNumber), sizeof(magicNumber)); 
		is.read(reinterpret_cast<char *>(&sequence), sizeof(sequence));
		is.read(reinterpret_cast<char *>(&pactyp), sizeof(pactyp));
		is.read(reinterpret_cast<char *>(&version), sizeof(version));
		is.read(reinterpret_cast<char *>(&performanceCountWhenSent), sizeof(performanceCountWhenSent));
		is.read(reinterpret_cast<char *>(&performanceCountWhenReceived), sizeof(performanceCountWhenReceived));
		is.read(reinterpret_cast<char *>(&performanceFrequency), sizeof(performanceFrequency));
		is.read(reinterpret_cast<char *>(&surgeon_mode), sizeof(surgeon_mode));
		is.read(reinterpret_cast<char *>(&current_state), sizeof(current_state));

		is.read(reinterpret_cast<char *>(&senderId), sizeof(senderId));  
		is.read(reinterpret_cast<char *>(&numberOfArms), sizeof(numberOfArms));
		is.read(reinterpret_cast<char *>(&numberOfCameraSystems), sizeof(numberOfCameraSystems));
		is.read(reinterpret_cast<char *>(&numberOfNetConverters), sizeof(numberOfNetConverters));

		for (int Arm = 0; Arm < NumberOfTaurusArms; Arm++) {
			is.read(reinterpret_cast<char *>(&jointsPerArm[Arm]), sizeof(jointsPerArm[Arm]));
			is.read(reinterpret_cast<char *>(&x[Arm]), sizeof(x[Arm]));
			is.read(reinterpret_cast<char *>(&y[Arm]), sizeof(y[Arm]));
			is.read(reinterpret_cast<char *>(&z[Arm]), sizeof(z[Arm]));
			is.read(reinterpret_cast<char *>(&yaw[Arm]), sizeof(yaw[Arm]));
			is.read(reinterpret_cast<char *>(&pitch[Arm]), sizeof(pitch[Arm]));
			is.read(reinterpret_cast<char *>(&roll[Arm]), sizeof(roll[Arm]));
			is.read(reinterpret_cast<char *>(&grasp[Arm]), sizeof(grasp[Arm]));

			is.read(reinterpret_cast<char *>(&jointsInLimits[Arm]), sizeof(jointsInLimits[Arm]));
			is.read(reinterpret_cast<char *>(&armLocked[Arm]), sizeof(armLocked[Arm])); 
			is.read(reinterpret_cast<char *>(&brakesEngaged[Arm]), sizeof(brakesEngaged[Arm]));
			is.read(reinterpret_cast<char *>(&gripperControlMode[Arm]), sizeof(gripperControlMode[Arm]));

			for(int i=0; i<MaxJointsPerArm; i++){
				is.read(reinterpret_cast<char *>(&jointUnitValues[Arm][i]), sizeof(jointUnitValues[Arm][i]));
				is.read(reinterpret_cast<char *>(&rawJointValues[Arm][i]), sizeof(rawJointValues[Arm][i]));
				is.read(reinterpret_cast<char *>(&normJointDistFromMid[Arm][i]), sizeof(normJointDistFromMid[Arm][i]));
			}

			double d;
			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					is.read(reinterpret_cast<char *>(&d), sizeof(TipPose[Arm][i][j]));
					if ( i == 3)
						TipPose[Arm][i][j] = d; 
					else if (j == 3)
						TipPose[Arm][i][j] = d * MEGA * M_per_INCH;
					else
						TipPose[Arm][i][j] = d * MEGA; 
				}
			}
			for (size_t ii = 0; ii < 6; ii++) { 
				is.read(reinterpret_cast<char *>(&stiffness[Arm][ii]), sizeof(stiffness[Arm][ii]));
			}
		}

		is.read(reinterpret_cast<char *>(&cameraTilt), sizeof(cameraTilt));
		is.read(reinterpret_cast<char *>(&cameraZoomPercent), sizeof(cameraZoomPercent));

		is.read(reinterpret_cast<char *>(&cameraFocusMode), sizeof(cameraFocusMode));
		is.read(reinterpret_cast<char *>(&cameraExposureMode), sizeof(cameraExposureMode));
		is.read(reinterpret_cast<char *>(&cameraExposureCompensation), sizeof(cameraExposureCompensation));
		is.read(reinterpret_cast<char *>(&cameraWhiteBalMode), sizeof(cameraWhiteBalMode));
		is.read(reinterpret_cast<char *>(&cameraIRFilterMode), sizeof(cameraIRFilterMode));

		is.read(reinterpret_cast<char *>(&whiteLEDLightMode), sizeof(whiteLEDLightMode));
		is.read(reinterpret_cast<char *>(&irLEDLightMode), sizeof(irLEDLightMode));

		is.read(reinterpret_cast<char *>(&currentState_aux1), sizeof(currentState_aux1));
		is.read(reinterpret_cast<char *>(&currentState_aux2), sizeof(currentState_aux2));

		is.read(reinterpret_cast<char *>(&vmeCommsState), sizeof(vmeCommsState));

		is.read(reinterpret_cast<char *>(&remoteSelfTestPercent), sizeof(remoteSelfTestPercent));
		is.read(reinterpret_cast<char *>(&eStopEnergized), sizeof(eStopEnergized));	
		is.read(reinterpret_cast<char *>(&totalSystemUpMinutes), sizeof(totalSystemUpMinutes));
		is.read(reinterpret_cast<char *>(&batteryPercent), sizeof(batteryPercent));
		is.read(reinterpret_cast<char *>(&batteryMinutes), sizeof(batteryMinutes));	

		//for (size_t ii=0; ii<MaxNetworkConverters; ii++) {
		//	is.read(reinterpret_cast<char *>(&remoteNetConvId[ii]), sizeof(remoteNetConvId[ii]));
		//	is.read(reinterpret_cast<char *>(&remoteNetConvSwVer[ii]), sizeof(remoteNetConvSwVer[ii]));
		//	is.read(reinterpret_cast<char *>(&remoteNetConvCommsState[ii]), sizeof(remoteNetConvCommsState[ii]));
		//	is.read(reinterpret_cast<char *>(&remoteNetConvAddr[ii][0]), sizeof(remoteNetConvAddr[ii]));
		//}
	}

	void DisplayFrame(void)
	{
		int Arm = 0; // left arm
		roll[Arm] = (int)(atan2(TipPose[Arm][2][1], TipPose[Arm][2][2])*MEGA);
		yaw[Arm] = (int)(atan2(TipPose[Arm][1][0], TipPose[Arm][0][0])*MEGA);
		pitch[Arm] = (int)(atan2(-TipPose[Arm][2][0], sqrt(pow(TipPose[Arm][0][0],2)+pow(TipPose[Arm][1][0],2)))*MEGA);

		std::cout << "Left Arm Pose:"<<std::endl;
		std::cout << ("roll:  ") << roll[Arm]*DEG_PER_RAD/MEGA<<std::endl;
		std::cout << ("pitch: ") << pitch[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
		std::cout << ("yaw:   ") << yaw[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
		std::cout << ("gripper: ") << grasp[Arm]<<std::endl; 

		std::cout << ("Tip Pose: ")<<std::endl;
		for (size_t i = 0; i < 3; i++) {
			for (size_t j = 0; j < 4; j++) {
				std::cout<<setw(12)<<(int)TipPose[Arm][i][j];
				if (j==3)
					std::cout << std::endl;
			}
		}

		std::cout << std::endl<<"Right Arm Pose"<<std::endl;
		Arm = 1; // right arm
		roll[Arm] = (int)(atan2(TipPose[Arm][2][1], TipPose[Arm][2][2])*MEGA);
		yaw[Arm] = (int)(atan2(TipPose[Arm][1][0], TipPose[Arm][0][0])*MEGA);
		pitch[Arm] = (int)(atan2(-TipPose[Arm][2][0], sqrt(pow(TipPose[Arm][0][0],2)+pow(TipPose[Arm][1][0],2)))*MEGA);
		std::cout << ("roll:  ") << roll[Arm]*DEG_PER_RAD/MEGA<<std::endl;
		std::cout << ("pitch: ") << pitch[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
		std::cout << ("yaw:   ") << yaw[Arm]*DEG_PER_RAD/MEGA<<std::endl; 
		std::cout << ("gripper: ") << grasp[Arm]<<std::endl; 

		std::cout << ("Tip Pose: ")<<std::endl;
		for (size_t i = 0; i < 3; i++) {
			for (size_t j = 0; j < 4; j++) {
				std::cout<<setw(12)<<(int)TipPose[Arm][i][j];
				if (j==3)
					std::cout << std::endl;
			}
		}
	}
	friend std::ostream &operator<<(std::ostream &os, const Packet_Heartbeat& _p);
};

inline std::ostream& operator<<(std::ostream & stream, const Packet_Pose& P) 
{
	stream<<("----------------------start-----------------------------------")<<std::endl;
	//stream<<"sequence: "<<std::hex<<P.sequence<<std::endl;
	//stream<<"pactyp: "<<std::hex<<P.pactyp<<std::endl;
	//stream<<"version: "<<std::hex<<P.version<<std::endl;
	//stream<<"current_state: "<<std::hex<<P.current_state<<std::endl;
	//stream<<"surgeon_mode: "<<std::hex<<P.surgeon_mode<<std::endl;
	//printf("performanceCountWhenSent: %I64d \n", P.performanceCountWhenSent);
	//printf("performanceCountWhenReceived: %I64d \n", P.performanceCountWhenReceived);
	//printf("performanceFrequency: %I64d \n\n", P.performanceFrequency);

	stream<<"grip_left: "<<P.grip[0]<<'\t'<<"   grip_right: "<<P.grip[1]<<std::endl;
	for (int Arm = 0; Arm < 2; Arm++) {
		if (Arm == 0)
			stream<<"Left Arm Pose: "<<std::endl;
		else
			stream<<"Right Arm Pose: "<<std::endl;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				stream<<setw(12)<<(int)P.TipPose[Arm][i][j];
				if (j==3)
					stream << std::endl;
			}
		}
	}	
	stream<<("----------------------end-----------------------------------")<<std::endl;
	return stream;
}
inline std::ostream& operator<<(std::ostream &os, const Packet_Haptic& HIS)
{
	for (int Arm = 0; Arm < 2; Arm++) {
		if ( Arm == 0)
			os << ("Haptic Struct for Left Arm")<<std::endl;
		else
			os << ("Haptic Struct for Right Arm")<<std::endl;
		os << (" x: ") << HIS.x[Arm]<<'\t'; 
		os << (" y: ") << HIS.y[Arm]<<'\t';  
		os << (" z: ") << HIS.z[Arm]<<std::endl;

		os << (" y: ") << HIS.yaw[Arm]<<'\t'; 
		os << (" p: ") << HIS.pitch[Arm]<<'\t'; 
		os << (" r: ") << HIS.roll[Arm]<<std::endl;

		os << (" g: ") << HIS.grasp[Arm]<<'\t'; 
		os << (" gripper mode: ") << HIS.gripperControlMode[Arm]<<std::endl;

		os << (" ax: ") << HIS.ax[Arm]<<'\t'; 
		os << (" ay: ") << HIS.ay[Arm]<<'\t'; 
		os << (" az: ") << HIS.az[Arm]<<std::endl;

		os << (" Limits: ") << HIS.JointsInLimits[Arm]<<std::endl;
		os << (" Joints on arm: ") << HIS.JointsPerArm[Arm]<<std::endl;

		os << (" Tip Pose: ")<<std::endl;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				os<<setw(12)<<(int)HIS.TipPose[Arm][i][j];
				if (j==3)
					os << std::endl;
			}
		}

		static const int MaxJointsPerArm = 6;
		os << (" Normalized Anglular Dist: ")<<std::endl;
		for (size_t jj = 0; jj < MaxJointsPerArm; jj++) {
			os << HIS.NormAngleFromMid[Arm][jj] << (", ");
		}
		os << std::endl;

		os << (" Tilt: ") << HIS.cameraTilt<<std::endl;
		os << (" Zoom: ") << HIS.cameraZoomPercent<<std::endl;

		os << (" current state aux1: ") << HIS.currentState_aux1<<std::endl;
		os << (" current state aux2: ") << HIS.currentState_aux2<<std::endl;

		os << (" Autonmous move: ") << HIS.autonomousMoveP<<std::endl;
	}
	return os;
}  
inline std::ostream& operator<<(std::ostream &os, const Packet_Heartbeat& HB)
{

	//os << ("Sequence Number: ") << HB.sequence  << std::endl;
	os << setprecision(4);

	os << (" senderId: ") << HB.senderId << std::endl; 
	//os << (" numberOfArms: ") << HB.numberOfArms << std::endl;
	//os << (" numberOfCameraSystems: ") << HB.numberOfCameraSystems << std::endl; 
	//os << (" numberOfNetConverters: ") << HB.numberOfNetConverters << std::endl; 

	for (int Arm = 0; Arm < HB.NumberOfArms; Arm++) {
		if ( Arm == 0)
			os << ("Heartbeat Struct for Left Arm")<<std::endl;
		else
			os << ("Heartbeat Struct for Right Arm")<<std::endl;

		//os << ("\tx, y, z\t") << HB.x[Arm] * INCHES_PER_MICRON << ("\t") << HB.y[Arm] * INCHES_PER_MICRON << ("\t")  << HB.z[Arm] * INCHES_PER_MICRON  << std::endl;  // send inches
		//os << ("\tr, p, y, g\t") << HB.roll[Arm] * RAD_PER_MICRORAD * DEG_PER_RAD << ("\t") // send in degrees
		//	<< HB.pitch[Arm] * RAD_PER_MICRORAD * DEG_PER_RAD << ("\t")
		//	<< HB.yaw[Arm]  * RAD_PER_MICRORAD * DEG_PER_RAD;
		os << (" Grasp: ") << HB.grasp[Arm] << std::endl; 
		os << (" Tip Pose: ")<<std::endl;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				os<<setw(12)<<HB.TipPose[Arm][i][j];
				if (j==3)
					os << std::endl;
			}
		}

		os << (" Joints on arm: ") << HB.jointsPerArm[Arm] << std::endl;;
		os << (" Joints in Limits:\t0x ") << ios::hex << (int)HB.jointsInLimits[Arm] << ios::dec << std::endl;

		os << (" armLocked: ") << HB.armLocked[Arm] << std::endl;
		os << (" brakesEngaged: ") << HB.brakesEngaged[Arm] << std::endl;
		os << (" gripperControlMode: ") << HB.gripperControlMode[Arm] << std::endl;

		os << (" Joint Values: ")  << std::endl; 
		//        int LimitStatus = 0;
		for(int i=0; i<HB.MaxJointsPerArm; i++) {
			//if (M7System::Radians == HB.jointUnitValues[Arm][i]) {
			os << HB.rawJointValues[Arm][i] / DEG_PER_RAD << ("\t"); // convert radians to degrees
			//}
			// TODO: handle other joint value units
		}
	}

	os << std::endl;  
	os << (" Tilt: ") << HB.cameraTilt << std::endl;
	os << (" Zoom: ") << HB.cameraZoomPercent << std::endl;

	os << (" Focus: ") << HB.cameraFocusMode << std::endl;
	os << (" Exposure: ") << HB.cameraExposureMode << std::endl;
	os << (" ExpComp: ") << HB.cameraExposureCompensation << std::endl;
	os << (" WhiteBal: ") << HB.cameraWhiteBalMode << std::endl;
	os << (" IRFilter: ") << HB.cameraIRFilterMode << std::endl;

	os << (" WhiteLED: ") << HB.whiteLEDLightMode << std::endl;
	os << (" IRLights: ") << HB.irLEDLightMode << std::endl;

	os << (" currentState_aux1: ") << HB.currentState_aux1 << std::endl;
	os << (" currentState_aux2: ") << HB.currentState_aux2 << std::endl;
	os << std::endl;

	os << (" vmeCommsState: ") << HB.vmeCommsState << std::endl;
	os << (" remoteSelfTestPercent: ") << HB.remoteSelfTestPercent << std::endl;
	os << (" eStopEnergized: ") << HB.eStopEnergized << std::endl;
	os << (" totalSystemUpMinutes: ") << HB.totalSystemUpMinutes << std::endl;
	os << (" batteryPercent: ") << HB.batteryPercent << std::endl;
	os << (" batteryMinutes: ") << HB.batteryMinutes << std::endl;
	os << std::endl;

	/*for (size ii=0; ii<MaxNetworkConverters; ii++) {
	os << (" remoteNetConvId[ii]: ") << HB.remoteNetConvId[ii];
	os << (" remoteNetConvSwVer[ii]: ") << HB.remoteNetConvSwVer[ii];
	os << (" remoteNetConvCommsState[ii]: ") << HB.remoteNetConvCommsState[ii];
	std::string tempAddr (HB.remoteNetConvAddr[ii]);
	os << (" remoteNetConvAddr[ii]: ") << tempAddr;
	}*/

	//os << ("Active UStruct Sequence Number: ") << HB.ActiveUStruct.sequence  << std::endl;
	//os << std::endl;

	return os;
}  

Packet_Haptic haptic_packet;

class TaurusX{
public:
	unsigned short port_number_M;			/* Port number to use */
	unsigned short port_number_A;			/* Port number to use */
	static const int b1=192, b2=168, b3=1, b4=118;					/* Components of address in xxx.xxx.xxx.xxx form */
	int length;					/* Length of client struct */
	struct sockaddr_in server;			/* Information about the server */
	struct sockaddr_in client_M;			/* Information about the client */
	struct sockaddr_in client_A;	/* Information about the client */
	struct hostent *hp;					/* Information about this computer */
	char host_name[256];				/* Name of the server */
	SOCKET sd_M;							/* Socket descriptor of server */
	SOCKET sd_A;					/* Socket descriptor of server */
	int bytes_received;					/* Bytes received from client */
	int bytes_sent;					/* Bytes received from client */
	char buffer_R1[BUFFER_SIZE];			/* Where to store received data */
	char buffer_R1_updated[BUFFER_SIZE];	/* Where to store updated data */
	char buffer_R2[BUFFER_SIZE];			/* Where to store received data */
	int pose_sequence_num;
	int	camera_sequence_num ;
	Packet_Pose pose_packet;
	Packet_Camera camera_packet;
	clock_t t, dt;
	std::ofstream control_out;
	bool incision_trigger;
	Point3D left_tip_position;
	bool abort_following_commands;
	char* controllogfilename; 
	bool enable_auto_incision;
	char enable_null_pose;

	// var for speech recognition
	int *bStart;
	double threshold; 
	bool initialization;
	string HMM;
	string LM;
	string DICT;
	string S;
	//// done
			


	int Initialize(char flag) {
		if (flag == 1) // Master to Agent connection
		{
			/* Use local address */
			if (sscanf_s("8023", "%u", &port_number_M) != 1)
			{
				//usage();
			}

			if (sscanf_s("13832", "%u", &port_number_A) != 1)
			{
				//usage();
			}

			/* Open a datagram socket */
			sd_M = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sd_M == INVALID_SOCKET)
			{
				fprintf(stderr, "Could not create socket.\n");
				pthread_exit(NULL);
				return 0;
			}

			/* Open a datagram socket */
			sd_A = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sd_A == INVALID_SOCKET)
			{
				fprintf(stderr, "Could not create socket.\n");
				pthread_exit(NULL);
				return 0;
			}

			/* Clear out server struct */
			memset((void *)&server, '\0', sizeof(struct sockaddr_in));

			/* Set family and port */
			server.sin_family = AF_INET;
			server.sin_port = htons(port_number_M);

			/* Get host name of this computer */
			gethostname(host_name, sizeof(host_name));
			hp = gethostbyname(host_name);

			/* Check for NULL pointer */
			if (hp == NULL)
			{
				fprintf(stderr, "Could not get host name.\n");
				closesocket(sd_M);
				pthread_exit(NULL);
				return 0;
			}


			/* Assign the address */
			server.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
			server.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
			server.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
			server.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];

			/* Bind address to socket */
			if ( ::bind(sd_M, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
			{
				fprintf(stderr, "Could not bind name to socket.\n");
				closesocket(sd_M);
				pthread_exit(NULL);
				return 0;

			}

			printf("Server Address is: %s \n", 
				inet_ntoa(server.sin_addr));

			printf("Server Port is: %d\n", ntohs(server.sin_port));

			// now we configure for the socket between the server and the agent
			/* Clear out client_A struct */
			memset((void *)&client_A, '\0', sizeof(struct sockaddr_in));

			/* Set family and port */
			client_A.sin_family = AF_INET;
			client_A.sin_port = htons(port_number_A);


			client_A.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)b1;
			client_A.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)b2;
			client_A.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)b3;
			client_A.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)b4;


			printf("Agent Address is: %s \n", 
				inet_ntoa(client_A.sin_addr));

			printf("Agent Port is: %d\n", ntohs(client_A.sin_port));
			printf("--------------------------------\n");

			length = (int)sizeof(struct sockaddr_in);
			//*********************************************************** My init. stuff
			
			pose_sequence_num = -1;
			camera_sequence_num = 0;
			incision_trigger = 0;
			enable_auto_incision = true;
			enable_null_pose = 1;

			//// init the speech recognition 
			bStart = new int;
			*bStart = 1;
			threshold = 80;
			HMM = "E:\\TaurusX\\TaurusX\\HeaderFile\\pocketsphinx\\model\\hmm\\en_US\\hub4wsj_sc_8k";
			LM = "E:\\TaurusX\\TaurusX\\HeaderFile\\0494.lm";
			DICT = "E:\\TaurusX\\TaurusX\\HeaderFile\\0494.dic";

			//////////// Speeach recognition
			//initialization = SPHINX::init(HMM.c_str(), LM.c_str(), DICT.c_str());

			////////////////////
			//t = clock();
			//time_t t = time(0);   // get time now
			//struct tm * now = localtime( & t );

			//char control_filename [80];
			//strftime (control_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
			//strcat(control_filename, "control_data.txt");
			//control_out.open(control_filename);

			//control_out.open(controllogfilename); // uncomment this if we want control output
			return 1;
		}

		else // Agent to Master connection
		{
			/* Use local address */
			if (sscanf("8024", "%u", &port_number_M) != 1)
			{
				//usage();
			}

			if (sscanf("13834", "%u", &port_number_A) != 1)
			{
				//usage();
			}

			/* Open a datagram socket */
			sd_M = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sd_M == INVALID_SOCKET)
			{
				fprintf(stderr, "Could not create socket.\n");
				pthread_exit(NULL);
				return 0;
			}

			/* Open a datagram socket */
			sd_A = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sd_A == INVALID_SOCKET)
			{
				fprintf(stderr, "Could not create socket.\n");
				pthread_exit(NULL);
				return 0;
			}

			/* Clear out server struct */
			memset((void *)&server, '\0', sizeof(struct sockaddr_in));

			/* Set family and port */
			server.sin_family = AF_INET;
			server.sin_port = htons(port_number_M);

			/* Get host name of this computer */
			gethostname(host_name, sizeof(host_name));
			hp = gethostbyname(host_name);

			/* Check for NULL pointer */
			if (hp == NULL)
			{
				fprintf(stderr, "Could not get host name.\n");
				closesocket(sd_M);
				pthread_exit(NULL);
				return 0;
			}


			/* Assign the address */
			server.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
			server.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
			server.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
			server.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];

			/* Bind address to socket */

			if ( ::bind(sd_M, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
			{
				fprintf(stderr, "Could not bind name to socket.\n");
				closesocket(sd_M);
				pthread_exit(NULL);
				return 0;
			}

			printf("Server Address is: %s \n", 
				inet_ntoa(server.sin_addr));
			printf("Server Port is: %d\n", ntohs(server.sin_port));

			// now we configure for the socket between the server and the agent
			/* Clear out client_A struct */
			memset((void *)&client_A, '\0', sizeof(struct sockaddr_in));

			/* Set family and port */
			client_A.sin_family = AF_INET;
			client_A.sin_port = htons(port_number_A);


			client_A.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)b1;
			client_A.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)b2;
			client_A.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)b3;
			client_A.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)b4;

			printf("Master Address is: %s \n", 
				inet_ntoa(client_A.sin_addr));

			printf("Master Port is: %d\n", ntohs(client_A.sin_port));
			printf("--------------------------------\n");

			length = (int)sizeof(struct sockaddr_in);
			return 1;
		}

	}

	void get_log_file_name(void* info)
	{
		controllogfilename = new char[60];
		//***********************************************************************************************************
		// to extract the char* from the void pointer, and use it as the name for the interface data log file
		string &logfilename = *(static_cast <std::string*> (info));
		char* ch_logfilename = new char[40];
		strcpy (ch_logfilename, logfilename.c_str());
		strcat(ch_logfilename, "-control.txt");
		char* name_with_path = new char[50];
		strcpy(name_with_path, "log/");
		strcat(name_with_path, ch_logfilename);
		strcpy(controllogfilename, name_with_path);
		//***********************************************************************************************************
	}

	bool check_control_input(bool i, Point3D p)
	{
		bool dangerous_flag = false;
		if ( 
			 (i==0 && (p.x > 500000 || p.x < 150000 || p.y <-400000 || p.z > 400000 || p.z < 40000))
			 ||
			 (i==1 && (p.x > 500000 || p.x < 150000 || p.y >400000 || p.z > 400000 || p.z < 40000))
		   )
		{
		   dangerous_flag = true;
		   sam.speak("Dangerous, control input out of range");
		}
		return dangerous_flag;
	}

	void Send_Command(const char _surgeon_mode = 3)
	{
		pose_sequence_num++; 
		*(u_int*)&buffer_R1[4] += pose_sequence_num; // update the sequence number!
		//Set_surgeon_mode(_surgeon_mode); // enable control optimization algorithm
		Set_surgeon_mode(enable_null_pose); // enable control optimization algorithm 
		memcpy(buffer_R1_updated, buffer_R1, BUFFER_SIZE);
		
		bool dangerous_flag;
		pthread_mutex_lock(&mutexHaptic);
		dangerous_flag = 
			(
			// safe region check for left arm
			haptic_packet.TipPose[0][0][3] > 500000 ||
			haptic_packet.TipPose[0][0][3] < 150000 ||
			haptic_packet.TipPose[0][1][3] >  60000 || 
			haptic_packet.TipPose[0][1][3] <-400000 || 
			haptic_packet.TipPose[0][2][3] > 400000 ||
			haptic_packet.TipPose[0][2][3] <  40000 ||

			// safe region check for right arm
			haptic_packet.TipPose[1][0][3] > 500000 ||
			haptic_packet.TipPose[1][0][3] < 150000 ||
			haptic_packet.TipPose[1][1][3] < -60000 ||
			haptic_packet.TipPose[1][1][3] > 400000 ||
			haptic_packet.TipPose[1][2][3] > 400000 ||
			haptic_packet.TipPose[1][2][3] <  40000 ||

			// JointsInLimits
			!(haptic_packet.JointsInLimits[0]&127)
			);
		for (size_t i_arm=0; i_arm<2; i_arm++) 
			for (size_t i_joint=0; i_joint<8; i_joint++)
				if (abs(haptic_packet.NormAngleFromMid[i_arm][i_joint]) > 0.99)
					dangerous_flag = 1;

		pthread_mutex_unlock(&mutexHaptic);
		if(dangerous_flag)
		{
			fprintf(stderr, "Arm reaching the end of safe region, can't move any more!\n");
			sam.speak("Dangerous, can't move any more");
		}
		else
		{
			Send();
			// the following is to write out the data for the input of robots
			Read_Pose_Packet();
			//for (int arm = 0; arm<2;arm++)
			//{
			//	for (int i=0;i<4;i++)
			//		for (int j=0;j<4;j++)
			//			control_out<<pose_packet.TipPose[arm][i][j]<<' ';
			//	control_out<<pose_packet.grip[arm]<<' '<<std::endl;
			//}

			if (incision_trigger == 0) // if the autonomous incision is happening (trigger == 1), then won't trigger again
			{
				// now we need to detect if the scalpel tip is close enough to the start of incision, and then set the incision trigger.
				jaw_frame_to_tip_position(0, pose_packet.TipPose[0], left_tip_position);
				update_incision_trigger (left_tip_position);
			}
			
		}
	}

	void Send()
	{
		bytes_sent = sendto(sd_A, buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&client_A, length);	

		// get click difference
		dt = clock() - t;
		if (dt<4) 
			Sleep(4-dt); // to ensure 250 Hz of communication rate

		// just for us to check if there are 4 clicks between commands, we should always get 4 clicks
		//dt = clock() - t;
		//printf ("It took me %f seconds.\n",((float)dt)/CLOCKS_PER_SEC);

		// update the current clock
		t = clock();

		if (bytes_sent < 0)
		{
			fprintf(stderr, "Could not send datagram.\n");
			closesocket(sd_A);
			WSACleanup();
			exit(0);
		}
	}

	bool get_incision_trigger()
	{
		return incision_trigger;
	}

	void update_incision_trigger(Point3D _p)
	{
		// if the current tip position is close enough to the starting point 
		if ((sqrt (pow(_p.x - 335352, 2) + pow(_p.y + 29272.9, 2) + pow(_p.z - 287455, 2) ) < 1 ) && enable_auto_incision) // 5000 (0.5 cm) distance // for the demo for Prof, I change it to 1 since I want to disable it
		{
			incision_trigger = 1;
			sam.speak("Taurus offers to finish the incision task, proceed or ignore?", SAM::ASYNC);
			std::cout<<"Taurus offers to finish the incision task, proceed(y) or ignore(n)?"<<std::endl;
			do 
			// the user has to make a decision to leave this loop. This is to avoid the Keyboard control module to absorb inputs from here
			{
				//if(SPHINX::utterance(bStart) == false) break;
				//if(SPHINX::probability > threshold){ 
				//	S = SPHINX::hypothesis;
				//	std::cout<<"Speech: "<<S<<", condifence: "<<SPHINX::probability<<std::endl;
				//	// Matching to directions
				//	if (S.compare("PROCEED") == 0){
				//		run_incision_routine();
				//		incision_trigger = 0;
				//	}
				//	if (S.compare("IGNORE") == 0){
				//		sam.speak("ignore auto incision");
				//		incision_trigger = 0;
				//		enable_auto_incision = false;
				//	}
				//}

				if (GetAsyncKeyState('Y') < 0)
				{
					run_incision_routine();
					incision_trigger = 0;

				}
				else if (GetAsyncKeyState('N') < 0)
				{
					sam.speak("ignore auto incision");
					incision_trigger = 0;
					enable_auto_incision = false;
				}
			} while(incision_trigger);	
		}
	}

	void check_discrepency_between_command_and_haptic(const char arm, const char pos_or_rot, const Point3D _p)
	{
		matrix <double> pose(4,4);
		matrix <double> haptic(4,4);
		pose = get_pose_packet(arm);
		haptic = get_haptic_packet(arm); 
		Point3D pose_rot, haptic_rot;
		pose_rot.x = atan2(pose(2,1), pose(2,2));
		pose_rot.y = atan2(-pose(2,0), sqrt(pow(pose(0,0),2)+pow(pose(1,0),2)));
		pose_rot.z = atan2(pose(1,0), pose(0,0));
		

		haptic_rot.x = atan2(haptic(2,1), haptic(2,2));
		haptic_rot.y = atan2(-haptic(2,0), sqrt(pow(haptic(0,0),2)+pow(haptic(1,0),2)));
		haptic_rot.z = atan2(haptic(1,0), haptic(0,0));
		
		
		std::cout << "Between POSE & HAPTIC "<<std::endl;
		std::cout << "dx (mm): " <<abs(pose(0,3)-haptic(0,3))/1000<<std::endl;
		std::cout << "dy (mm): " <<abs(pose(1,3)-haptic(1,3))/1000<<std::endl;
		std::cout << "dz (mm): " <<abs(pose(2,3)-haptic(2,3))/1000<<std::endl;
		std::cout << "droll(deg) : "  <<abs(pose_rot.x-haptic_rot.x)*DEG_PER_RAD<<std::endl;
		std::cout << "dpitch(deg): " <<abs(pose_rot.y-haptic_rot.y)*DEG_PER_RAD<<std::endl;
		std::cout << "dyaw(deg)  : " <<abs(pose_rot.z-haptic_rot.z)*DEG_PER_RAD<<std::endl;

		if (pos_or_rot == 0)
		{
			std::cout << "Between Command position & actual position"<<std::endl;
			std::cout << "dx (mm): " <<abs(_p.x-haptic(0,3))/1000<<std::endl;
			std::cout << "dy (mm): " <<abs(_p.y-haptic(1,3))/1000<<std::endl;
			std::cout << "dz (mm): " <<abs(_p.z-haptic(2,3))/1000<<std::endl;
		}
		else if (pos_or_rot == 1)
		{
			std::cout << "Discrepency between destination and actual position"<<std::endl;
			std::cout << "droll(deg) : " <<abs(_p.x-haptic_rot.x*DEG_PER_RAD)<<std::endl;
			std::cout << "dpitch(deg): " <<abs(_p.y-haptic_rot.y*DEG_PER_RAD)<<std::endl;
			std::cout << "dyaw(deg)  : " <<abs(_p.z-haptic_rot.z*DEG_PER_RAD)<<std::endl<<std::endl;
		}
	}

	int transition_sub_routine(double _px, double _py, double _pz, char arm = 0, const double incision_increment_length = 30, const int transition_wait_time = 200)
	{
		Point3D tempP;
		tempP.set_point(_px, _py, _pz);
		
		{
			// Now I just stupidly do it three times, the right way to do it is to detect the discrepency between control command and report position,
			// and keep sending until the discrepency goes below a threshold (or after several steps)
			// get to a finer and finer scale, so the length will decrease
			Set_Arm_Position(arm, tempP, incision_increment_length);
			Set_Arm_Position(arm, tempP, 0.5*incision_increment_length);
			Set_Arm_Position(arm, tempP, 0.1*incision_increment_length);
		}
		
		Sleep(transition_wait_time);
		check_discrepency_between_command_and_haptic(arm, 0, tempP);
		if (GetAsyncKeyState('S') < 0) 
		{
			std::cout<<"Stop incision"<<std::endl; 
			sam.speak("Stop auto incision");
			abort_following_commands = 1; 
			return 1;
		}
		else
			return 0;
	}

	int rotation_sub_routine(double _roll, double _pitch, double _yaw, char arm = 0, const double angle_step_length = 0.1, const int rotation_wait_time = 800)
	{
		//Increase_Arm_Position(0, 0, 0, 0); // send a packet to the robot so that the pose packet gets fulfilled once and is not all zero anymore
		//now I use haptic packet as reference, not the pose packet any more.
		
		Point3D tempP;
		tempP.set_point(_roll, _pitch, _yaw);
		matrix <double> old_frame(4,4);
		// Now I just stupidly do it three times, the right way to do it is to detect the discrepency between control command and report position,
		// and keep sending until the discrepency goes below a threshold (or after several steps)
		// get to a finer and finer scale, so the length will decrease
		old_frame = get_haptic_packet(arm); 
		Set_Pose_Rotate(arm, _roll, _pitch, _yaw, old_frame, angle_step_length);
		old_frame = get_haptic_packet(arm); 
		Set_Pose_Rotate(arm, _roll, _pitch, _yaw, old_frame, 0.5*angle_step_length);
		old_frame = get_haptic_packet(arm); 
		Set_Pose_Rotate(arm, _roll, _pitch, _yaw, old_frame, 0.1*angle_step_length);
		Sleep(rotation_wait_time);
		check_discrepency_between_command_and_haptic(arm, 1, tempP);
		if (GetAsyncKeyState('S') < 0) 
		{
			std::cout<<"Stop incision"<<std::endl; 
			sam.speak("Stop auto incision");
			abort_following_commands = 1; 
			return 1;
		}
		else
			return 0;
	}

	int run_incision_routine()
	{
		std::cout<<"Auto-incision begins, to stop it please press key ""s"""<<std::endl;
		sam.speak("Auto-incision begins, to stop it please press key ""s""", SAM::ASYNC);
		///////////////*****************************start*********************////////////////////
		// Fixed Ready State that I personally defined
		// if (transition_sub_routine(358081, -81338, 111896)) return 0;

		if (rotation_sub_routine(100.815, -65.5325, 77.5265)) return 0;
		sam.speak("Reach for start point", SAM::ASYNC);
		
		if (transition_sub_routine( 328865, -61267.1, 223960)) return 0; 

		if (transition_sub_routine( 302800, -60609.6, 223908)) return 0;
		sam.speak("Segment one finished", SAM::ASYNC);
		if (transition_sub_routine( 277655, -65751.4, 223849)) return 0; 
		sam.speak("Segment two finished", SAM::ASYNC);
		if (transition_sub_routine( 258724, -73907.3, 223889)) return 0;  
		sam.speak("Segment three finished", SAM::ASYNC);
		if (transition_sub_routine( 258514, -73857.2, 214402)) return 0;   

		sam.speak("Auto-incision finished, go back to start point", SAM::ASYNC);
		if (transition_sub_routine( 319646, -63896.3, 214153, 0, 80)) return 0;    

		

		/*if (rotation_sub_routine(100.815, -65.5325, 77.5265)) return 0;
		if (transition_sub_routine( 325777,      -42543.4,       204557, 0, 60)) return 0;
		sam.speak("Change scalpel tilt angle and reach for start point", SAM::ASYNC);
		if (rotation_sub_routine(-95.1621, -76.7502, -71.9024 )) return 0;
		
		if (transition_sub_routine(322614,      -44838.2,       217296)) return 0; 
		if (transition_sub_routine(302986,      -45647.2,       217089)) return 0; 
		sam.speak("Segment one finished", SAM::ASYNC);
		if (transition_sub_routine(281181,      -48584.4,       217023)) return 0;   
		sam.speak("Segment two finished", SAM::ASYNC);
		if (transition_sub_routine(259950,       -55619.7,       217089)) return 0;    
		sam.speak("Segment three finished", SAM::ASYNC);

*/

		//*************************************************************
		/*if (transition_sub_routine(295241,-65965, 230018, 0, 60)) return 0;

		if (transition_sub_routine(276645, -64161.1, 230015)) return 0;
		sam.speak("Segment one finished", SAM::ASYNC);

		if (rotation_sub_routine(152.864, -44.5984, 25.3784)) return 0;
		if (transition_sub_routine(251639, -69474.5,230420)) return 0;
		sam.speak("Segment two finished", SAM::ASYNC);


		if (transition_sub_routine(251639, -69474.5,228420)) return 0;
		if (rotation_sub_routine(170.291, -48.5911, 28.0539)) return 0;
		if (transition_sub_routine(250696, -73168, 231822)) return 0; 
		if (transition_sub_routine(230021, -81794.8, 231507)) return 0; 
		sam.speak("Segment three finished", SAM::ASYNC);*/
		//*************************************************************


		//if (transition_sub_routine(259726,      -55539.1,       210080)) return 0; 

		// go back to the starting point and get ready for the knot tying
		//sam.speak("Auto-incision finished, go back to start point", SAM::ASYNC);
		//if (transition_sub_routine(321439,      -47284.3,       209844, 0, 80)) return 0;  
		///////////////*****************************end*********************////////////////////

		Set_Grip(0, 100);
		Sleep(3000);
		sam.speak("Get ready for suture", SAM::ASYNC);
		if (transition_sub_routine(306925, -67268, 209845, 0, 80)) return 0;   
		if (transition_sub_routine(316913, 35073.6, 197739, 1, 80)) return 0;   

		
		//if (rotation_sub_routine(-175.466, 0.706858, 93.1189, 0 )) return 0;
		if (rotation_sub_routine(-93.6746, -87.3862, -83.9894, 1 )) return 0;

		sam.speak("Pick up needle", SAM::ASYNC);
		Set_Grip(1, 100);
		Sleep(500);
		if (transition_sub_routine(311530, -21793.1, 251589, 1, 80)) return 0;   
		Set_Grip(1, 0);
		Sleep(1000);
		if (transition_sub_routine(311530, -11793.1, 234804, 1, 80)) return 0; 



		abort_following_commands = 1;
		enable_auto_incision = false;
		enable_null_pose = 3;
		return 1;

	}


	void Set_surgeon_mode (char _surgeon_mode)
	// When surgeonmode == 3, an optimization algorithm will work to give robot an easy pose. (both pedal down)
	// when surgeonmode == 1, it's common teleoperation.  (clutch is down)
	// when surgeonmode == 2, it means camera pedal is down
	{
		*(u_int*)&buffer_R1[40] = _surgeon_mode;
	}

	void Fill_in_buffer(Packet_Pose &p)
	{
		for (int Arm = 0; Arm < 2; Arm++) {
			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					if (Arm == 0)
					{
						*(double*)&buffer_R1[48 + 32*i + 8*j] = p.TipPose[Arm][i][j];
						*(double*)&buffer_R1[176] = p.grip[Arm];

					}
					else
					{
						*(double*)&buffer_R1[184 + 32*i + 8*j] = p.TipPose[Arm][i][j];
						*(double*)&buffer_R1[312] = p.grip[Arm];
					}	
				}
			}
		}
	}


	// Write the data to a stream for transmission
	size_t Write(ostream& os, Packet_Pose _p) {
		os.write(reinterpret_cast<char const *>(&_p.magicNumber), sizeof(_p.magicNumber)); 
		os.write(reinterpret_cast<char const *>(&_p.sequence), sizeof(_p.sequence));
		os.write(reinterpret_cast<char const *>(&_p.pactyp), sizeof(_p.pactyp));
		os.write(reinterpret_cast<char const *>(&_p.version), sizeof(_p.version));
		os.write(reinterpret_cast<char const *>(&_p.performanceCountWhenSent), sizeof(_p.performanceCountWhenSent));
		os.write(reinterpret_cast<char const *>(&_p.performanceCountWhenReceived), sizeof(_p.performanceCountWhenReceived));
		os.write(reinterpret_cast<char const *>(&_p.performanceFrequency), sizeof(_p.performanceFrequency));
		os.write(reinterpret_cast<char const *>(&_p.surgeon_mode), sizeof(_p.surgeon_mode));
		os.write(reinterpret_cast<char const *>(&_p.current_state), sizeof(_p.current_state));

		double d=0;

		for (int ArmNum = 0; ArmNum < 2; ArmNum++) {

			for (int i=0;i<4;i++){
				for (int j=0;j<4;j++){
					d=(double)(_p.TipPose[ArmNum][i][j]);
					os.write((char*)(&d), sizeof(double));
				}
			}

			os.write( (char*)(&_p.grip[ArmNum]), sizeof(double));
		}	
		return 1;
	};

	// update the position of left arm. dx dy dz(unit mirons) are the xyz coordinates in camera's view.
	// notice that the largest allowable distance for movement each time is 1cm. (choose this value personally)
	// buffer is where to contain position information.

	//int MAX_MOVE = 200; // this is used in function Set_Arm_Position, to indicate the max step length for each position update
	// for autonomous move, I used 200 so that it moves smoothly and with a decent speed. 
	// for controlling with Omega, since Omega required high communication frequency to enable stability, we increase this value to 4000 so that the tremor will be decreased
	// this is not crazy, since through Omega we can still have stable control.
	void Increase_Arm_Position(bool _left_or_right, double dx, double dy, double dz, double max_move = 200)
	{
		int offset;
		if (_left_or_right == 0)
			offset = 48;
		else
			offset = 184;

		int N_step_x = static_cast<int> (abs(dx)/max_move + 1);
		int N_step_y = static_cast<int> (abs(dy)/max_move + 1);
		int N_step_z = static_cast<int> (abs(dz)/max_move + 1);

		int N_step_max = (N_step_x>N_step_y)?N_step_x:N_step_y;
		N_step_max = (N_step_max>N_step_z)?N_step_max:N_step_z;

		//std::cout<<"Takes "<<N_step_max<<" increments to finish the transition"<<std::endl;

		double step_x, step_y, step_z;
		step_x = dx / N_step_max;
		step_y = dy / N_step_max;
		step_z = dz / N_step_max;	
		for (int i = 0; i<N_step_max; i++)
		{
			*(double*)&buffer_R1[offset + 32*0 + 8*3] += step_x;
			*(double*)&buffer_R1[offset + 32*1 + 8*3] += step_y;
			*(double*)&buffer_R1[offset + 32*2 + 8*3] += step_z;
			//control_out<< *(double*)&buffer_R1[offset + 32*0 + 8*3] << ' '<< *(double*)&buffer_R1[offset + 32*1 + 8*3]<<' '<<*(double*)&buffer_R1[offset + 32*2 + 8*3]<<std::endl;
			Send_Command();
			if (abort_following_commands) 
			{ 
				abort_following_commands = 0;
				return;
			}
		}							
	}

	// Set the position of arm. dx dy dz(unit in micron) are the xyz coordinates in camera's view.
	// buffer is where to contain position information.
	void Set_Arm_Position(bool _left_or_right, Point3D dest_p, double max_move = 200)
	{
		int offset;
		if (_left_or_right == 0)
			offset = 48;
		else
			offset = 184;

		//dest_p.x *= MEGA;
		//dest_p.y *= MEGA;
		//dest_p.z *= MEGA;
		double current_x = *(double*)&buffer_R1[offset + 32*0 + 8*3];
		double current_y = *(double*)&buffer_R1[offset + 32*1 + 8*3];
		double current_z = *(double*)&buffer_R1[offset + 32*2 + 8*3];

		int N_step_x = static_cast<int> (abs(dest_p.x-current_x)/max_move + 1);
		int N_step_y = static_cast<int> (abs(dest_p.y-current_y)/max_move + 1);
		int N_step_z = static_cast<int> (abs(dest_p.z-current_z)/max_move + 1);

		int N_step_max = (N_step_x>N_step_y)?N_step_x:N_step_y;
		N_step_max = (N_step_max>N_step_z)?N_step_max:N_step_z;

		
		if (N_step_max > 10)
		{
			std::cout<<"Ignore large movements. Takes "<<N_step_max<<" increments to finish this transition"<<std::endl;
		}
		else
		{
			double step_x, step_y, step_z;
			step_x = (dest_p.x - current_x) / N_step_max;
			step_y = (dest_p.y - current_y) / N_step_max;
			step_z = (dest_p.z - current_z) / N_step_max;	
			for (int i = 0; i<N_step_max; i++)
			{
				*(double*)&buffer_R1[offset + 32*0 + 8*3] += step_x;
				*(double*)&buffer_R1[offset + 32*1 + 8*3] += step_y;
				*(double*)&buffer_R1[offset + 32*2 + 8*3] += step_z;
				//control_out<< *(double*)&buffer_R1[offset + 32*0 + 8*3] << ' '<< *(double*)&buffer_R1[offset + 32*1 + 8*3]<<' '<<*(double*)&buffer_R1[offset + 32*2 + 8*3]<<std::endl;
				Send_Command();
				if (abort_following_commands) 
				{ 
					abort_following_commands = 0;
					return;
				}
			}	
		}
	}

	// increment the openness of gripper
	// arg is the increment of the gripper value, positive means to open more, negative means to open less
	void Increase_Grip(bool _left_or_right, double arg)
	{
		int offset;
		if (_left_or_right == 0)
			offset = 176;
		else
			offset = 312;

		double set_value = *(double*)&buffer_R1[offset] + arg;
		if (set_value > 75)
			*(double*)&buffer_R1[offset] = 75;
		else if (set_value < 20)
			*(double*)&buffer_R1[offset] = 20;
		else 
			*(double*)&buffer_R1[offset] = set_value;
		Send_Command();
		if (abort_following_commands) 
		{ 
			abort_following_commands = 0;
			return;
		}
	}


	// set gripper value. 
	// arg is the offset argument, 20 is close, 100 is full open
	void Set_Grip(bool _left_or_right, double arg)
	{
		int offset;
		if (_left_or_right == 0)
			offset = 176;
		else
			offset = 312;
		if (arg > 75)
			*(double*)&buffer_R1[offset] = 75;
		else if (arg < 20)
			*(double*)&buffer_R1[offset] = 20;
		else 
			*(double*)&buffer_R1[offset] = arg;

		Send_Command();
		if (abort_following_commands) 
		{ 
			abort_following_commands = 0;
			return;
		}
	}

	// set camera tilt, it's in radians. range from 0 to 1
	// arg is the argument, 0 is horizontal, 1 is vertical
	void Set_Camera_Tilt(double arg)
	{
		if (arg > 1.5)
			*(double*)&buffer_R1[100] = 1.5;
		else if (arg < 0)
			*(double*)&buffer_R1[100] = 0;
		else 
			*(double*)&buffer_R1[100] = arg;

		camera_sequence_num++;
		*(u_int*)&buffer_R1[4] += camera_sequence_num;
		Send();
		Read_Camera_Packet();
	}

	void jaw_frame_to_tip_position (bool left_or_right, bnu::matrix<double> frame, Point3D &tip_position)
	{
		if (left_or_right == 0) // left arm
		{
			tip_position.x = frame (0,3) + 0.072 * frame (0,0) - 0.005 * frame (0,1);
			tip_position.y = frame (1,3) + 0.072 * frame (1,0) - 0.005 * frame (1,1);
			tip_position.z = frame (2,3) + 0.072 * frame (2,0) - 0.005 * frame (2,1);
		}
		else // right arm
		{
			tip_position.x = frame (0,3) + 0.03 * frame (0,0);
			tip_position.y = frame (1,3) + 0.03 * frame (1,0);
			tip_position.z = frame (2,3) + 0.03 * frame (2,0);
		}
	}

	void jaw_frame_to_tip_position (bool left_or_right, double frame[4][4], Point3D &tip_position)
	{
		if (left_or_right == 0) // left arm
		{
			tip_position.x = frame [0][3] + 0.072 * frame [0][0] - 0.005 * frame [0][1];
			tip_position.y = frame [1][3] + 0.072 * frame [1][0] - 0.005 * frame [1][1];
			tip_position.z = frame [2][3] + 0.072 * frame [2][0] - 0.005 * frame [2][1];
		}
		else // right arm
		{
			tip_position.x = frame [0][3] + 0.03 * frame [0][0];
			tip_position.y = frame [1][3] + 0.03 * frame [1][0];
			tip_position.z = frame [2][3] + 0.03 * frame [2][0];
		}
	}

	// this is to rotate the gripper along only one axis referenced in tip frame using yaw, roll and pitch
	void Update_Pose_Rotate(bool left_or_right, char rotation_axis, double rotation_degree, matrix<double> old_frame, const double angle_step = 0.01)
	{
		double rotation_radian = rotation_degree/DEG_PER_RAD;
		int N = int (abs(rotation_degree)/angle_step); 
		//std::cout<<"Takes "<<N<<" increments to finish the rotation"<<std::endl;
		double each_step_rotation_radian = rotation_radian/N;	
		matrix<double> rotation(4,4);
		switch (rotation_axis)
		{
		case 'x': 
			rotation(0,0) = 1; 
			rotation(0,1) = 0;
			rotation(0,2) = 0; 
			rotation(1,0) = 0; 
			rotation(1,1) = cos(each_step_rotation_radian); 
			rotation(1,2) = -sin(each_step_rotation_radian); 
			rotation(2,0) = 0; 
			rotation(2,1) = sin(each_step_rotation_radian); 
			rotation(2,2) = cos(each_step_rotation_radian); 
			break;
		case 'y':
			rotation(0,0) = cos(each_step_rotation_radian); 
			rotation(0,1) = 0;
			rotation(0,2) = sin(each_step_rotation_radian); 
			rotation(1,0) = 0; 
			rotation(1,1) = 1; 
			rotation(1,2) = 0; 
			rotation(2,0) = -sin(each_step_rotation_radian); 
			rotation(2,1) = 0; 
			rotation(2,2) = cos(each_step_rotation_radian); 
			break;
		case 'z':
			rotation(0,0) = cos(each_step_rotation_radian); 
			rotation(0,1) = -sin(each_step_rotation_radian);
			rotation(0,2) = 0; 
			rotation(1,0) = sin(each_step_rotation_radian); 
			rotation(1,1) = cos(each_step_rotation_radian); 
			rotation(1,2) = 0; 
			rotation(2,0) = 0; 
			rotation(2,1) = 0; 
			rotation(2,2) = 1; 
			break;
		default:
			std::cout<<"Wrong axis"<<std::endl;
			break;
		}
		rotation(0,3) = 0; 
		rotation(1,3) = 0; 
		rotation(2,3) = 0; 
		rotation(3,0) = 0; 
		rotation(3,1) = 0; 
		rotation(3,2) = 0; 
		rotation(3,3) = 1; 

		//std::cout<< old_frame << std::endl;
		//std::cout<< rotation << std::endl;

		matrix <double> new_frame(4,4); 

		for (int i = 1; i<N; i++)
		{
			new_frame = prod(old_frame, rotation);
			//std::cout<< new_frame << std::endl;

			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					if (left_or_right == 0)
						*(double*)&buffer_R1[48 + 32*i + 8*j] = new_frame(i,j);
					else
						*(double*)&buffer_R1[184 + 32*i + 8*j] = new_frame(i,j);
				}
			}

			for (size_t i = 0;i < 3; i++){
				if (abs(old_frame(i,3)-new_frame(i,3))> 10)
				{
					std::cout<<"Something is wrong! Position changes in rotation!"<<std::endl;
					return;
				}
			}
			Send_Command();
			old_frame = new_frame;
			if (abort_following_commands) 
			{ 
				abort_following_commands = 0;
				return;
			}
		}

	}

	// OK, finally I got it solved, in this function the pose matrix 4x4 will be updated, there is no step involved, 
	// all the update is controlled by the interface, Omega, Kinect or LeapMotion
	// rotation matrix should be 3x3
	void Update_T_matrix(bool left_or_right, matrix<double> rotation_matrix, matrix<double> reference, Point3D dest_p)
	{
		int offset;
		if (left_or_right == 0)
			offset = 48;
		else
			offset = 184;

		matrix_range <matrix<double> > reference_rotation (reference, range (0, 3), range (0, 3));
		matrix<double> new_frame(4,4);
		matrix<double> temp(3,3);
		temp = prod(reference_rotation, rotation_matrix);
		for (size_t i = 0; i < 3; i++) 
			for (size_t j = 0; j < 3; j++) 
				new_frame(i,j) = temp(i,j)*MEGA;

		new_frame(0,3) = dest_p.x;
		new_frame(1,3) = dest_p.y;
		new_frame(2,3) = dest_p.z;
		new_frame(3,0) = 0;
		new_frame(3,1) = 0; 
		new_frame(3,2) = 0; 
		new_frame(3,3) = 1; 

		/*std::cout<<"reference"<<std::endl<< reference << std::endl;
		std::cout<<"rotation_matrix "<<std::endl<<rotation_matrix<<std::endl;
		std::cout<<"current_frame"<<std::endl<< current_frame << std::endl;
		std::cout<<"new_frame"<<std::endl<< new_frame << std::endl;*/

		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				if (left_or_right == 0)
					*(double*)&buffer_R1[48 + 32*i + 8*j] = new_frame(i,j);
				else
					*(double*)&buffer_R1[184 + 32*i + 8*j] = new_frame(i,j);
			}
		}
		Send_Command();
		if (abort_following_commands) 
		{ 
			abort_following_commands = 0;
			return;
		}
	}


	// Given the target frame, rotate towards that. That is the destination. The input is target_frame
	void Rotate_to_Direct_Target_Frame(bool left_or_right, matrix<double> target_frame, matrix<double> old_frame, double angle_step_length = 0.1)
	{

		double final_roll = atan2(target_frame(2,1),target_frame(2,2));
		double final_pitch = atan2(-target_frame(2,0),sqrt(pow(target_frame(0,0),2)+pow(target_frame(1,0),2)));
		double final_yaw = atan2(target_frame(1,0),target_frame(0,0));

		double old_roll = atan2(old_frame(2,1),old_frame(2,2));
		double old_pitch = atan2(-old_frame(2,0),sqrt(pow(old_frame(0,0),2)+pow(old_frame(1,0),2)));
		double old_yaw = atan2(old_frame(1,0),old_frame(0,0));


		int N_roll, N_pitch, N_yaw, N;
		N_roll = static_cast<int> (abs(final_roll-old_roll)*DEG_PER_RAD/angle_step_length);
		N_pitch = static_cast<int> (abs(final_pitch-old_pitch)*DEG_PER_RAD/angle_step_length);
		N_yaw = static_cast<int> (abs(final_yaw-old_yaw)*DEG_PER_RAD/angle_step_length);

		N = (N_roll>N_pitch)?N_roll:N_pitch;
		N = (N>N_yaw)?N:N_yaw;

		//std::cout<<"Takes "<<N<<" increments to finish the rotation"<<std::endl;

		double step_roll, step_pitch, step_yaw;
		step_roll = (final_roll-old_roll)/N;
		step_pitch = (final_pitch-old_pitch)/N;
		step_yaw = (final_yaw-old_yaw)/N;

		matrix<double> new_frame_roll(3,3);
		matrix<double> new_frame_pitch(3,3);
		matrix<double> new_frame_yaw(3,3);
		matrix<double> new_rotation(3,3);
		matrix<double> new_frame(4,4);

		for (int i = 0;i < N;i++)
		{
			new_frame_roll(0,0) = 1; 
			new_frame_roll(0,1) = 0;
			new_frame_roll(0,2) = 0; 
			new_frame_roll(1,0) = 0; 
			new_frame_roll(1,1) = cos(old_roll + step_roll*i); 
			new_frame_roll(1,2) = -sin(old_roll + step_roll*i); 
			new_frame_roll(2,0) = 0; 
			new_frame_roll(2,1) = sin(old_roll + step_roll*i); 
			new_frame_roll(2,2) = cos(old_roll + step_roll*i); 


			new_frame_pitch(0,0) = cos(old_pitch + step_pitch*i); 
			new_frame_pitch(0,1) = 0;
			new_frame_pitch(0,2) = sin(old_pitch + step_pitch*i); 
			new_frame_pitch(1,0) = 0; 
			new_frame_pitch(1,1) = 1; 
			new_frame_pitch(1,2) = 0; 
			new_frame_pitch(2,0) = -sin(old_pitch + step_pitch*i); 
			new_frame_pitch(2,1) = 0; 
			new_frame_pitch(2,2) = cos(old_pitch + step_pitch*i); 

			new_frame_yaw(0,0) = cos(old_yaw + step_yaw*i); 
			new_frame_yaw(0,1) = -sin(old_yaw + step_yaw*i);
			new_frame_yaw(0,2) = 0; 
			new_frame_yaw(1,0) = sin(old_yaw + step_yaw*i); 
			new_frame_yaw(1,1) = cos(old_yaw + step_yaw*i); 
			new_frame_yaw(1,2) = 0; 
			new_frame_yaw(2,0) = 0; 
			new_frame_yaw(2,1) = 0; 
			new_frame_yaw(2,2) = 1; 

			new_rotation = prod(new_frame_yaw, new_frame_pitch);
			new_rotation = prod(new_rotation, new_frame_roll);

			for (size_t ii = 0; ii < 3; ii++) {
				for (size_t jj = 0; jj < 3; jj++) {
					new_frame(ii,jj) = new_rotation(ii,jj)*MEGA;
				}
			}

			new_frame(3,0) = 0; 
			new_frame(3,1) = 0; 
			new_frame(3,2) = 0; 
			new_frame(3,3) = 1; 

			new_frame(0,3) = old_frame(0,3);  // this is only rotation, for the position, it remains the same
			new_frame(1,3) = old_frame(1,3); 
			new_frame(2,3) = old_frame(2,3); 

			//std::cout<< old_frame << std::endl;
			//std::cout<< new_frame_roll << std::endl;
			//std::cout<< new_frame_pitch << std::endl;
			//std::cout<< new_frame_yaw << std::endl;
			//std::cout<< new_rotation << std::endl;
			//std::cout<< new_frame << std::endl;

			for (size_t iii = 0; iii < 4; iii++) {
				for (size_t jjj = 0; jjj < 4; jjj++) {
					if (left_or_right == 0)
						*(double*)&buffer_R1[48 + 32*iii + 8*jjj] = new_frame(iii,jjj);
					else
						*(double*)&buffer_R1[184 + 32*iii + 8*jjj] = new_frame(iii,jjj);
				}
			}
			Send_Command();
			if (abort_following_commands) 
			{ 
				abort_following_commands = 0;
				return;
			}
		}
	}



	void Set_Pose_Rotate(bool left_or_right, double final_roll, double final_pitch, double final_yaw, matrix<double> old_frame, double angle_step_length = 0.1)
		// the input is final roll, pitch and yaw (in degree)
	{

		final_roll = final_roll/DEG_PER_RAD;
		final_pitch = final_pitch/DEG_PER_RAD;
		final_yaw = final_yaw/DEG_PER_RAD;

		double old_roll = atan2(old_frame(2,1),old_frame(2,2));
		double old_pitch = atan2(-old_frame(2,0),sqrt(pow(old_frame(0,0),2)+pow(old_frame(1,0),2)));
		double old_yaw = atan2(old_frame(1,0),old_frame(0,0));

		
		int N_roll, N_pitch, N_yaw, N;
		N_roll = static_cast<int> (abs(final_roll-old_roll)*DEG_PER_RAD/angle_step_length);
		N_pitch = static_cast<int> (abs(final_pitch-old_pitch)*DEG_PER_RAD/angle_step_length);
		N_yaw = static_cast<int> (abs(final_yaw-old_yaw)*DEG_PER_RAD/angle_step_length);

		N = (N_roll>N_pitch)?N_roll:N_pitch;
		N = (N>N_yaw)?N:N_yaw;

		//std::cout<<"Takes "<<N<<" increments to finish the rotation"<<std::endl;
		double step_roll, step_pitch, step_yaw;
		step_roll = (final_roll-old_roll)/N;
		step_pitch = (final_pitch-old_pitch)/N;
		step_yaw = (final_yaw-old_yaw)/N;


		matrix<double> new_frame_roll(3,3);
		matrix<double> new_frame_pitch(3,3);
		matrix<double> new_frame_yaw(3,3);
		matrix<double> new_rotation(3,3);
		matrix<double> new_frame(4,4);

		for (int i = 0;i < N;i++)
		{
			new_frame_roll(0,0) = 1; 
			new_frame_roll(0,1) = 0;
			new_frame_roll(0,2) = 0; 
			new_frame_roll(1,0) = 0; 
			new_frame_roll(1,1) = cos(old_roll + step_roll*i); 
			new_frame_roll(1,2) = -sin(old_roll + step_roll*i); 
			new_frame_roll(2,0) = 0; 
			new_frame_roll(2,1) = sin(old_roll + step_roll*i); 
			new_frame_roll(2,2) = cos(old_roll + step_roll*i); 


			new_frame_pitch(0,0) = cos(old_pitch + step_pitch*i); 
			new_frame_pitch(0,1) = 0;
			new_frame_pitch(0,2) = sin(old_pitch + step_pitch*i); 
			new_frame_pitch(1,0) = 0; 
			new_frame_pitch(1,1) = 1; 
			new_frame_pitch(1,2) = 0; 
			new_frame_pitch(2,0) = -sin(old_pitch + step_pitch*i); 
			new_frame_pitch(2,1) = 0; 
			new_frame_pitch(2,2) = cos(old_pitch + step_pitch*i); 

			new_frame_yaw(0,0) = cos(old_yaw + step_yaw*i); 
			new_frame_yaw(0,1) = -sin(old_yaw + step_yaw*i);
			new_frame_yaw(0,2) = 0; 
			new_frame_yaw(1,0) = sin(old_yaw + step_yaw*i); 
			new_frame_yaw(1,1) = cos(old_yaw + step_yaw*i); 
			new_frame_yaw(1,2) = 0; 
			new_frame_yaw(2,0) = 0; 
			new_frame_yaw(2,1) = 0; 
			new_frame_yaw(2,2) = 1; 

			new_rotation = prod(new_frame_yaw, new_frame_pitch);
			new_rotation = prod(new_rotation, new_frame_roll);

			for (size_t ii = 0; ii < 3; ii++) {
				for (size_t jj = 0; jj < 3; jj++) {
					new_frame(ii,jj) = new_rotation(ii,jj)*MEGA;
				}
			}

			new_frame(3,0) = 0; 
			new_frame(3,1) = 0; 
			new_frame(3,2) = 0; 
			new_frame(3,3) = 1; 

			new_frame(0,3) = old_frame(0,3); 
			new_frame(1,3) = old_frame(1,3); 
			new_frame(2,3) = old_frame(2,3); 

			//std::cout<< old_frame << std::endl;
			//std::cout<< new_frame_roll << std::endl;
			//std::cout<< new_frame_pitch << std::endl;
			//std::cout<< new_frame_yaw << std::endl;
			//std::cout<< new_rotation << std::endl;
			//std::cout<< new_frame << std::endl;

			for (size_t iii = 0; iii < 4; iii++) {
				for (size_t jjj = 0; jjj < 4; jjj++) {
					if (left_or_right == 0)
						*(double*)&buffer_R1[48 + 32*iii + 8*jjj] = new_frame(iii,jjj);
					else
						*(double*)&buffer_R1[184 + 32*iii + 8*jjj] = new_frame(iii,jjj);
				}
			}
			Send_Command();
			if (abort_following_commands) 
			{ 
				abort_following_commands = 0;
				return;
			}
		}

	}


	bnu::matrix<double> Construct_Rotation_from_rpy(double p_roll, double p_pitch, double p_yaw)
	{
		matrix <double> Composit_Rotation(3,3);
		matrix <double> Single_Rotation_Roll(3,3);
		matrix <double> Single_Rotation_Pitch(3,3);
		matrix <double> Single_Rotation_Yaw(3,3);

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

		Composit_Rotation = prod(Single_Rotation_Yaw,Single_Rotation_Pitch);
		Composit_Rotation = prod(Composit_Rotation, Single_Rotation_Roll);
		return Composit_Rotation;
	}

	void register_Omega_initial(Omega* omega_obj, Point3D & left, Point3D &right, matrix <double> rotation_init[]) // register the initial control position of Omega
	{
		dhdEnableForce (DHD_OFF); // disable the force for Omega to return to starting position
		double sum_x[2] = {0,0};
		double sum_y[2] = {0,0};
		double sum_z[2] = {0,0};
		double sum_roll[2] = {0,0};
		double sum_pitch[2] = {0,0};
		double sum_yaw[2] = {0,0};
		Point3D Omega_report_position[2];
		Point3D Omega_report_rotation[2];
		for (size_t i = 0; i < 10; i++)
		{
			omega_obj->update_position();
			for (char arm = 0; arm < 2; arm ++ )
			{
				
				Omega_report_position[arm] = omega_obj->get_position(arm);
				Omega_report_rotation[arm] = omega_obj->get_rpy(arm);
				sum_x[arm] += Omega_report_position[arm].x;
				sum_y[arm] += Omega_report_position[arm].y;
				sum_z[arm] += Omega_report_position[arm].z;
				sum_roll[arm] += Omega_report_rotation[arm].x;
				sum_pitch[arm] += Omega_report_rotation[arm].y;
				sum_yaw[arm] += Omega_report_rotation[arm].z;

			}

		}
		left.set_point(sum_x[0]/10, sum_y[0]/10, sum_z[0]/10);
		right.set_point(sum_x[1]/10, sum_y[1]/10, sum_z[1]/10);

		// here in get the init rotation, we have to have a coordinate mapping here, too.
		rotation_init[0] =  Construct_Rotation_from_rpy(-sum_yaw[0]/10, -sum_pitch[0]/10, -sum_roll[0]/10);
		rotation_init[1] =  Construct_Rotation_from_rpy(-sum_yaw[1]/10, -sum_pitch[1]/10, -sum_roll[1]/10);
	}

	bool register_Leap_initial(LeapCoordinates *_obj, std::vector<Point3D> &leap_init, std::vector<Point3D> &leap_init_rotation) // register the initial control position of Omega
	{
		double sum_x = 0;
		double sum_y = 0;
		double sum_z = 0;
		double sum_roll = 0;
		double sum_pitch = 0;
		double sum_yaw = 0;
		leap_init.resize(2);
		leap_init_rotation.resize(2);
		for (size_t j = 0; j<2 ; j++)
		{
			for (size_t i = 0; i < 10; i++)
			{
				_obj->calcParam();
				sum_x += _obj->x[j];
				sum_y += _obj->y[j];
				sum_z += _obj->z[j];
				sum_roll += _obj->roll[j];
				sum_pitch += _obj->pitch[j];
				sum_yaw += _obj->yaw[j];
			}

			leap_init[j].set_point(sum_x/10, sum_y/10, sum_z/10);
			leap_init_rotation[j].set_point(sum_roll/10, sum_pitch/10, sum_yaw/10);
			sum_x = 0; sum_y = 0; sum_z = 0;
			sum_roll = 0; sum_pitch = 0; sum_yaw = 0;
		}
		return 1;
	}


	bnu::matrix<double> update_reference(bool left_or_right) // update the reference used for rotation
	{
		matrix <double> reference(4,4);
		// get the first reference T matrix
		pthread_mutex_lock(&mutexHaptic);
		for (unsigned i = 0;i<reference.size1();++i)
			for (unsigned j = 0; j<reference.size2();j++)
				if (i<3 && j<3)
					reference(i,j) = haptic_packet.TipPose[left_or_right][i][j]/MEGA;
				else
					reference(i,j) = haptic_packet.TipPose[left_or_right][i][j];
		pthread_mutex_unlock(&mutexHaptic);
		return reference;
	}

	bnu::matrix <double> get_haptic_packet(bool _left_or_right)
	{
		bnu::matrix <double> old_frame(4,4);
		pthread_mutex_lock(&mutexHaptic);
		//haptic_packet.DisplayFrame();
		for (unsigned i = 0;i<old_frame.size1();++i)
			for (unsigned j = 0; j<old_frame.size2();j++)
				old_frame(i,j) = haptic_packet.TipPose[_left_or_right][i][j];
		pthread_mutex_unlock(&mutexHaptic);
		return old_frame;
	}

	bnu::matrix <double> get_pose_packet(bool _left_or_right)
	{
		bnu::matrix <double> old_frame(4,4);
		//haptic_packet.DisplayFrame();
		for (unsigned i = 0;i<old_frame.size1();++i)
			for (unsigned j = 0; j<old_frame.size2();j++)
				old_frame(i,j) = pose_packet.TipPose[_left_or_right][i][j];
		return old_frame;
	}

	double discrete_translation_control_from_kinect ( double mag ) // speed from 25,50,75,100. In total 8 levels. mag should range from 0 to 200
	{
		double speed;
		mag = abs(mag);
		if (mag == 0)
			speed = 0;
		else if (mag > 0 && mag < 100)
			speed = 40;
		else if (mag < 250)
			speed = 80;
		else 
			speed = 200; // the fastest movement mode. It only happens when the user really pushes to that direction. 

		//if (mag > 0 && mag < 50)
		//	speed = 25;
		//else if (mag < 100)
		//	speed = 50;
		//else if (mag < 150)
		//	speed = 75;
		//else
		//	speed = 100;
		return speed;
	}

	double discrete_rotation_control_from_kinect ( double mag ) // speed from 25,50,75,100. In total 8 levels. mag should range from 0.25 - 1
	{
		double speed;
		mag = abs(mag);
		if (mag > 0 && mag < 50)
			speed = 0.25;
		else if (mag < 100)
			speed = 0.5;
		else if (mag < 150)
			speed = 0.75;
		else
			speed = 1;
		return speed;
	}

	void Terminate(void)
	{
		SPHINX::release();
		closesocket(sd_M);
		closesocket(sd_A);
		pthread_exit(NULL);
		std::free(buffer_R1);	
	}


	void Read_Camera_Packet(void)
	{
		std::string string_to_hold_buffer(buffer_R1, BUFFER_SIZE);
		istringstream infile (string_to_hold_buffer, istringstream::in);
		camera_packet.Read(infile);
		//camera_packet.display();
	}

	void Read_Pose_Packet(void)
	{
		std::string string_to_hold_buffer(buffer_R1, BUFFER_SIZE);
		istringstream infile (string_to_hold_buffer, istringstream::in);
		pose_packet.Read(infile);
		//std::cout<<pose_packet<<std::endl;
	}

};


int run_sphinx(int& word)
{
	// variable declaration for speech recognition
	int *bStart;
	double threshold; 
	bool sphinx_init;
	string HMM;
	string LM;
	string DICT;
	string S;


	// variable initialization for speech recognition
	bStart = new int;
	*bStart = 1;
	threshold = 50;
	HMM = ".\\HeaderFile\\pocketsphinx\\model\\hmm\\en_US\\hub4wsj_sc_8k";
	LM = ".\\HeaderFile\\1.lm";
	DICT = ".\\HeaderFile\\1.dic";
	sphinx_init = SPHINX::init(HMM.c_str(), LM.c_str(), DICT.c_str());

	while(1)
	{
		////////// Speeach recognition
		if(SPHINX::utterance(bStart) == false) // the actual recognition part
			return -1;
		if(SPHINX::probability > threshold){ 
			S = SPHINX::hypothesis;
			std::cout<<"Speech: "<<S<<", condifence: "<<SPHINX::probability<<std::endl;
			// Matching to directions
			if (S.compare("OPEN") == 0){
				//print_star();
				//printf("OPEN\n");
				//print_star();
				word = 0;
			}
			else if (S.compare("CLOSE") == 0){
				//print_star();
				//printf("CLOSE\n");
				//print_star();
				word = 1;
			}
			//else if (S.compare("HALF") == 0){
			//	//print_star();
			//	//printf("HALF\n");
			//	//print_star();
			//	word = 2;
			//}
			else
				word = -1;

	
		}
	}
	return 1;
}

#endif