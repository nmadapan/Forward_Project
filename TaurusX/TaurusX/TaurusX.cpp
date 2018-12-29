#include "TaurusX.h"

TaurusX taurus;
LeapCoordinates tdObj;
Omega omega_obj;
std::vector<Haptic_Seed> seed;
bool haptic_init_flag = false;
bool tele_operating_flag = false;

#include "Feedback.h" // since in Feedback.h, it will use the above global variables, we have to put it here after the def. of these global variables.

void* Master2Agent(void* info)
{
	taurus.get_log_file_name(info);
	//***********************************************************************************************************
	// to extract the char* from the void pointer, and use it as the name for the interface data log file
	string &logfilename = *(static_cast <std::string*> (info));
	char* ch_logfilename = new char[40];
	strcpy (ch_logfilename, logfilename.c_str());
	strcat(ch_logfilename, "_interface.txt");
	char* name_with_path = new char[50];
	strcpy(name_with_path, "log/");
	strcat(name_with_path, ch_logfilename);
	ch_logfilename = name_with_path;
	//***********************************************************************************************************

	//sam.init();
	taurus.Initialize(1);
	// initialize haptic packet, since Taurus won't offer to send Haptic feedback, we have to firstly send some junk info so that a 
	// haptic packet is sent back for initialization

	taurus.Send();
	std::ifstream infile ("temp_haptic_packet.txt",std::ifstream::binary);
	pthread_mutex_lock(&mutexHaptic);
	haptic_packet.Read(infile);
	pthread_mutex_unlock(&mutexHaptic);
	infile.close();

	bool pose_update_flag, camera_update_flag;
	char SRI_flag = '1';
	//std::cout<<"To use SRI software, press 1."<<std::endl<<"To use ISAT software, press 0."<<std::endl;
	//cin>>SRI_flag;
	// Loop and get packets from client, or generate packet in the server side
	while (1)
	{		
		if (SRI_flag == '1')
		{
			// this is the with_master situation, where we are receiving commands from the Master
			/* Receive bytes from client */
			//std::cout<<"Record packet, press 1; otherwise press 0"<<std::endl;
			//char record_packet_flag = 0;
			////cin>> record_packet_flag;
			//double right_grip = 0, left_grip = 0;


			//int word_result = -1;
			//int last_word = -1;
			//boost::thread* SphinxThread = new boost::thread(run_sphinx, boost::ref(word_result));
			//clock_t t, dt;
			//int Num_clutches = 0;
			//t = clock(); // init the time zero

			//TimeRecorder time_obj;
			//boost::thread TimeRecorder_Thread (&TimeRecorder::runTimer, &time_obj);
			while (1)
			{
				// get the sent packet from SRI software
				taurus.bytes_received = recvfrom(taurus.sd_M, taurus.buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&taurus.client_M, &taurus.length);	
				
				// detect the time difference between two packets, if it's longer than half a second, we say that one more cluth is added!!!
				//dt = clock() - t;
				//t = clock();
				//if (float(dt)/CLOCKS_PER_SEC > 0.5)  printf ("Number of Clutches: %i \n", Num_clutches++);

				// just for us to check if there are 4 clicks between commands, we should always get 4 clicks
				//dt = clock() - t;
				//printf ("It took me %f seconds.\n",((float)dt)/CLOCKS_PER_SEC);

				// do a safety checking for the SRI software, to make sure that the left gripper didn't open or close too much
				//left_grip = *(double*)&taurus.buffer_R1[176];
				////printf ("Current Left gripper value: %f \n", left_grip);
				//if (left_grip > 75) left_grip = 75;
				//if (left_grip < 20) left_grip = 20;

				////double check!!!!
				//if (left_grip > 75 || left_grip < 20)
				//{
				//	printf ("Left Gripper safety check failure!!!!!\n");
				//	break;
				//}


				// read the current value of the right gripper
				//printf ("Current right gripper value: %f \n", *(double*)&taurus.buffer_R1[312]);

				///////////////////////////////////KEYBOARD VERSION OF CONTROLLING OMEGA 6 OPEN AND CLOSE
				//if (GetAsyncKeyState(VK_UP) < 0) 
				//{
				//	printf ("Up!!! \n");
				//	right_grip += 1;
				//}
				//if (GetAsyncKeyState(VK_DOWN) < 0) 
				//{
				//	printf ("Down!!! \n");
				//	right_grip -= 1;
				//}
				///////////////////////////////////KEYBOARD VERSION OF CONTROLLING OMEGA 6 OPEN AND CLOSE


				///////////////////////////////////SPHINX VERSION OF CONTROLLING OMEGA 6 OPEN AND CLOSE
				//if (word_result != last_word)
				//{			
				//	if (word_result == 0)
				//	{
				//		print_star();
				//		printf("OPEN\n");
				//		print_star();
				//		right_grip = 70;
				//	}
				//	else if (word_result == 1){
				//		print_star();
				//		printf("CLOSE\n");
				//		print_star();
				//		right_grip = 20;
				//	}
				//	/*else if (word_result == 2){
				//		print_star();
				//		printf("HALF\n");
				//		print_star();
				//		right_grip = 40;
				//	}*/
				//	last_word = word_result;

				//}
				///////////////////////////////////SPHINX VERSION OF CONTROLLING OMEGA 6 OPEN AND CLOSE

				
				// gripper safety range detection
				//if (right_grip > 75) right_grip = 75;
				//if (right_grip < 20) right_grip = 20;

				////double check!!!!
				//if (right_grip > 75 || right_grip < 20)
				//{
				//	printf ("Right Gripper safety check failure!!!!!\n");
				//	break;
				//}


				// update the value of both grippers
				/**(double*)&taurus.buffer_R1[176] = left_grip;
				*(double*)&taurus.buffer_R1[312] = right_grip;*/

				// double check that the value is correct
				//printf ("Updated Left Gripper value: %f \n", *(double*)&taurus.buffer_R1[176]);
				//printf ("Updated Right Gripper value: %f \n", *(double*)&taurus.buffer_R1[312]);

				// send the packet to Taurus agent
				taurus.bytes_sent = sendto(taurus.sd_A, taurus.buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&taurus.client_A, taurus.length);
				/*if (record_packet_flag == '1')
				{
					FILE * pFile;
					pFile = fopen ("newMoves/temp.bin", "wb");
					fwrite (taurus.buffer_R1 , sizeof(char), sizeof(taurus.buffer_R1), pFile);
					fclose (pFile);
				}*/
			}
		}
		else // ISAT software //TODO add the camera tilt
		{	
			// wait for the command from user, now it only supports several basic command
			char input;
			std::cout<<"Wait for command:"<<std::endl;
			std::cout<<"s: go to Stowed"<<std::endl
				<<"a: go to Approach"<<std::endl
				<<"r: go to Ready"<<std::endl
				<<"{: turn lights on"<<std::endl
				<<"[: turn lights off"<<std::endl
				<<"u: update pose packet"<<std::endl
				<<"c: change camera tilt"<<std::endl
				<<"}: turn haptic on"<<std::endl
				<<"]: turn haptic off"<<std::endl;
			std::cin>>input;
			std::cout<<std::endl;

			// saved the working buffer in the local disk in Server, then based on the command of the user,
			// the program will load corresponding buffer and then send it to the Agent

			// basic file operation
			FILE * pFile;
			long lSize;
			size_t result;
			pose_update_flag = 0;
			camera_update_flag = 0;

			switch (input)
			{
			case 's': 
				std::cout<<"Go to Stowed"<<std::endl; 
				pFile = fopen ( "gotoStowed.bin" , "rb" ); break;
			case 'a': 
				std::cout<<"Go to Approach"<<std::endl;
				pFile = fopen ( "gotoApproach.bin" , "rb" ); break;
			case 'r': 
				std::cout<<"Go to Ready"<<std::endl;
				pFile = fopen ( "gotoSafe.bin" , "rb" );
				taurus.pose_sequence_num = 0; break; //initialize the sequence number, so that the local buffer will again get loaded
			case '{': 
				std::cout<<"TurnVisibleLightOn"<<std::endl;
				pFile = fopen ( "TurnVisibleLightOn1.bin" , "rb" ); break;
			case '[': 
				std::cout<<"TurnVisibleLightOff"<<std::endl;
				pFile = fopen ( "TurnVisibleLightOff1.bin" , "rb" ); break;
			case 'c':
				pFile = fopen ( "TurnVisibleLightOn2.bin" , "rb" ); // I guess this is a camera packet. 
				camera_update_flag = 1; break;
			case 'u':
				pFile = fopen ( "ReadyPosition.bin" , "rb" );
				pose_update_flag = 1; break;
			case '}':
				std::cout<<"Turn Haptic On"<<std::endl;
				pFile = fopen ( "TurnHapticOn.bin" , "rb" ); break;  
			case ']':
				std::cout<<"Turn Haptic Off"<<std::endl;
				pFile = fopen ( "TurnHapticOff.bin" , "rb" ); break;
			default:
				std::cout<<"Wrong command!"<<std::endl; 
				pFile = fopen ( "TurnVisibleLightOn1.bin" , "rb" ); break;
			}


			// basic file operation
			if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
			// obtain file size:
			fseek (pFile , 0 , SEEK_END);
			lSize = ftell (pFile);
			std::rewind (pFile);

			// allocate memory to contain the whole file:
			// char * buffer;
			// buffer = (char*) malloc (sizeof(char)*lSize);
			// copy the file into the buffer, the whole file is now loaded in the memory buffer. 
			result = fread (taurus.buffer_R1,1,lSize,pFile);
			fclose (pFile);


			if (camera_update_flag == 1)
			{
				double tilt_value;
				std::cout<<"Please enter the camera tilt value, range [0,1.5] in radians"<<std::endl;
				std::cout<<"0 means horizontal, 1.5 means nearly vertical"<<std::endl;
				cin>>tilt_value;
				taurus.Set_Camera_Tilt(tilt_value);		
				camera_update_flag = 0;
			}


			else if (pose_update_flag == 1)  
				//this corresponds to the 'U' case, where we need to update the pose matrix and then send it to Client
			{
				if (taurus.pose_sequence_num != 0) // we only need to copy the local buffer into the new buffer once
					memcpy(taurus.buffer_R1, taurus.buffer_R1_updated, BUFFER_SIZE);

				char update_choice;
				bool left_or_right;

				matrix <double> old_frame(4,4);
				matrix <double> reference(4,4);

				Point3D target_point;
				Point3D new_origin[2];

				//sam.speak("Please enter choice");
				std::cout<<"Please enter corresponding numbers to indicate which attribute to update. "<<std::endl;	
				std::cout<<"0: Toggle NULL mode"<<std::endl;
				std::cout<<"1: Display reported frame"<<std::endl;
				std::cout<<"2: Set gripper value"<<std::endl;
				std::cout<<"3: Increase tip position"<<std::endl;
				std::cout<<"4: Set tip position"<<std::endl;
				std::cout<<"5: Set tip orientation based on target roll, pitch and yaw"<<std::endl;
				std::cout<<"7: Make the incision"<<std::endl;
				std::cout<<"8: Follow a trajectory"<<std::endl;
				std::cout<<"9: Suture"<<std::endl;
				std::cout<<"i: go to incision start point"<<std::endl;
				std::cout<<"l: use LeapMotion to teleoperate Taurus"<<std::endl;
				std::cout<<"k: use Kinect to teleoperate Taurus"<<std::endl;
				std::cout<<"o: use Omega to teleoperate Taurus"<<std::endl;
				std::cout<<"h: use Hydra to teleoperate Taurus"<<std::endl;
				std::cout<<"e: use Keyboard to teleoperate Taurus"<<std::endl;
				std::cin>>update_choice;
				std::cout<<std::endl;
				switch(update_choice)
				{
				case 'i':
					{
						target_point.set_point(325052, -67644.9, 218673);
						taurus.Set_Arm_Position(0, target_point);
						break;
					}
				case '0':
					{
						if (taurus.enable_null_pose == 3) { horiz_star(); taurus.enable_null_pose = 1; std::cout<<"NORMAL mode"<<std::endl; horiz_star(); }
						else if (taurus.enable_null_pose == 1) {horiz_star(); taurus.enable_null_pose = 3; std::cout<<"NULL move mode"<<std::endl; horiz_star(); }
						break;
					}
				case '1':
					{
						std::cout<<"Extract reported frame from Haptic packet and display it"<<std::endl;
						pthread_mutex_lock(&mutexHaptic);
						haptic_packet.DisplayFrame();
						//std::cout<<haptic_packet;
						pthread_mutex_unlock(&mutexHaptic);
						break;
					}
				case '2': 
					{
						double grip_value;
						std::cout<<"Set gripper value. For left arm, hit 0. For right arm, hit 1"<<std::endl;
						cin>>left_or_right;
						std::cout<<"Enter the value for gripper, 0 is totally closed, 100 is totally open"<<std::endl;
						std::cin>>grip_value;
						taurus.Set_Grip(left_or_right, grip_value);
						break;
					}
				case '3': 
					{
						double dx, dy, dz;
						std::cout<<"Increase tip position. For left arm, hit 0. For right arm, hit 1"<<std::endl;
						cin>>left_or_right;
						std::cout<<"Please enter the increment of x, y, z(unit in micron)"<<std::endl;
						std::cin>>dx>>dy>>dz;
						taurus.Increase_Arm_Position(left_or_right, dx, dy, dz);
						break;
					}
				case '4': 
					{
						double dx, dy, dz;
						std::cout<<"Set tip position. For left arm, hit 0. For right arm, hit 1"<<std::endl;
						cin>>left_or_right;
						std::cout<<"Please enter the target position of x, y, z(unit in micron)"<<std::endl;
						std::cin>>dx>>dy>>dz;
						target_point.set_point(dx, dy, dz);
						taurus.Set_Arm_Position(left_or_right, target_point);
						break;
					}
				case '5':
					{
						double ref_roll, ref_pitch, ref_yaw;
						std::cout<<"Set tip orientation based on target roll, pitch and yaw (degree)"<<std::endl;
						std::cout<<"For left arm, hit 0. For right arm, hit 1"<<std::endl;
						cin>>left_or_right;
						old_frame = taurus.get_haptic_packet(left_or_right);

						std::cout<<"Enter target roll, pitch, yaw in order (degree)"<<std::endl;
						std::cin>>ref_roll>>ref_pitch>>ref_yaw;;
						taurus.Set_Pose_Rotate(left_or_right, ref_roll, ref_pitch, ref_yaw, old_frame);
						break;
					}
				case '7': 
					{
						std::cout<<"Now the robot will make the horizontal incision"<<std::endl;
						Point3D tempP;
						/*std::vector <Point3D> incision_position;
						std::vector <Point3D> incision_angles;
						incision_position.resize(10);
						incision_angles.resize(10);*/

						Point3D left_tip_position;
						double incision_increment_length = 30;
						taurus.Increase_Arm_Position(0, 0, 0, 0); // send a packet to the robot so that the pose packet gets fulfilled once and is not all zero anymore

						tempP.set_point(322380, -60037, 219293);
						taurus.Set_Arm_Position(0, tempP);

						old_frame = taurus.get_pose_packet(0);
						std::cout<<old_frame<<std::endl;
						taurus.jaw_frame_to_tip_position(0, old_frame, left_tip_position);

						old_frame = taurus.get_pose_packet(0); 
						taurus.Set_Pose_Rotate(0, 117.965, -60.1691, 50.2225, old_frame);

						tempP.set_point(305912, -58329, 224649);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(281104, -55552, 224600);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(281104, -55552, 215602);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						old_frame = taurus.get_pose_packet(0);
						taurus.Set_Pose_Rotate(0, 135.688, -60.1837, 50.2807, old_frame);

						tempP.set_point(280716, -56953, 223964);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(256330, -61158, 223962);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(256330, -61158, 220334);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						old_frame = taurus.get_pose_packet(0);
						taurus.Set_Pose_Rotate(0, 154.582, -60.7517, 50.7809, old_frame);

						tempP.set_point(255896, -65632, 228149);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(233796, -76295, 228701);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);

						tempP.set_point(233398, -76425, 219741);
						taurus.Set_Arm_Position(0, tempP, incision_increment_length);


						/*taurus.Set_Grip(0, 20);
						old_frame = taurus.get_haptic_packet(left_or_right);
						taurus.Update_Pose_Rotate(1, 'y', 20, old_frame);*/
						break;
					}
				case '8':
					{
						matrix <double> traj_frame(4,4);
						ifstream trajectory_reader;

						Point3D traj_p;
						//double traj_grip;
						string traj_s;
						Packet_Pose traj_pose;
						int down_sample = 0;
						string traj_name;

						std::cout<<"Now the robot will replay a trajectory, please specify trajectory name:"<<std::endl;
						cin>>traj_name;
						taurus.Read_Pose_Packet();

						trajectory_reader.open(traj_name, ios::in);
						if (trajectory_reader.is_open())
						{
							while(!trajectory_reader.eof())
							{
								traj_pose = taurus.pose_packet;
								for (size_t i_arm = 0; i_arm<2; i_arm++)
								{
									getline(trajectory_reader,traj_s);
									istringstream sin(traj_s);
									for (size_t i_row = 0; i_row < 4; i_row ++)
										for (size_t j_row = 0; j_row < 4; j_row++)
										{
											sin>>traj_pose.TipPose[i_arm][i_row][j_row];
											//std::cout<<traj_pose.TipPose[i_arm][i_row][j_row]<<'\t';
											//traj_frame(i_row,j_row) = taurus.pose_packet.TipPose[i_arm][i_row][j_row];
										}
										sin>>traj_pose.grip[i_arm];

										// in the following, I tried to fetch the transition, rotation and gripper from the packet, and use all my functions to do these sub-tasks sequencially. 
										// but this is not very good, what I can is to just fill in the buffer and send the packet to Taurus.
										//std::cout<<std::endl;
										//traj_p.set_point(taurus.pose_packet.TipPose[i_arm][0][3]/MEGA, taurus.pose_packet.TipPose[i_arm][1][3]/MEGA, taurus.pose_packet.TipPose[i_arm][2][3]/MEGA);
										//taurus.Set_Arm_Position(i_arm, traj_p);	
										//taurus.Set_Grip(i_arm, traj_grip);	


										//pthread_mutex_lock(&mutexHaptic);
										//for (unsigned i = 0;i<old_frame.size1();++i)
										//	for (unsigned j = 0; j<old_frame.size2();j++)
										//		old_frame(i,j) = haptic_packet.TipPose[i_arm][i][j];
										//pthread_mutex_unlock(&mutexHaptic);
										//taurus.Rotate_to_Direct_Target_Frame(i_arm, traj_frame, old_frame);
										//taurus.Send_Command();
								}
								taurus.Fill_in_buffer(traj_pose);
								taurus.pose_sequence_num++; 
								*(u_int*)&taurus.buffer_R1[4] += 1; // update the sequence number!
								Sleep(8); // replay in old speed is too fast, let's slow it down a little bit
								taurus.Send();
							}	
						}
						trajectory_reader.close();
						break;
					}
				case '9':
					{
						taurus.rotation_sub_routine(-162.147, 5.14385, 97.2908, 0);
						taurus.rotation_sub_routine(-106.239, 22.899, -98.5392, 1);
						break;
					}
				case 'l': 
					{

						TimeRecorder time_obj;
						boost::thread TimeRecorder_Thread (&TimeRecorder::runTimer, &time_obj);

						bool done_flag = 0;

						// the threshold for 
						seed[0].set_warning_threshold(3.5); //3.5 is the value in theory, but 3 works better
						seed[1].set_warning_threshold(3.5);
						seed[0].set_damping_factor(0.8);
						seed[1].set_damping_factor(0.8);


						//// those values are the ones that I used for the experiment in the IROS workshop. After a while those values didn't work any more, and now I need to change them. 11/9/2014
						//seed[0].set_warning_threshold(3); //3.5 is the value in theory, but 3 works better
						//seed[1].set_warning_threshold(3);
						//seed[0].set_damping_factor(0.8);
						//seed[1].set_damping_factor(0.8);

						tele_operating_flag = true;
						Point3D new_origin[2];
						Point3D target_point[2];
						matrix<double> reference[2] = {matrix<double>(4, 4), matrix<double>(4, 4)};

						//matrix <double> L_reference(4,4);
						//matrix <double> R_reference(4,4);

						// local variable for LeapMotion
						//matrix <double> L_Rotation_Leap(3,3); // used to store rotation from roll pitch yaw
						//matrix <double> R_Rotation_Leap(3,3); // used to store rotation from roll pitch yaw

						matrix <double> Rotation_Leap [2] = {matrix <double> (3,3), matrix <double> (3,3)}; // used to store rotation from roll pitch yaw
						identity_matrix <double> Identity(3);
						ofstream out; // write Leap data into txt for processing
						// %Leap

						sam.speak("Use LeapMotion to control Taurus", SAM::ASYNC);
						std::cout<<"Use LeapMotion to control Taurus, hit 'q' to quit., hit 'c' for clutch off"<<std::endl;
						//std::cout<<"For left arm, hit 0. For right arm, hit 1"<<std::endl;
						//cin>>left_or_right;
						// MAX_MOVE = 400;
						// for controlling with Leap, since Leap required high communication frequency to enable stability, we increase this value so that the tremor will be decreased 
						// and also the delay will be decreased

						// initialize the stepper, the stepper will update global variable, and here accordingly to the global variable, we will decide how to move Taurus
						Stepper stepper;
						stepper.initialize_stepper();

						for (char lr = 0; lr < 2; lr++)
						{
							// update the reference
							reference[lr] = taurus.update_reference(lr);
							new_origin[lr].set_point(reference[lr](0,3), reference[lr](1,3), reference[lr](2,3));
						}

						// open the file 
						//time_t t = time(0);   // get time now
						//struct tm * now = localtime( & t );

						//char leap_filename [80];
						//strftime (leap_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
						//strcat(leap_filename, "Leap_data.txt");

						out.open(ch_logfilename, ios::out);

						std::vector <Point3D> leap_init;
						std::vector <Point3D> leap_init_rotation;
						taurus.register_Leap_initial(&tdObj, leap_init, leap_init_rotation);
						// NEED TO CHECK HERE

						while(1)
						{
							if(GetAsyncKeyState(VK_ESCAPE))
							{
								printf ("Finished using Leap Motion to control Taurus \n");
								break;
							}

							// nothing happens in this case, we just update the reference matrix, that's all
							if (stepper.neither_down())
							{
								tdObj.calcParam(); // this requires sevelal packets!!!
								if (tdObj.CheckSafety())
								{
									for (char lr = 0; lr < 2 ; lr++)
									{
										// update the reference
										reference[lr] = taurus.update_reference(lr);
										new_origin[lr].set_point(reference[lr](0,3), reference[lr](1,3), reference[lr](2,3));
									}
									taurus.register_Leap_initial(&tdObj, leap_init, leap_init_rotation);
								}
								
							}
							else
							{
								tdObj.calcParam(); //get the update-to-date values (or most recent)				
								double scaling = 2;
								if (tdObj.CheckSafety())
								{						
									// only gripper and position
									if (stepper.left_down())
									{
										for (char lr = 0; lr < 2; lr++)
										{
											// Get the current control position
											target_point[lr].set_point(new_origin[lr].x-(tdObj.z[lr]-leap_init[lr].z)*1000/scaling, new_origin[lr].y+(tdObj.x[lr]-leap_init[lr].x)*1000/scaling, new_origin[lr].z-(tdObj.y[lr]-leap_init[lr].y)*1000/scaling);
											if (!taurus.check_control_input(lr, target_point[lr]))
											{
												taurus.Set_Arm_Position(lr, target_point[lr], 400);
												//taurus.Update_T_matrix(lr, Identity, reference[lr], target_point[lr]);
												out<<target_point[lr].x<<' '<<target_point[lr].y<<' '<<target_point[lr].z<<' '<<std::endl;
											}
											taurus.Set_Grip(lr, tdObj.get_gripper_value(lr));
										} // end of two hands						 
									} // end of stepper left down

									// gripper and orientation
									else if (stepper.right_down())
									{
										for (char lr = 0; lr < 2; lr++)
										{
											// Get the rotation matrix
											Rotation_Leap[lr] = taurus.Construct_Rotation_from_rpy(
												(tdObj.yaw[lr]-leap_init_rotation[lr].z), 
												-(tdObj.pitch[lr]-leap_init_rotation[lr].y), 
												- (tdObj.roll[lr]-leap_init_rotation[lr].x));	
											pthread_mutex_lock(&mutexHaptic);
											new_origin[lr].set_point(haptic_packet.TipPose[lr][0][3], haptic_packet.TipPose[lr][1][3], haptic_packet.TipPose[lr][2][3]);
											pthread_mutex_unlock(&mutexHaptic);
											taurus.Update_T_matrix(lr, Rotation_Leap[lr], reference[lr], new_origin[lr]);
											taurus.Set_Grip(lr, tdObj.get_gripper_value(lr));
										} // end of two hands		
									} // end of stepper right down

									// everything: gripper, position and orientation
									else if (stepper.both_down())
									{
										for (char lr = 0; lr < 2; lr++)
										{
											// Get the current control position
											target_point[lr].set_point(new_origin[lr].x-(tdObj.z[lr]-leap_init[lr].z)*1000/scaling, new_origin[lr].y+(tdObj.x[lr]-leap_init[lr].x)*1000/scaling, new_origin[lr].z-(tdObj.y[lr]-leap_init[lr].y)*1000/scaling);
											// Get the rotation matrix
											Rotation_Leap[lr] = taurus.Construct_Rotation_from_rpy(
												(tdObj.yaw[lr]-leap_init_rotation[lr].z), 
												-(tdObj.pitch[lr]-leap_init_rotation[lr].y), 
												- (tdObj.roll[lr]-leap_init_rotation[lr].x));	
											taurus.Set_Arm_Position(lr, target_point[lr], 400);
											taurus.Update_T_matrix(lr, Rotation_Leap[lr], reference[lr], target_point[lr]);
											taurus.Set_Grip(lr, tdObj.get_gripper_value(lr));
										} // end of two hands												
									} // end of stepper both down
								}
							}
						}
						stepper.terminate_stepper();	
						tele_operating_flag = false;
						haptic_init_flag = false;
						break;
					}
				case 'k':
					{
						seed[0].set_warning_threshold(3);
						seed[1].set_warning_threshold(3);
						seed[0].set_damping_factor(0.4);
						seed[1].set_damping_factor(0.4);
						tele_operating_flag = true;
						sam.speak("Use Kinect to control Taurus", SAM::ASYNC);
						std::cout<<"Use Kinect to control Taurus"<<std::endl;
						//std::cout<<"For left arm, hit 0. For right arm, hit 1"<<std::endl;
						//cin>>left_or_right;
						Point3D init_hand[2];
						Point3D control_hand[2];
						Point3D target_point[2];
						//double scaling;
						matrix<double> reference[2] = {matrix<double>(4, 4), matrix<double>(4, 4)};
						identity_matrix <double> Identity(3);
						bool registered_flag = false;
						bool temp_flag = false;
						Point3D control_pt_K_space[2];
						std::ofstream out;
						// open the file 
						//time_t t = time(0);   // get time now
						//struct tm * now = localtime( & t );

						//char kinect_filename [80];
						//strftime (kinect_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
						//strcat(kinect_filename, "kinect_data.txt");
						//out.open(kinect_filename, ios::out);

						out.open(ch_logfilename, ios::out);

						//clock_t time_ref = clock();


						// initialize the stepper, the stepper will update global variable, and here accordingly to the global variable, we will decide how to move Taurus
						Stepper stepper;
						stepper.initialize_stepper();

						for (char lr = 0; lr<2; lr++)
						{
							// update the reference for left arm
							reference[lr] = taurus.update_reference(lr);
							//std::cout<<"reference: "<<reference<<std::endl;
							new_origin[lr].set_point(reference[lr](0,3), reference[lr](1,3), reference[lr](2,3));		
						}


						pthread_mutex_init(&mutex_User_tracking, NULL);

						Kinect kinect;
						//boost::thread* pThread = new boost::thread(&Kinect::User_tracking_Kinect, &kinect);
						boost::thread* pThread = new boost::thread(&Kinect::getGestureEvent, &kinect, boost::ref(registered_flag));
						//int User_tracking_rc = pthread_create(&User_tracking_thread,NULL, &Kinect::StaticThreadProc,NULL);
						//if (User_tracking_rc ) std::cout<<"Failure to create threads"<<std::endl;
						double speed;

						int moving_position_threshold = 100; // the big cube of origin, safe region 150 also works but makes the user tired
						int moving_angle_step_length = 1; // in degree
						double grip_angle_step_length = 0.3; //  0 - 100

						while(1)
						{
							if(GetAsyncKeyState(VK_ESCAPE))
							{
								printf ("Finished using Kinect to control Taurus \n");
								break;
							}

							if (kinect.Kinect_tracking_flag == true)
							{		
								if (stepper.neither_down())  // no movement
								{
									for (char lr = 0; lr<2; lr++)
									{
										// update the reference for both arm
										reference[lr] = taurus.update_reference(lr);
										//std::cout<<"reference: "<<reference<<std::endl;
										new_origin[lr].set_point(reference[lr](0,3), reference[lr](1,3), reference[lr](2,3));	
									}
									registered_flag = false;
								}

								else // to extract hand positions
								{
									if (registered_flag == false)
									{
										//////
										pthread_mutex_lock(&mutex_User_tracking);
										// init the starting control position	
										if (abs(kinect.hand[0]->getPosition().x) < 5000)
										{
											init_hand[0].set_point(kinect.hand[0]->getPosition().x, kinect.hand[0]->getPosition().y, kinect.hand[0]->getPosition().z);		
											init_hand[1].set_point(kinect.hand[1]->getPosition().x, kinect.hand[1]->getPosition().y, kinect.hand[1]->getPosition().z);	
											registered_flag = true;
										}
										pthread_mutex_unlock(&mutex_User_tracking);
										//////
									}


									for (char i = 0; i<2; i++)
									{
										/////
										pthread_mutex_lock(&mutex_User_tracking);
										if (abs(kinect.hand[i]->getPosition().x) < 5000)
										{
											control_hand[i].set_point(kinect.hand[i]->getPosition().x-init_hand[i].x, kinect.hand[i]->getPosition().y-init_hand[i].y, kinect.hand[i]->getPosition().z-init_hand[i].z); 	
											temp_flag = true;
										}
										else
										{
											control_hand[i].set_point(0,0,0);
											temp_flag = false;
										}
										pthread_mutex_unlock(&mutex_User_tracking);
										//if (i==0) std::cout<<"Arm "<<(int)i<<": "<<control_hand[i]<<std::endl;
										//////

										if (stepper.left_down()) 
										{
											if (abs(control_hand[i].x) < 10*moving_position_threshold )
												out<<control_hand[i].x<<' '<<control_hand[i].y<<' '<<control_hand[i].z<<std::endl;
											if (abs(control_hand[i].x)>=abs(control_hand[i].y) && abs(control_hand[i].x)>=abs(control_hand[i].z)) // x is max
											{
												if (control_hand[i].x > moving_position_threshold && control_hand[i].x < 10*moving_position_threshold) // add the upper limit because sometimes, control hand will give me very large numbers (NAN) which is not an useful command
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].x - moving_position_threshold);
													taurus.Increase_Arm_Position(i, 0, speed, 0);
													std::cout<<"Right with speed "<<speed<<std::endl;
												}
												else if (control_hand[i].x < -moving_position_threshold && control_hand[i].x > -10*moving_position_threshold )
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].x + moving_position_threshold);
													taurus.Increase_Arm_Position(i, 0, -speed, 0);
													std::cout<<"Left with speed "<<speed<<std::endl;
												}

											}

											else if (abs(control_hand[i].y)>=abs(control_hand[i].z) && abs(control_hand[i].y)>=abs(control_hand[i].x)) // y is max
											{
												if (control_hand[i].y > moving_position_threshold && control_hand[i].y < 10*moving_position_threshold)
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].y - moving_position_threshold);
													taurus.Increase_Arm_Position(i, 0, 0, -speed);
													std::cout<<"Up with speed "<<speed<<std::endl;
												}

												else if (control_hand[i].y < -moving_position_threshold && control_hand[i].y > -10*moving_position_threshold)
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].y + moving_position_threshold);
													taurus.Increase_Arm_Position(i, 0, 0, speed);
													std::cout<<"Down with speed "<<speed<<std::endl;
												}
											}
											else if (abs(control_hand[i].z)>=abs(control_hand[i].x) && abs(control_hand[i].z)>=abs(control_hand[i].y))// z is max
											{
												if (control_hand[i].z > moving_position_threshold && control_hand[i].z < 10* moving_position_threshold)
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].z - moving_position_threshold);
													taurus.Increase_Arm_Position(i, -speed, 0, 0);
													std::cout<<"Backward with speed "<<speed<<std::endl;
												}
												else if (control_hand[i].z < -moving_position_threshold && control_hand[i].z > -10*moving_position_threshold)
												{
													speed = taurus.discrete_translation_control_from_kinect(control_hand[i].z + moving_position_threshold);
													taurus.Increase_Arm_Position(i, speed, 0, 0);
													std::cout<<"Forward with speed "<<speed<<std::endl;
													
												}
											}
										}
										//else if (stepper.right_down())
										//{	
										//	reference[i] = taurus.get_haptic_packet(i);
										//	if (abs(control_hand[i].x)>=abs(control_hand[i].y) && abs(control_hand[i].x)>=abs(control_hand[i].z)) // x is max
										//	{
										//		if (control_hand[i].x > moving_position_threshold && control_hand[i].x < 10*moving_position_threshold) // add the upper limit because sometimes, control hand will give me very large numbers (NAN) which is not an useful command
										//			taurus.Update_Pose_Rotate(i, 'z', -moving_angle_step_length, reference[i], 0.5); // 0.5 is a good angle_step for Kinect
										//		else if (control_hand[i].x < -moving_position_threshold && control_hand[i].x > -10*moving_position_threshold )
										//			taurus.Update_Pose_Rotate(i, 'z', moving_angle_step_length, reference[i], 0.5);
										//	}

										//	else if (abs(control_hand[i].y)>=abs(control_hand[i].z) && abs(control_hand[i].y)>=abs(control_hand[i].x)) // y is max
										//	{
										//		if (control_hand[i].y > moving_position_threshold && control_hand[i].y < 10*moving_position_threshold)
										//			taurus.Update_Pose_Rotate(i, 'y', -moving_angle_step_length, reference[i], 0.5);
										//		else if (control_hand[i].y < -moving_position_threshold && control_hand[i].y > -10*moving_position_threshold)
										//			taurus.Update_Pose_Rotate(i, 'y', moving_angle_step_length, reference[i], 0.5);
										//	}
										//	else if (abs(control_hand[i].z)>=abs(control_hand[i].x) && abs(control_hand[i].z)>=abs(control_hand[i].y))// z is max
										//	{
										//		if (control_hand[i].z > moving_position_threshold && control_hand[i].z < 10* moving_position_threshold)
										//			taurus.Update_Pose_Rotate(i, 'x', moving_angle_step_length, reference[i], 0.5);
										//		else if (control_hand[i].z < -moving_position_threshold && control_hand[i].z > -10*moving_position_threshold)
										//			taurus.Update_Pose_Rotate(i, 'x', -moving_angle_step_length, reference[i], 0.5);
										//	}
										//} 
										else if (stepper.both_down())
										{
											if (control_hand[i].y > moving_position_threshold && control_hand[i].y < 10*moving_position_threshold)
												taurus.Increase_Grip(i,grip_angle_step_length);
											else if (control_hand[i].y < -moving_position_threshold && control_hand[i].y > -10*moving_position_threshold)
												taurus.Increase_Grip(i,-grip_angle_step_length);
										}
									}
								}
							}
						}


						kinect.Stop_tracking = true;
						nite::NiTE::shutdown();
						stepper.terminate_stepper();	
						tele_operating_flag = false;
						haptic_init_flag = false;
						break;	
					}
				case 'o':
					{
						seed[0].set_warning_threshold(3); //3.5 is the value in theory, but 3 works better
						seed[1].set_warning_threshold(3);
						seed[0].set_damping_factor(1.1);
						seed[1].set_damping_factor(1.1);
						tele_operating_flag = true;
						std::vector <Point3D> Omega_report_position;
						Omega_report_position.resize(2);
						std::vector <Point3D> target_point;
						target_point.resize(2);
						std::vector<Point3D> Omega_init;
						Omega_init.resize(2);
						matrix<double> raw_rotation_matrix(3,3);
						matrix<double> rotation_matrix(3,3);
						matrix<double> rotation_init[2] = {matrix<double>(3, 3), matrix<double>(3, 3)};

						bool register_original_position = true;
						ofstream out; // write Omega data into txt for processing
						//time_t t = time(0);   // get time now
						//struct tm * now = localtime( & t );

						//char omega_filename [80];
						//strftime (omega_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
						//strcat(omega_filename, "Omega_data.txt");
						//out.open(omega_filename, ios::out);

						out.open(ch_logfilename, ios::out);

						identity_matrix <double> Identity(3);
						sam.speak("Use Omega to control Taurus", SAM::ASYNC);
						std::cout<<"Use Omega7 to control Taurus"<<std::endl;
						//std::cout<<"For left arm, hit 0. For right arm, hit 1"<<std::endl;
						//cin>>left_or_right;
						//left_or_right = 0;
						//MAX_MOVE = 4000;
						// for controlling with Omega, since Omega required high communication frequency to enable stability, we increase this value to 4000 so that the tremor will be decreased
						// this is not crazy, since through Omega we can still have stable control.

						// initialize the stepper, the stepper will update global variable, and here accordingly to the global variable, we will decide how to move Taurus
						Stepper stepper;
						stepper.initialize_stepper();

						if (omega_obj.init() != 0)
						{
							printf ("Error in initializing Omega!!! Break!!!! \n");
							break;
						}

						matrix <double> reference[2] = {matrix <double>(4,4), matrix<double>(4,4) };
						// update the reference
						reference[0] = taurus.update_reference(0);
						reference[1] = taurus.update_reference(1);
						//std::cout<<"reference: "<<reference<<std::endl;

						std::vector <Point3D> new_origin;
						new_origin.resize(2);
						new_origin[0].set_point(reference[0](0,3), reference[0](1,3), reference[0](2,3));
						new_origin[1].set_point(reference[1](0,3), reference[1](1,3), reference[1](2,3));

						// init the starting control position and rotation
						taurus.register_Omega_initial(&omega_obj, Omega_init[0], Omega_init[1], rotation_init);
						//std::cout<<rotation_init[0]<<std::endl;
						//std::cout<<rotation_init[1]<<std::endl;
						while(!omega_obj.get_done())
						{
							omega_obj.update_position();
							for (char arm = 0 ; arm < omega_obj.get_device_count(); arm ++)
							{
								//taurus.Set_Grip(left_or_right, 0); // hold scalpal all the time

								if (stepper.left_down())
								{
									Omega_report_position[arm] = omega_obj.get_position(arm);
									//std::cout<<Omega_report_position.x<<' '<<Omega_report_position.y<<' ' <<Omega_report_position.z<<std::endl;
									target_point[arm].set_point(new_origin[arm].x-(Omega_report_position[arm].x-Omega_init[arm].x)*MEGA, new_origin[arm].y+(Omega_report_position[arm].y-Omega_init[arm].y)*MEGA, new_origin[arm].z-(Omega_report_position[arm].z-Omega_init[arm].z)*MEGA);			
									//target_point.set_point(new_origin[left_or_right].x-(Omega_report_position.x+0.05)*MEGA, new_origin[left_or_right].y+(Omega_report_position.y)*MEGA, new_origin[left_or_right].z-(Omega_report_position.z)*MEGA);						

									// disable all the forces so that there is no tremer between switching space
									//dhdEnableForce (DHD_ON); // resume the force
									taurus.Set_Arm_Position(arm, target_point[arm], 4000);

									//taurus.Update_T_matrix(left_or_right, Identity, reference, target_point);
									out<<target_point[arm].x<<' '<<target_point[arm].y<<' '<<target_point[arm].z<<std::endl;
									// comment the set_grip because this will also trigger a Haptic feedback packet, here we don't want it
									taurus.Set_Grip(arm, omega_obj.get_gripper(arm));
								}
								else if (stepper.right_down())
								{

									//rotation_matrix = omega_obj.get_rotation(arm);
									// to get the raw rotation matrix from roll pitch yaw, notice that the mapping happens 
									raw_rotation_matrix = omega_obj.get_rotation_from_rpy(arm);
									//std::cout<<raw_rotation_matrix<<std::endl;

									// the get the rotation matrix relative to the init control pose
									rotation_matrix = bnu::prod(bnu::trans(rotation_init[arm]), raw_rotation_matrix);
									//std::cout<<arm<<': '<<rotation_matrix<<std::endl;

									//dhdEnableForce (DHD_ON); // resume the force
									taurus.Update_T_matrix(arm, rotation_matrix, reference[arm], new_origin[arm]);
									taurus.Set_Grip(arm, omega_obj.get_gripper(arm));
								}
								else if (stepper.both_down())
								{

									//rotation_matrix = omega_obj.get_rotation(arm);
									// to get the raw rotation matrix from roll pitch yaw, notice that the mapping happens 
									raw_rotation_matrix = omega_obj.get_rotation_from_rpy(arm);
									//std::cout<<raw_rotation_matrix<<std::endl;

									// the get the rotation matrix relative to the init control pose
									rotation_matrix = bnu::prod(bnu::trans(rotation_init[arm]), raw_rotation_matrix);
									//std::cout<<arm<<': '<<rotation_matrix<<std::endl;


									Omega_report_position[arm] = omega_obj.get_position(arm);
									//std::cout<<Omega_report_position.x<<' '<<Omega_report_position.y<<' ' <<Omega_report_position.z<<std::endl;
									target_point[arm].set_point(new_origin[arm].x-(Omega_report_position[arm].x-Omega_init[arm].x)*MEGA, new_origin[arm].y+(Omega_report_position[arm].y-Omega_init[arm].y)*MEGA, new_origin[arm].z-(Omega_report_position[arm].z-Omega_init[arm].z)*MEGA);			
									//target_point.set_point(new_origin[left_or_right].x-(Omega_report_position.x+0.05)*MEGA, new_origin[left_or_right].y+(Omega_report_position.y)*MEGA, new_origin[left_or_right].z-(Omega_report_position.z)*MEGA);						


									//dhdEnableForce (DHD_ON); // resume the force
									taurus.Update_T_matrix(arm, rotation_matrix, reference[arm], target_point[arm]);
									taurus.Set_Grip(arm, omega_obj.get_gripper(arm));
								}

								else if (stepper.neither_down())
								{
									//dhdEnableForce (DHD_OFF); // disable the force for Omega to return to starting position
									// update the reference
									reference[arm] = taurus.update_reference(arm);
									new_origin[arm].set_point(reference[arm](0,3), reference[arm](1,3), reference[arm](2,3));
									taurus.register_Omega_initial(&omega_obj, Omega_init[0], Omega_init[1], rotation_init);
									//std::cout<<rotation_init[0]<<std::endl;
									//std::cout<<rotation_init[1]<<std::endl;

								}
							}
						}	
						stepper.terminate_stepper();
						omega_obj.terminate();
						tele_operating_flag = false;
						haptic_init_flag = false;
						break;
					}
				case 'e':
					{
						char axis;
						char p_or_n;
						char escape_flag = 0;
						seed[0].set_warning_threshold(3);
						seed[1].set_warning_threshold(3);
						seed[0].set_damping_factor(3.75);
						seed[1].set_damping_factor(3.75);
						tele_operating_flag = true;
						bool proper_input_flag = true;
						bool rotate_trigger = false;
						std::cout<<"Use Keyboard to teleoperate Taurus"<<std::endl;
						// initialize the stepper, the stepper will update global variable, and here accordingly to the global variable, we will decide how to move Taurus
						Stepper stepper;
						stepper.initialize_stepper();
						// only gripper and position
						double length_increment = 50; // the old value is 300
						while( escape_flag == 0)
						{	
							if (stepper.left_down())
							{						
								if (GetAsyncKeyState('V') < 0) 
									escape_flag = 1;
								// for displaying current frame
								if (GetAsyncKeyState('C') < 0) 
								{
									pthread_mutex_lock(&mutexHaptic);
									haptic_packet.DisplayFrame();
									pthread_mutex_unlock(&mutexHaptic);
								}
								if (GetAsyncKeyState('R') < 0) 
									// write landmark data into a txt file which can be further used
								{
									std::ofstream trajectory_writer;
									//trajectory_writer.open(haptic_filename, ios::app); // for doing the experiment
									trajectory_writer.open("recorded_landmark.txt", ios::app);
									if (trajectory_writer.is_open())
									{
										pthread_mutex_lock(&mutexHaptic);
										for (int arm = 0; arm<2;arm++)
										{
											for (int i=0;i<4;i++)
												for (int j=0;j<4;j++)
													trajectory_writer<<haptic_packet.TipPose[arm][i][j]<<' ';
											//trajectory_writer<<haptic_packet.grasp[arm]<<' '<<std::endl;
											trajectory_writer<<20<<' '<<std::endl; //I want the gripper to be always closed, thus always writing 20
										}
										pthread_mutex_unlock(&mutexHaptic);
									}
								}

								// left arm move
								if (GetAsyncKeyState('W') < 0) 
									taurus.Increase_Arm_Position(0, length_increment, 0, 0);
								if (GetAsyncKeyState('S') < 0) 
									taurus.Increase_Arm_Position(0, -length_increment, 0, 0);
								if (GetAsyncKeyState('D') < 0) 
									taurus.Increase_Arm_Position(0, 0, length_increment, 0);
								if (GetAsyncKeyState('A') < 0)  
									taurus.Increase_Arm_Position(0, 0, -length_increment, 0);
								if (GetAsyncKeyState('Q') < 0) 
									taurus.Increase_Arm_Position(0, 0, 0, length_increment);
								if (GetAsyncKeyState('E') < 0) 
									taurus.Increase_Arm_Position(0, 0, 0, -length_increment);

								// right arm move 
								if (GetAsyncKeyState('I') < 0) 
									taurus.Increase_Arm_Position(1, length_increment, 0, 0);
								if (GetAsyncKeyState('K') < 0) 
									taurus.Increase_Arm_Position(1, -length_increment, 0, 0);
								if (GetAsyncKeyState('L') < 0) 
									taurus.Increase_Arm_Position(1, 0, length_increment, 0);
								if (GetAsyncKeyState('J') < 0) 
									taurus.Increase_Arm_Position(1, 0, -length_increment, 0);
								if (GetAsyncKeyState('U') < 0) 
									taurus.Increase_Arm_Position(1, 0, 0, length_increment);
								if (GetAsyncKeyState('O') < 0) 
									taurus.Increase_Arm_Position(1, 0, 0, -length_increment);	

								// gripper open
								if (GetAsyncKeyState('Z') < 0) 
									taurus.Increase_Grip(0, 0.5);
								if (GetAsyncKeyState('X') < 0) 
									taurus.Increase_Grip(0, -0.5);
								if (GetAsyncKeyState('N') < 0) 
									taurus.Increase_Grip(1, 0.5);
								if (GetAsyncKeyState('M') < 0) 	
									taurus.Increase_Grip(1, -0.5);

							}
							else if (stepper.right_down())
							{

								if (GetAsyncKeyState('V') < 0) 
									escape_flag = 1;
								// for displaying current frame
								if (GetAsyncKeyState('C') < 0) 
								{
									pthread_mutex_lock(&mutexHaptic);
									haptic_packet.DisplayFrame();
									pthread_mutex_unlock(&mutexHaptic);
								}
								if (GetAsyncKeyState('R') < 0) 
									// write landmark data into a txt file which can be further used
								{
									std::ofstream trajectory_writer;
									//trajectory_writer.open(haptic_filename, ios::app); // for doing the experiment
									trajectory_writer.open("recorded_landmark.txt", ios::app);
									if (trajectory_writer.is_open())
									{
										pthread_mutex_lock(&mutexHaptic);
										for (int arm = 0; arm<2;arm++)
										{
											for (int i=0;i<4;i++)
												for (int j=0;j<4;j++)
													trajectory_writer<<haptic_packet.TipPose[arm][i][j]<<' ';
											//trajectory_writer<<haptic_packet.grasp[arm]<<' '<<std::endl;
											trajectory_writer<<20<<' '<<std::endl; //I want the gripper to be always closed, thus always writing 20
										}
										pthread_mutex_unlock(&mutexHaptic);
									}
								}
								if (GetAsyncKeyState('E') < 0) 
								{left_or_right = 0; axis = 'x'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('Q') < 0) 
								{left_or_right = 0; axis = 'x'; p_or_n = -1; rotate_trigger = true;}
								if (GetAsyncKeyState('S') < 0) 
								{left_or_right = 0; axis = 'y'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('W') < 0) 
								{left_or_right = 0; axis = 'y'; p_or_n = -1; rotate_trigger = true;}
								if (GetAsyncKeyState('A') < 0) 
								{left_or_right = 0; axis = 'z'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('D') < 0) 
								{left_or_right = 0; axis = 'z'; p_or_n = -1; rotate_trigger = true;}

								if (GetAsyncKeyState('O') < 0) 
								{left_or_right = 1; axis = 'x'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('U') < 0) 
								{left_or_right = 1; axis = 'x'; p_or_n = -1; rotate_trigger = true;}
								if (GetAsyncKeyState('K') < 0) 
								{left_or_right = 1; axis = 'y'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('I') < 0) 
								{left_or_right = 1; axis = 'y'; p_or_n = -1; rotate_trigger = true;}
								if (GetAsyncKeyState('J') < 0) 
								{left_or_right = 1; axis = 'z'; p_or_n = 1; rotate_trigger = true;}
								if (GetAsyncKeyState('L') < 0) 
								{left_or_right = 1; axis = 'z'; p_or_n = -1; rotate_trigger = true;}

								if ( rotate_trigger == true)
								{
									rotate_trigger = false;
									old_frame = taurus.get_haptic_packet(left_or_right);
									double increment_rotation_degree = 2;
									taurus.Update_Pose_Rotate(left_or_right, axis, p_or_n*increment_rotation_degree, old_frame, 0.04); // from experiment, 0.01 is a perfect value for the frequency of keyboard
								}
							}

							//taurus.Read_Pose_Packet();
						}

						stepper.terminate_stepper();	
						tele_operating_flag = false;
						haptic_init_flag = false;
						break;
					}
				case 'h':
					{
						seed[0].set_warning_threshold(3); //3.5 is the value in theory, but 3 works better
						seed[1].set_warning_threshold(3);
						seed[0].set_damping_factor(1);
						seed[1].set_damping_factor(1);
						tele_operating_flag = true;
						sam.speak("Use Hydra to control Taurus");
						std::cout<<"Use Hydra to teleoperate Taurus"<<std::endl;
						std::ofstream out;

						char rotation_enable;
						std::cout<<"To enable rotation, please enter 1, otherwise enter anything"<<std::endl;
						// std::cin>>rotation_enable;
						rotation_enable = '1'; // currently we just allow this possibility. The developement is not finished yet.
						if (rotation_enable == '1') std::cout<< "Enable rotation"<<std::endl;
						else std::cout<< "Disable rotation"<<std::endl;

						bnu::matrix <double> orig_rotation_pose (3,3);
						orig_rotation_pose(0,0) = 89458;
						orig_rotation_pose(0,1) = -9926;
						orig_rotation_pose(0,2) = 995941;
						orig_rotation_pose(1,0) = 404425;
						orig_rotation_pose(1,1) = -913441;
						orig_rotation_pose(1,2) = -45431;
						orig_rotation_pose(2,0) = 910185;
						orig_rotation_pose(2,1) = 406847;
						orig_rotation_pose(2,2) = -77700;
						cout<<"orig_rotation_pose "<< orig_rotation_pose;

						bnu::matrix <double> init_frame(4,4);
						init_frame = taurus.get_haptic_packet(0);
						cout<<"init_frame "<<init_frame;

						//time_t t = time(0);   // get time now
						//struct tm * now = localtime( & t );

						//char hydra_filename [80];-
						//strftime (hydra_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
						//strcat(hydra_filename, "Hydra_data.txt");
						//out.open(hydra_filename, ios::out);
						out.open(ch_logfilename, ios::out);
						while(1)
						{
							if(GetAsyncKeyState(VK_ESCAPE))
							{
								printf ("Finished using Hydra to control Taurus \n");
								break;
							}

							taurus.bytes_received = recvfrom(taurus.sd_M, taurus.buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&taurus.client_M, &taurus.length);
							//taurus.Read_Pose_Packet();
							//for (int arm = 0; arm<2;arm++)
							//{
							//	for (int i=0;i<4;i++)
							//		for (int j=0;j<4;j++)
							//			out<<taurus.pose_packet.TipPose[arm][i][j]<<' ';
							//	out<<taurus.pose_packet.grip[arm]<<' '<<std::endl;
							//	//trajectory_writer<<20<<' '<<std::endl;
							//}
							if (rotation_enable == '1')
							{
								taurus.bytes_sent = sendto(taurus.sd_A, taurus.buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&taurus.client_A, taurus.length);
							}
							//else
							//{
							//	for (size_t i = 0; i < 3; i++) {
							//		for (size_t j = 0; j < 3; j++) {
							//			if (left_or_right == 0)
							//				cout<<"original "<<*(double*)&taurus.buffer_R1[48 + 32*i + 8*j]<<'\t';
							//				*(double*)&taurus.buffer_R1[48 + 32*i + 8*j] = orig_rotation_pose(i,j);
							//				cout<<"altered "<<*(double*)&taurus.buffer_R1[48 + 32*i + 8*j]<<'\t';
							//			//else
							//				//*(double*)&taurus.buffer_R1[184 + 32*i + 8*j] = new_frame(i,j);
							//		}
							//	}

							//	taurus.bytes_sent = sendto(taurus.sd_A, taurus.buffer_R1, BUFFER_SIZE, 0, (struct sockaddr *)&taurus.client_A, taurus.length);
							//}
						}
						tele_operating_flag = false;
						haptic_init_flag = false;
						break;
					}
				default:
					std::cout<<"Wrong command!"<<std::endl; break;
				}
				//taurus.Read_Pose_Packet();	 
				pose_update_flag = 0;
			}
			else
				// this corresponds to the common case, there is no update, we send the packet to Client
				taurus.Send();	
		}
	}
	taurus.Terminate();
	return 0;
}

void* Agent2Master(void* info)
{
	//***********************************************************************************************************
	// to extract the char* from the void pointer, and use it as the name for the interface data log file
	string &logfilename = *(static_cast <std::string*> (info));
	char* ch_logfilename_hap = new char[40];
	strcpy (ch_logfilename_hap, logfilename.c_str());
	strcat(ch_logfilename_hap, "_haptic.txt");
	char* name_with_path = new char[50];
	strcpy(name_with_path, "log/");
	strcat(name_with_path, ch_logfilename_hap);
	ch_logfilename_hap = name_with_path;

	//char* ch_logfilename_warn = new char[40];
	//strcpy (ch_logfilename_warn, logfilename.c_str());
	//strcat(ch_logfilename_warn, "_warning.txt");
	//char* name_with_path2 = new char[50];
	//strcpy(name_with_path2, "log/");
	//strcat(name_with_path2, ch_logfilename_warn);
	//ch_logfilename_warn = name_with_path2;

	//***********************************************************************************************************

	//Packet_Heartbeat heartbeat_packet;
	TaurusX taurus2;
	taurus2.Initialize(0);
	seed.resize(2);
	//time_t t = time(0);   // get time now
	//struct tm * now = localtime( & t );

	//char haptic_filename [80];
	//strftime (haptic_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
	//strcat(haptic_filename, "haptic_trajectory.txt");

	//char warning_filename [80];
	//strftime (warning_filename,80,"%Y-%m-%d-%H-%M-%S-",now);
	//strcat(warning_filename, "warning_data.txt");



	/* Loop and get data from clients */
	while (1)
	{   

		/* Receive bytes from client */
		taurus2.bytes_received = recvfrom(taurus2.sd_M, taurus2.buffer_R2, BUFFER_SIZE, 0, (struct sockaddr *)&taurus2.client_M, &taurus2.length);

		if (taurus2.bytes_received < 0)
		{
			fprintf(stderr, "Could not receive datagram.\n");
			closesocket(taurus2.sd_M);
			pthread_exit(NULL);
			return 0;
		}

		std::string string_to_hold_buffer( taurus2.buffer_R2, BUFFER_SIZE);
		if (*(u_int*)&taurus2.buffer_R2[8] == 0x109)
		{

			// ******************************************************************************//
			// the old version of reading packet values using txt files as a temporary result
			//std::ofstream outfile ("temp_haptic_packet.txt",std::ofstream::binary);
			//outfile.write (taurus2.buffer_R2, BUFFER_SIZE);
			//outfile.close();
			//std::ifstream infile ("temp_haptic_packet.txt",std::ifstream::binary);
			///*std::ifstream infile;
			//infile.get(taurus2.buffer_R2, BUFFER_SIZE);*/
			//pthread_mutex_lock(&mutexHaptic);
			//haptic_packet.Read(infile);
			////haptic_packet.DisplayFrame();
			//infile.close();
			// *******************************************************************************//


			istringstream infile (string_to_hold_buffer, istringstream::in);
			pthread_mutex_lock(&mutexHaptic);
			haptic_packet.Read(infile);
			pthread_mutex_unlock(&mutexHaptic);


			// ******************** Experiment with TipForceVecotr***********************//
			// all zero, not implemented yet! Have to check the "Haptic" in the menu
			// std::cout<<haptic_packet.TipForceVector[0][0]<<' '<<haptic_packet.TipForceVector[0][1]<<' ' <<haptic_packet.TipForceVector[0][2]<<std::endl;
			/*std::ofstream TipForceVector_writer;
			TipForceVector_writer.open("TipForceVector.txt", ios::app);
			if (TipForceVector_writer.is_open())
			{
			TipForceVector_writer<<haptic_packet.TipForceVector[0][0]<<' '<<haptic_packet.TipForceVector[0][1]<<' ' <<haptic_packet.TipForceVector[0][2]<<std::endl;
			}*/
			// ******************** Experiment with TipForceVecotr***********************//

			bool record_trajectory = 1;
			if(record_trajectory)
			{
				std::ofstream trajectory_writer;
				trajectory_writer.open(ch_logfilename_hap, ios::app); 
				if (trajectory_writer.is_open())
				{
					string currentTime = currentDateTime();
					for (int arm = 0; arm<2;arm++)
					{
						trajectory_writer << currentTime << ' ' << arm << ' ';
						for (int i=0;i<4;i++)
							for (int j=0;j<4;j++)
								trajectory_writer<<haptic_packet.TipPose[arm][i][j]<<' ';
						trajectory_writer<<haptic_packet.grasp[arm]<<' '<<std::endl;
						printf("THE ARM %d has a n opening of %f", arm, haptic_packet.grasp[arm]);
						//trajectory_writer<<20<<' '<<std::endl; //I want the gripper to be always closed, thus always writing 20
					}
				}
			}

			// *********************write warning data into txt file*********************//
			/*std::ofstream warning_writer;
			warning_writer.open(ch_logfilename_warn, ios::app);
			if (warning_writer.is_open())
			{
				warning_writer<<seed[0].get_raw_warning_signal()<<std::endl<<seed[1].get_raw_warning_signal()<<std::endl;
			}*/
			// *********************write warning data into txt file*********************//


		}

		// *********************heartbeat packet*********************//
		//if (*(u_int*)&taurus2.buffer_R2[8] == 0x101)
		//{
		//	//std::ofstream outfile ("temp_heartbeat_packet.txt",std::ofstream::binary);
		//	//outfile.write (taurus2.buffer_R2, BUFFER_SIZE);
		//	//outfile.close();
		//	//std::ifstream infile ("temp_heartbeat_packet.txt",std::ifstream::binary);
		//	//std::ifstream infile;
		//	//std::stringstream infile;
		//	//infile<<taurus2.buffer_R2;
		//	//infile.get(taurus2.buffer_R2, BUFFER_SIZE);

		//	
		//	istringstream infile (string_to_hold_buffer, istringstream::in);
		//	pthread_mutex_lock(&mutexHeartbeat);
		//	heartbeat_packet.Read(infile);
		//	pthread_mutex_unlock(&mutexHeartbeat);
		//	//heartbeat_packet.DisplayFrame();
		//}
		// *********************heartbeat packet*********************//


		/* Send data to the master */
		taurus2.bytes_sent = sendto(taurus2.sd_A, taurus2.buffer_R2, BUFFER_SIZE, 0, (struct sockaddr *)&taurus2.client_A, taurus2.length);
		if ( taurus2.bytes_sent < 0)
		{
			fprintf(stderr, "Error sending datagram.\n");
			closesocket(taurus2.sd_A);
			pthread_exit(NULL);
		}
	}
	closesocket(taurus2.sd_M);
	closesocket(taurus2.sd_A);
	pthread_exit(NULL);
	return 0;
}

int main(int    argc, char **argv)
{
	WSADATA w;							/* Used to open windows connection */
	///* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}

	// Initialize the openGL window
	//glutInit(&argc, argv);
	

	// get input from user to specify which log file to write to
	string logfilename; // specify the log file name, all threads need to talk to it.
	std::cout<< "Please enter log file name" <<std::endl;
	std::cout<< "For Example ""kinect_1_2"", subject one, second trial: "<< std::endl;
	cin>>logfilename;

	pthread_t master2agent_thread, agent2master_thread, visual_feedback_thread;

	pthread_mutex_init(&mutexHaptic, NULL);
	pthread_mutex_init(&mutexHeartbeat, NULL);

	int x_client = 1, x_server = 2, x_leap = 3;

	int client_rc = pthread_create(&master2agent_thread,NULL,Master2Agent, static_cast<void*> (&logfilename));
	int server_rc = pthread_create(&agent2master_thread,NULL,Agent2Master, static_cast<void*> (&logfilename));
	int visualfeed_rc = 0; // = pthread_create(&visual_feedback_thread,NULL,Visual_Feedback,(void*)x_server);
	if(client_rc || server_rc || visualfeed_rc) 
	{
		std::cout<<"Failure to create threads"<<std::endl; return -1;
	}

	seed.resize(2);
	while(1)
	{
		;
	}

	WSACleanup();
	std::cout<<"Closing threads .."<<std::endl;
	pthread_mutex_destroy(&mutexHaptic);
	pthread_mutex_destroy(&mutexHeartbeat);
	pthread_exit(NULL);
	return 0;
}

