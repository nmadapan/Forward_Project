'''
	Eliminate the features that are not in common in both the simulator and real domains.
	Each domain has only seven features.
'''
import numpy as np
import sys
import os
from os.path import join, basename, dirname, isfile, isdir
import math
from helpers import *

####################
## Initialization ##
####################

base_data_path = r'data'
simulator = False
normalize = True

both_hands = True
left_hand_flag = True

robot = 'taurus'
ignore_angles = False

if(simulator):
	input_file_name = robot + '_sim_kinematics_feature_x.txt'
	output_file_name = robot + '_sim_kinematics_feature_unified_x.txt'
else:
	input_file_name = robot + '_kinematics_feature_x.txt'
	output_file_name = robot + '_kinematics_feature_unified_x.txt'

if(robot == 'taurus'):
	A = np.loadtxt(join(base_data_path, input_file_name), delimiter = ' ')
	A1 = A[:, :int(A.shape[1]/2)] # add int for python 3?
	A2 = A[:, int(A.shape[1]/2):]

	## Consider only the EE position (3), orientation (3) and gripper status (1)
	if('sim' in input_file_name):
		# Change from 3 to 6 if you want to include the yaw pitch and roll
		if(ignore_angles):
			A1 = np.concatenate((A1[:, :3], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
			A2 = np.concatenate((A2[:, :3], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
		else:
			A1 = np.concatenate((A1[:, :6], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
			A2 = np.concatenate((A2[:, :6], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
	else:
		# Change -4 to -7 if you want to include the yaw pitch and roll
		if(ignore_angles):
			A1 = np.concatenate((A1[:, -4:-1], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
			A2 = np.concatenate((A2[:, -4:-1], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
		else:
			A1 = np.concatenate((A1[:, -7:-1], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
			A2 = np.concatenate((A2[:, -7:-1], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
		print(A1.shape, A2.shape)

if(robot == 'yumi'):
	A = np.loadtxt(join(base_data_path, input_file_name), delimiter = ' ')
	A1 = A[:, :A.shape[1]/2] # Get the left hand
	A2 = A[:, A.shape[1]/2:] # Get the right hand

	T1 = A1[:, 7:10] # Get the translation of the left hand
	T1[:,-1] = -1 * T1[:, -1] # Switch the z axis because Taurus is left handed coordinate system.
	G1 = A1[:, -1].reshape(-1, 1) # Get the gripper status of left

	if(not ignore_angles):
		R1 = convert_all(A1[:, 10:-1]) # Get the rotation as 9 elements. convert 9 elements to 3 (yaw pitch and roll)
		A1 = np.concatenate((T1, R1, G1), axis = 1)
	else:
		A1 = np.concatenate((T1, G1), axis = 1)

	## Right hand
	T2 = A2[:, 7:10]
	T2[:, -1] = -1 * T2[:, -1]
	G2 = A2[:, -1].reshape(-1, 1)

	if(not ignore_angles):
		R2 = convert_all(A2[:, 10:-1])
		A2 = np.concatenate((T2, R2, G2), axis = 1)
	else:
		A2 = np.concatenate((T2, G2), axis = 1)

if(both_hands):
	A = np.concatenate((A1, A2), axis = 1)
else:
	if(left_hand_flag): A = A1
	else: A = A2

if(normalize):
	A = (A - np.min(A, axis = 0))/(np.max(A, axis = 0)-np.min(A, axis = 0))

np.savetxt(join(base_data_path, output_file_name), A, delimiter = ' ', fmt='%.06f')
