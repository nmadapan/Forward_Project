#include "LeapCoordinates.h"

Controller controller;

void LeapCoordinates::displayParam()
{
	for (size_t i = 0; i<1 ; i++)
	{
		std::cout<<"X: "<<x[i]<<", Y: "<<y[i]<<", Z: "<<z[i]<<std::endl;
		std::cout<<"Roll: "<<roll[i]<<", Pitch: "<<pitch[i]<<", Yaw: "<<yaw[i]<<std::endl;
		std::cout<<"Pinch: "<<pinch[i]<<std::endl;
		std::cout<<"Confidence: "<<confidence[i]<<std::endl;
		//printf ("Confidence: %f \r", confidence[i]);
	}
	
}

void LeapCoordinates::calcParam()
{
	// to enable our program to accept data from Leap even it's running in the back ground, need to check the ""Allow background" in Leap Settings
	controller.setPolicyFlags(Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);
	
	// Get frame for current iteration
	const Frame frame = controller.frame();
	
	//If there is a hand presesnt
	if (!frame.hands().isEmpty()) 
	{
		//double frame_perS = frame.currentFramesPerSecond();
		//printf ("%f frames per second.\n", frame_perS);
		//Get all hands
		HandList Hands = frame.hands();

		num_of_hands = Hands.count();
		//printf("%d hands detected\n", num_of_hands);
		left_hand_count = 0;
		right_hand_count = 0;
		try 
		{
			for (int i = 0; i < num_of_hands; i++)
			{
				const Hand hand = Hands[i];
				int index;
				if (hand.isLeft() && hand.isValid()) 
				{
					index = 0;
					left_hand_count = 1;
				}
				else if (hand.isRight() && hand.isValid()) 
				{
					index = 1;
					right_hand_count = 1;
				}
				else 	
					continue;


				//Get hand confidence
				confidence[index] = hand.confidence();

				//Get postion of hand center
				Vector handCenter = hand.palmPosition();
				//Vector handCenter = hand.stabilizedPalmPosition();

				//Assign values to x, y and z, units in mm
				x[index] = handCenter.x;
				y[index] = handCenter.y;
				z[index] = handCenter.z;

				//Get vector normal to plane of hand
				const Vector normal = hand.palmNormal();

				//Get vector pointing in direction of hand
				const Vector direction = hand.direction();
	
				// Calculate roll, pitch and yaw
				roll[index] = (double) normal.roll();
				pitch[index] = (double) direction.pitch();
				yaw[index] =  (double) direction.yaw();

				//Get Thumb, leftmost finger of right hand or vice versa
				thumb[index] = (index == 0) ? hand.fingers().rightmost() : hand.fingers().leftmost();
				ThumbTipPosition[index] = thumb[index].tipPosition();

				//Get vector for thumb
				//const Vector thumbDirection = thumb.direction();

				//Calculate angle between thumb tip and hand direction
				//angle[i] = thumbDirection.angleTo(direction) * R2D;
				pinch[index] = hand.pinchStrength() ;
			}
		}
		catch (int e)
		{
			printf("An exception occurred. Exception Nr. %i \n",e );
		}
	}
}

double LeapCoordinates::get_gripper_value(const bool hand_flag )
{
	if (pinch[hand_flag] < 0.2)
		return 75;
	else 
		return 20;


	//if (pinch[hand_flag] < 0.5) // you need to push very open to open the gripper
		//return 100 * (1 - pinch[hand_flag]);
	//else // we are unfavoing the motion of opening the gripper, thus put 50 here, so that it's very likely that the gripper will close most of time
		//return (80 - 60*pinch[hand_flag]);
}

int LeapCoordinates::CheckSafety()
{
	if (num_of_hands != 2 || left_hand_count != 1 || right_hand_count != 1 || confidence[0] < 0.5 || confidence[1] < 0.5 || ThumbTipPosition[0].distanceTo(ThumbTipPosition[1]) < 20)
	{
		if (fail_check++ % 500 == 0)
		{
			if (left_hand_count != 1)
				printf ("%i, Left hand Mssing\n", fail_check);
			if (right_hand_count != 1)
				printf ("%i, Right hand Mssing\n", fail_check);
			if (confidence[0] < 0.5)
				printf ("%i, Left hand Confidence low \n", fail_check);
			if (confidence[1] < 0.5)
				printf ("%i, Right hand Confidence low \n", fail_check);
			if (ThumbTipPosition[0].distanceTo(ThumbTipPosition[1]) < 20)
				printf ("%i, Too close \n", fail_check);
		}
		//printf ("%i, SafetyCheckFailed, (%i, %i, %i), (%f, %f), (%f)\n", fail_check, num_of_hands, left_hand_count, right_hand_count, confidence[0], confidence[1], ThumbTipPosition[0].distanceTo(ThumbTipPosition[1]));
		return 0;
	}
	else
		return 1;
}