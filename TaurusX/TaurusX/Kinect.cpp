#include "Kinect.h"
#include <conio.h>

using namespace openni;
using namespace nite;
using namespace cv;

void Kinect::updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
	USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
	USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
	USER_MESSAGE("Out of Scene")
	else if (user.isLost())
	USER_MESSAGE("Lost")

	g_visibleUsers[user.getId()] = user.isVisible();


	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")

				break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
				break;
		case nite::SKELETON_TRACKED:
			//USER_MESSAGE("Tracking!") // I don't want to have so many Tracking messages displayed here
				break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
				break;
		}
	}
}

extern pthread_mutex_t mutex_User_tracking; // this is defined in the TarusX file, so we need to specify that it's an extern variable

void Kinect::User_tracking_Kinect()
{
	Kinect_tracking_flag = false;
	printf("Start the User tracking thread \n" );
	nite::UserTracker userTracker;
	nite::Status niteRc;

	nite::NiTE::initialize();

	cv::Mat cImageBGR;

	niteRc = userTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
	}
	printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");

	nite::UserTrackerFrameRef userTrackerFrame;

	while (!Stop_tracking)
	{
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}

		const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
		for (int i = 0; i < min(1, users.getSize()); ++i)
		{
			const nite::UserData& user = users[i];
			updateUserState(user,userTrackerFrame.getTimestamp());
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
				//registered_flag = false;
			}
			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				pthread_mutex_lock(&mutex_User_tracking);
				control_skeleton = &user.getSkeleton();
				hand[0] = &control_skeleton->getJoint(nite::JOINT_LEFT_HAND);
				hand[1] = &control_skeleton->getJoint(nite::JOINT_RIGHT_HAND);
				if (abs(hand[0]->getPosition().x) > 10000 || abs(hand[1]->getPosition().x) > 10000)
					Kinect_tracking_flag = false;
				else
				{ 
					//std::cout<<hand[0]->getPosition().x<<', '<<hand[0]->getPosition().y<<', '<<hand[0]->getPosition().z<<std::endl;
					//std::cout<<hand[1]->getPosition().x<<', '<<hand[1]->getPosition().y<<', '<<hand[1]->getPosition().z<<std::endl<<std::endl;
					Kinect_tracking_flag = true;
				}
				pthread_mutex_unlock(&mutex_User_tracking);
				
			}
		}

	}
	pthread_mutex_unlock(&mutex_User_tracking);
	nite::NiTE::shutdown();
	pthread_exit(NULL);
}

void Kinect::getGestureEvent(bool& registered_flag){

	Kinect_tracking_flag = false;
	printf("Start the User tracking thread \n" );

    std::vector <Kinect::Point2f>  pt, pt1;
    double theta = 0;


    OpenNI::initialize();
    mDevice.open(openni::ANY_DEVICE);
    // o4. create depth stream
    mDepthStream.create(mDevice, SENSOR_DEPTH);
    // o4a. set video mode
    mDMode.setResolution(640,480);
    mDMode.setFps(30);
    mDMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
    mDepthStream.setVideoMode(mDMode);

    // o5. Create color stream
    mColorStream.create( mDevice, SENSOR_COLOR );
    // o5a. set video mode
    mCMode.setResolution(640,480);
    mCMode.setFps(30);
    mCMode.setPixelFormat(PIXEL_FORMAT_RGB888);
    mColorStream.setVideoMode(mCMode);

    // o6. image registration
    mDevice.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);

    // create OpenCV Window
    cv::namedWindow("User Image", CV_WINDOW_AUTOSIZE);

    // Set the recorder

    //**************************************************
    //openni::Recorder mRecorder;
    //std::string recorder_path = "C:\\Starry\\ONI\\";
    //std::string recorder_end = ".oni";
    //std::string recorder_name = "10";
    //std::string recorder_full = recorder_path + recorder_name + recorder_end;
    //mRecorder.create(recorder_full.c_str());

    //if (mRecorder.isValid())
    //{
    //   std::cout<<"The recorder is valid!"<<std::endl;
    //}
    //mRecorder.attach(mDepthStream);
    //mRecorder.attach(mColorStream);

    // p1. start
    mColorStream.start();
    mDepthStream.start();
    //mRecorder.start();
    mDevice.setDepthColorSyncEnabled(TRUE);

    // n2. Initial NiTE
    NiTE::initialize();

    // n3. create user tracker
    UserTracker mUserTracker;
    mUserTracker.create(&mDevice);
    mUserTracker.setSkeletonSmoothingFactor(0.1f);
    cv::Point2f aPoint[15];

    while(!Stop_tracking){
        // p2. prepare background
        cv::Mat cImageBGR;
        // p2a. get color frame
        VideoFrameRef mColorFrame;
        mColorStream.readFrame(&mColorFrame);

        // p2b. convert data to OpenCV format
        const cv::Mat mImageRGB(mColorFrame.getHeight(), mColorFrame.getWidth(),
                                CV_8UC3, (void*)mColorFrame.getData());
        // p2c. convert form RGB to BGR
        cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);

        // p3. get user frame
        UserTrackerFrameRef mUserFrame;
        mUserTracker.readFrame(&mUserFrame);

        // p4. get users data
        const nite::Array<UserData>& aUsers = mUserFrame.getUsers();
        nite::SkeletonJoint aJoints[15];
		
        for(int i = 0; i < min(1, aUsers.getSize()); ++i){
            const UserData& rUser = aUsers[i];

			updateUserState(rUser,mUserFrame.getTimestamp());
			if (rUser.isNew())
			{
				mUserTracker.startSkeletonTracking(rUser.getId());
				//registered_flag = false;
			}
			else if (rUser.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				pthread_mutex_lock(&mutex_User_tracking);
				control_skeleton = &rUser.getSkeleton();
				hand[0] = &control_skeleton->getJoint(nite::JOINT_LEFT_HAND);
				hand[1] = &control_skeleton->getJoint(nite::JOINT_RIGHT_HAND);
				if (abs(hand[0]->getPosition().x) > 10000 || abs(hand[1]->getPosition().x) > 10000)
					Kinect_tracking_flag = false;
				else
				{ 
					//std::cout<<hand[0]->getPosition().x<<', '<<hand[0]->getPosition().y<<', '<<hand[0]->getPosition().z<<std::endl;
					//std::cout<<hand[1]->getPosition().x<<', '<<hand[1]->getPosition().y<<', '<<hand[1]->getPosition().z<<std::endl<<std::endl;
					Kinect_tracking_flag = true;
				}
				pthread_mutex_unlock(&mutex_User_tracking);
				
			}



            // p4a. check user status
            if(rUser.isNew()){
                //start tracking for new user
                mUserTracker.startSkeletonTracking(rUser.getId());
            }
            if(rUser.isVisible()){
                // p4b. get user skeleton
                const Skeleton& rSkeleton = rUser.getSkeleton();
                if(rSkeleton.getState() == SKELETON_TRACKED){
                    // p4c. build joints array

                    aJoints[ 0] = rSkeleton.getJoint( JOINT_HEAD );
                    aJoints[ 1] = rSkeleton.getJoint( JOINT_NECK );
                    aJoints[ 2] = rSkeleton.getJoint( JOINT_LEFT_SHOULDER );
                    aJoints[ 3] = rSkeleton.getJoint( JOINT_RIGHT_SHOULDER );
                    aJoints[ 4] = rSkeleton.getJoint( JOINT_LEFT_ELBOW );
                    aJoints[ 5] = rSkeleton.getJoint( JOINT_RIGHT_ELBOW );
                    aJoints[ 6] = rSkeleton.getJoint( JOINT_LEFT_HAND );
                    aJoints[ 7] = rSkeleton.getJoint( JOINT_RIGHT_HAND );
                    aJoints[ 8] = rSkeleton.getJoint( JOINT_TORSO );
                    aJoints[ 9] = rSkeleton.getJoint( JOINT_LEFT_HIP );
                    aJoints[10] = rSkeleton.getJoint( JOINT_RIGHT_HIP );
                    aJoints[11] = rSkeleton.getJoint( JOINT_LEFT_KNEE );
                    aJoints[12] = rSkeleton.getJoint( JOINT_RIGHT_KNEE );
                    aJoints[13] = rSkeleton.getJoint( JOINT_LEFT_FOOT );
                    aJoints[14] = rSkeleton.getJoint( JOINT_RIGHT_FOOT );

                    // p4d. convert joint position to image


                    for(int s = 0; s < 15; ++s){
                        const nite::Point3f& rPos = aJoints[s].getPosition();
                        mUserTracker.convertJointCoordinatesToDepth(rPos.x, rPos.y, rPos.z, &(aPoint[s].x), &(aPoint[s].y));
                    }

                    //p4e. draw line
                    cv::line( cImageBGR, aPoint[ 0], aPoint[ 1], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 1], aPoint[ 2], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 1], aPoint[ 3], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 2], aPoint[ 4], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 3], aPoint[ 5], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 4], aPoint[ 6], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 5], aPoint[ 7], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 1], aPoint[ 8], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 8], aPoint[ 9], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 8], aPoint[10], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[ 9], aPoint[11], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[10], aPoint[12], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[11], aPoint[13], cv::Scalar( 255, 0, 0 ), 3 );
                    cv::line( cImageBGR, aPoint[12], aPoint[14], cv::Scalar( 255, 0, 0 ), 3 );

                    // p4f. draw joint

                    for( int  s = 0; s < 15; ++ s )
                    {
                        if( aJoints[s].getPositionConfidence() > 0.5 )
                            cv::circle( cImageBGR, aPoint[s], 3, cv::Scalar( 0, 255, 0 ), 2 ); // green dot means good trakcing
                        else
                            cv::circle( cImageBGR, aPoint[s], 3, cv::Scalar( 0, 0, 255 ), 2 ); // red dot means bad trakcing
                    }

      //              if (aPoint[7].x > LB_X && aPoint[7].x < UB_X && aPoint[7].y > LB_Y && aPoint[7].y < UB_Y && aJoints[7].getPositionConfidence() > 0.5)
						//pt1.push_back (Kinect::Point2f(aPoint[7].x, aPoint[7].y));
      //              if (pt1.size() > 100){
      //                  if (pt1.size() < 200)
      //                      cv::putText(cImageBGR, "Ready", cv::Point(50,50),CV_FONT_HERSHEY_SIMPLEX ,2,cv::Scalar(255,255,0),2,8);
      //                  if(aPoint[7].x < LB_X || aPoint[7].x > UB_X || aPoint[7].y < LB_Y || aPoint[7].y > UB_Y && aJoints[7].getPositionConfidence() > 0.5){
      //                      //t2 = clock();
						//	pt.push_back( Kinect::Point2f(aPoint[7].x-cen_x, aPoint[7].y - cen_y) );

      //                  }
      //              }

                    //QString qstr = QString::fromStdString(recog);

                }
            }

        }

        //if (pt.size()>15 && aPoint[7].x > LB_X && aPoint[7].x < UB_X && aPoint[7].y > LB_Y && aPoint[7].y < UB_Y){
        //    //for (size_t i=0; i<pt.size();i++){
        //        theta = atan2(-pt[(int)pt.size()/2].y, pt[(int)pt.size()/2].x);
        //    //}
        //    //theta /= pt.size();

        //    if (theta > -PI/8 && theta< PI/8){
        //        recog = "RIGHT";
        //    }
        //    else if (theta> PI/8 && theta < 3*PI/8){
        //        recog = "UPPER RIGHT";
        //    }
        //    else if (theta> 3*PI/8 && theta < 5*PI/8){
        //        recog = "UP";;
        //    }
        //    else if (theta > 5*PI/8 && theta < 7*PI/8){
        //        recog = "UPPER LEFT";
        //    }
        //    else if (theta > 7*PI/8 || theta< -7*PI/8){
        //        recog = "LEFT";
        //    }
        //    else if (theta > -7*PI/8 && theta < -5*PI/8){
        //        recog = "LOWER LEFT";
        //    }
        //    else if (theta> -5*PI/8 && theta < -3*PI/8){
        //        recog = "DOWN";
        //    }
        //    else if (theta > -3*PI/8 && theta < -PI/8){
        //        recog = "LOWER RIGHT";
        //    }
        //    //emit Kinect::GestureAction(recog);
        //    pt.clear();
        //}
        //cv::putText(cImageBGR, recog, cv::Point(50,50),CV_FONT_HERSHEY_SIMPLEX ,2,cv::Scalar(255,255,0),2,8);

        //p5. show image
		

		cv::Point2f rectPoint[16]; // this is for me to compute the pixel values for 100 in real distance
		nite::SkeletonJoint recorded_aJoints[2];

		if (!registered_flag) // neither foot pedal down, move the rectangular with the hands
		{
			recorded_aJoints[0] = aJoints[6];
			recorded_aJoints[1] = aJoints[7];
		}

		// draw the first rect
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x - 100, recorded_aJoints[0].getPosition().y - 100, recorded_aJoints[0].getPosition().z, &(rectPoint[0].x), &(rectPoint[0].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x + 100, recorded_aJoints[0].getPosition().y + 100, recorded_aJoints[0].getPosition().z, &(rectPoint[1].x), &(rectPoint[1].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x - 100, recorded_aJoints[1].getPosition().y - 100, recorded_aJoints[1].getPosition().z, &(rectPoint[2].x), &(rectPoint[2].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x + 100, recorded_aJoints[1].getPosition().y + 100, recorded_aJoints[1].getPosition().z, &(rectPoint[3].x), &(rectPoint[3].y));

		rectangle(cImageBGR, cv::Point((int)rectPoint[0].x, (int)rectPoint[0].y), cv::Point((int)rectPoint[1].x, (int)rectPoint[1].y), cv::Scalar(0,255,0),2,8);
		rectangle(cImageBGR, cv::Point((int)rectPoint[2].x, (int)rectPoint[2].y), cv::Point((int)rectPoint[3].x, (int)rectPoint[3].y), cv::Scalar(0,255,0),2,8);

		// draw the second rect
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x - 200, recorded_aJoints[0].getPosition().y - 200, recorded_aJoints[0].getPosition().z, &(rectPoint[4].x), &(rectPoint[4].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x + 200, recorded_aJoints[0].getPosition().y + 200, recorded_aJoints[0].getPosition().z, &(rectPoint[5].x), &(rectPoint[5].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x - 200, recorded_aJoints[1].getPosition().y - 200, recorded_aJoints[1].getPosition().z, &(rectPoint[6].x), &(rectPoint[6].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x + 200, recorded_aJoints[1].getPosition().y + 200, recorded_aJoints[1].getPosition().z, &(rectPoint[7].x), &(rectPoint[7].y));

		rectangle(cImageBGR, cv::Point((int)rectPoint[4].x, (int)rectPoint[4].y), cv::Point((int)rectPoint[5].x, (int)rectPoint[5].y), cv::Scalar(0,255,255),2,8);
		rectangle(cImageBGR, cv::Point((int)rectPoint[6].x, (int)rectPoint[6].y), cv::Point((int)rectPoint[7].x, (int)rectPoint[7].y), cv::Scalar(0,255,255),2,8);

		// draw the line for depth 
		double depth_diff[2];
		depth_diff[0] = (aJoints[6].getPosition().z - recorded_aJoints[0].getPosition().z);
		depth_diff[1] = (aJoints[7].getPosition().z - recorded_aJoints[1].getPosition().z);
		
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x - 300 , recorded_aJoints[0].getPosition().y, recorded_aJoints[0].getPosition().z, &(rectPoint[8].x), &(rectPoint[8].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[0].getPosition().x - 300 , recorded_aJoints[0].getPosition().y - depth_diff[0], recorded_aJoints[0].getPosition().z, &(rectPoint[9].x), &(rectPoint[9].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x + 300 , recorded_aJoints[1].getPosition().y , recorded_aJoints[1].getPosition().z, &(rectPoint[10].x), &(rectPoint[10].y));
		mUserTracker.convertJointCoordinatesToDepth(recorded_aJoints[1].getPosition().x + 300 , recorded_aJoints[1].getPosition().y - depth_diff[1], recorded_aJoints[1].getPosition().z, &(rectPoint[11].x), &(rectPoint[11].y));
		
		if (abs(depth_diff[0]) < 100)
			cv::line( cImageBGR, rectPoint[8], rectPoint[9], cv::Scalar( 0, 255, 0 ), 8 );
		else if (abs(depth_diff[0]) < 200)
			cv::line( cImageBGR, rectPoint[8], rectPoint[9], cv::Scalar( 0, 255, 255 ), 8 );
		else
			cv::line( cImageBGR, rectPoint[8], rectPoint[9], cv::Scalar( 0, 0, 255 ), 8 );

		if (abs(depth_diff[1]) < 100)
			cv::line( cImageBGR, rectPoint[10], rectPoint[11], cv::Scalar( 0, 255, 0 ), 8 );
		else if (abs(depth_diff[1]) < 200)
			cv::line( cImageBGR, rectPoint[10], rectPoint[11], cv::Scalar( 0, 255, 255 ), 8 );
		else
			cv::line( cImageBGR, rectPoint[10], rectPoint[11], cv::Scalar( 0, 0, 255 ), 8 );

        cv::imshow("User Image", cImageBGR);
        // p6. check keyboard
        if( cv::waitKey( 1 ) == 'q')
            break;
    }


    //mRecorder.stop();
    //mRecorder.destroy();

    //p7. stop
    mUserTracker.destroy();
    mColorStream.destroy();
    mDepthStream.destroy();
    mDevice.close();
    NiTE::shutdown();
    OpenNI::shutdown();
    //emit Kinect::finished();

}
