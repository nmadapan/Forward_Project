# Forward_Project

## Table of Contents
   * [Introduction](#introduction)
   * [Dataset Description](#dataset-description)
   * [Taurus](#taurus)
   * [Taurus Simulator](#taurus-sim)
   * [YuMi](#yumi)

## Introduction
In this project, we present the DESK (Dexterous Surgical Skill) dataset. It comprises a set of surgical robotic skills collected during a surgical training task using three robotic platforms: the Taurus II robot, Taurus II simulated robot, and the YuMi robot. This dataset was used to test the idea of transferring knowledge across different domains (e.g. from Taurus to YuMi robot) for a surgical gesture classification task with seven gestures. We explored three different scenarios: 1) No transfer, 2) Transfer from simulated Taurus to real Taurus and 3) Transfer from Simulated Taurus to the YuMi robot. We conducted extensive experiments with three supervised learning models and provided baselines in each of these scenarios. Results show that using simulation data during training enhances the performance on the real robot where limited real data is available. In particular, we obtained an accuracy of 55% on the real Taurus data using a model that is trained only on the simulator data. Furthermore, we achieved an accuracy improvement of 34% when 3% of the real data is added into the training process.

Link to the arxiv submission: [paper](https://arxiv.org/abs/1903.00959).

Link to the dataset: [data](https://purdue0-my.sharepoint.com/:f:/g/personal/gonza337_purdue_edu/EoSrFoOdZtNCp3vLGtY6TS4BDZhiA1cr3769AbRISE318w)

## Dataset Description
This dataset consists of a peg transfer task performed by several participants using the following three robotic platforms: Taurus II, Taurus II simulator and YuMi robot. Each peg transfer consists of a set seven surgemes and each surgeme was marked as a success or failure. The surgemes are annotated using a annotation tool developed by us. Furthermore, we also provide the data related to the segmentation of the objects on the peg board. The detailed information regarding the robots and the statistical design of data collection can be found in the paper. The variations in this dataset include:three robots, ten subjects, various peg board positions and orientations, and direction of peg transfer.

The link to the dataset points to the following folders.
### **Taurus**
----
	It contains 'Readme.txt' and 'data' folder which contains all of the files.

	For each Subject x and task j there will be a folder in the 'data' in the format Sx.
	Where x = 1...8, and j = 1...6. Each data folder Sx contains the following.
	Every file follows a CSV format, where the separator is a space/comma.

#### Kinematics files

	There will be one for each gripper, named:
		SxTj_kinematics_left.txt
		SxTk_ninematics_right.txt

	Important note 1. the data is recorded ONLY when the foot pedal was pressed,
	meaning, only when the robot whas active or in movement. If the robot was inactive, no kinematic data is recorded.

	Important note 2. The camera is oposite to the robot.
	Thus the percieved camera "Left" and "Right" are inverted from the robot's left and right.
	The robot's left and right are the ones that are used as a reference in the setup

	These are the elements in each line or data frame:

	* ut: unix time stamp
	* Tip pose matrix elements (in inches and radians):
		rot_00    rot_01    rot_02    tr_x
		rot_10    rot_11    rot_12    tr_y
		rot_20    rot_21    rot_22    tr_z
		0         0         0         1

	* Absolute pose of the tip in microns (with respect to the robot's origin):
	x: absolute x pose of the tip
	y: absolute y pose of the tip
	z: absolute z pose of the tip

	* pitch: pitch  of the robot's tip ( Radians * 1e6)
	* yaw: roll  of the robot's tip ( Radians * 1e6)
	( roll: roll  of the robot's tip ( Radians * 1e6)

	* gs: state of the gripper. A floating point that ranges from 30 (completely closed) to 100 (completely open)

	Each line in this file has the following format:

	ut rot_00 rot_01 rot_02 rot_10 rot_11 rot_12 rot_20 rot_21 rot_22 tr_x tr_y tr_z x y z gs

#### Peg segmentation files

	For every color video in the dataset you will find a file called SxTj_peg_segmentation.txt where you will find
	all the information to draw the bounding boxes on each of the three pegs on each frame on the video. Each line
	of this file have the following format:

	frameNumber peg1_x peg1_y peg1_w peg1_h peg2_x peg2_y peg2_w peg2_h peg3_x peg3_y peg3_w peg3_h acc1 acc2 acc3

	The frameNumber as the name implies correspond to what frame on the video is the information corresponding to,
	then you will find 4 numbers for each of the 3 pegs in the video. The format of each of these sets of 4 numbers
	is the following: the first pair of coordinates of the peg (pegN_x and pegN_y) corresponds to the x and y position
	of the left corner of the bounding box and the second pair of coordinates of the peg (pegN_w and pegN_h) corresponds
	respectively to the width and height of the bounding box.

#### Color videos
	There will be one for each subject and task named:
		* SxTj_color.avi

		This file will contain a color video with the entire task.

		There will be a corresonding file named:
		* SxTj_color_ts.txt

		This file will have the unix timestamp of each frame
		(so it can be traced back to the kinematic file timestamp and de depth timestamp)

#### Depth videos
	There will be one for each subject and task named:
		* SxTj_depth.avi

		This file will contain a depth video with the entire task.

		There will be a corresonding file named:
		* SxTj_depth_ts.txt

		This file will have the unix timestamp of each frame (so it can be traced back to the kinematic file timestamp and de depth timestamp)
----

### **Taurus Simulator**
----
	It contains 'Readme.txt' and 'data' folder which contains all of the files.

	For each Subject x and task j there will be a folder in the 'data' in the format Sx. Were x = 1...8, and j = 1...6. Each data folder Sx contains the following. Every file follows a CSV format, where the separator is a space.

#### Kinematics files

	There will be one for each gripper, named:
		Sx_Tj_left_kinematics.txt
		Sx_Tk_right_kinematics.txt

	These are the elements in each line or data frame:

	* ut: UTC
	* Robot arm kinematics
	* Absolute pose of the tip in inches (with respect to the robot's origin):
	x: absolute x pose of the tip
	y: absolute y pose of the tip
	z: absolute z pose of the tip

	* End effector orientation in radians:
	rx:	Rotation about robot x-axis
	ry:	Rotation about robot y-axis
	rz:	Rotation about robot z-axis

	* Joint angles for each arm in radians: J0-J6

	* gs: state of the gripper. A floating point that ranges from 30 (completely closed) to 100 (comppletely open)

	Each line in this file has the following format:

	ut  x y z rx ry rz J0 J1 J2 J3 J4 J5 J6 gs

#### Color videos
	There will be three files for each subject and task named:
	* Sx_Tj_left_color.avi : Robot's Left Stereo Camera view
	* Sx_Tj_right_color.avi : Robot's Left Stereo Camera view
	* Sx_Tj_oscr.avi : Oculus screen view

		There will be a corresonding file named:
		* Sx_Tj_color_ts.txt

		This file will have the UTC timestamp of each frame
		(so it can be traced back to the kinematic file timestamp and de depth timestamp)
----
### **YuMi**
	It contains 'data' folder and 'Readme.txt'

	For each Subject x and task j there will be a folder in the 'data' in the format Sx. Were x = 1...8, and j = 1...6. Each data folder Sx contains the following. Every file follows a CSV format, where the separator is a comma/space.

	Important note: The percieved camera (RGB and Depth) "Left" and "Right" correspond the robot's left and right.


#### Kinematics files
	There will be one for each gripper, named:
		Sx_Tj_yumi_left.txt
		Sx_Tk_yumi_right.txt

	These are the elements in each line or data frame:

	* ut: unix time stamp

	* Robot state (in degrees): rs_i robot_state joint i for the corresponding arm

	* Absolute position ( Pose of the arm tooltip with respect to the Robot's base)
		tr_x: absolute x position of the tip
		tr_y: absolute y position of the tip
		tr_z: absolute z position of the tip

	* rotation matrix rounded to 6 digits
		rot_00 rot_01 rot_02
		rot_10 rot_11 rot_12
		rot_20 rot_21 rot_22

	* gs: state of the gripper -> boolean variable
		closed: 1
		open: 0

	Each line in this file has the following format:

	rs_1, rs_2, rs_3, rs_4, rs_5, rs_6, rs_7, tr_x, tr_y, tr_z, rot_00, rot_01, rot_02, rot_10, rot_11, rot_12, rot_20, rot_21, rot_22, gs

#### Peg segmentation files

	For every color video in the dataset you will find a file called SxTj_peg_segmentation.txt where you will find
	all the information to draw the bounding boxes on each of the three pegs on each frame on the video. Each line
	of this file have the following format:

	frameNumber peg1_x peg1_y peg1_w peg1_h peg2_x peg2_y peg2_w peg2_h peg3_x peg3_y peg3_w peg3_h acc1 acc2 acc3

	The frameNumber as the name implies correspond to what frame on the video is the information corresponding to,
	then you will find 4 numbers for each of the 3 pegs in the video. The format of each one of these sets of 4 numbers
	is the following: the first pair of coordinates of the peg (pegN_x and pegN_y) corresponds to the x and y position
	of the upper-left corner of the bounding box and the second pair of coordinates of the peg (pegN_w and pegN_h) corresponds
	respectively to the width and height of the bounding box.

#### Color videos
	There will be one for each subject and task named:
		* Sx_Tj_color.avi

		This file will contain a color video with the entire task.

		There will be a corresonding file named:
		* Sx_Tj_color_ts.txt

		This file will have the unix timestamp of each frame
		(so it can be traced back to the kinematic file timestamp and de depth timestamp)

#### Depth videos
	There will be one for each subject and task named:
		* Sx_Tj_depth.avi

		This file will contain a depth video with the entire task.

		There will be a corresonding file named:
		* Sx_Tj_depth_ts.txt

		This file will have the unix timestamp of each frame (so it can be traced back to the kinematic file timestamp and de depth timestamp)
