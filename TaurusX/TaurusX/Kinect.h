#ifndef __KINECT_H__
#define __KINECT_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <pthread.h>
#include <iostream>
// for Kinect
#include "NiTE.h"
#include <OpenNI.h>

#define MAX_USERS 1
#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

#define PI 3.14159
//#define LB_X 390
//#define LB_Y 225
//#define UB_X 515
//#define UB_Y 350

class Kinect
{

public:
	typedef cv::Point_<double> Point2d;
	typedef cv::Point_<float> Point2f;
	// member variable
	void getGestureEvent(bool& registered_flag);
	bool g_visibleUsers[MAX_USERS]; 
	nite::SkeletonState g_skeletonStates[MAX_USERS];
	const nite::Skeleton* control_skeleton;
	bool Kinect_tracking_flag;
	bool Stop_tracking;
	std::vector<const nite::SkeletonJoint*> hand;
	void User_tracking_Kinect();
	// Ctor
	Kinect()
	{
		g_visibleUsers[MAX_USERS] = false;
		g_skeletonStates[MAX_USERS] = nite::SKELETON_NONE;
		Stop_tracking = false;
		hand.resize(2);
	}

	// member function
	void updateUserState(const nite::UserData& user, unsigned long long ts);

	// the wrapper for pthread
	//static void* StaticThreadProc(void *arg)
	//{
	//	return reinterpret_cast<Kinect*>(arg)->User_tracking_Kinect();
	//}

private:
    openni::Device mDevice;
    openni::VideoStream mDepthStream;
    openni::VideoMode mDMode;
    openni::VideoStream mColorStream;
    openni::VideoMode mCMode;
	std::string recog;

};

#endif