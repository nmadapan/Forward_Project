
import time
import datetime
import numpy as np
import sys
import math

def findclosesttimestamp(target, kin):
	timestamp = target # large number
	mindiff = target
	for key in kin:
		#print (key)
		diff = abs(target - key)
		if diff < mindiff: #update timestamp
			timestamp = key
			mindiff = diff
	return timestamp
def ts2frame(dir, ST, arm):
	fwc_left = open("yumi_kinematics_left.txt", "a") # append
	fwc_right = open("yumi_kinematics_right.txt", "a")
	
	fcolor_annot = dir+"/"+ST+"_color_annot.txt"
	fcolor_ts = dir+"/"+ST+"_color_ts.txt"
	fkin_arm = dir+"/"+ST+"_yumi_"+arm+".txt"
	#fkin_right = dir+"/"+ST+"_right_kinematics.txt"
	kinematics_data = {}
	with open(fkin_arm) as f:
		for line in f:
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			# values = line.split(" ")
			values = line.split(",") # for yumi, comma separated
			timestamp = float(values[0])
			timestamp = round(timestamp, 2) #match with the video frame timestamp
			if timestamp not in kinematics_data:
				kinematics_data[timestamp] = line
	print (len(kinematics_data))
	frame2annotation = {}
	with open(fcolor_annot) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")

			print (line)
			sf, ef, surgeme, sucess = line.split(" ")
			sf = int(sf)
			ef = int(ef)
			for i in range(sf, ef+1):
				frame2annotation[i] = str(surgeme) + "," + str(sucess)
	# left arm
	fw = open(dir+"/"+ST+"_kinematics_"+arm+"_frame_ts.txt", "w")
	framecount = 0
	label = ""
	with open(fcolor_ts) as f:
		for line in f:
			framecount += 1
			if framecount in frame2annotation:
				label = frame2annotation[framecount]
				valid = "True"
			else: 
				label = "NA,NA"
				valid = "False"

			line = line.replace("\n", "")
			line = line.replace("\r", "")
			timestamp = float(line)
			timestamp = round(timestamp, 2) #match with the video frame timestamp, in case it's not formated to two decimal
			# find closest timestamp
			timestamp = findclosesttimestamp(timestamp, kinematics_data)
			if timestamp in kinematics_data:
				frame = kinematics_data[timestamp]

				# fw.write(ST+" "+frame + " " +str(valid)+ " " +label+ "\n")
				fw.write(ST+","+frame + "," +str(valid)+ "," +label+ "\n") # yumi
				if arm == "left":
					fwc_left.write(ST+","+frame + "," +str(valid)+ "," +label+ "\n")
				else:
					fwc_right.write(ST+","+frame + "," +str(valid)+ "," +label+ "\n")
			else:
				print("Timestamp missing: "+ str(timestamp))
	fw.close()
	fwc_left.close()
	fwc_right.close()
	print ("Done")
def filterdata(dfile):
	unique_ts = []
	fw = open("filtered/"+"dfile"+"_filtered.txt", "w")
	with open(dfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")

			#print line
			parts = line.split(" ")
			ts = int(float(parts[1]))
			if ts not in unique_ts:
				fw.write(line+"\n")
				unique_ts.append(ts)
	fw.close()
def merge_left_right(fleft, fright):
	fw = open("yumi_kinematics.txt", "w")
	left = []
	right = []
	with open(fleft) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			left.append(line)
	with open(fright) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			right.append(line)
	print (len(left))
	print (len(right))
	for i in range(0, len(left)):
		lelem = left[i]
		relem = right[i]
		lparts = lelem.split(",")
		rparts = relem.split(",")
		melem = ""

		# check discripency in timestamp of left right arm
		# if i > 0:
		# 	if lparts[1] != rparts[1]:
					
		if (lparts[0] == rparts[0]): # check trial id and not checking timestamp for left and right arm
			for i in range(0, len(lparts)-4):
				melem = melem + " " + lparts[i]
			if lparts[len(lparts)-4] == "True":
				melem = melem + " " + "1" 
			elif lparts[len(lparts)-4] == "False":
				melem = melem + " " + "0" 
			else:
				print("Not true not false, should not print")
			
			for i in range(2, len(rparts)-4): # with surgeme annotation
				melem = melem + " " + rparts[i]
			
			if rparts[len(rparts)-4] == "True":
				melem = melem + " " + "1" 
			elif rparts[len(rparts)-4] == "False":
				melem = melem + " " + "0" 
			else:
				print("Not true not false, should not print")

			for i in range(len(rparts)-3, len(rparts)): # with surgeme annotation
				melem = melem + " " + rparts[i]  
		else:
			print("Miss match - should not happen, check left right file carefully")
			a=1
		melem = melem.strip()
		if melem != "":
			fw.write(melem+"\n")
		else:
			print("Miss match during writing- should not happen, check left right file carefully")
			a=1
	fw.close()


# File: taurus_kinematics.txt
#     This file contains both left and right arms kinematic information as follows
#     Each line in this file has the following format: each value separated by space
#     // taskid ut l_rot_00 l_rot_01 l_rot_02 l_rot_10 l_rot_11 l_rot_12 l_rot_20 l_rot_21 l_rot_22 l_tr_x l_tr_y l_tr_z l_x l_y l_z l_gs r_rot_00 r_rot_01 r_rot_02 r_rot_10 r_rot_11 r_rot_12 r_rot_20 r_rot_21 r_rot_22 r_tr_x r_tr_y r_tr_z r_x r_y r_z r_gs valid surgeme_label success

#     Details:
#     1 = trial_id
#     2 = timestamp
#     3-21 = left arm kinematics
#     22-40 = right arm kinematics
#     41 = valid field; if True then valid and associated with the surgeme. If False then not associated with any surgeme task.
#     42 = surgeme_label => surgeme label (S1, S2 ...)
#     43 = success field; If True, surgeme completed successfully, if False, it's failed trial 

def process_for_sk():
	fwx = open("data/yumi_kinematics_feature_X.txt", "w")
	fwy = open("data/yumi_kinematics_feature_y.txt", "w")
	with open("yumi_kinematics.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			Xfeature = ""
			yfeature = str(parts[41]).replace("S","")# take only the integer number
			# print(parts[39])
			# print(len(parts))
			if parts[40] == "True": #and parts[42] == "True": #valid data point
				for i in range(2, 40):
					if i == 11:
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					elif i == 30:
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					else:
						Xfeature = Xfeature + " " + parts[i]
				Xfeature = Xfeature.strip()
				fwx.write(Xfeature+"\n")
				fwy.write(yfeature+"\n")
	fwx.close()
	fwy.close()
def process_kin_loc_for_sk():
	fwx = open("data/taurus_kinematics_location_onlysuccess_feature_X.txt", "w")
	fwy = open("data/taurus_kinematics_location_onlysuccess_feature_y.txt", "w")
	count_all = 0
	count_onlysuccess = 0
	with open("data/taurus_kinematics_peglocation.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			Xfeature = ""
			yfeature = str(parts[53]).replace("S","")# take only the integer number
			#print(parts[40])
			if parts[52] == "True":
				count_all += 1
			# if parts[52] == "True": #valid data point, success and unsuccess
			if parts[52] == "True" and parts[54] == "True": #valid data point, only success
				count_onlysuccess += 1
				for i in range(2, 52): # skip rowid, timestamp, and last three value
					Xfeature = Xfeature + " " + parts[i]
				Xfeature = Xfeature.strip()
				fwx.write(Xfeature+"\n")
				fwy.write(yfeature+"\n")
	print(count_onlysuccess, count_all)
	fwx.close()
	fwy.close()
def process_only_loc_for_sk():
	fwx = open("data/taurus_only_location_feature_X.txt", "w")
	fwy = open("data/taurus_only_location_feature_y.txt", "w")
	with open("data/taurus_only_peglocation.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			Xfeature = ""
			yfeature = str(parts[14]).replace("S","")# take only the integer number
			#print(parts[40])
			# if parts[52] == "True": #valid data point, success and unsuccess
			if parts[13] == "True" and parts[15] == "True": #valid data point, only success
				for i in range(1, 13): # skip rowid, and last three value
					Xfeature = Xfeature + " " + parts[i]
				Xfeature = Xfeature.strip()
				fwx.write(Xfeature+"\n")
				fwy.write(yfeature+"\n")
	fwx.close()
	fwy.close()

def totimestamp(timestr):
	timeformat = str(timestr).replace(".", ",")

	# print (time.mktime(datetime.datetime.strptime(s, "%d/%m/%Y").timetuple()))
	#1322697600.0
	# myDate = "2019-01-23 08:28:42,468"
	myDate = "2019-01-30" + " " + timeformat

	timestamp = time.mktime(datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f").timetuple())
	# print (timestamp)

	dt = datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f")
	tm = time.mktime(dt.timetuple()) + (dt.microsecond / 1000000.0)
	return tm
def merge_kinematic_visual_location():
	kinfile = "data/taurus_kinematics.txt"
	locfile = "data/pegpole_location.txt"
	kinlist = []
	loclist = []
	with open(kinfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			kinlist.append(line)
	with open(locfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			loclist.append(line)
	print(len(kinlist))
	print(len(loclist))
	fw = open("data/taurus_kinematics_peglocation.txt", "w")
	for i in range(0, len(kinlist)):
		row1 = (kinlist[i]).split(" ")
		row2 = (loclist[i]).split(" ")
		row = ""
		if row1[0] == row2[0]: # same trial and frame
			for i in range(0, len(row1)-3): # kinematic, except last three (valid, label, success)
				row = row+" "+row1[i]
			for i in range(1, len(row2)): # peg location, except rowid
				row = row+" "+row2[i]
			for i in range(len(row1)-3, len(row1)): # kinematic only last three (valid, label, success)
				row = row+" "+row1[i]
			fw.write(row+"\n")
	fw.close()

def merge_only_visual_location():
	kinfile = "data/taurus_kinematics.txt"
	locfile = "data/pegpole_location.txt"
	kinlist = []
	loclist = []
	with open(kinfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			kinlist.append(line)
	with open(locfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			loclist.append(line)
	print(len(kinlist))
	print(len(loclist))
	fw = open("data/taurus_only_peglocation.txt", "w")
	for i in range(0, len(kinlist)):
		row1 = (kinlist[i]).split(" ")
		row2 = (loclist[i]).split(" ")
		row = ""
		if row1[0] == row2[0]: # same trial and frame
			# for i in range(0, len(row1)-3): # kinematic, except last three (valid, label, success)
			# 	row = row+" "+row1[i]
			for i in range(0, len(row2)): # peg location, except rowid
				row = row+" "+row2[i]
			for i in range(len(row1)-3, len(row1)): # kinematic only last three (valid, label, success)
				row = row+" "+row1[i]
			fw.write(row+"\n")
	fw.close()


def process_for_video():
	fwx = open("data/video_yumi_kinematics_feature_X.txt", "w")
	fwy = open("data/video_yumi_kinematics_feature_y.txt", "w")
	count = 0
	
	prev_label = "0"
	with open("yumi_kinematics.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			count += 1
			Xfeature = ""
			yfeature = str(parts[41]).replace("S","")# take only the integer number
			# print(parts[39])
			# print(len(parts))
			if prev_label != parts[0]:
				count = 1 
			prev_label = parts[0]

			if parts[40] == "True": #and parts[42] == "True": #valid data point
				for i in range(2, 40):
					if i == 11: # take care of  inconsistency in the data collection process
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					elif i == 30: # take care of  inconsistency in the data collection process
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					else:
						Xfeature = Xfeature + " " + parts[i]
				
				Xfeature = Xfeature.strip()
				fwx.write(str(parts[0])+"_color_frame"+str(count)+".jpg" + " "+Xfeature+"\n")
				fwy.write(str(parts[0])+"_color_frame"+str(count)+".jpg" + " "+yfeature+"\n")

			
	fwx.close()
	fwy.close()
def merge_kinematic_video():
	fwx = open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X.txt", "w")
	fwy = open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_y.txt", "w")
	count = 0
	# sample_rate = 20
	prev_label = "0"
	kindata = {}
	with open("data/video_yumi_kinematics_feature_X.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			kf = ""
			for i in range(1, len(parts)):
				kf = kf +" "+ parts[i]
			kf = kf.strip()
			kindata[parts[0]] = kf
	
	# with open("data/video_yumi_kinematics_feature_y.txt") as f:
	# 	for line in f:
	# 		line = line.strip()
	# 		line = line.replace("\n", "")
	# 		line = line.replace("\r", "")
	# 		parts = line.split(" ")
			
			# yT_kin.append(int(float(parts[1])))
	# print(kindata['S1_T1_color_frame4883.jpg'])
	# print(len(kindata))
	# print(len(kindata['S1_T1_color_frame4883.jpg'].split(" ")))
	frameids = []
	with open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/frameid_sorted_nodiff_all_1.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			frameid = (line.split("/")[-1]).strip()
			frameids.append(frameid)

	print("Loading video features")
	features = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/features_sorted_nodiff_all_1.cnn", delimiter=' ')
	
	XT, yT = features[:, :-1], features[:, -1]
	print("Applying pca")

	from sklearn.decomposition import PCA
	pca = PCA(n_components=30)
	pca.fit(XT) # need to do it only on train data to simulate that we are using training on pca approach. verify?
	# print(pca.explained_variance_ratio_)  
	# print(pca.singular_values_)  
	XT = pca.transform(XT) 
	print(len(XT), len(XT[0]))

	assert(len(frameids) == len(XT))
	
	for frameid in frameids:
		if frameid in kindata:
			kinfeature = kindata[frameid]
		# else:
		# 	print("Key not found: "+str(frameid))

			vdata = XT[frameids.index(frameid)]
			label = yT[frameids.index(frameid)]
			vfeature = ""
			for val in vdata.tolist():
				vfeature = vfeature + " " + str(val)
			vfeature = vfeature.strip()
			
			# print(len(kinfeature.split(" ")))

			fwx.write(kinfeature+" "+vfeature+"\n") 
			fwy.write(str(label)+"\n")
		else:
			print("Skipping, Key not found: "+str(frameid))
	# all kinematic
	# for frameid in kindata:
	# 	kinfeature = kindata[frameid]
	# 	label = yT_kin[kindata.index(frameid)]
	# 	if frameid in frameids:
	# 	# else:
	# 	# 	print("Key not found: "+str(frameid))
	# 		vdata = XT[frameids.index(frameid)]
	# 		# label = yT[frameids.index(frameid)]
	# 		vfeature = ""
	# 		for val in vdata.tolist():
	# 			vfeature = vfeature + " " + str(val)
	# 		vfeature = vfeature.strip()
			
	# 		# print(len(kinfeature.split(" ")))

	# 		fwx.write(kinfeature+" "+vfeature+"\n") 
	# 		fwy.write(str(label)+"\n")
	# 	else:
	# 		print("Skipping, Key not found, but adding 0 image feature: "+str(frameid))
	# 		fwx.write(kinfeature+" "+("0"*30).strip()+"\n") 
	# 		fwy.write(str(label)+"\n")


	fwx.close()
	fwy.close()
def totimestamp(timestr):
	timeformat = str(timestr).replace(".", ",")

	# print (time.mktime(datetime.datetime.strptime(s, "%d/%m/%Y").timetuple()))
	#1322697600.0
	# myDate = "2019-01-23 08:28:42,468"
	myDate = "2019-01-30" + " " + timeformat

	timestamp = time.mktime(datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f").timetuple())
	# print (timestamp)

	dt = datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f")
	tm = time.mktime(dt.timetuple()) + (dt.microsecond / 1000000.0)
	return tm
# Calculates rotation matrix to euler angles
# The result is the same as MATLAB except the order
# of the euler angles ( x and z are swapped ).
def rotationMatrixToEulerAngles(R) :
	# assert(isRotationMatrix(R))
	sy = math.sqrt(R[0,0] * R[0,0] +  R[1,0] * R[1,0])
	singular = sy < 1e-6
	if  not singular :
		x = math.atan2(R[2,1] , R[2,2])
		y = math.atan2(-R[2,0], sy)
		z = math.atan2(R[1,0], R[0,0])
	else :
		x = math.atan2(-R[1,2], R[1,1])
		y = math.atan2(-R[2,0], sy)
		z = 0
	return z, y, -1*x
	# x, -1*y, z is kinda good but still random

def convert_all(RMat):
	'''
		RMat is a 2D np.ndarray. Each row is a flattened version of the rotation matrix.
	'''
	res = []
	for row in RMat:
		# print(len(row))
		# print(row)
		R = np.reshape(row, (3, 3))
		res.append(rotationMatrixToEulerAngles(R))
	return np.array(res)
def merge_unify_kin_video(video=True, kin=True):
	ignore_angles = False # True, False
	normalize = False
	A = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X.txt", delimiter = ' ')
	print(A.shape)
	
	A1 = A[:, :20] # add int for python 3?
	A2 = A[:, 20:40]
	A3 = A[:, 40:]
	print(A1.shape, A2.shape, A3.shape)
	print(A1[1], A2[1], A3[1])
	# sys.exit()
	# A1 = np.concatenate((A1[:, -7:-1], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
	# A2 = np.concatenate((A2[:, -7:-1], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
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


	print(len(A[0]))
	print(A1.shape, A2.shape, A3.shape)
	print(A1[1], A2[1], A3[1])
	if video == True and kin == True:
		A_new = np.concatenate((A1, A2, A3), axis = 1)
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X_unify.txt', A_new, delimiter=' ')
	elif video == False and kin == True:
		A_new = np.concatenate((A1, A2), axis = 1)
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_only_feature_1_X_unify.txt', A_new, delimiter=' ')
	elif video == True and kin == False:
		A_new = A3
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_video_only_feature_1_X_unify.txt', A_new, delimiter=' ')

if __name__ == '__main__':
	# scount = 8
	# tcount = 6
	# for sc in range(3, scount+1):
	# 	for tc in range(1, tcount+1):
	# 		ts2frame("data/Yumi/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "left")
	# 		ts2frame("data/Yumi/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "right")
	# 		#filterdata("dataset/Taurus/S"+str(sc)+"/S"+str(sc)+"_T"+str(tc)+"_kinematics_left_frame_ts.txt")
	# merge_left_right("yumi_kinematics_left.txt", "yumi_kinematics_right.txt")
	# process_for_sk()
	# print (totimestamp("20:56:10.234"))
	# print (totimestamp("20:56:10.275"))
	# print (totimestamp("20:56:11.018"))
	# merge_kinematic_visual_location()
	# process_kin_loc_for_sk()
	# merge_only_visual_location()
	# process_for_sk()
	# process_kin_loc_for_sk()
	

	# **** kinematic and video ****
	# process_for_video()
	# merge_kinematic_video()
	merge_unify_kin_video(video=True, kin=True)
	merge_unify_kin_video(video=True, kin=False)
	merge_unify_kin_video(video=False, kin=True)



