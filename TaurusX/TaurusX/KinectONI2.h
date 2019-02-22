#ifndef __KINECT_ONI2_H_
#define __KINECT_ONI2_H_

// Header Includes
#include <opencv2/core/core.hpp>
#pragma comment(lib,"opencv_core248d.lib")

#include <opencv2/highgui/highgui.hpp>
#pragma comment(lib,"opencv_highgui248d.lib")

#include <OpenNI.h>
#pragma comment(lib,"openni2.lib")

#include <NiTE.h>
#pragma comment(lib,"nite2.lib")
#include <iostream>
#include <vector>

using namespace std;

class KINECT
{
public:
	openni::Device device;
	openni::VideoStream color;
	openni::VideoStream depth;
	openni::Recorder rec;
	openni::PlaybackControl *player;
	string oni_file;
	int frame_index;
	bool started;
	// NiTE
	nite::UserTracker user_tracker;
	nite::UserData user;
	std::vector<nite::SkeletonState> g_skeletonStates;
	std::vector<bool> g_visibleUsers;
	std::vector<nite::SkeletonJoint> skel3;
	std::vector<nite::SkeletonJoint> skel2;
	bool skeleton;
	// Frame Information Reference
	openni::VideoFrameRef colorFrame;
	openni::VideoFrameRef depthFrame;
	int minColorValue;
	int maxColorValue;
	int minDepthValue;
	int maxDepthValue;
	int dImgWidth;
	int dImgHeight;
	int cImgWidth; 
	int cImgHeight;
	// Mat objects
	cv::Mat D;
	cv::Mat C;
	// Misc
	bool good;
	enum MODE{LIVE,ONI,RECORD,SEEK,STREAM};
	MODE mode;
	KINECT(MODE m, string oni = string()):mode(m),oni_file(oni),started(false)
	{
	}
	~KINECT()
	{
		cout<<"Shutting down Kinect ..."<<endl;
		if(rec.isValid()) 
		{
			std::cout<<"Releasing recorder"<<std::endl;
			rec.stop();
			rec.destroy();
		}
		// Destroy Streams
		if(skeleton) user_tracker.destroy();
		color.destroy();
		depth.destroy();
		// Close Device
		device.close();
		shutdown();
	}
	bool init(const int id = 0)
	{
		skeleton = false;
		string device_uri;
		// Initialize OpenNI Module
		openni::Status ret = openni::OpenNI::initialize();
		std::cout << "OpenNI Initialization: " << openni::OpenNI::getExtendedError() << std::endl;
		if(mode == LIVE || mode == RECORD)
		{
			// Device Counts & Informations
			openni::Array< openni::DeviceInfo > devicesInfo;
			openni::OpenNI::enumerateDevices( &devicesInfo );
			std::cout << "Device Counts: " << devicesInfo.getSize() << std::endl;
			for ( int i = 0 ; i < devicesInfo.getSize() ; i++ )
			{
				std::cout << "Device Info [ " << i << " ] : " << devicesInfo[i].getUri() << std::endl;
			}
			ret = device.open(devicesInfo[id].getUri()); 
		}
		else if(mode == ONI)
		{
			if(oni_file.empty()) 
			{
				std::cout<<"ONI file not specified!"<<std::endl;
				return false;
			}
			else ret = device.open(oni_file.c_str()); 
		}
		// Set up recorder if necessary
		if(mode == RECORD) 
		{
			if(oni_file.empty()) 
			{
				std::cout<<"Recording file not specified!"<<std::endl;
				return false;
			}
			else
			{
				// Create
				std::cout<<"Opening "<<oni_file<<std::endl;
				if(rec.create(oni_file.c_str()) != openni::STATUS_OK) 
				{
					std::cout<<"Failed to open "<<oni_file<<" for writing!"<<std::endl;
					return false;
				}
			}
		}
		// Init color & depth
		if(ret == openni::STATUS_OK)
		{			
			if(!init_color()) return false;
			if(!init_depth()) return false;
			//ret = device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
			//std::cout<<"Color/Depth Registered: "<<(ret == openni::STATUS_OK ? "[OK]" : "[FAIL]")<<std::endl;
			ret = device.setDepthColorSyncEnabled(true);
			std::cout<<"Color/Depth Sync: "<<(ret == openni::STATUS_OK ? "[OK]" : "[FAIL]")<<std::endl;
			good = true;
		}
		else 		
		{
			std::cout << "Device Open Failed" << std::endl;
			shutdown();
			good = false;
		}
		// Set up playback control
		if(mode == ONI)
		{
			player = device.getPlaybackControl();
			if(player == NULL) 
			{
				cout<<"Failed to create playback control"<<endl;
				good = false;
			}
			else
			{
				ret = player->setRepeatEnabled(true);
				std::cout<<"Loop video"<<(ret == openni::STATUS_OK ? "[OK]" : "[FAIL]")<<std::endl;
				ret = player->setSpeed(-1);
				std::cout<<"Manual frame read"<<(ret == openni::STATUS_OK ? "[OK]" : "[FAIL]")<<std::endl;
				std::cout<<"# frames = "<<player->getNumberOfFrames(depth)<<std::endl;
				frame_index = 1;
			}
		}
		// Ready
		return good;
	}
	bool start_streams()
	{
		if(color.start() != openni::STATUS_OK || depth.start() != openni::STATUS_OK)
		{
			std::cout<<"Failed to start color & depth streams!"<<std::endl;
			return false;
		}
		// Start the recorder (already been attached in init_color & init_depth)
		if(mode == RECORD)
		{
			// Record
			if(rec.start() != openni::STATUS_OK) 
			{
				std::cout<<"Failed to start recorder!"<<std::endl;
				return false;
			}
			else return true;
		}
		return true;
	}
	bool init_skeleton()
	{
		nite::NiTE::initialize();		
		g_visibleUsers.resize(10);
		g_skeletonStates.resize(10);
		if (user_tracker.create(&device) != nite::STATUS_OK) skeleton = false;
		else skeleton = true;
		return skeleton;
	}
	// Legacy
	bool IsEOF()
	{
		return false;
	}
	bool isGood()
	{
		return good;
	}
	void shutdown()
	{
		// Shutdown
		if(skeleton) nite::NiTE::shutdown();
		openni::OpenNI::shutdown();
		good = false;
	}

	bool init_color()
	{
		// Create Color Image
		openni::Status ret = color.create( device, openni::SENSOR_COLOR );
		if ( ret == openni::STATUS_OK )
		{
			if(mode == RECORD) 
			{
				// Color
				std::cout<<"Attaching to color ..."<<std::endl;
				if(rec.attach(color) != openni::STATUS_OK) return false;
			} 
		}
		// Check Valid State
		if ( !color.isValid() )
		{
			std::cout << "Color image invalid" << std::endl;
			return false;
		}
		// Get Color Stream Min-Max Value
		minColorValue = color.getMinPixelValue();
		maxColorValue = color.getMaxPixelValue();
		std::cout << "Color min-Max Value : " << minColorValue << "-" << maxColorValue << std::endl;
		// Get Sensor Resolution Information
		cImgWidth = color.getVideoMode().getResolutionX();
		cImgHeight = color.getVideoMode().getResolutionY();
		std::cout << "Color Resolution : " << cImgWidth << "x" << cImgHeight << std::endl;
		// Color Image & Depth Image Matrix
		C = cv::Mat( cImgHeight, cImgWidth, CV_8UC3 );
		// Get FPS Information
		std::cout << "Color : " << color.getVideoMode().getFps() << "(fps)"<<std::endl;
		return true;
	}
	bool init_depth()
	{
		// Create Depth Image
		openni::Status ret = depth.create( device, openni::SENSOR_DEPTH );
		if ( ret == openni::STATUS_OK )
		{
			if(mode == RECORD) 
			{
				// Depth
				std::cout<<"Attaching to depth ..."<<std::endl;
				if(rec.attach(depth) != openni::STATUS_OK) return false;
			}
		}
		// Check Valid State
		if ( !color.isValid() )
		{
			std::cout << "Color image invalid" << std::endl;
			return false;
		}
		// Get Depth Stream Min-Max Value
		minDepthValue = depth.getMinPixelValue();
		maxDepthValue = depth.getMaxPixelValue();
		std::cout << "Depth min-Max Value : " << minDepthValue << "-" << maxDepthValue << std::endl;
		// Get Sensor Resolution Information
		dImgWidth = depth.getVideoMode().getResolutionX();
		dImgHeight = depth.getVideoMode().getResolutionY();
		std::cout << "Depth Resolution : " << dImgWidth << "x" << dImgHeight << std::endl;
		//Depth Image Matrix		
		D = cv::Mat( dImgHeight, dImgWidth, CV_16U );
		// Get FPS info
		std::cout<<"Depth : " << depth.getVideoMode().getFps() << "(fps)" << std::endl;
		return true;
	}
	void offset_frame_index(int f)
	{
		if(mode == KINECT::ONI)
		{			
			if(f < 0) frame_index = max(0,frame_index + f);
			else if(player->getNumberOfFrames(depth)==-1) frame_index +=f;
			else frame_index = min(player->getNumberOfFrames(color),frame_index + f);
		}
		else 
		{
			cout<<"Only available in playback mode!"<<endl;
			exit(0);
		}
	}
	bool pull()
	{
		if(!isGood()) return false;
		if(started == false)
		{
			if(!start_streams())
			{
				cout<<"Failed to start streams!"<<endl;
				return false;
			}
			started = true;
		}
		// Read a Frame from VideoStream
		if(mode == KINECT::ONI)
		{			
			if(frame_index == player->getNumberOfFrames(depth)) return false;
			openni::Status rc = player->seek(depth,frame_index);
			if((rc == openni::STATUS_NOT_IMPLEMENTED) || (rc == openni::STATUS_NOT_SUPPORTED) || (rc == openni::STATUS_BAD_PARAMETER) || (rc == openni::STATUS_NO_DEVICE))
			{
				cout<<"Seeking not supported!"<<endl;
			}		
			offset_frame_index(1);
		}		
		color.readFrame( &colorFrame );
		depth.readFrame( &depthFrame );
		return true;
	}	

	bool refresh()
	{
		if(!pull()) return false;
		// Copy To Mat
		openni::RGB888Pixel* colorImgRaw = (openni::RGB888Pixel*)colorFrame.getData();
		for ( size_t i = 0 ; i < ( colorFrame.getDataSize() / sizeof( openni::RGB888Pixel ) ) ; i++ )
		{
			size_t idx = i * 3; // cv::Mat is BGR
			unsigned char* data = &C.data[idx];
			data[0] = (unsigned char)colorImgRaw[i].b;
			data[1] = (unsigned char)colorImgRaw[i].g;
			data[2] = (unsigned char)colorImgRaw[i].r;
		}
		openni::DepthPixel* depthImgRaw = (openni::DepthPixel*)depthFrame.getData();
		for (int i = 0 ; i < D.rows ; i++ )
		{	
			for(int j = 0; j < D.cols; ++j)
			{
				D.at<openni::DepthPixel>(i,j) = depthImgRaw[i*D.cols+j];
			}
		}
		if(skeleton) getSkeleton();
		return true;
	}	

	void USER_MESSAGE(const char *msg)
	{
		std::cout<<msg<<std::endl;
	}
#define PRIu64 "llu"

	void updateUserState(const nite::UserData& user, uint64_t ts)
	{
		if (user.isNew())
		{
			USER_MESSAGE("New");
		}
		else if (user.isVisible() && !g_visibleUsers[user.getId()])
			printf("[%08" PRIu64 "] User #%d:\tVisible\n", ts, user.getId());
		else if (!user.isVisible() && g_visibleUsers[user.getId()])
			printf("[%08" PRIu64 "] User #%d:\tOut of Scene\n", ts, user.getId());
		else if (user.isLost())
		{
			USER_MESSAGE("Lost");
		}
		g_visibleUsers[user.getId()] = user.isVisible();


		if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
		{
			switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
			{
			case nite::SKELETON_NONE:
				USER_MESSAGE("Stopped tracking.");
				break;
			case nite::SKELETON_CALIBRATING:
				USER_MESSAGE("Calibrating...");
				break;
			case nite::SKELETON_TRACKED:
				USER_MESSAGE("Tracking!");
				break;
			case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
			case nite::SKELETON_CALIBRATION_ERROR_HANDS:
			case nite::SKELETON_CALIBRATION_ERROR_LEGS:
			case nite::SKELETON_CALIBRATION_ERROR_HEAD:
			case nite::SKELETON_CALIBRATION_ERROR_TORSO:
				USER_MESSAGE("Calibration Failed... :-|");
				break;
			}
		}
	}

	void getSkeleton()
	{
		nite::UserTrackerFrameRef userTrackerFrame;
		openni::VideoFrameRef depthFrame;
		nite::Status rc = user_tracker.readFrame(&userTrackerFrame);
		if (rc != nite::STATUS_OK)
		{
			printf("GetNextData failed\n");
			return;
		}
		depthFrame = userTrackerFrame.getDepthFrame();
		const nite::UserMap& userLabels = userTrackerFrame.getUserMap();
		const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
		for (int i = 0; i < users.getSize(); ++i)
		{
			const nite::UserData& user = users[i];

			updateUserState(user, userTrackerFrame.getTimestamp());
			if (user.isNew())
			{
				user_tracker.startSkeletonTracking(user.getId());
				user_tracker.startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
			}
			else if (!user.isLost())
			{
				if (users[i].getSkeleton().getState() == nite::SKELETON_TRACKED)
				{
					DrawSkeleton(&user_tracker,user);
				}
			}
		}
	}

	void DrawLimb(nite::UserTracker* pUserTracker, const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2, int color)
	{
		cv::Point2f j[2];
		pUserTracker->convertJointCoordinatesToDepth(joint1.getPosition().x, joint1.getPosition().y, joint1.getPosition().z, &j[0].x, &j[0].y);
		pUserTracker->convertJointCoordinatesToDepth(joint2.getPosition().x, joint2.getPosition().y, joint2.getPosition().z, &j[1].x, &j[1].y);
		for(int i = 0; i < 2; ++i) cv::circle(C,j[i],5,cv::Scalar(255,255,255));
		cv::line(C,j[0],j[1],cv::Scalar(0,0,255),5);
	}

	void DrawSkeleton(nite::UserTracker* pUserTracker, const nite::UserData& userData)
	{
		const int colorCount = 3;
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_HEAD), userData.getSkeleton().getJoint(nite::JOINT_NECK), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HAND), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getId() % colorCount);


		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_LEFT_FOOT), userData.getId() % colorCount);

		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE), userData.getId() % colorCount);
		DrawLimb(pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT), userData.getId() % colorCount);
	}
};

#endif